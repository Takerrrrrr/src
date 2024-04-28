/*Origin Author: Hugo*/
#include "behavior.h"

u32 getSystemTime(void)
{
    return get_systemTime();
}

// CRC-CCITT-FALSE冗余循环检验查表计算校验值
const u16 crcTab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

u16 crcCalculation(u8 *ptr, u32 len)
{
    u16 crc = 0xffff;

    while (len-- != 0)
    {
        u16 high = (unsigned int)(crc / 256); // 取CRC左移8位
        crc <<= 8;
        crc ^= crcTab[high ^ *ptr];
        ptr++;
    }

    return crc;
}

void sendData(dataPayload_t payload)
{
    // Description: 向上位机发送陀螺数据
    u8 tmp[54] = {0};
    tmp[0] = (u8)(payload.gyroID);

    tmp[1] = (u8)(payload.time >> 24);
    tmp[2] = (u8)((payload.time >> 16) & 0x000000FF);
    tmp[3] = (u8)((payload.time >> 8) & 0x000000FF);
    tmp[4] = (u8)(payload.time & 0x000000FF);

    tmp[5] = (u8)(payload.amp_cha_i >> 24);
    tmp[6] = (u8)((payload.amp_cha_i >> 16) & 0x000000FF);
    tmp[7] = (u8)((payload.amp_cha_i >> 8) & 0x000000FF);
    tmp[8] = (u8)(payload.amp_cha_i & 0x000000FF);

    tmp[9] = (u8)(payload.amp_cha_q >> 24);
    tmp[10] = (u8)((payload.amp_cha_q >> 16) & 0x000000FF);
    tmp[11] = (u8)((payload.amp_cha_q >> 8) & 0x000000FF);
    tmp[12] = (u8)(payload.amp_cha_q & 0x000000FF);

    tmp[13] = (u8)(payload.amp_cha >> 24);
    tmp[14] = (u8)((payload.amp_cha >> 16) & 0x000000FF);
    tmp[15] = (u8)((payload.amp_cha >> 8) & 0x000000FF);
    tmp[16] = (u8)(payload.amp_cha & 0x000000FF);

    tmp[17] = (u8)(payload.amp_chb_i >> 24);
    tmp[18] = (u8)((payload.amp_chb_i >> 16) & 0x000000FF);
    tmp[19] = (u8)((payload.amp_chb_i >> 8) & 0x000000FF);
    tmp[20] = (u8)(payload.amp_chb_i & 0x000000FF);

    tmp[21] = (u8)(payload.amp_chb_q >> 24);
    tmp[22] = (u8)((payload.amp_chb_q >> 16) & 0x000000FF);
    tmp[23] = (u8)((payload.amp_chb_q >> 8) & 0x000000FF);
    tmp[24] = (u8)(payload.amp_chb_q & 0x000000FF);

    tmp[25] = (u8)(payload.amp_chb >> 24);
    tmp[26] = (u8)((payload.amp_chb >> 16) & 0x000000FF);
    tmp[27] = (u8)((payload.amp_chb >> 8) & 0x000000FF);
    tmp[28] = (u8)(payload.amp_chb & 0x000000FF);

    tmp[29] = (u8)(payload.loopError >> 24);
    tmp[30] = (u8)((payload.loopError >> 16) & 0x000000FF);
    tmp[31] = (u8)((payload.loopError >> 8) & 0x000000FF);
    tmp[32] = (u8)(payload.loopError & 0x000000FF);

    tmp[33] = (u8)(payload.loopOutput >> 24);
    tmp[34] = (u8)((payload.loopOutput >> 16) & 0x000000FF);
    tmp[35] = (u8)((payload.loopOutput >> 8) & 0x000000FF);
    tmp[36] = (u8)(payload.loopOutput & 0x000000FF);

    tmp[37] = (u8)(payload.loopReserved >> 24);
    tmp[38] = (u8)((payload.loopReserved >> 16) & 0x000000FF);
    tmp[39] = (u8)((payload.loopReserved >> 8) & 0x000000FF);
    tmp[40] = (u8)(payload.loopReserved & 0x000000FF);

    tmp[41] = (u8)(payload.reserved1 >> 24);
    tmp[42] = (u8)((payload.reserved1 >> 16) & 0x000000FF);
    tmp[43] = (u8)((payload.reserved1 >> 8) & 0x000000FF);
    tmp[44] = (u8)(payload.reserved1 & 0x000000FF);

    tmp[45] = (u8)(payload.reserved2 >> 24);
    tmp[46] = (u8)((payload.reserved2 >> 16) & 0x000000FF);
    tmp[47] = (u8)((payload.reserved2 >> 8) & 0x000000FF);
    tmp[48] = (u8)(payload.reserved2 & 0x000000FF);

    tmp[49] = (u8)(payload.reserved3 >> 24);
    tmp[50] = (u8)((payload.reserved3 >> 16) & 0x000000FF);
    tmp[51] = (u8)((payload.reserved3 >> 8) & 0x000000FF);
    tmp[52] = (u8)(payload.reserved3 & 0x000000FF);
    tmp[53] = (u8)(payload.type);

    u16 crc = crcCalculation(tmp, 54);

    u8 tmp_array[60] = {0};

    tmp_array[0] = 0xEB;
    tmp_array[1] = 0x90;
    tmp_array[56] = (u8)((crc >> 8) & 0x00FF);
    tmp_array[57] = (u8)(crc & 0x00FF);
    tmp_array[58] = 0x0D;
    tmp_array[59] = 0x0A;
    for (int i = 0; i < 54; i++)
    {
        tmp_array[2 + i] = tmp[i];
    }

    spiWrite(tmp_array, 60);
}

void sendCmdResponse(cmdPayload_t payload, u8 direction)
{
    // Description: 向上位机发送指令反馈
    u8 tmp[35] = {0};
    tmp[0] = (u8)(payload.gyroID);

    tmp[1] = (u8)(payload.type >> 8);
    tmp[2] = (u8)(payload.type & 0x00FF);

    tmp[3] = (u8)(payload.data0 >> 24);
    tmp[4] = (u8)((payload.data0 >> 16) & 0x000000FF);
    tmp[5] = (u8)((payload.data0 >> 8) & 0x000000FF);
    tmp[6] = (u8)(payload.data0 & 0x000000FF);

    tmp[7] = (u8)(payload.data1 >> 24);
    tmp[8] = (u8)((payload.data1 >> 16) & 0x000000FF);
    tmp[9] = (u8)((payload.data1 >> 8) & 0x000000FF);
    tmp[10] = (u8)(payload.data1 & 0x000000FF);

    tmp[11] = (u8)(payload.data2 >> 24);
    tmp[12] = (u8)((payload.data2 >> 16) & 0x000000FF);
    tmp[13] = (u8)((payload.data2 >> 8) & 0x000000FF);
    tmp[14] = (u8)(payload.data2 & 0x000000FF);

    tmp[15] = (u8)(payload.data3 >> 24);
    tmp[16] = (u8)((payload.data3 >> 16) & 0x000000FF);
    tmp[17] = (u8)((payload.data3 >> 8) & 0x000000FF);
    tmp[18] = (u8)(payload.data3 & 0x000000FF);

    tmp[19] = (u8)(payload.data4 >> 24);
    tmp[20] = (u8)((payload.data4 >> 16) & 0x000000FF);
    tmp[21] = (u8)((payload.data4 >> 8) & 0x000000FF);
    tmp[22] = (u8)(payload.data4 & 0x000000FF);

    tmp[23] = (u8)(payload.data5 >> 24);
    tmp[24] = (u8)((payload.data5 >> 16) & 0x000000FF);
    tmp[25] = (u8)((payload.data5 >> 8) & 0x000000FF);
    tmp[26] = (u8)(payload.data5 & 0x000000FF);

    tmp[27] = (u8)(payload.data6 >> 24);
    tmp[28] = (u8)((payload.data6 >> 16) & 0x000000FF);
    tmp[29] = (u8)((payload.data6 >> 8) & 0x000000FF);
    tmp[30] = (u8)(payload.data6 & 0x000000FF);

    tmp[31] = (u8)(payload.data7 >> 24);
    tmp[32] = (u8)((payload.data7 >> 16) & 0x000000FF);
    tmp[33] = (u8)((payload.data7 >> 8) & 0x000000FF);
    tmp[34] = (u8)(payload.data7 & 0x000000FF);

    u16 crc = crcCalculation(tmp, 35);

    u8 tmp_array[42] = {0};

    tmp_array[0] = 0x00;
    tmp_array[1] = 0x00;
    tmp_array[3] = 0x24;
    tmp_array[4] = direction;
    tmp_array[39] = (u8)((crc >> 8) & 0x00FF);
    tmp_array[40] = (u8)(crc & 0x00FF);
    tmp_array[41] = 0x24;
    for (int i = 0; i < 35; i++)
    {
        tmp_array[4 + i] = tmp[i];
    }

    spiWrite(tmp_array, 42);
}

/*****************************************************************************
Function: cmdPayload_t recvCmd()
Description: 下行，命令
*******************************************************************************/
cmdPayload_t recvCmd(void)
{
    cmdPayload_t temp;
    if (get_recvCmd() == 1)
    {					// 已经接收到指令帧
        set_recvCmd(0); // 清空指令帧收到的标志
                        // 锟斤拷校锟斤拷CRC锟斤拷锟斤拷CRC校锟斤拷晒锟斤拷锟斤拷锟皆结构锟斤拷锟斤拷懈锟街碉拷锟斤拷锟斤拷锟斤拷锟斤拷锟?
        if (recvCrc == crcCalculation(recvCmdPayload, 35))
        {
            set_CmdValid(1);
            temp.gyroID = recvCmdPayload[0];
            temp.type = (recvCmdPayload[1] << 8) | recvCmdPayload[2];
            temp.data0 = (recvCmdPayload[3] << 24) | (recvCmdPayload[4] << 16) | (recvCmdPayload[5] << 8) | recvCmdPayload[6];
            temp.data1 = (recvCmdPayload[7] << 24) | (recvCmdPayload[8] << 16) | (recvCmdPayload[9] << 8) | recvCmdPayload[10];
            temp.data2 = (recvCmdPayload[11] << 24) | (recvCmdPayload[12] << 16) | (recvCmdPayload[13] << 8) | recvCmdPayload[14];
            temp.data3 = (recvCmdPayload[15] << 24) | (recvCmdPayload[16] << 16) | (recvCmdPayload[17] << 8) | recvCmdPayload[18];
            temp.data4 = (recvCmdPayload[19] << 24) | (recvCmdPayload[20] << 16) | (recvCmdPayload[21] << 8) | recvCmdPayload[22];
            temp.data5 = (recvCmdPayload[23] << 24) | (recvCmdPayload[24] << 16) | (recvCmdPayload[25] << 8) | recvCmdPayload[26];
            temp.data6 = (recvCmdPayload[27] << 24) | (recvCmdPayload[28] << 16) | (recvCmdPayload[29] << 8) | recvCmdPayload[30];
            temp.data7 = (recvCmdPayload[31] << 24) | (recvCmdPayload[32] << 16) | (recvCmdPayload[33] << 8) | recvCmdPayload[34];
        }
        else
        {
            set_CmdValid(0);
            temp.gyroID = 0;
            temp.type = 0;
            temp.data0 = 0;
            temp.data1 = 0;
            temp.data2 = 0;
            temp.data3 = 0;
            temp.data4 = 0;
            temp.data5 = 0;
            temp.data6 = 0;
            temp.data7 = 0;
        }
    }
    return temp; // 锟斤拷锟截结构锟斤拷
}

u8 isCmdValid(void)
{
    u8 status = get_CmdValid();

    if (status == 1)
    {
        set_CmdValid(0);
        return 1;
    }
    return status;
}

void resetCmdValidStatus(void)
{
    set_CmdValid(0);
}


void write_freword(u8 Gyro_ID, u32 freword)
{
    // Description: 写入陀螺ID的频率字的值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u32 freword：写入频率字的值
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        G1_write_freword(freword);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        G2_write_freword(freword);
    }
    else
    {
    }
}

void write_amp_i_modulation(u8 Gyro_ID, u8 channel, short amp_i_modulation)
{
    // Description: 写入陀螺ID，同相驱动力调幅系数的值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     short amp_i_modulation：同相驱动力调幅系数的值
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        G1_write_amp_i_modulation(channel, amp_i_modulation);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        G2_write_amp_i_modulation(channel, amp_i_modulation);
    }
    else
    {
    }
}

void write_amp_q_modulation(u8 Gyro_ID, u8 channel, short amp_q_modulation)
{
    // Description: 写入陀螺ID，正交驱动力调幅系数的值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     short amp_i_modulation：正交驱动力调幅系数的值
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        G1_write_amp_q_modulation(channel, amp_q_modulation);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        G2_write_amp_q_modulation(channel, amp_q_modulation);
    }
    else
    {
    }
}

void write_drive_phase_modulation(u8 Gyro_ID, u8 channel, short sin, short cos)
{
    // Description: 写入陀螺ID，调整驱动调相器的相位
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     short sin：相位对应sin值，定点数格式：1位符号+1位整数+14位小数
    //     short cos：相位对应cos值，定点数格式：1位符号+1位整数+14位小数
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        G1_write_drive_phase_modulation(channel, sin, cos);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        G2_write_drive_phase_modulation(channel, sin, cos);
    }
    else
    {
    }
}

void write_demod_phase_modulation(u8 Gyro_ID, u8 channel, short sin, short cos)
{
    // Description: 陀螺ID号和通道号下调整解调调相器的相位
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     short sin：相位对应sin值，定点数格式：1位符号+1位整数+14位小数
    //     short cos：相位对应cos值，定点数格式：1位符号+1位整数+14位小数
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        G1_write_demod_phase_modulation(channel, sin, cos);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        G2_write_demod_phase_modulation(channel, sin, cos);
    }
    else
    {
    }
}

void write_HV(u8 Gyro_ID, u16 channel, u16 data)
{
    // Description: 陀螺ID号和通道号下的I或Q值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1234
    G12_write_HVout(data, channel + 4 * (Gyro_ID % 2));
}

s32 read_demod_IQ(u8 Gyro_ID, u8 channel, u32 Gyro_IQ)
{
    // Description: 陀螺ID号和通道号下的I或Q值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     u32 Gyro_IQ：陀螺IQ选择，1为读取Q，2为读取I
    // Return: s32
    s32 temp = 0;
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        temp = G1_read_demod_IQ(channel, Gyro_IQ);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        temp = G2_read_demod_IQ(channel, Gyro_IQ);
    }
    else
    {
    }
    return temp;
}

u32 read_demod_amp(u8 Gyro_ID, u8 channel)
{
    // Description: 陀螺ID号和通道号下解调后的幅值
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     u32 demod_amp：解调幅值
    // Return: u32
    u32 temp = 0;
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        temp = G1_read_demod_amp(channel);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        temp = G2_read_demod_amp(channel);
    }
    else
    {
    }
    return temp;
}

void write_SR(u8 Gyro_ID,s32 r_value, s32 s_value)
{
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        write_whole_angle_parameter(CHR, r_value);
        write_whole_angle_parameter(CHS, s_value);
    }
}

#define angle_factor (double)(1<<29)        // scaling factor
s32 read_angle_scale(u8 Gyro_ID)
{
    s32 angle_raw = 0;
    angle_raw = read_standing_wave_angle();
    return angle_raw;                       // (-1,1) 32位 补码 定点数
    // float angle_scale = angle_raw/angle_factor;             // binary -> double
    // return angle_scale;
}

s32 read_demod_phase(u8 Gyro_ID, u8 channel)
{
    // Description: 陀螺ID号和通道号下解调后的相位
    // Input:
    //     u8 Gyro_ID: 陀螺ID号，1为G1, 2为G2
    //     u8 channel: 陀螺通道号，1为A通道，2为B通道
    //     u32 demod_amp：解调相位
    // Return: s32
    s32 temp = 0;
    if (((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_1))&&(Gyro_ID == 1))
    {
        temp = G1_read_demod_phase(channel);
    }
    else if ((GYRO_MASK == USE_GYRO_SLOT_BOTH) || (GYRO_MASK == USE_GYRO_SLOT_2)&&(Gyro_ID == 2))
    {
        temp = G2_read_demod_phase(channel);
    }
    else
    {
    }
    return temp;
}
