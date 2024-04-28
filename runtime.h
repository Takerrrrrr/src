/*Origin Author: Hugo*/
#ifndef SRC_RUNTIME_H_
#define SRC_RUNTIME_H_
#include "global.h"

// Runtime Data (ps->pc) type macros

typedef enum
{
    RUNTIME_FIELD_AMP_CHA_I = 1,
    RUNTIME_FIELD_AMP_CHA_Q,
    RUNTIME_FIELD_AMP_CHA,
    RUNTIME_FIELD_AMP_CHB_I,
    RUNTIME_FIELD_AMP_CHB_Q,
    RUNTIME_FIELD_AMP_CHB,
    RUNTIME_FIELD_PHASE_A,
    RUNTIME_FIELD_PHASE_B,

    RUNTIME_FIELD_CURRENT_FREQUENCY = 10,
    RUNTIME_FIELD_EST_RESONANT_FREQUENCY = 11,
    RUNTIME_FIELD_MIN_AGC_OUTPUT = 12,

    RUNTIME_FIELD_PLL_CURRENT_ERROR = 21,
    RUNTIME_FIELD_PLL_CURRENT_PHASE = 22,
    RUNTIME_FIELD_PLL_CURRENT_SET_PHASE = 23,

    RUNTIME_FIELD_PID_AI_CURRENT_ERROR = 31,
    RUNTIME_FIELD_PID_AI_CURRENT_OUTPUT = 32,

    RUNTIME_FIELD_PID_AQ_CURRENT_ERROR = 41,
    RUNTIME_FIELD_PID_AQ_CURRENT_OUTPUT = 42,

    RUNTIME_FIELD_PID_BI_CURRENT_ERROR = 51,
    RUNTIME_FIELD_PID_BI_CURRENT_OUTPUT = 52,

    RUNTIME_FIELD_PID_BQ_CURRENT_ERROR = 61,
    RUNTIME_FIELD_PID_BQ_CURRENT_OUTPUT = 62,

    RUNTIME_FIELD_QFACTOR_START_FREQUENCY = 71,
    RUNTIME_FIELD_QFACTOR_SHORT_TAU = 72,
    RUNTIME_FIELD_QFACTOR_LONG_TAU = 73,
    RUNTIME_FIELD_QFACTOR_MIN = 74,
    RUNTIME_FIELD_QFACTOR_MAX = 75,

    RUNTIME_FIELD_PHASEFINDER_AGCAVG = 81,
    RUNTIME_FIELD_PHASEFINDER_DEBUG = 82,

} runtimeData_e;

// Gyro output
typedef enum
{
    OUTPUT_FIELD_FREQUENCY,
    OUTPUT_FIELD_DRIVE_CHA_I,
    OUTPUT_FIELD_DRIVE_CHA_Q,
    OUTPUT_FIELD_DRIVE_CHB_I,
    OUTPUT_FIELD_DRIVE_CHB_Q,
    OUTPUT_FIELD_HV_1,
    OUTPUT_FIELD_HV_2,
    OUTPUT_FIELD_HV_3,
    OUTPUT_FIELD_HV_4,
    OUTPUT_FIELD_DRIVE_PHASE_A,
    OUTPUT_FIELD_DRIVE_PHASE_B,
    OUTPUT_FIELD_DEMOD_PHASE_A,
    OUTPUT_FIELD_DEMOD_PHASE_B,
} gyroOutput_e;

// gyro data (runtime)
typedef struct
{
    u8 gyroid;
    // reading data from axi peripherals
    s32 amp_cha_i;
    s32 amp_cha_q;
    u32 amp_cha;
    s32 amp_chb_i;
    s32 amp_chb_q;
    u32 amp_chb;
    u32 phase_a;
    u32 phase_b;
    // data from sweeper and pll
    u32 currentFrequency; // variable
    u32 estResonantFrequency;
    u32 minAgcOutput;
    // sweeper data
    u32 amp_max_a;
    u32 fre_when_amp_max_a;
    u32 phase_when_amp_max_a;
    u32 amp_max_b;
    u32 fre_when_amp_max_b;
    u32 phase_when_amp_max_b;
    // PLL data
    s32 pllCurrentError;
    s32 pllCurrentPhase;
    s32 pllCurrentSetPhase;
    // AI PID data
    s32 pidAICurrentError;
    s32 pidAICurrentOutput;
    // AQ PID data
    s32 pidAQCurrentError;
    s32 pidAQCurrentOutput;
    // BI PID data
    s32 pidBICurrentError;
    s32 pidBICurrentOutput;
    // BQ PID data
    s32 pidBQCurrentError;
    s32 pidBQCurrentOutput;

    // Q factor Data
    u32 startFrequency;
    u32 shortTau;
    u32 longTau;
    u32 qFactorMin;
    u32 qFactorMax;

    // Phase Finder
    u32 agcAvg;
    u32 debug;

    // // frequency tuner
    // u32 resonantFrequency_a;
    // u32 resonantFrequency_b;
    // u32 currentHV;
} gyroDataRuntime_t;

typedef struct
{
    u8 gyroid;
    u32 freq;
    u16 channelAI;
    u16 channelAQ;
    u16 channelBI;
    u16 channelBQ;
    u16 hv1;
    u16 hv2;
    u16 hv3;
    u16 hv4;
    u32 drivePhaseShiftA;
    u32 drivePhaseShiftB;
    u32 demodPhaseShiftA;
    u32 demodPhaseShiftB;
} gyroOutput_t;

// gyro runtime data instance 
gyroDataRuntime_t gyroData_1;
gyroDataRuntime_t gyroData_2;
// gyro output (?) instance
gyroOutput_t gyroOutput_1;
gyroOutput_t gyroOutput_2;

void initGyroData(u8 Gyro_ID, gyroDataRuntime_t *localData);
void readGyroData(u8 Gyro_ID);
void setGyroData(u8 Gyro_ID, runtimeData_e field, u32 data);
u32 getGyroData(u8 Gyro_ID, runtimeData_e field);
void initGyroOutput(u8 Gyro_ID, gyroOutput_t *localGyroOutput);
void setGyroOutput(u8 Gyro_ID, gyroOutput_e field, u32 data);
void applyToGyro(u8 Gyro_ID);

#endif /* SRC_RUNTIME_H_ */
