/*Origin Author: Hugo*/
#ifndef SRC_BEHAVIOR_H_
#define SRC_BEHAVIOR_H_

#include "peripheral.h"

#define CHS 2
#define CHR 1

typedef struct
{
    u8 gyroID;
    u16 type;
    u32 data0;
    u32 data1;
    u32 data2;
    u32 data3;
    u32 data4;
    u32 data5;
    u32 data6;
    u32 data7;
} cmdPayload_t;

typedef struct{
    u8 gyroID;
    u32 time;
    s32 amp_cha_i;
    s32 amp_cha_q;
    u32 amp_cha;
    s32 amp_chb_i;
    s32 amp_chb_q;
    u32 amp_chb;
    s32 loopError;
    s32 loopOutput;
    s32 loopReserved;
    s32 reserved1;
    s32 reserved2;
    s32 reserved3;
    u8 type;
}dataPayload_t;

u32 getSystemTime(void);

void sendData(dataPayload_t payload);
void sendCmdResponse(cmdPayload_t payload, u8 direction);
cmdPayload_t recvCmd();
u8 isCmdValid(void);
void resetCmdValidStatus(void);

void write_freword(u8 Gyro_ID,u32 freword);										//写入陀螺ID的频率字的值
void write_amp_i_modulation(u8 Gyro_ID,u8 channel,short amp_i_modulation);		//写入陀螺ID，同相驱动力调幅系数的值
void write_amp_q_modulation(u8 Gyro_ID,u8 channel,short amp_q_modulation);	    //写入陀螺ID，正交驱动力调幅系数的值
void write_drive_phase_modulation(u8 Gyro_ID,u8 channel,short sin,short cos);	//写入陀螺ID，调整对应驱动调相器的相位
void write_demod_phase_modulation(u8 Gyro_ID,u8 channel,short sin,short cos);	//写入陀螺ID，调整对应解调调相器的相位
void write_HV(u8 Gyro_ID,u16 channel,u16 data);
s32 read_demod_IQ(u8 Gyro_ID,u8 channel, u32 Gyro_IQ); 							//陀螺ID号和通道号下的I或Q值
u32 read_demod_amp(u8 Gyro_ID,u8 channel );								        //陀螺ID号和通道号下解调后的幅值
s32 read_demod_phase(u8 Gyro_ID,u8 channel);							        //陀螺ID号和通道号下解调后的相位

// function for whole angle mode
void write_SR(u8 Gyro_ID,s32 r_value, s32 s_value);
s32 read_angle_scale(u8 Gyro_ID);
#endif /* SRC_BEHAVIOR_H_ */
