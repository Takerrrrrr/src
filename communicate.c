/*Origin Author: Hugo*/
#include "behavior.h"
#include "runtime.h"
#include "communicate.h"
#include "gyro.h"

#define GYRO1 1
#define GYRO2 2

typedef enum
{
    FRE_STATE_OP = 0x01,
    FRE_STATE_SWEEPER = 0x02,
    FRE_STATE_PLL = 0x03,
} frequencyStateLock_e;
void processFrequencyLock(u8 Gyro_ID, frequencyStateLock_e newState, u8 newStateValue)
{
    openloopConfig_t tmpOpenloopConfig = (Gyro_ID == 1 ? g1_openloopConfig : g2_openloopConfig);
    sweeperConfig_t tmpSweeperConfig = (Gyro_ID == 1 ? g1_sweeper : g2_sweeper);
    loopConfig_t tmpPLLConfig = (Gyro_ID == 1 ? g1_smPLLConfig : g2_smPLLConfig);

    if (newStateValue != 0)
    {
        tmpOpenloopConfig.frequency_enable = 0;
        tmpSweeperConfig.enable = 0;
        tmpPLLConfig.loopSource = 0;
        switch (newState)
        {
        case FRE_STATE_OP:
        {
            tmpOpenloopConfig.frequency_enable = newStateValue;
            break;
        }
        case FRE_STATE_SWEEPER:
        {
            tmpSweeperConfig.enable = newStateValue;
            break;
        }
        case FRE_STATE_PLL:
        {
            tmpPLLConfig.loopSource = newStateValue;
            break;
        }
        }
        if (Gyro_ID == GYRO1)
        {
            g1_openloopConfig = tmpOpenloopConfig;
            g1_sweeper = tmpSweeperConfig;
            g1_smPLLConfig = tmpPLLConfig;
        }
        else if (Gyro_ID == GYRO2)
        {
            g2_openloopConfig = tmpOpenloopConfig;
            g2_sweeper = tmpSweeperConfig;
            g2_smPLLConfig = tmpPLLConfig;
        }
    }
}
// telemetry type selector
u32 g1_tlm_sel;
u32 g2_tlm_sel;
dataPayload_t buildDataPayload(u8 Gyro_ID) // 发送数据帧结构
{
    u32 tlm_selector = (Gyro_ID == GYRO1 ? g1_tlm_sel : g2_tlm_sel);
    gyroOutput_t localGyroOutput = (Gyro_ID == GYRO1 ? gyroOutput_1 : gyroOutput_2);
    //    if (Gyro_ID == GYRO1)
    //    {
    //        tlm_selector = g1_tlm_sel;
    //        // localData = gyroData_1;
    //    }
    //    else if (Gyro_ID == GYRO2)
    //    {
    //        tlm_selector = g2_tlm_sel;
    //        // localData = gyroData_2;
    //    }
    dataPayload_t localDataPayload;
    localDataPayload.gyroID = Gyro_ID;
    localDataPayload.time = getSystemTime();
    localDataPayload.amp_cha_i = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHA_I);
    localDataPayload.amp_cha_q = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHA_Q);
    localDataPayload.amp_cha = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHA);
    localDataPayload.amp_chb_i = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHB_I);
    localDataPayload.amp_chb_q = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHB_Q);
    localDataPayload.amp_chb = getGyroData(Gyro_ID, RUNTIME_FIELD_AMP_CHB);

    switch (tlm_selector)
    {
    case MFIELDS_TYPE_OPLP:
    {
        localDataPayload.loopError = 0;
        localDataPayload.loopOutput = 0;
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }

    case MFIELDS_TYPE_SWEEPER:
    {
        localDataPayload.loopError = 0;
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_CURRENT_FREQUENCY);
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_PHASE_A);
        localDataPayload.reserved2 = getGyroData(Gyro_ID, RUNTIME_FIELD_PHASE_B);
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_SM_PLL:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PLL_CURRENT_ERROR);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_CURRENT_FREQUENCY);
        localDataPayload.loopReserved = getGyroData(Gyro_ID, RUNTIME_FIELD_PLL_CURRENT_PHASE);
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_E);     // WAM_E transfer to upper computer
        localDataPayload.reserved2 = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_ANGLE); // WAM_Angle transfer to upper computer
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PID_AI:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_AI_CURRENT_ERROR);
        // localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT);
        localDataPayload.loopOutput = localGyroOutput.channelAI;
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PID_AQ:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_AQ_CURRENT_ERROR);
        // localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT);
        localDataPayload.loopOutput = localGyroOutput.channelAI;
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PID_BI:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_BI_CURRENT_ERROR);
        // localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT);
        localDataPayload.loopOutput = localGyroOutput.channelBI;
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PID_BQ:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_BQ_CURRENT_ERROR);
        // localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT);
        localDataPayload.loopOutput = localGyroOutput.channelBQ;
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_QFACTOR:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_QFACTOR_START_FREQUENCY);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_QFACTOR_SHORT_TAU);
        localDataPayload.loopReserved = getGyroData(Gyro_ID, RUNTIME_FIELD_QFACTOR_LONG_TAU);
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_QFACTOR_MIN);
        localDataPayload.reserved2 = getGyroData(Gyro_ID, RUNTIME_FIELD_QFACTOR_MAX);
        localDataPayload.reserved3 = 0;
        break;
    }

    case MFIELDS_TYPE_RESONANT_EST:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_EST_RESONANT_FREQUENCY);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_MIN_AGC_OUTPUT);
        localDataPayload.loopReserved = 0;
        localDataPayload.reserved1 = 0;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PHASE_FINDER:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PLL_CURRENT_ERROR);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_CURRENT_FREQUENCY);
        localDataPayload.loopReserved = getGyroData(Gyro_ID, RUNTIME_FIELD_PLL_CURRENT_PHASE);
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_PHASEFINDER_AGCAVG);
        localDataPayload.reserved2 = getGyroData(Gyro_ID, RUNTIME_FIELD_PLL_CURRENT_SET_PHASE);
        localDataPayload.reserved3 = getGyroData(Gyro_ID, RUNTIME_FIELD_PHASEFINDER_DEBUG);
        break;
    }
    case MFIELDS_TYPE_PID_EC:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_EC_CURRENT_ERROR);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_EC_CURRENT_OUTPUT);
        localDataPayload.loopReserved = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_ANGLE);
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_E);     // WAM_E transfer to upper computer;
        localDataPayload.reserved2 = 0;
        localDataPayload.reserved3 = 0;
        break;
    }
    case MFIELDS_TYPE_PID_QC:
    {
        localDataPayload.loopError = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_QC_CURRENT_ERROR);
        localDataPayload.loopOutput = getGyroData(Gyro_ID, RUNTIME_FIELD_PID_QC_CURRENT_OUTPUT);
        localDataPayload.loopReserved = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_ANGLE);
        localDataPayload.reserved1 = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_Q);     // WAM_Q transfer to upper computer;
        localDataPayload.reserved2 = getGyroData(Gyro_ID, RUNTIME_FIELD_WAM_E);
        localDataPayload.reserved3 = 0;
        break;
    }
    }

    localDataPayload.type = tlm_selector;
    return localDataPayload;
}
/*****************************************************************************
Function: collect_telemetry(u8 times,u16 gyro_mask)
Description: 采样发送数据，每5ms进行采样
Input:
    u8 times ：第几次采样
    u16 gyro_mask ：陀螺掩码，支持16个陀螺，每一位置1则发送当前陀螺数据，
                                当前仅设计2个陀螺，即最低两位。若仅有1个陀螺有效，20ms
                                内4帧均为该陀螺信息，速率200Hz/s，若2个陀螺均有效，则
                                第0ms采样1号，第5ms采样2号，第10ms再次采样1号，第15ms采样2号，速率100Hz/s。
Return: void
*******************************************************************************/
u8 responseFlag = 0;
u8 reponseCmdGyroID = 0;
u16 responseType = 0;
void decodeCmdPayload(cmdPayload_t cmd)
{

    reponseCmdGyroID = cmd.gyroID;
    responseType = cmd.type;
    switch (cmd.type)
    {
    case CMD_SYSTEM_RESERVED:
    {
        responseFlag = 1;
        break;
    }
    case CMD_TLM_SELECTOR:
    {
        responseFlag = 1;
        u8 tmp_tlm_selector = (u8)((cmd.data0 >> 24) & 0x000000FF);
        if (cmd.gyroID == 1)
            g1_tlm_sel = tmp_tlm_selector;
        else if (cmd.gyroID == 2)
            g2_tlm_sel = tmp_tlm_selector;
        break;
    }
    case CMD_OPENLOOP_FREQUENCY:
    {
        responseFlag = 1;
        openloopConfig_t tmp = (cmd.gyroID == 1 ? g1_openloopConfig : g2_openloopConfig);
        tmp.frequency_enable = cmd.data0;
        tmp.freq = cmd.data1;
        if (cmd.gyroID == 1)
            g1_openloopConfig = tmp;
        else if (cmd.gyroID == 2)
            g2_openloopConfig = tmp;
        processFrequencyLock(cmd.gyroID, FRE_STATE_OP, tmp.frequency_enable);
        break;
    }
    case CMD_OPENLOOP_ACT:
    {
        responseFlag = 1;
        openloopConfig_t tmp = (cmd.gyroID == 1 ? g1_openloopConfig : g2_openloopConfig);
        tmp.amp_enable = cmd.data0;
        tmp.channelAI = cmd.data1;
        tmp.channelAQ = cmd.data2;
        tmp.channelBI = cmd.data3;
        tmp.channelBQ = cmd.data4;
        if (cmd.gyroID == 1)
            g1_openloopConfig = tmp;
        else if (cmd.gyroID == 2)
            g2_openloopConfig = tmp;
        break;
    }
    case CMD_OPENLOOP_HV:
    {
        responseFlag = 1;
        openloopConfig_t tmp = (cmd.gyroID == 1 ? g1_openloopConfig : g2_openloopConfig);
        tmp.hv_enable = cmd.data0;
        tmp.hv1 = cmd.data1;
        tmp.hv2 = cmd.data2;
        tmp.hv3 = cmd.data3;
        tmp.hv4 = cmd.data4;
        if (cmd.gyroID == 1)
            g1_openloopConfig = tmp;
        else if (cmd.gyroID == 2)
            g2_openloopConfig = tmp;
        break;
    }
    case CMD_OPENLOOP_PHASE:
    {
        responseFlag = 1;
        openloopConfig_t tmp = (cmd.gyroID == 1 ? g1_openloopConfig : g2_openloopConfig);
        tmp.drivePhaseShiftA = cmd.data0;
        tmp.drivePhaseShiftB = cmd.data1;
        tmp.demodPhaseShiftA = cmd.data2;
        tmp.demodPhaseShiftB = cmd.data3;
        if (cmd.gyroID == 1)
            g1_openloopConfig = tmp;
        else if (cmd.gyroID == 2)
            g2_openloopConfig = tmp;
        break;
    }
    case CMD_SWEEPER:
    {
        responseFlag = 1;
        sweeperConfig_t tmp = (cmd.gyroID == 1 ? g1_sweeper : g2_sweeper);
        tmp.enable = (u8)((cmd.data0 >> 24) & 0x000000FF);
        tmp.runningMode = (u8)((cmd.data0 >> 16) & 0x000000FF);
        tmp.direction = (u8)((cmd.data0 >> 8) & 0x000000FF);
        tmp.llf = cmd.data1;
        tmp.ulf = cmd.data2;
        tmp.step_freq = cmd.data3;
        tmp.waitingTime = cmd.data4;
        if ((tmp.enable == 1) && (tmp.direction == 0))
        {
            tmp.currentFrequency = tmp.llf;
        }
        else if ((tmp.enable == 1) && (tmp.direction == 1))
        {
            tmp.currentFrequency = tmp.ulf;
        }
        if (cmd.gyroID == GYRO1)
        {
            g1_sweeper = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_sweeper = tmp;
        }
        processFrequencyLock(cmd.gyroID, FRE_STATE_SWEEPER, tmp.enable);
        break;
    }
    case CMD_SM_PLL:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPLLConfig : g2_smPLLConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPLLConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPLLConfig = tmp;
        }
        processFrequencyLock(cmd.gyroID, FRE_STATE_PLL, tmp.loopSource);
        break;
    }

    case CMD_SM_PID_AI:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidAiConfig : g2_smPidAiConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_AI;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidAiConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidAiConfig = tmp;
        }
        break;
    }
    case CMD_SM_PID_AQ:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidAqConfig : g2_smPidAqConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_AQ;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidAqConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidAqConfig = tmp;
        }
        break;
    }
    case CMD_SM_PID_BI:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidBiConfig : g2_smPidBiConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_BI;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidBiConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidBiConfig = tmp;
        }
        break;
    }
    case CMD_SM_PID_BQ:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidBqConfig : g2_smPidBqConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_BQ;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidBqConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidBqConfig = tmp;
        }
        break;
    }
    case CMD_Q_FACTOR:
    {
        responseFlag = 1;
        qFactorConfig_t tmp = (cmd.gyroID == 1 ? g1_qFactor : g2_qFactor);
        tmp.enable = (u8)(cmd.data0 & 0x000000FF);
        tmp.targetLoop = (u8)(cmd.data1 & 0x000000FF);
        tmp.scaleFactorFrequency = cmd.data2;
        tmp.startDelay = cmd.data3;
        if (cmd.gyroID == GYRO1)
        {
            g1_qFactor = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_qFactor = tmp;
        }
        break;
    }

    case CMD_RESONANT_EST:
    {
        responseFlag = 1;
        resonantEstimatorConfig_t tmp = (cmd.gyroID == 1 ? g1_resonantEstimatorConfig : g2_resonantEstimatorConfig);
        tmp.agcTargetLoop = (u8)(cmd.data0 & 0x000000FF);
        if (cmd.gyroID == GYRO1)
        {
            g1_resonantEstimatorConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_resonantEstimatorConfig = tmp;
        }
        break;
    }

    case CMD_PHASE_FINDER:
    {
        responseFlag = 1;
        phaseFinderConfig_t tmp = (cmd.gyroID == 1 ? g1_phaseFinderConfig : g2_phaseFinderConfig);
        tmp.enable = (u8)(cmd.data0 >> 16 & 0x000000FF);
        tmp.agcTargetLoop = (u8)(cmd.data0 & 0x000000FF);
        tmp.setTime = cmd.data1;
        tmp.leftPhase = cmd.data2;
        tmp.rightPhase = cmd.data3;
        tmp.minPhase = cmd.data4;
        tmp.leftAgcAvg = 0;
        tmp.rightAgcAvg = 0;
        tmp.currentWaitingTime = 0;
        if (cmd.gyroID == GYRO1)
        {
            g1_phaseFinderConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_phaseFinderConfig = tmp;
        }
        break;
    }
    case CMD_MODE_SWITCH:
    {
        responseFlag = 1;
        modeSwitchingConfig_t tmp = (cmd.gyroID == 1 ? g1_modeSwitchingConfig : g2_modeSwitchingConfig);
        tmp.enable =  (u8)(cmd.data0 & 0x000000FF);
        tmp.copyMode = (u8)(cmd.data1 & 0x000000FF);
        tmp.switchTime = cmd.data2;
        if (cmd.gyroID == GYRO1)
        {
            g1_modeSwitchingConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_modeSwitchingConfig = tmp;
        }
        break;
    }
    case CMD_WAM_PID_EC:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidEcConfig : g2_smPidEcConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_WAM_ENERGY;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidEcConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidEcConfig = tmp;
        }
        break;
    }
    case CMD_WAM_PID_QC:
    {
        responseFlag = 1;
        loopConfig_t tmp = (cmd.gyroID == 1 ? g1_smPidQcConfig : g2_smPidQcConfig);
        tmp.loopSource = (u8)(cmd.data0 & 0x000000FF);
        tmp.loopTarget = LOOP_PID_WAM_QUADRATURE;
        tmp.loop_setvalue = cmd.data1;
        tmp.loopReferenceValue = cmd.data2;
        tmp.PID_P = cmd.data3;
        tmp.PID_I = cmd.data4;
        tmp.PID_D = cmd.data5;
        tmp.outputLimit = cmd.data6;
        if (cmd.gyroID == GYRO1)
        {
            g1_smPidQcConfig = tmp;
        }
        else if (cmd.gyroID == GYRO2)
        {
            g2_smPidQcConfig = tmp;
        }
        break;
    }
    }
}
cmdPayload_t buildCmdFeedbackPayload(u8 Gyro_ID, u16 type)
{
    cmdPayload_t localCmdPayload;
    localCmdPayload.gyroID = Gyro_ID;
    switch (type)
    {
    case CMD_SYSTEM_RESERVED:
    {
        localCmdPayload.data0 = 0;
        localCmdPayload.data1 = 0;
        localCmdPayload.data2 = 0;
        localCmdPayload.data3 = 0;
        localCmdPayload.data4 = 0;
        localCmdPayload.data5 = 0;
        localCmdPayload.data6 = 0;
        localCmdPayload.data7 = 0;
        break;
    }
    case CMD_OPENLOOP_FREQUENCY:
    {
        openloopConfig_t tmp = (Gyro_ID == 1 ? g1_openloopConfig : g2_openloopConfig);
        localCmdPayload.data0 = tmp.frequency_enable;
        localCmdPayload.data1 = tmp.freq;
        localCmdPayload.data2 = 0;
        localCmdPayload.data3 = 0;
        localCmdPayload.data4 = 0;
        localCmdPayload.data5 = 0;
        localCmdPayload.data6 = 0;
        localCmdPayload.data7 = 0;
        break;
    }
    case CMD_OPENLOOP_ACT:
    {
        openloopConfig_t tmp = (Gyro_ID == 1 ? g1_openloopConfig : g2_openloopConfig);
        localCmdPayload.data0 = tmp.amp_enable;
        localCmdPayload.data1 = tmp.channelAI;
        localCmdPayload.data2 = tmp.channelAQ;
        localCmdPayload.data3 = tmp.channelBI;
        localCmdPayload.data4 = tmp.channelBQ;
        localCmdPayload.data5 = 0;
        localCmdPayload.data6 = 0;
        localCmdPayload.data7 = 0;
        break;
    }
    case CMD_OPENLOOP_HV:
    {
        openloopConfig_t tmp = (Gyro_ID == 1 ? g1_openloopConfig : g2_openloopConfig);
        localCmdPayload.data0 = tmp.hv_enable;
        localCmdPayload.data1 = tmp.hv1;
        localCmdPayload.data2 = tmp.hv2;
        localCmdPayload.data3 = tmp.hv3;
        localCmdPayload.data4 = tmp.hv4;
        localCmdPayload.data5 = 0;
        localCmdPayload.data6 = 0;
        localCmdPayload.data7 = 0;
        break;
    }
    case CMD_OPENLOOP_PHASE:
    {
        openloopConfig_t tmp = (Gyro_ID == 1 ? g1_openloopConfig : g2_openloopConfig);
        localCmdPayload.data0 = tmp.drivePhaseShiftA;
        localCmdPayload.data1 = tmp.drivePhaseShiftB;
        localCmdPayload.data2 = tmp.demodPhaseShiftA;
        localCmdPayload.data3 = tmp.demodPhaseShiftB;
        localCmdPayload.data4 = 0;
        localCmdPayload.data5 = 0;
        localCmdPayload.data6 = 0;
        localCmdPayload.data7 = 0;
        break;
    }
    case CMD_SWEEPER:
    {
        sweeperConfig_t tmp = (Gyro_ID == 1 ? g1_sweeper : g2_sweeper);
        localCmdPayload.data0 = tmp.enable;
        localCmdPayload.data1 = tmp.runningMode;
        localCmdPayload.data2 = tmp.direction;
        localCmdPayload.data3 = tmp.llf;
        localCmdPayload.data4 = tmp.ulf;
        localCmdPayload.data5 = tmp.step_freq;
        localCmdPayload.data6 = tmp.currentFrequency;
        localCmdPayload.data7 = tmp.waitingTime;
        break;
    }
    case CMD_SM_PLL:
    {
        loopConfig_t tmp = (Gyro_ID == 1 ? g1_smPLLConfig : g1_smPLLConfig);
        localCmdPayload.data0 = tmp.loopSource;
        localCmdPayload.data1 = tmp.loopReferenceValue;
        localCmdPayload.data2 = tmp.PID_P;
        localCmdPayload.data3 = tmp.PID_I;
        localCmdPayload.data4 = tmp.PID_D;
        localCmdPayload.data5 = tmp.outputLimit;
        localCmdPayload.data6 = tmp.currentError;
        localCmdPayload.data7 = tmp.loopOutput;
        break;
    }
    }

    responseFlag = 0;
    return localCmdPayload;
}
