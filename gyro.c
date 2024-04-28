/*Origin Author: Hugo*/
#include "gyro.h"
#include "runtime.h"
#include "service.h"
#include <stdlib.h>

// typedef struct{
//     float coeA;
//     float coeB;
//     float prev;
// }lpfConfig_t;

// void initLpf(lpfConfig_t *localCfg){
//     localCfg->coeA = 0.2;
//     localCfg->coeB = 0.8;
//     localCfg->prev = 0;
// }

// float updateLpf(u32 new, lpfConfig_t *localCfg){
//     float out = localCfg->coeA * new + localCfg->coeB * prev;
//     localCfg->prev = out;
//     return out;
// }

// lpfConfig_t estResonanatFreLpf_1;
// lpfConfig_t estResonanatFreLpf_2;
// lpfConfig_t minAgcOutLpf_1;
// lpfConfig_t minAgcOutLpf_2;

void initOpenloopConfig(u8 gyroID, openloopConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->frequency_enable = 0;
    localCfg->freq = (1000 * 4294967295 / 5000000);
    localCfg->amp_enable = 0;
    localCfg->channelAI = 0;
    localCfg->channelAQ = 0;
    localCfg->channelBI = 0;
    localCfg->channelBQ = 0;
    localCfg->hv_enable = 0;
    localCfg->hv1 = 32768;
    localCfg->hv2 = 32768;
    localCfg->hv3 = 32768;
    localCfg->hv4 = 32768;
    localCfg->drivePhaseShiftA = ((0x0000 << 16) | 0x4000);
    localCfg->drivePhaseShiftB = ((0x0000 << 16) | 0x4000);
    localCfg->demodPhaseShiftA = ((0x0000 << 16) | 0x4000);
    localCfg->demodPhaseShiftB = ((0x0000 << 16) | 0x4000);
}

void initSweeperConfig(u8 gyroID, sweeperConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->enable = 0;
    localCfg->runningMode = 0;
    localCfg->direction = 0;
    localCfg->llf = 0;
    localCfg->ulf = 0;
    localCfg->step_freq = 0;
    localCfg->currentFrequency = 0;
    localCfg->waitingTime = 0;
    localCfg->currentWaitingTime = 0;
}

void initPidConfig(u8 gyroID, loopConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->loopSource = 0;
    localCfg->loopTarget = 0;
    localCfg->loop_setvalue = 0;
    localCfg->loopReferenceValue = 0;
    localCfg->PID_P = 0;
    localCfg->PID_I = 0;
    localCfg->PID_D = 0;
    localCfg->currentError = 0;
    localCfg->previousError = 0;
    localCfg->lastError = 0;
    localCfg->loopOutput = 0;
    localCfg->outputLimit = 0;
    localCfg->currentReading = 0;
}

void initQFactorConfig(u8 gyroID, qFactorConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->enable = 0;
    localCfg->targetLoop = 0;
    localCfg->scaleFactorFrequency = 0;
    localCfg->startDelay = 0;
    localCfg->stage = 0;
    localCfg->currentWaitingTime = 0;
    localCfg->startFrequency = 0;
    localCfg->startAmp = 0;
    localCfg->ampBuffer = (u32 *)malloc(100 * sizeof(u32));
    memset(localCfg->ampBuffer, 0, 100 * sizeof(u32));
    localCfg->freBuffer = (u32 *)malloc(100 * sizeof(u32));
    memset(localCfg->freBuffer, 0, 100 * sizeof(u32));
    localCfg->startTimeStamp = 0;
    localCfg->endTimeStamp_1 = 0;
    localCfg->endTimeStamp_2 = 0;
}

void initResonantEstimator(u8 gyroID, resonantEstimatorConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->agcTargetLoop = 0;
    localCfg->bufferLen = 100;

    localCfg->agcOutputBuffer = (u32 *)malloc(((localCfg->bufferLen) * sizeof(u32)));
    memset(localCfg->agcOutputBuffer, 0, ((localCfg->bufferLen) * sizeof(u32)));

    //    localCfg->agcOutputAvgBuf = (u32 *)malloc(((localCfg->freBufferLenth) * sizeof(u32)));
    //    memset(localCfg->agcOutputAvgBuf, 0, ((localCfg->freBufferLenth) * sizeof(u32)));

    localCfg->frewordBuffer = (u32 *)malloc(((localCfg->bufferLen) * sizeof(u32)));
    memset(localCfg->frewordBuffer, 0, ((localCfg->bufferLen) * sizeof(u32)));

    //    localCfg->frewordAvgBuf = (u32 *)malloc(((localCfg->freBufferLenth) * sizeof(u32)));
    //    memset(localCfg->frewordAvgBuf, 0, ((localCfg->freBufferLenth) * sizeof(u32)));
    localCfg->prevAgc = 0;
    localCfg->prevFre = 0;
}

void initPhaseFinder(u8 gyroID, phaseFinderConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->enable = 0;
    localCfg->agcTargetLoop = 0;
    localCfg->stage = 0;
    localCfg->setTime = 1000;
    localCfg->currentWaitingTime = 0;
    localCfg->bufferLen = 100;

    localCfg->agcOutputBuffer = (s32 *)malloc(((localCfg->bufferLen) * sizeof(s32)));
    memset(localCfg->agcOutputBuffer, 0, ((localCfg->bufferLen) * sizeof(s32)));

    localCfg->leftPhase = 0;
    localCfg->leftAgcAvg = 0;
    localCfg->rightPhase = 0;
    localCfg->rightAgcAvg = 0;
    localCfg->midPhase = 0;
    localCfg->midAgcAvg = 0;
    localCfg->minPhase = 0;
}

void initModeSwitching(u8 gyroID, modeSwitchingConfig_t *localCfg)
{
    localCfg->gyroid = gyroID;
    localCfg->enable = 0;
    localCfg->copyMode = 0;
    localCfg->switchTime = 0;
    localCfg->currentTime = 0;
    localCfg->currentState = 0;
}

// Basic Functions
u32 calcAverage(u32 *data, u32 len)
{
    float result = 0.0;
    for (int i = 0; i < len; i++)
    {
        result = result + (data[i] / len);
    }
    return (u32)result;
}
int swapDirection(int direction)
{
    // called in sweeper()
    if (direction == 1)
        return -1;
    else if (direction == -1)
        return 1;
    return 0;
}
u32 getMin(u32 *buffer)
{
    // called in calculateQFactor()
    u32 min = buffer[0];
    for (int i = 0; i < sizeof(buffer); i++)
    {
        if (min >= buffer[i])
        {
            min = buffer[i];
        }
    }
    return min;
}
u32 getMax(u32 *buffer)
{
    // called in calculateQFactor()
    u32 max = buffer[0];
    for (int i = 0; i < sizeof(buffer); i++)
    {
        if (max <= buffer[i])
        {
            max = buffer[i];
        }
    }
    return max;
}
u32 getMinIndex(s32 *buffer, u32 len)
{
    // called in calculateQFactor()
    s32 min = buffer[0];
    u32 minIndex = 0;
    for (u32 i = 0; i < len; i++)
    {
        if (min >= buffer[i])
        {
            min = buffer[i];
            minIndex = i;
        }
    }
    return minIndex;
}
void moveBehind32(u32 *target, u32 len)
{

    for (u32 i = len - 1; i >= 1; i--)
    {
        target[i] = target[i - 1];
    }
}

// PID loop, applied on AI AQ BI BQ control
void abstractLoop(loopConfig_t *localCfg)
{
    if (localCfg->loopSource != LOOP_SOURCE_NONE &&
        (localCfg->PID_P != 0 || localCfg->PID_I != 0 || localCfg->PID_D != 0))
    {
        // read PID value
        float fP = (localCfg->PID_P) / 100000.0;
        float fI = (localCfg->PID_I) / 100000.0;
        float fD = (localCfg->PID_D) / 100000.0;
        // read value
        if (localCfg->loopSource == LOOP_SOURCE_AMP)
        {
            switch (localCfg->loopTarget)
            {
            case LOOP_PID_AI:
            case LOOP_PID_AQ:
            {
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA);
                break;
            }
            case LOOP_PID_BI:
            case LOOP_PID_BQ:
            {
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB);
                break;
            }
            default:
                break;
            }
            // calcu the error
            localCfg->currentError = localCfg->loop_setvalue - localCfg->currentReading;
        }
        else if (localCfg->loopSource == LOOP_SOURCE_COMPONENT)
        {
            switch (localCfg->loopTarget)
            {
            case LOOP_PID_AI:
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA_I);
                break;
            case LOOP_PID_AQ:
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA_Q);
                break;
            case LOOP_PID_BI:
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB_I);
                break;
            case LOOP_PID_BQ:
                localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB_Q);
                break;
            default:
                break;
            }
            localCfg->currentError = localCfg->currentReading - localCfg->loop_setvalue;
        }

        // PID calculation
        s32 out = localCfg->loopOutput +
                  (fP * (localCfg->currentError - localCfg->previousError) + fI * localCfg->currentError + fD * (localCfg->currentError - 2 * localCfg->previousError + localCfg->lastError)) / 1000;

        // save errors
        localCfg->lastError = localCfg->previousError;
        localCfg->previousError = localCfg->currentError;

        // output regulation
        s32 ula = localCfg->loopReferenceValue + localCfg->outputLimit; // upper limit amp
        s32 lla = localCfg->loopReferenceValue - localCfg->outputLimit; // lower limit amp
        if (out <= ula && out >= lla)
        {
            localCfg->loopOutput = out;
        }
        else if (out > ula)
        {
            localCfg->loopOutput = ula;
        }
        else if (out < lla)
        {
            localCfg->loopOutput = lla;
        }

        // apply the new excite amp
        switch (localCfg->loopTarget)
        {
        case LOOP_PID_AI:
        {
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AI_CURRENT_ERROR, localCfg->currentError);
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT, localCfg->loopOutput);
            setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHA_I, localCfg->loopOutput);
            break;
        }
        case LOOP_PID_AQ:
        {
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AQ_CURRENT_ERROR, localCfg->currentError);
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT, localCfg->loopOutput);
            setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHA_Q, localCfg->loopOutput);
            break;
        }
        case LOOP_PID_BI:
        {
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BI_CURRENT_ERROR, localCfg->currentError);
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT, localCfg->loopOutput);
            setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHB_I, localCfg->loopOutput);
            break;
        }
        case LOOP_PID_BQ:
        {
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BQ_CURRENT_ERROR, localCfg->currentError);
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT, localCfg->loopOutput);
            setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHB_Q, localCfg->loopOutput);
            break;
        }
        default:
            break;
        }
    }
}

void phaseLockLoop(loopConfig_t *localCfg)
{
    if (localCfg->loopSource != LOOP_SOURCE_NONE &&
        (localCfg->PID_P != 0 || localCfg->PID_I != 0 || localCfg->PID_D != 0))
    {
        float fP = (localCfg->PID_P) / 100000.0;
        float fI = (localCfg->PID_I) / 100000.0;
        float fD = (localCfg->PID_D) / 100000.0;

        // read and calc errors
        // localCfg->currentReading = read_demod_phase(Gyro_ID, localCfg->loopSource);
        if (localCfg->loopSource == LOOP_SOURCE_PHASE_A)            //  锁A模态的相位
        {
            localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASE_A);    // 拿到运行时A模态的相位
        }
        else if (localCfg->loopSource == LOOP_SOURCE_PHASE_B)
        {
            localCfg->currentReading = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASE_B);
        }
        // PID误差项 e = 运行时的相位 - 上位机设定值
        localCfg->currentError = localCfg->currentReading - localCfg->loop_setvalue;

        // PID输出 u
        u32 out = localCfg->loopOutput +
                  (fP * (localCfg->currentError - localCfg->previousError) +
                   fI * localCfg->currentError +
                   fD * (localCfg->currentError - 2 * localCfg->previousError + localCfg->lastError)) /
                      1000000;

        // save errors
        localCfg->lastError = localCfg->previousError;
        localCfg->previousError = localCfg->currentError;

        // output regulation
        u32 ulf = localCfg->loopReferenceValue + localCfg->outputLimit; // upper limit frequency
        u32 llf = localCfg->loopReferenceValue - localCfg->outputLimit; // lower limit frequency
        if (out <= ulf && out >= llf)
        {
            localCfg->loopOutput = out;
        }
        else if (out > ulf)
        {
            localCfg->loopOutput = ulf;
        }
        else if (out < llf)
        {
            localCfg->loopOutput = llf;
        }

        // apply the new frequency word
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_FREQUENCY, localCfg->loopOutput);

        // modify runtime data in runtime data structure
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_CURRENT_FREQUENCY, localCfg->loopOutput);
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_PLL_CURRENT_ERROR, localCfg->currentError);
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_PLL_CURRENT_PHASE, localCfg->currentReading);
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_PLL_CURRENT_SET_PHASE, localCfg->loop_setvalue);
    }
    // return localCfg;
}

void openLoop(openloopConfig_t *localCfg, u8 field)
{
    switch (field)
    {
    case OP_FRE:
    {
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_FREQUENCY, localCfg->freq);
        break;
    }
    case OP_AMP:
    {
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHA_I, localCfg->channelAI);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHA_Q, localCfg->channelAQ);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHB_I, localCfg->channelBI);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_CHB_Q, localCfg->channelBQ);
        break;
    }
    case OP_HV:
    {
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_HV_1, localCfg->hv1);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_HV_2, localCfg->hv2);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_HV_3, localCfg->hv3);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_HV_4, localCfg->hv4);
        break;
    }
    case OP_PHASE:
    {
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_PHASE_A, localCfg->drivePhaseShiftA);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DRIVE_PHASE_B, localCfg->drivePhaseShiftB);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DEMOD_PHASE_A, localCfg->demodPhaseShiftA);
        setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_DEMOD_PHASE_B, localCfg->demodPhaseShiftB);
        break;
    }
    default:
        break;
    }
}

void sweeper(sweeperConfig_t *localCfg)
{
    localCfg->currentWaitingTime++;
    if (localCfg->currentWaitingTime >= localCfg->waitingTime)
    {
        localCfg->currentWaitingTime = 0;
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_CURRENT_FREQUENCY, localCfg->currentFrequency);
        // check if we are at the end of the sweeper event
        if (localCfg->currentFrequency <= localCfg->ulf && localCfg->currentFrequency >= localCfg->llf)
        {
            if (localCfg->direction == 0)
                localCfg->currentFrequency = localCfg->currentFrequency + localCfg->step_freq;
            else
                localCfg->currentFrequency = localCfg->currentFrequency - localCfg->step_freq;
            setGyroOutput(localCfg->gyroid, OUTPUT_FIELD_FREQUENCY, localCfg->currentFrequency);
        }
        else
        {
            if (localCfg->runningMode == 0)
                localCfg->enable = 0;
            else
            {
                if (localCfg->direction == 0)
                    localCfg->currentFrequency = localCfg->llf;
                else
                    localCfg->currentFrequency = localCfg->ulf;
            }
        }
    }
}

void calculateQFactor(qFactorConfig_t *localCfg)
{
    if ((localCfg->enable != 0) && (localCfg->targetLoop != 0))
    {
        localCfg->currentWaitingTime++;
        memmove(&localCfg->freBuffer[1], &localCfg->freBuffer[0], (100 - 1) * sizeof(u32));
        localCfg->freBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_CURRENT_FREQUENCY);

        if (localCfg->currentWaitingTime >= localCfg->startDelay + 101) // 101 is force delay 101ms
        {
            if (localCfg->stage == 0)
            {
                // stage 0: init stage
                // firstly init some start up data
                // localCfg->startFrequency = getGyroData(localCfg->gyroid, RUNTIME_FIELD_CURRENT_FREQUENCY);
                localCfg->startFrequency = calcAverage(localCfg->freBuffer, 100);
                switch (localCfg->targetLoop)
                {
                case LOOP_PID_AI:
                case LOOP_PID_AQ:
                {

                    localCfg->startAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA);
                    break;
                }
                case LOOP_PID_BI:
                case LOOP_PID_BQ:
                {
                    localCfg->startAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB);
                    break;
                }
                default:
                    break;
                }
                for (int i = 0; i < 100; i++)
                {
                    localCfg->ampBuffer[i] = localCfg->startAmp;
                }
                localCfg->startTimeStamp = getSystemTime();

                // now we need to disable pll and pids
                if (localCfg->gyroid == GYRO1)
                {
                    // firstly stash all config
                    g1_qfOpenloopConfig = g1_openloopConfig;
                    g1_qfPLLConfig = g1_smPLLConfig;
                    g1_qfPidAiConfig = g1_smPidAiConfig;
                    g1_qfPidAqConfig = g1_smPidAqConfig;
                    g1_qfPidBiConfig = g1_smPidBiConfig;
                    g1_qfPidBqConfig = g1_smPidBqConfig;
                    // then we can disable all of them
                    g1_openloopConfig.channelAI = 0;
                    g1_openloopConfig.channelAQ = 0;
                    g1_openloopConfig.channelBI = 0;
                    g1_openloopConfig.channelBQ = 0;
                    g1_smPLLConfig.loopSource = 0;
                    g1_smPidAiConfig.loopSource = 0;
                    g1_smPidAqConfig.loopSource = 0;
                    g1_smPidBiConfig.loopSource = 0;
                    g1_smPidBqConfig.loopSource = 0;
                }
                else if (localCfg->gyroid == GYRO2)
                {
                    // firstly stash all config
                    g2_qfOpenloopConfig = g2_openloopConfig;
                    g2_qfPLLConfig = g2_smPLLConfig;
                    g2_qfPidAiConfig = g2_smPidAiConfig;
                    g2_qfPidAqConfig = g2_smPidAqConfig;
                    g2_qfPidBiConfig = g2_smPidBiConfig;
                    g2_qfPidBqConfig = g2_smPidBqConfig;
                    // then we can disable all of them
                    g2_openloopConfig.channelAI = 0;
                    g2_openloopConfig.channelAQ = 0;
                    g2_openloopConfig.channelBI = 0;
                    g2_openloopConfig.channelBQ = 0;
                    g2_smPLLConfig.loopSource = 0;
                    g2_smPidAiConfig.loopSource = 0;
                    g2_smPidAqConfig.loopSource = 0;
                    g2_smPidBiConfig.loopSource = 0;
                    g2_smPidBqConfig.loopSource = 0;
                }
                localCfg->stage = 1;
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_START_FREQUENCY, localCfg->startFrequency);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_SHORT_TAU, 0);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_LONG_TAU, 0);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_MIN, 0);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_MAX, 0);
            }
            else if (localCfg->stage == 1)
            {
                // stage 1: buffering stage, get the min amp from the buffer
                u32 tmpAmp = 0;
                switch (localCfg->targetLoop)
                {
                case LOOP_PID_AI:
                case LOOP_PID_AQ:
                {

                    tmpAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA);
                    break;
                }
                case LOOP_PID_BI:
                case LOOP_PID_BQ:
                {
                    tmpAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB);
                    break;
                }
                default:
                {
                    break;
                }
                }
                memmove(&localCfg->ampBuffer[1], &localCfg->ampBuffer[0], (100 - 1) * sizeof(u32));
                localCfg->ampBuffer[0] = tmpAmp;
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_SHORT_TAU, getSystemTime() - localCfg->startTimeStamp);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_LONG_TAU, getSystemTime() - localCfg->startTimeStamp);
                if (getMin(localCfg->ampBuffer) < 0.3678794411714 * localCfg->startAmp)
                {
                    localCfg->endTimeStamp_1 = getSystemTime();
                    localCfg->stage = 2;
                }
            }
            else if (localCfg->stage == 2)
            {
                // stage 2: buffering stage, get the max amp from the buffer
                u32 tmpAmp = 0;
                switch (localCfg->targetLoop)
                {
                case LOOP_PID_AI:
                case LOOP_PID_AQ:
                {

                    tmpAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHA);
                    break;
                }
                case LOOP_PID_BI:
                case LOOP_PID_BQ:
                {
                    tmpAmp = getGyroData(localCfg->gyroid, RUNTIME_FIELD_AMP_CHB);
                    break;
                }
                default:
                {
                    break;
                }
                }
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_LONG_TAU, getSystemTime() - localCfg->startTimeStamp);
                memmove(&localCfg->ampBuffer[1], &localCfg->ampBuffer[0], (100 - 1) * sizeof(u32));
                localCfg->ampBuffer[0] = tmpAmp;
                if (getMax(localCfg->ampBuffer) < 0.3678794411714 * localCfg->startAmp)
                {
                    localCfg->endTimeStamp_2 = getSystemTime();
                    localCfg->stage = 3;
                }
            }
            else if (localCfg->stage == 3)
            {
                float shortTau = (float)((localCfg->endTimeStamp_1 - localCfg->startTimeStamp) / 1000.0);
                float longTau = (float)((localCfg->endTimeStamp_2 - localCfg->startTimeStamp) / 1000.0);
                float realFreq = (float)(localCfg->startFrequency / localCfg->scaleFactorFrequency);
                float QFactor_1 = realFreq * 3.14159265359 * shortTau;
                float QFactor_2 = realFreq * 3.14159265359 * longTau;
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_MIN, (u32)QFactor_1);
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_QFACTOR_MAX, (u32)QFactor_2);
                // load back all config
                if (localCfg->gyroid == GYRO1)
                {
                    // firstly stash all config
                    g1_openloopConfig = g1_qfOpenloopConfig;
                    g1_smPLLConfig = g1_qfPLLConfig;
                    g1_smPidAiConfig = g1_qfPidAiConfig;
                    g1_smPidAqConfig = g1_qfPidAqConfig;
                    g1_smPidBiConfig = g1_qfPidBiConfig;
                    g1_smPidBqConfig = g1_qfPidBqConfig;
                }
                else if (localCfg->gyroid == GYRO2)
                {
                    // firstly stash all config
                    g2_openloopConfig = g2_qfOpenloopConfig;
                    g2_smPLLConfig = g2_qfPLLConfig;
                    g2_smPidAiConfig = g2_qfPidAiConfig;
                    g2_smPidAqConfig = g2_qfPidAqConfig;
                    g2_smPidBiConfig = g2_qfPidBiConfig;
                    g2_smPidBqConfig = g2_qfPidBqConfig;
                }
                // reset qFactorConfig to exit
                initQFactorConfig(localCfg->gyroid, localCfg);
            }
        }
    }
}

void resonantEstimator(resonantEstimatorConfig_t *localCfg)
{
    u8 gyroid = localCfg->gyroid;
    loopConfig_t localPllConfig = (gyroid == GYRO1 ? g1_smPLLConfig : g2_smPLLConfig);
    if ((localPllConfig.loopSource != 0) && (localCfg->agcTargetLoop != 0))
    {
        // step 1: buffering frequency word
        moveBehind32(localCfg->frewordBuffer, localCfg->bufferLen);
        // memmove(&localCfg->frewordBuffer[1], &localCfg->frewordBuffer[0], (localCfg->freBufferLenth - 1) * sizeof(u32));
        localCfg->frewordBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_CURRENT_FREQUENCY);

        // step 2: buffering AGC (a PID) output
        moveBehind32(localCfg->agcOutputBuffer, localCfg->bufferLen);
        // memmove(&localCfg->agcOutputBuffer[1], &localCfg->agcOutputBuffer[0], (localCfg->agcBufferLenth - 1) * sizeof(u32));
        u32 tmpOutput = 0;
        switch (localCfg->agcTargetLoop)
        {
        case LOOP_PID_AI:
        {
            tmpOutput = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT);
            break;
        }
        case LOOP_PID_AQ:
        {
            tmpOutput = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT);
            break;
        }
        case LOOP_PID_BI:
        {
            tmpOutput = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT);
            break;
        }
        case LOOP_PID_BQ:
        {
            tmpOutput = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT);
            break;
        }
        }
        localCfg->agcOutputBuffer[0] = tmpOutput;

        // for (int i = 0; i < localCfg->bufferLen; i++){
        //     if (localCfg->agcOutputBuffer[i]<0){
        //         localCfg->agcOutputBuffer[i] = - localCfg->agcOutputBuffer[i];
        //     }
        // }

        // step 3: get the index of the min value of the agcbuffer
        u32 index = getMinIndex(localCfg->agcOutputBuffer, localCfg->bufferLen);

        // step 4: calc average value
        // moveBehind32(localCfg->frewordAvgBuf, localCfg->bufferLen);
        // localCfg->frewordAvgBuf[0] = localCfg->frewordBuffer[index];
        // u32 avgFre = calcAverage(localCfg->frewordAvgBuf, localCfg->bufferLen);

        // moveBehind32(localCfg->agcOutputAvgBuf, localCfg->bufferLen);
        // localCfg->agcOutputAvgBuf[0] = localCfg->agcOutputBuffer[index];
        // u32 avgAgc = calcAverage(localCfg->agcOutputAvgBuf, localCfg->bufferLen);

        // setGyroData(localCfg->gyroid, RUNTIME_FIELD_MIN_AGC_OUTPUT, avgFre);
        // setGyroData(localCfg->gyroid, RUNTIME_FIELD_EST_RESONANT_FREQUENCY, avgFre);

        // localCfg->prevAgc = 0.9 * localCfg->prevAgc + 0.1 * localCfg->agcOutputBuffer[index];
        // setGyroData(localCfg->gyroid, RUNTIME_FIELD_MIN_AGC_OUTPUT, localCfg->prevAgc);

        setGyroData(localCfg->gyroid, RUNTIME_FIELD_MIN_AGC_OUTPUT, localCfg->agcOutputBuffer[index]);

        localCfg->prevFre = 0.99 * localCfg->prevFre + 0.01 * localCfg->frewordBuffer[index];
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_EST_RESONANT_FREQUENCY, localCfg->prevFre);

        if (localCfg->frewordBuffer[index] < 0x00000100)
        {
            u32 temp = 1;
            temp += 1;
        }
    }
}

void phaseFinder(phaseFinderConfig_t *localCfg)
{
    u8 gyroid = localCfg->gyroid;
    loopConfig_t localPllConfig = (gyroid == GYRO1 ? g1_smPLLConfig : g2_smPLLConfig);
    if ((localCfg->enable == 1) && (localCfg->agcTargetLoop != 0) && (localPllConfig.loopSource != 0))
    {
        localCfg->currentWaitingTime++;
        setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 1);
        if ((localCfg->currentWaitingTime >= localCfg->setTime) && (localCfg->currentWaitingTime < localCfg->setTime + localCfg->bufferLen))
        {
            memmove(&localCfg->agcOutputBuffer[1], &localCfg->agcOutputBuffer[0], (100 - 1) * sizeof(u32));
            switch (localCfg->agcTargetLoop)
            {
            case LOOP_PID_AI:
            {
                localCfg->agcOutputBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT);
                break;
            }
            case LOOP_PID_AQ:
            {
                localCfg->agcOutputBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT);
                break;
            }
            case LOOP_PID_BI:
            {
                localCfg->agcOutputBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT);
                break;
            }
            case LOOP_PID_BQ:
            {
                localCfg->agcOutputBuffer[0] = getGyroData(localCfg->gyroid, RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT);
                break;
            }
            }
        }
        else if (localCfg->currentWaitingTime >= localCfg->setTime + localCfg->bufferLen)
        {
            localCfg->currentWaitingTime = 0;
            localCfg->midAgcAvg = calcAverage(localCfg->agcOutputBuffer, localCfg->bufferLen);
            // TODO: set mid avg into runtime
            // setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 1);
            setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_AGCAVG, localCfg->midAgcAvg);
            // binary search stage
            if ((localCfg->leftAgcAvg == 0) && (localCfg->rightAgcAvg == 0))
            {
                localCfg->leftAgcAvg = localCfg->midAgcAvg;
                localPllConfig.loop_setvalue = localCfg->rightPhase;
                setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 2);
                if (gyroid == GYRO1)
                {
                    g1_smPLLConfig = localPllConfig;
                }
                else if (gyroid == GYRO2)
                {
                    g2_smPLLConfig = localPllConfig;
                }
            }
            else
            {
                if ((localCfg->leftAgcAvg != 0) && (localCfg->rightAgcAvg == 0))
                {
                    localCfg->rightAgcAvg = localCfg->midAgcAvg;
                    setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 3);
                }
                else if ((localCfg->leftAgcAvg == 0) && (localCfg->rightAgcAvg != 0))
                {
                    localCfg->leftAgcAvg = localCfg->midAgcAvg;
                    setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 4);
                }

                if (localCfg->leftPhase - localCfg->rightPhase <= localCfg->minPhase)
                {
                    localCfg->enable = 0; // stop
                    setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 0);
                }
                else if (localCfg->leftAgcAvg > localCfg->rightAgcAvg)
                {
                    localCfg->leftPhase = (localCfg->leftPhase + localCfg->rightPhase) / 2 + 1;
                    localCfg->leftAgcAvg = 0;
                    localPllConfig.loop_setvalue = localCfg->leftPhase;
                    if (gyroid == GYRO1)
                    {
                        g1_smPLLConfig = localPllConfig;
                    }
                    else if (gyroid == GYRO2)
                    {
                        g2_smPLLConfig = localPllConfig;
                    }
                    setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 5);
                }
                else if (localCfg->leftAgcAvg < localCfg->rightAgcAvg)
                {
                    localCfg->rightPhase = (localCfg->leftPhase + localCfg->rightPhase) / 2 - 1;
                    localCfg->rightAgcAvg = 0;
                    localPllConfig.loop_setvalue = localCfg->rightPhase;
                    if (gyroid == GYRO1)
                    {
                        g1_smPLLConfig = localPllConfig;
                    }
                    else if (gyroid == GYRO2)
                    {
                        g2_smPLLConfig = localPllConfig;
                    }
                    setGyroData(localCfg->gyroid, RUNTIME_FIELD_PHASEFINDER_DEBUG, 6);
                }
            }
        }
    }
}

void modeSwitching(modeSwitchingConfig_t *localCfg)
{
    if (localCfg->copyMode == 1)
    {
        localCfg->copyMode = 0;
        if (localCfg->gyroid == GYRO1)
        {
            copy1_g1_openloopConfig = g1_openloopConfig;
            copy1_g1_sweeper = g1_sweeper;
            copy1_g1_smPLLConfig = g1_smPLLConfig;
            copy1_g1_smPidAiConfig = g1_smPidAiConfig;
            copy1_g1_smPidAqConfig = g1_smPidAqConfig;
            copy1_g1_smPidBiConfig = g1_smPidBiConfig;
            copy1_g1_smPidBqConfig = g1_smPidBqConfig;
        }
        else if (localCfg->gyroid == GYRO2)
        {
            copy1_g2_openloopConfig = g2_openloopConfig;
            copy1_g2_sweeper = g2_sweeper;
            copy1_g2_smPLLConfig = g2_smPLLConfig;
            copy1_g2_smPidAiConfig = g2_smPidAiConfig;
            copy1_g2_smPidAqConfig = g2_smPidAqConfig;
            copy1_g2_smPidBiConfig = g2_smPidBiConfig;
            copy1_g2_smPidBqConfig = g2_smPidBqConfig;
        }
    }
    else if (localCfg->copyMode == 2)
    {
        localCfg->copyMode = 0;
        if (localCfg->gyroid == GYRO1)
        {
            copy2_g1_openloopConfig = g1_openloopConfig;
            copy2_g1_sweeper = g1_sweeper;
            copy2_g1_smPLLConfig = g1_smPLLConfig;
            copy2_g1_smPidAiConfig = g1_smPidAiConfig;
            copy2_g1_smPidAqConfig = g1_smPidAqConfig;
            copy2_g1_smPidBiConfig = g1_smPidBiConfig;
            copy2_g1_smPidBqConfig = g1_smPidBqConfig;
        }
        else if (localCfg->gyroid == GYRO2)
        {
            copy2_g2_openloopConfig = g2_openloopConfig;
            copy2_g2_sweeper = g2_sweeper;
            copy2_g2_smPLLConfig = g2_smPLLConfig;
            copy2_g2_smPidAiConfig = g2_smPidAiConfig;
            copy2_g2_smPidAqConfig = g2_smPidAqConfig;
            copy2_g2_smPidBiConfig = g2_smPidBiConfig;
            copy2_g2_smPidBqConfig = g2_smPidBqConfig;
        }
    }
    if (localCfg->enable == 1)
    {
        localCfg->currentTime++;
        if (localCfg->currentTime >= localCfg->switchTime)
        {
            localCfg->currentTime = 0;
            // we should now switch profile
            if (localCfg->currentState == 0)
            {
                // X - > Y
                localCfg->currentState = 1;
                if (localCfg->gyroid == GYRO1)
                {
                    g1_openloopConfig = copy1_g1_openloopConfig;
                    g1_sweeper = copy1_g1_sweeper;
                    g1_smPLLConfig = copy1_g1_smPLLConfig;
                    g1_smPidAiConfig = copy1_g1_smPidAiConfig;
                    g1_smPidAqConfig = copy1_g1_smPidAqConfig;
                    g1_smPidBiConfig = copy1_g1_smPidBiConfig;
                    g1_smPidBqConfig = copy1_g1_smPidBqConfig;
                }
                else if (localCfg->gyroid == GYRO2)
                {
                    g2_openloopConfig = copy1_g2_openloopConfig;
                    g2_sweeper = copy1_g2_sweeper;
                    g2_smPLLConfig = copy1_g2_smPLLConfig;
                    g2_smPidAiConfig = copy1_g2_smPidAiConfig;
                    g2_smPidAqConfig = copy1_g2_smPidAqConfig;
                    g2_smPidBiConfig = copy1_g2_smPidBiConfig;
                    g2_smPidBqConfig = copy1_g2_smPidBqConfig;
                }
            }
            else if (localCfg->currentState == 1)
            {
                // Y - > X
                localCfg->currentState = 0;
                if (localCfg->gyroid == GYRO1)
                {
                    g1_openloopConfig = copy2_g1_openloopConfig;
                    g1_sweeper = copy2_g1_sweeper;
                    g1_smPLLConfig = copy2_g1_smPLLConfig;
                    g1_smPidAiConfig = copy2_g1_smPidAiConfig;
                    g1_smPidAqConfig = copy2_g1_smPidAqConfig;
                    g1_smPidBiConfig = copy2_g1_smPidBiConfig;
                    g1_smPidBqConfig = copy2_g1_smPidBqConfig;
                }
                else if (localCfg->gyroid == GYRO2)
                {
                    g2_openloopConfig = copy2_g2_openloopConfig;
                    g2_sweeper = copy2_g2_sweeper;
                    g2_smPLLConfig = copy2_g2_smPLLConfig;
                    g2_smPidAiConfig = copy2_g2_smPidAiConfig;
                    g2_smPidAqConfig = copy2_g2_smPidAqConfig;
                    g2_smPidBiConfig = copy2_g2_smPidBiConfig;
                    g2_smPidBqConfig = copy2_g2_smPidBqConfig;
                }
            }
        }
    }
}

//void parameterCalculation()
