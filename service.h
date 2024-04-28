/*Origin Author: Hugo*/
#ifndef SRC_SERVICE_H_
#define SRC_SERVICE_H_

#include "behavior.h"

#define GYRO1 1
#define GYRO2 2

void serviceInit();
void serviceCommunication();
void serviceGyro();

/*****************************************************************************
Function: serviceCommunication()
Description:  通信业务，完成陀螺数据的发送封包和接收处理
                             下行数据表
        type_value	功能及对应表示						    格式
        0x0001			A通道同相和正交调幅系数				s16 同相+s16 正交
        0x0002			B通道同相和正交调幅系数				s16 同相+s16 正交
        0x0003			开环驱动频率						u32 频率字
        0x0004			A通道驱动移相器相位					s16 sin+s16 cos
        0x0005			B通道驱动移相器相位					s16 sin+s16 cos
        0x0006			A通道解调移相器相位					s16 sin+s16 cos
        0x0007			B通道解调移相器相位					s16 sin+s16 cos
        0x0008			高压输出							u16 通道号+u16 数据

        0x0009			扫频_初始频率设置					u32 初始频率字
        0x000A		扫频_终止频率设置					u32 初始频率字
        0x000B			扫频_步长设置						u32 步长
        0x000C		扫频_等待时间设置					u32 等待时间
        0x000D		扫频_模式与陀螺选择					u32 运行模式
                                                                LSB:	mode select:			A or B, 0=OFF, 1=A, 2=B
                                                                        sweeper running (MSB): 	01（单扫）、10(连扫)、00(停止)
                                                                        sweeper direction:		0正扫，1反扫）

        0x0011			锁相环_锁定模态						u8 0关 1A模态 2B模态
        0x0012			锁相环_锁定相位						s32
        0x0013			锁相环_中心驱动频率					u32
        0x0014			锁相环_P							s32放缩转double
        0x0015			锁相环_I							s32放缩double
        0x0016			锁相环_D							s32放缩double
        0x0017			锁相环_输出限幅						s32放缩double

        0x0021			稳幅环_锁定模态						u8 0关 1BI模态 2AI模态  3BQ模态 4AQ模态
        0x0022			稳幅环_锁定幅值						u32
        0x0023			稳幅环_中心驱动幅值					s16
        0x0024			稳幅环_P							s32放缩double
        0x0025			稳幅环_I							s32放缩double
        0x0026			稳幅环_D							s32放缩double
        0x0027			稳幅环_输出限幅						s32放缩double


        0x0031			正交抑制环_锁定模态					u8 0关 1BI模态 2AI模态  3BQ模态 4AQ模态
        0x0032			正交抑制环_锁定幅值					s32
        0x0033			正交抑制环_中心驱动幅值				s16
        0x0034			正交抑制环_P						s32放缩double
        0x0035			正交抑制环_I						s32放缩double
        0x0036			正交抑制环_D						s32放缩double
        px0037			正交抑制环_输出限幅					s32放缩double

        0x0041			力平衡环_锁定模态					u8 0关 1BI模态 2AI模态  3BQ模态 4AQ模态
        0x0042			力平衡环_锁定幅值					s32
        0x0043			力平衡环_中心驱动幅值				s16
        0x0044			力平衡环_P							s32放缩double
        0x0045			力平衡环_I							s32放缩double
        0x0046			力平衡环_D							s32放缩double
        0x0047			力平衡环_输出限幅					s32放缩double

        未实现//0x0100			陀螺状态							u16 每一位代表一个陀螺有效
        0x0101        	环路状态							u8   1锁相环	2稳幅环	3正交抑制环	4力平衡环

                            上行数据表
*******************************************************************************/

#endif /* SRC_SERVICE_H_ */
