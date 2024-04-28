/*Origin Author: Hugo*/
#include "service.h"
#include "gyro.h"
#include "communicate.h"
#include "runtime.h"

//// System services
static u8 comm_count = 0;
void serviceCommunication(void)
{
    dataPayload_t tmpData;
    //    cmdPayload_t tmpCmdResponse;
    if (comm_count == 2)
    {
        if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))
        {
            tmpData = buildDataPayload(GYRO1);
        }
        else
        {
            tmpData = buildDataPayload(GYRO2);
        }
        sendData(tmpData);

        cmdPayload_t tmpCmd = recvCmd();
        decodeCmdPayload(tmpCmd);
    }
    //    else if (comm_count == 3)
    //    {
    //        if (responseFlag == 1)
    //        {
    //            tmpCmdResponse = buildCmdFeedbackPayload(reponseCmdGyroID, responseType);
    //            sendCmdResponse(tmpCmdResponse, 0x3C);
    //            cmdPayload_t tmpCmd = spiRead();
    //            decodeCmdPayload(tmpCmd);
    //        }
    //    }
    else if (comm_count == 4)
    {
        comm_count = 0;
        if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2))
        {
            tmpData = buildDataPayload(GYRO2);
        }
        else
        {
            tmpData = buildDataPayload(GYRO1);
        }

        sendData(tmpData);
        cmdPayload_t tmpCmd = recvCmd();
        decodeCmdPayload(tmpCmd);
    }
    comm_count++; // 1ms计数，每5ms获取一包信息，每20ms进行一次传输
}

void serviceGyro1(void)
{
    readGyroData(GYRO1);

    // pre-core-service data analyze/collect
    resonantEstimator(&g1_resonantEstimatorConfig);
    phaseFinder(&g1_phaseFinderConfig);
    // mode switch
    modeSwitching(&g1_modeSwitchingConfig);

    // --- core service ---
    // check lock on frequency word:
    if (g1_openloopConfig.frequency_enable != 0)
    {
        openLoop(&g1_openloopConfig, OP_FRE);
    }
    else if (g1_sweeper.enable != 0)
    {
        sweeper(&g1_sweeper);
    }
    else if (g1_smPLLConfig.loopSource != 0)
    {
        phaseLockLoop(&g1_smPLLConfig);
    }

    // excite signal
    openLoop(&g1_openloopConfig, OP_AMP);
    abstractLoop(&g1_smPidAiConfig);
    abstractLoop(&g1_smPidAqConfig);
    abstractLoop(&g1_smPidBiConfig);
    abstractLoop(&g1_smPidBqConfig);
    // whole angle mode 
    wamEQcalculation(GYRO1);
    wamSRoperation(GYRO1);
    wamAngleCalucation(GYRO1);
    if( g1_smPidEcConfig.loopSource != 0 || g1_smPidQcConfig.loopSource != 0){        
        abstractLoop(&g1_smPidEcConfig);
        abstractLoop(&g1_smPidQcConfig);
        wamDriveOperation(&g1_smPidEcConfig);
        wamPhaseCompensation(GYRO1);
    }
    
    // check hv lock
    if (g1_openloopConfig.hv_enable != 0)
    {
        openLoop(&g1_openloopConfig, OP_HV);
    }
    // else if (HV tuner)
    // --- END of core service ---

    // applie phase shifers
    openLoop(&g1_openloopConfig, OP_PHASE);

    // post-core-service data analyze/collect
    calculateQFactor(&g1_qFactor);

    applyToGyro(GYRO1);
}
void serviceGyro2(void)
{
    readGyroData(GYRO2);

    resonantEstimator(&g2_resonantEstimatorConfig);
    // phaseFinder(&g2_phaseFinderConfig);
    // mode switch
    modeSwitching(&g2_modeSwitchingConfig);
    if (g2_openloopConfig.frequency_enable != 0)
    {
        openLoop(&g2_openloopConfig, OP_FRE);
    }
    else if (g2_sweeper.enable != 0)
    {
        sweeper(&g2_sweeper);
    }
    else if (g2_smPLLConfig.loopSource != 0)
    {
        phaseLockLoop(&g2_smPLLConfig);
    }
    // else if (////PLL lock)

    openLoop(&g2_openloopConfig, OP_AMP);
    abstractLoop(&g2_smPidAiConfig);
    abstractLoop(&g2_smPidAqConfig);
    abstractLoop(&g2_smPidBiConfig);
    abstractLoop(&g2_smPidBqConfig);

    if (g2_openloopConfig.hv_enable != 0)
    {
        openLoop(&g2_openloopConfig, OP_HV);
    }
    // else if (HV tuner)

    openLoop(&g2_openloopConfig, OP_PHASE);

    calculateQFactor(&g2_qFactor);

    applyToGyro(GYRO2);
}

void serviceGyro(void)
{
    if (GYRO_MASK == USE_GYRO_SLOT_1)
    {
        serviceGyro1();
    }
    else if (GYRO_MASK == USE_GYRO_SLOT_2)
    {
        serviceGyro2();
    }
    else if (GYRO_MASK == USE_GYRO_SLOT_BOTH)
    {
        serviceGyro1();
        serviceGyro2();
    }
}

void serviceInitX(u8 gyroID)
{
    if (gyroID == GYRO1)
    {
        initGyroData(gyroID, &gyroData_1);
        initGyroOutput(gyroID, &gyroOutput_1);
        initOpenloopConfig(gyroID, &g1_openloopConfig);
        initSweeperConfig(gyroID, &g1_sweeper);
        initPidConfig(gyroID, &g1_smPLLConfig);
        initPidConfig(gyroID, &g1_smPidAiConfig);
        initPidConfig(gyroID, &g1_smPidAqConfig);
        initPidConfig(gyroID, &g1_smPidBiConfig);
        initPidConfig(gyroID, &g1_smPidBqConfig);
        initPidConfig(gyroID, &g1_smPidEcConfig);
        initPidConfig(gyroID, &g1_smPidQcConfig);
        initQFactorConfig(gyroID, &g1_qFactor);
        initOpenloopConfig(gyroID, &g1_qfOpenloopConfig);
        initPidConfig(gyroID, &g1_qfPLLConfig);
        initPidConfig(gyroID, &g1_qfPidAiConfig);
        initPidConfig(gyroID, &g1_qfPidAqConfig);
        initPidConfig(gyroID, &g1_qfPidBiConfig);
        initPidConfig(gyroID, &g1_qfPidBqConfig);
        initResonantEstimator(gyroID, &g1_resonantEstimatorConfig);
        initPhaseFinder(gyroID, &g1_phaseFinderConfig);
        initModeSwitching(gyroID, &g1_modeSwitchingConfig);

        // copy1:
        initOpenloopConfig(gyroID, &copy1_g1_openloopConfig);
        initSweeperConfig(gyroID, &copy1_g1_sweeper);
        initPidConfig(gyroID, &copy1_g1_smPLLConfig);
        initPidConfig(gyroID, &copy1_g1_smPidAiConfig);
        initPidConfig(gyroID, &copy1_g1_smPidAqConfig);
        initPidConfig(gyroID, &copy1_g1_smPidBiConfig);
        initPidConfig(gyroID, &copy1_g1_smPidBqConfig);

        // copy2:
        initOpenloopConfig(gyroID, &copy2_g1_openloopConfig);
        initSweeperConfig(gyroID, &copy2_g1_sweeper);
        initPidConfig(gyroID, &copy2_g1_smPLLConfig);
        initPidConfig(gyroID, &copy2_g1_smPidAiConfig);
        initPidConfig(gyroID, &copy2_g1_smPidAqConfig);
        initPidConfig(gyroID, &copy2_g1_smPidBiConfig);
        initPidConfig(gyroID, &copy2_g1_smPidBqConfig);
    }
    else if (gyroID == GYRO2)
    {
        initGyroData(gyroID, &gyroData_2);
        initGyroOutput(gyroID, &gyroOutput_2);
        initOpenloopConfig(gyroID, &g2_openloopConfig);
        initSweeperConfig(gyroID, &g2_sweeper);
        initPidConfig(gyroID, &g2_smPLLConfig);
        initPidConfig(gyroID, &g2_smPidAiConfig);
        initPidConfig(gyroID, &g2_smPidAqConfig);
        initPidConfig(gyroID, &g2_smPidBiConfig);
        initPidConfig(gyroID, &g2_smPidBqConfig);
        initPidConfig(gyroID, &g2_smPidEcConfig);
        initPidConfig(gyroID, &g2_smPidQcConfig);
        initQFactorConfig(gyroID, &g2_qFactor);
        initOpenloopConfig(gyroID, &g2_qfOpenloopConfig);
        initPidConfig(gyroID, &g2_qfPLLConfig);
        initPidConfig(gyroID, &g2_qfPidAiConfig);
        initPidConfig(gyroID, &g2_qfPidAqConfig);
        initPidConfig(gyroID, &g2_qfPidBiConfig);
        initPidConfig(gyroID, &g2_qfPidBqConfig);
        initResonantEstimator(gyroID, &g2_resonantEstimatorConfig);
        initPhaseFinder(gyroID, &g2_phaseFinderConfig);
        initModeSwitching(gyroID, &g2_modeSwitchingConfig);

        // copy1:
        initOpenloopConfig(gyroID, &copy1_g2_openloopConfig);
        initSweeperConfig(gyroID, &copy1_g2_sweeper);
        initPidConfig(gyroID, &copy1_g2_smPLLConfig);
        initPidConfig(gyroID, &copy1_g2_smPidAiConfig);
        initPidConfig(gyroID, &copy1_g2_smPidAqConfig);
        initPidConfig(gyroID, &copy1_g2_smPidBiConfig);
        initPidConfig(gyroID, &copy1_g2_smPidBqConfig);

        // copy2:
        initOpenloopConfig(gyroID, &copy2_g2_openloopConfig);
        initSweeperConfig(gyroID, &copy2_g2_sweeper);
        initPidConfig(gyroID, &copy2_g2_smPLLConfig);
        initPidConfig(gyroID, &copy2_g2_smPidAiConfig);
        initPidConfig(gyroID, &copy2_g2_smPidAqConfig);
        initPidConfig(gyroID, &copy2_g2_smPidBiConfig);
        initPidConfig(gyroID, &copy2_g2_smPidBqConfig);
    }
}

void serviceInit(void)
{
    if (GYRO_MASK == USE_GYRO_SLOT_1)
    {
        serviceInitX(GYRO1);
    }
    else if (GYRO_MASK == USE_GYRO_SLOT_2)
    {
        serviceInitX(GYRO2);
    }
    else if (GYRO_MASK == USE_GYRO_SLOT_BOTH)
    {
        serviceInitX(GYRO1);
        serviceInitX(GYRO2);
    }
}
