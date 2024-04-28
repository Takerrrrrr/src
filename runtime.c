/*Origin Author: Hugo*/
#include "runtime.h"
#include "service.h"
#include "gyro.h"

// Gyro runtime data
void initGyroData(u8 Gyro_ID, gyroDataRuntime_t *localData)
{
    localData->gyroid = Gyro_ID;
    localData->amp_cha_i = 0;
    localData->amp_cha_q = 0;
    localData->amp_cha = 0;
    localData->amp_chb_i = 0;
    localData->amp_chb_q = 0;
    localData->amp_chb = 0;
    localData->phase_a = 0;
    localData->phase_b = 0;
    localData->currentFrequency = 0;
    localData->amp_max_a = 0;
    localData->fre_when_amp_max_a = 0;
    localData->phase_when_amp_max_a = 0;
    localData->amp_max_b = 0;
    localData->fre_when_amp_max_b = 0;
    localData->phase_when_amp_max_b = 0;
    localData->pllCurrentError = 0;
    localData->pllCurrentPhase = 0;
    localData->pidAICurrentError = 0;
    localData->pidAICurrentOutput = 0;
    localData->pidAQCurrentError = 0;
    localData->pidAQCurrentOutput = 0;
    localData->pidBICurrentError = 0;
    localData->pidBICurrentOutput = 0;
    localData->pidBQCurrentError = 0;
    localData->pidBQCurrentOutput = 0;
    localData->startFrequency = 0;
    localData->shortTau = 0;
    localData->longTau = 0;
    localData->qFactorMin = 0;
    localData->qFactorMax = 0;
    localData->agcAvg = 0;
    localData->debug = 0;
}

void readGyroData(u8 Gyro_ID)
{
    gyroDataRuntime_t localData = (Gyro_ID == GYRO1 ? gyroData_1 : gyroData_2);   // temporary data

    localData.amp_cha_i = read_demod_IQ(Gyro_ID, CHA, COMPONENTI);
    localData.amp_cha_q = read_demod_IQ(Gyro_ID, CHA, COMPONENTQ);
    localData.amp_cha = read_demod_amp(Gyro_ID, CHA);
    localData.amp_chb_i = read_demod_IQ(Gyro_ID, CHB, COMPONENTI);
    localData.amp_chb_q = read_demod_IQ(Gyro_ID, CHB, COMPONENTQ);
    localData.amp_chb = read_demod_amp(Gyro_ID, CHB);
    localData.phase_a = read_demod_phase(Gyro_ID, CHA);
    localData.phase_b = read_demod_phase(Gyro_ID, CHB);

    if (Gyro_ID == GYRO1)
        gyroData_1 = localData;
    else if (Gyro_ID == GYRO2)
        gyroData_2 = localData;
}

void setGyroData(u8 Gyro_ID, runtimeData_e field, u32 data)
{
    gyroDataRuntime_t localData = (Gyro_ID == GYRO1 ? gyroData_1 : gyroData_2);

    // START: load data into runtime
    switch (field)
    {
    case RUNTIME_FIELD_CURRENT_FREQUENCY:
    {
        localData.currentFrequency = data;
        break;
    }
    case RUNTIME_FIELD_EST_RESONANT_FREQUENCY:
    {
        localData.estResonantFrequency = data;
        break;
    }
    case RUNTIME_FIELD_MIN_AGC_OUTPUT:
    {
        localData.minAgcOutput = data;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_ERROR:
    {
        localData.pllCurrentError = data;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_PHASE:
    {
        localData.pllCurrentPhase = data;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_SET_PHASE:
    {
        localData.pllCurrentSetPhase = data;
        break;
    }
    case RUNTIME_FIELD_PID_AI_CURRENT_ERROR:
    {
        localData.pidAICurrentError = data;
        break;
    }
    case RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT:
    {
        localData.pidAICurrentOutput = data;
        break;
    }
    case RUNTIME_FIELD_PID_AQ_CURRENT_ERROR:
    {
        localData.pidAQCurrentError = data;
        break;
    }
    case RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT:
    {
        localData.pidAQCurrentOutput = data;
        break;
    }
    case RUNTIME_FIELD_PID_BI_CURRENT_ERROR:
    {
        localData.pidBICurrentError = data;
        break;
    }
    case RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT:
    {
        localData.pidBICurrentOutput = data;
        break;
    }
    case RUNTIME_FIELD_PID_BQ_CURRENT_ERROR:
    {
        localData.pidBQCurrentError = data;
        break;
    }
    case RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT:
    {
        localData.pidBQCurrentOutput = data;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_START_FREQUENCY:
    {
        localData.startFrequency = data;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_SHORT_TAU:
    {
        localData.shortTau = data;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_LONG_TAU:
    {
        localData.longTau = data;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_MIN:
    {
        localData.qFactorMin = data;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_MAX:
    {
        localData.qFactorMax = data;
        break;
    }
    case RUNTIME_FIELD_PHASEFINDER_AGCAVG:
    {
        localData.agcAvg = data;
        break;
    }
    case RUNTIME_FIELD_PHASEFINDER_DEBUG:
    {
        localData.debug = data;
        break;
    }

    default:
        break;
    }

    if (Gyro_ID == GYRO1)
        gyroData_1 = localData;
    else if (Gyro_ID == GYRO2)
        gyroData_2 = localData;
};
u32 getGyroData(u8 Gyro_ID, runtimeData_e field)
{
    u32 value = 0;
    gyroDataRuntime_t localData = (Gyro_ID == GYRO1 ? gyroData_1 : gyroData_2);
    //    if (Gyro_ID == GYRO1)
    //        localData = gyroData_1;
    //    else if (Gyro_ID == GYRO2)
    //        localData = gyroData_2;

    switch (field)
    {
    case RUNTIME_FIELD_AMP_CHA_I:
    {
        value = localData.amp_cha_i;
        break;
    }
    case RUNTIME_FIELD_AMP_CHA_Q:
    {
        value = localData.amp_cha_q;
        break;
    }
    case RUNTIME_FIELD_AMP_CHA:
    {
        value = localData.amp_cha;
        break;
    }
    case RUNTIME_FIELD_AMP_CHB_I:
    {
        value = localData.amp_chb_i;
        break;
    }
    case RUNTIME_FIELD_AMP_CHB_Q:
    {
        value = localData.amp_chb_q;
        break;
    }
    case RUNTIME_FIELD_AMP_CHB:
    {
        value = localData.amp_chb;
        break;
    }
    case RUNTIME_FIELD_PHASE_A:
    {
        value = localData.phase_a;
        break;
    }
    case RUNTIME_FIELD_PHASE_B:
    {
        value = localData.phase_b;
        break;
    }
    case RUNTIME_FIELD_CURRENT_FREQUENCY:
    {
        value = localData.currentFrequency;
        break;
    }
    case RUNTIME_FIELD_EST_RESONANT_FREQUENCY:
    {
        value = localData.estResonantFrequency;
        break;
    }
    case RUNTIME_FIELD_MIN_AGC_OUTPUT:
    {
        value = localData.minAgcOutput;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_ERROR:
    {
        value = localData.pllCurrentError;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_PHASE:
    {
        value = localData.pllCurrentPhase;
        break;
    }
    case RUNTIME_FIELD_PLL_CURRENT_SET_PHASE:
    {
        value = localData.pllCurrentSetPhase;
        break;
    }
    case RUNTIME_FIELD_PID_AI_CURRENT_ERROR:
    {
        value = localData.pidAICurrentError;
        break;
    }
    case RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT:
    {
        value = localData.pidAICurrentOutput;
        break;
    }
    case RUNTIME_FIELD_PID_AQ_CURRENT_ERROR:
    {
        value = localData.pidAQCurrentError;
        break;
    }
    case RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT:
    {
        value = localData.pidAQCurrentOutput;
        break;
    }
    case RUNTIME_FIELD_PID_BI_CURRENT_ERROR:
    {
        value = localData.pidBICurrentError;
        break;
    }
    case RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT:
    {
        value = localData.pidBICurrentOutput;
        break;
    }
    case RUNTIME_FIELD_PID_BQ_CURRENT_ERROR:
    {
        value = localData.pidBQCurrentError;
        break;
    }
    case RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT:
    {
        value = localData.pidBQCurrentOutput;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_START_FREQUENCY:
    {
        value = localData.startFrequency;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_SHORT_TAU:
    {
        value = localData.shortTau;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_LONG_TAU:
    {
        value = localData.longTau;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_MIN:
    {
        value = localData.qFactorMin;
        break;
    }
    case RUNTIME_FIELD_QFACTOR_MAX:
    {
        value = localData.qFactorMax;
        break;
    }
    case RUNTIME_FIELD_PHASEFINDER_AGCAVG:
    {
        value = localData.agcAvg;
        break;
    }
    case RUNTIME_FIELD_PHASEFINDER_DEBUG:
    {
        value = localData.debug;
        break;
    }

    default:
        break;
    }
    return value;
};

// Gyro output (?)
void initGyroOutput(u8 Gyro_ID, gyroOutput_t *localGyroOutput)
{
    localGyroOutput->gyroid = Gyro_ID;
    localGyroOutput->freq = (1000 * 4294967295 / 5000000);
    localGyroOutput->channelAI = 0;
    localGyroOutput->channelAQ = 0;
    localGyroOutput->channelBI = 0;
    localGyroOutput->channelBQ = 0;
    localGyroOutput->hv1 = 32768;
    localGyroOutput->hv2 = 32768;
    localGyroOutput->hv3 = 32768;
    localGyroOutput->hv4 = 32768;
    localGyroOutput->drivePhaseShiftA = ((0x0000 << 16) | 0x4000);
    localGyroOutput->drivePhaseShiftB = ((0x0000 << 16) | 0x4000);
    localGyroOutput->demodPhaseShiftA = ((0x0000 << 16) | 0x4000);
    localGyroOutput->demodPhaseShiftB = ((0x0000 << 16) | 0x4000);
}

void setGyroOutput(u8 Gyro_ID, gyroOutput_e field, u32 data)
{
    gyroOutput_t localGyroOutput = (Gyro_ID == GYRO1 ? gyroOutput_1 : gyroOutput_2);

    switch (field)
    {
    case OUTPUT_FIELD_FREQUENCY:
    {
        localGyroOutput.freq = data;
        break;
    }
    case OUTPUT_FIELD_DRIVE_CHA_I:
    {
        localGyroOutput.channelAI = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_DRIVE_CHA_Q:
    {
        localGyroOutput.channelAQ = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_DRIVE_CHB_I:
    {
        localGyroOutput.channelBI = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_DRIVE_CHB_Q:
    {
        localGyroOutput.channelBQ = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_HV_1:
    {
        localGyroOutput.hv1 = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_HV_2:
    {
        localGyroOutput.hv2 = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_HV_3:
    {
        localGyroOutput.hv3 = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_HV_4:
    {
        localGyroOutput.hv4 = (u16)(data);
        break;
    }
    case OUTPUT_FIELD_DRIVE_PHASE_A:
    {
        localGyroOutput.drivePhaseShiftA = data;
        break;
    }
    case OUTPUT_FIELD_DRIVE_PHASE_B:
    {
        localGyroOutput.drivePhaseShiftB = data;
        break;
    }
    case OUTPUT_FIELD_DEMOD_PHASE_A:
    {
        localGyroOutput.demodPhaseShiftA = data;
        break;
    }
    case OUTPUT_FIELD_DEMOD_PHASE_B:
    {
        localGyroOutput.demodPhaseShiftB = data;
        break;
    }
    }

    if (Gyro_ID == GYRO1)
    {
        gyroOutput_1 = localGyroOutput;
    }
    else if (Gyro_ID == GYRO2)
    {
        gyroOutput_2 = localGyroOutput;
    }
}
void applyToGyro(u8 Gyro_ID)
{
    gyroOutput_t localGyroOutput = (Gyro_ID == GYRO1 ? gyroOutput_1 : gyroOutput_2);
    //    if (Gyro_ID == GYRO1)
    //    {
    //        localGyroOutput = gyroOutput_1;
    //    }
    //    else if (Gyro_ID == GYRO2)
    //    {
    //        localGyroOutput = gyroOutput_2;
    //    }
    write_freword(Gyro_ID, localGyroOutput.freq);
    write_amp_i_modulation(Gyro_ID, CHA, localGyroOutput.channelAI);
    write_amp_q_modulation(Gyro_ID, CHA, localGyroOutput.channelAQ);
    write_amp_i_modulation(Gyro_ID, CHB, localGyroOutput.channelBI);
    write_amp_q_modulation(Gyro_ID, CHB, localGyroOutput.channelBQ);
    write_HV(Gyro_ID, CH1, localGyroOutput.hv1);
    write_HV(Gyro_ID, CH2, localGyroOutput.hv2);
    write_HV(Gyro_ID, CH3, localGyroOutput.hv3);
    write_HV(Gyro_ID, CH4, localGyroOutput.hv4);
    write_drive_phase_modulation(Gyro_ID, CHA, (short)((localGyroOutput.drivePhaseShiftA >> 16) & 0x0000FFFF), (short)(localGyroOutput.drivePhaseShiftA & 0x0000FFFF));
    write_drive_phase_modulation(Gyro_ID, CHB, (short)((localGyroOutput.drivePhaseShiftB >> 16) & 0x0000FFFF), (short)(localGyroOutput.drivePhaseShiftB & 0x0000FFFF));
    write_demod_phase_modulation(Gyro_ID, CHA, (short)((localGyroOutput.demodPhaseShiftA >> 16) & 0x0000FFFF), (short)(localGyroOutput.demodPhaseShiftA & 0x0000FFFF));
    write_demod_phase_modulation(Gyro_ID, CHB, (short)((localGyroOutput.demodPhaseShiftB >> 16) & 0x0000FFFF), (short)(localGyroOutput.demodPhaseShiftB & 0x0000FFFF));
}
