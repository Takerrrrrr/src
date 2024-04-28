/*Origin Author: Hugo*/
#ifndef SRC_GYRO_H_
#define SRC_GYRO_H_

#include "global.h"

#define CHA 1
#define CHB 2

#define COMPONENTQ 1
#define COMPONENTI 2

#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4

#define OP_FRE 1
#define OP_AMP 2
#define OP_HV 3
#define OP_PHASE 4


typedef enum{
    LOOP_SOURCE_NONE = 0,
    LOOP_SOURCE_PHASE_A = 1,
    LOOP_SOURCE_PHASE_B = 2,
    LOOP_SOURCE_AMP = 3,
    LOOP_SOURCE_COMPONENT = 4,
} loopSource_e;

typedef enum
{
    LOOP_PUR = 0x00,
    LOOP_PLL = 0x01,
    LOOP_PID_AI = 0x02,
    LOOP_PID_AQ = 0x03,
    LOOP_PID_BI = 0x04,
    LOOP_PID_BQ = 0x05,
    SWEEPER = 0x10,
} loopIndex_e;

typedef struct
{
    u8 gyroid;
    u8 loopSource; // PLL: mode A or B; Excite: amp or I/Q component. use loopSource_e
    u8 loopTarget; // AI AQ BI BQ, use loopIndex_e
    s32 loop_setvalue;      // loop input
    s32 loopReferenceValue;
    s32 PID_P;
    s32 PID_I;
    s32 PID_D;
    s32 currentError;  // k
    s32 previousError; // k-1
    s32 lastError;     // k-2
    s32 loopOutput;
    s32 outputLimit;
    s32 currentReading; // for debug use case
} loopConfig_t;
// control loop universal structure

// Config/parameters type for openloop mode
typedef struct
{
    u8 gyroid;
    u8 frequency_enable;
    u32 freq;
    u8 amp_enable;
    u16 channelAI;
    u16 channelAQ;
    u16 channelBI;
    u16 channelBQ;
    u8 hv_enable;
    u16 hv1;
    u16 hv2;
    u16 hv3;
    u16 hv4;
    u32 drivePhaseShiftA;
    u32 drivePhaseShiftB;
    u32 demodPhaseShiftA;
    u32 demodPhaseShiftB;
} openloopConfig_t;

// Config/parameters type for sweeper
typedef struct
{
    u8 gyroid;
    u8 enable; // on or off
    u8 runningMode;
    u8 direction;
    u32 llf; // lower-limit frequency
    u32 ulf; // upper-limit frequency
    u32 step_freq;
    u32 currentFrequency; // variable
    u32 currentWaitingTime;
    u32 waitingTime;
} sweeperConfig_t;

typedef struct
{
    u8 gyroid;
    u8 enable;
    loopIndex_e targetLoop;   // AI AQ BI BQ
    u32 scaleFactorFrequency; // for get the correct amp (and Q factor)
    u32 startDelay;           // after x ticks, clap start

    u8 stage;
    u32 currentWaitingTime;
    u32 startFrequency; // the frequency when clap start
    u32 startAmp;       // raw value
    u32 *freBuffer;
    u32 *ampBuffer;     // for checking if the amp arrives the target amp (0.673startamp)
    u32 startTimeStamp; // the timestamp for clap start
    u32 endTimeStamp_1; // the timestamp for buffer's min value is below 0.673amp
    u32 endTimeStamp_2; // the timestamp for buffer's max value is below 0.673amp
} qFactorConfig_t;

typedef struct
{
    u8 gyroid;
    u8 agcTargetLoop;
    u32 bufferLen;
    s32 *agcOutputBuffer;
    s32 *agcOutputAvgBuf;
    u32 *frewordBuffer;
    u32 *frewordAvgBuf;
    u32 prevAgc;
    u32 prevFre;
} resonantEstimatorConfig_t;

typedef struct
{
    u8 gyroid;        // need be configured from host
    u8 enable;        // need be configured from host
    u8 agcTargetLoop; // need be configured from host
    u8 stage;
    u32 setTime;      // need be configured from host
    u32 currentWaitingTime;
    u32 bufferLen;
    s32 *agcOutputBuffer;
    s32 leftPhase; // need be configured from host
    s32 leftAgcAvg;
    s32 rightPhase; // need be configured from host
    s32 rightAgcAvg;
    s32 midPhase;
    s32 midAgcAvg;
    s32 minPhase; // need be configured from host
} phaseFinderConfig_t;

typedef struct
{
    u8 gyroid;
    u8 enable;
    u8 copyMode;
    u32 switchTime;
    u32 currentTime;
    u32 currentState;
} modeSwitchingConfig_t;
//running state
// open loop config/parameters
openloopConfig_t g1_openloopConfig;
openloopConfig_t g2_openloopConfig;

// sweeper config/parameters
sweeperConfig_t g1_sweeper;
sweeperConfig_t g2_sweeper;

// single mode config/parameters
loopConfig_t g1_smPLLConfig;
loopConfig_t g1_smPidAiConfig;
loopConfig_t g1_smPidAqConfig;
loopConfig_t g1_smPidBiConfig;
loopConfig_t g1_smPidBqConfig;
loopConfig_t g2_smPLLConfig;
loopConfig_t g2_smPidAiConfig;
loopConfig_t g2_smPidAqConfig;
loopConfig_t g2_smPidBiConfig;
loopConfig_t g2_smPidBqConfig;


// copy1 state
// open loop config/parameters
openloopConfig_t copy1_g1_openloopConfig;
openloopConfig_t copy1_g2_openloopConfig;

// sweeper config/parameters
sweeperConfig_t copy1_g1_sweeper;
sweeperConfig_t copy1_g2_sweeper;

// single mode config/parameters
loopConfig_t copy1_g1_smPLLConfig;
loopConfig_t copy1_g1_smPidAiConfig;
loopConfig_t copy1_g1_smPidAqConfig;
loopConfig_t copy1_g1_smPidBiConfig;
loopConfig_t copy1_g1_smPidBqConfig;
loopConfig_t copy1_g2_smPLLConfig;
loopConfig_t copy1_g2_smPidAiConfig;
loopConfig_t copy1_g2_smPidAqConfig;
loopConfig_t copy1_g2_smPidBiConfig;
loopConfig_t copy1_g2_smPidBqConfig;


//copy2 state
// open loop config/parameters
openloopConfig_t copy2_g1_openloopConfig;
openloopConfig_t copy2_g2_openloopConfig;

// sweeper config/parameters
sweeperConfig_t copy2_g1_sweeper;
sweeperConfig_t copy2_g2_sweeper;

// single mode config/parameters
loopConfig_t copy2_g1_smPLLConfig;
loopConfig_t copy2_g1_smPidAiConfig;
loopConfig_t copy2_g1_smPidAqConfig;
loopConfig_t copy2_g1_smPidBiConfig;
loopConfig_t copy2_g1_smPidBqConfig;
loopConfig_t copy2_g2_smPLLConfig;
loopConfig_t copy2_g2_smPidAiConfig;
loopConfig_t copy2_g2_smPidAqConfig;
loopConfig_t copy2_g2_smPidBiConfig;
loopConfig_t copy2_g2_smPidBqConfig;

// q factor calculation config and buffer configs for configs of single mode
qFactorConfig_t g1_qFactor;
openloopConfig_t g1_qfOpenloopConfig;
loopConfig_t g1_qfPLLConfig;
loopConfig_t g1_qfPidAiConfig;
loopConfig_t g1_qfPidAqConfig;
loopConfig_t g1_qfPidBiConfig;
loopConfig_t g1_qfPidBqConfig;
qFactorConfig_t g2_qFactor;
openloopConfig_t g2_qfOpenloopConfig;
loopConfig_t g2_qfPLLConfig;
loopConfig_t g2_qfPidAiConfig;
loopConfig_t g2_qfPidAqConfig;
loopConfig_t g2_qfPidBiConfig;
loopConfig_t g2_qfPidBqConfig;

// resonant frequency estimator
resonantEstimatorConfig_t g1_resonantEstimatorConfig;
resonantEstimatorConfig_t g2_resonantEstimatorConfig;

// phase finder (for best resonant phase)
phaseFinderConfig_t g1_phaseFinderConfig;
phaseFinderConfig_t g2_phaseFinderConfig;

//modeSwitchingConfig_t
modeSwitchingConfig_t g1_modeSwitchingConfig;
modeSwitchingConfig_t g2_modeSwitchingConfig;

void initOpenloopConfig(u8 gyroID, openloopConfig_t *localCfg);
void initSweeperConfig(u8 gyroID, sweeperConfig_t *localCfg);
void initPidConfig(u8 gyroID, loopConfig_t *localCfg);
void initQFactorConfig(u8 gyroID, qFactorConfig_t *localCfg);
void initResonantEstimator(u8 gyroID, resonantEstimatorConfig_t *localCfg);
void initPhaseFinder(u8 gyroID, phaseFinderConfig_t *localCfg);
void initModeSwitching(u8 gyroID, modeSwitchingConfig_t *localCfg);

void abstractLoop(loopConfig_t *localCfg);
void phaseLockLoop(loopConfig_t *localCfg);
void openLoop(openloopConfig_t *localCfg, u8 field);
void sweeper(sweeperConfig_t *localCfg);

void calculateQFactor(qFactorConfig_t *localCfg);
void resonantEstimator(resonantEstimatorConfig_t *localCfg);
void phaseFinder(phaseFinderConfig_t *localCfg);
void modeSwitching(modeSwitchingConfig_t *localCfg);
#endif /* SRC_GYRO_H_ */
