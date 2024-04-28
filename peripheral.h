/*Origin Author: Hugo*/
#ifndef SRC_PERIPHERAL_H_
#define SRC_PERIPHERAL_H_

#include "global.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "xspi.h"
#include "xgpio.h"
#include "peripheral.h"

#define USE_GYRO_SLOT_1 1
#define USE_GYRO_SLOT_2 2
#define USE_GYRO_SLOT_BOTH 3

// hardware group
// #define USE_HARDWARE_SPEC_SIGNLE_GYRO_STD
// #define USE_HARDWARE_SPEC_SINGLE_GYRO_100MHZ
#define USE_HARDWARE_SPEC_SINGLE_GYRO_HV
// #define USE_HARDWARE_SPEC_DUAL_GYRO_STD
// #define USE_HARDWARE_SPEC_DUAL_GYRO_FRREQUNCEY

// defines the number of gyro slots
#if defined(USE_HARDWARE_SPEC_DUAL_GYRO_STD) || defined(USE_HARDWARE_SPEC_DUAL_GYRO_FRREQUNCEY)
#define GYRO_MASK USE_GYRO_SLOT_BOTH
#else
#define GYRO_MASK USE_GYRO_SLOT_1
// #define GYRO_MASK USE_GYRO_SLOT_2
#endif

#define SPI_COM
// #define UART_COM

void set_tickFlag(u8 value);
u8 get_tickStatus();
u32 get_systemTime(void);
int timer_init();

u16 crcCalculation(u8 *ptr, u32 len);

#define DATA_FRAME_HEADER_1 0xEB               // 陀螺数据帧头1
#define DATA_FRAME_HEADER_2 0x90               // 陀螺数据帧头2
#define DATA_FRAME_FOOTER_1 0x0D               // 陀螺数据帧尾1
#define DATA_FRAME_FOOTER_2 0x0A               // 陀螺数据帧尾2
#define CMD_FRAME_HEAD 0x24                    // 指令帧头 $
#define CMD_FRAME_END 0x24                     // 指令帧尾 $
#define CMD_PAYLOAD_LENGTH 35                  // 指令载荷长度
#define DATA_FRAME_HEADER_1 0xEB               // 陀螺数据帧头1
#define DATA_FRAME_HEADER_2 0x90               // 陀螺数据帧头2
#define DATA_FRAME_FOOTER_1 0x0D               // 陀螺数据帧尾1
#define DATA_FRAME_FOOTER_2 0x0A               // 陀螺数据帧尾2
#define CMD_FRAME_HEAD 0x24                    // 指令帧头 $
#define CMD_FRAME_END 0x24                     // 指令帧尾 $
#define CMD_PAYLOAD_LENGTH 35                  // 指令载荷长度
int spi_init();
void spiWrite(u8 *send_temp, u8 length);
u16 recvCrc;							 // 接收的指令帧的CRC数据
u8 recvCmdPayload[CMD_PAYLOAD_LENGTH]; // 接收的指令帧的数据长度
u16 recvCrc;							 // 接收的指令帧的CRC数据
u8 recvCmdPayload[CMD_PAYLOAD_LENGTH]; // 接收的指令帧的数据长度
void set_CmdValid(u8 value);
u8 get_CmdValid(void);

#if defined (USE_HARDWARE_SPEC_SINGLE_GYRO_HV)
void axi_gpio_init(void);
void write_gpio_debug(u8 value);
void G12_write_HVout(u16 Data_CHx, u16 Channel_CHx);
void G1_write_freword(u32 freword);
void G1_write_amp_i_modulation(u8 channel, short amp_i_modulation);
void G1_write_amp_q_modulation(u8 channel, short amp_q_modulation);
void G1_write_drive_phase_modulation(u8 channel, short sin, short cos);
void G1_write_demod_phase_modulation(u8 channel, short sin, short cos);
s32 G1_read_demod_IQ(u8 channel, u32 Gyro_IQ);
u32 G1_read_demod_amp(u8 channel);
s32 G1_read_demod_phase(u8 channel);
void write_whole_angle_parameter(u8 channel, s32 RS_value);
s32 read_standing_wave_angle(void);
#endif /* USE_HARDWARE_SPEC_xxx */

#endif /* SRC_PERIPHERAL_H_ */
