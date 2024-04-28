/*Origin Author: Hugo*/
#ifndef SRC_COMMUNICATE_H_
#define SRC_COMMUNICATE_H_

#include "behavior.h"

dataPayload_t buildDataPayload(u8 Gyro_ID);
void decodeCmdPayload(cmdPayload_t cmd);
cmdPayload_t buildCmdFeedbackPayload(u8 Gyro_ID, u16 type);

// Command (pc->ps) type macros
typedef enum
{
    CMD_SYSTEM_RESERVED = 0x0001,

    CMD_TLM_SELECTOR = 0x0008,

    CMD_OPENLOOP_FREQUENCY = 0x0011,
    CMD_OPENLOOP_ACT = 0x0012,
    CMD_OPENLOOP_HV = 0x0013,
    CMD_OPENLOOP_PHASE = 0x0014,

    CMD_SWEEPER = 0x0020,

    CMD_SM_PLL = 0x0031,
    CMD_SM_PID_AI = 0x0032,
    CMD_SM_PID_AQ = 0x0033,
    CMD_SM_PID_BI = 0x0034,
    CMD_SM_PID_BQ = 0x0035,

    CMD_Q_FACTOR = 0x0041,

    CMD_RESONANT_EST = 0x0051,

    CMD_PHASE_FINDER = 0x0061,

    CMD_MODE_SWITCH = 0x0071,

    CMD_WAM_PID_EC = 0x0081,        // 129 接收上位机传来的数据到 能量控制环路控制结构体g1_smPidEcConfig 中;
    CMD_WAM_PID_QC = 0x0082,        // 130 同上 保存到g1_smPidQcConfig
    // CMD_MS_OPENLOOP_ACT = 0x0060,
    // CMD_MS_PLL = 0x0061,
    // CMD_MS_PID_AI = 0x0062,
    // CMD_MS_PID_AQ = 0x0063,
    // CMD_MS_PID_BI = 0x0064,
    // CMD_MS_PID_BQ = 0x0065,
} cmdType_e;

// Data (ps->pc) type macros
typedef enum
{
    MFIELDS_TYPE_OPLP = 0x00,
    MFIELDS_TYPE_SWEEPER = 0x01,
    MFIELDS_TYPE_SM_PLL = 0x09,
    MFIELDS_TYPE_PID_AI = 0x0A,
    MFIELDS_TYPE_PID_AQ = 0x0B,
    MFIELDS_TYPE_PID_BI = 0x0C,
    MFIELDS_TYPE_PID_BQ = 0x0D,
    MFIELDS_TYPE_QFACTOR= 0x0F,
    MFIELDS_TYPE_RESONANT_EST = 0x10,
    MFIELDS_TYPE_PHASE_FINDER = 0x11,
    MFIELDS_TYPE_MODE_SWITCH = 0x12,
    MFIELDS_TYPE_PID_EC = 0x13,             // 19 上位机 全角模式 接收数据
    MFIELDS_TYPE_PID_QC = 0x14,             // 20 显示正交控制数据（最上侧按钮）
} subTelemetryType_e;

#endif /* SRC_COMMUNICATE_H_ */
