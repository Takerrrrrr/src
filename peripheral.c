/*Origin Author: Hugo*/
#include "peripheral.h"

#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID // 系统中断控制器ID
XScuGic Intc;                                       // 系统中断控制器实例

#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID // 定时器设备ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR         // 定时器中断控制器
#define TIMER_LOAD_VALUE 0x000514C7                // 定时器装填值，对应 1ms
XScuTimer Timer;                                   // 定时器设备实例

static u8 tickOneMs = 0;   // 1ms定时器标志
static u32 systemTime = 0; // 系统时间，粒度为1ms

u32 get_systemTime(void)
{
    // Description: 获取系统当前时间
    return systemTime;
}

u8 get_tickStatus(void)
{
    // Description: 获取当前系统滴答状态
    return tickOneMs;
}

void set_tickFlag(u8 value)
{
    // Description: 复位当前系统滴答状态
    tickOneMs = value;
}

void timer_intr_handler(void *CallBackRef)
{
    // Description: 定时器中断处理函数
    XScuTimer *timer_ptr = (XScuTimer *)CallBackRef;
    XScuTimer_ClearInterruptStatus(timer_ptr); // Clear the timer interrupt flag
    systemTime++;
    tickOneMs = 1;
}

int timer_init()
{
    // Description: 初始化系统滴答定时器
    int status;
    XScuTimer_Config *timer_cfg_ptr;
    timer_cfg_ptr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    if (NULL == timer_cfg_ptr)
        return XST_FAILURE;
    status = XScuTimer_CfgInitialize(&Timer, timer_cfg_ptr,
                                     timer_cfg_ptr->BaseAddr);
    if (status != XST_SUCCESS)
        return XST_FAILURE;
    XScuTimer_SelfTest(&Timer);
    XScuTimer_LoadTimer(&Timer, TIMER_LOAD_VALUE); // Load count value
    XScuTimer_EnableAutoReload(&Timer);            // Set auto load mode
    XScuTimer_Start(&Timer);
    // Initialize the interrupt controller
    XScuGic_Config *intc_cfg_ptr;
    Xil_ExceptionInit();
    intc_cfg_ptr = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(&Intc, intc_cfg_ptr,
                          intc_cfg_ptr->CpuBaseAddress);
    // Setting and turn on interrupt exception handling
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);
    Xil_ExceptionEnable();

    // Setting timer interrupts
    XScuGic_Connect(&Intc, TIMER_IRPT_INTR,
                    (Xil_ExceptionHandler)timer_intr_handler, (void *)&Timer);

    XScuGic_Enable(&Intc, TIMER_IRPT_INTR); // Enable the timer in GIC to interrupt
    XScuTimer_EnableInterrupt(&Timer);      // Enable timer interrupt
    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
    return XST_SUCCESS;
}

#define DATA_FRAME_HEADER_1 0xEB               // 陀螺数据帧头1
#define DATA_FRAME_HEADER_2 0x90               // 陀螺数据帧头2
#define DATA_FRAME_FOOTER_1 0x0D               // 陀螺数据帧尾1
#define DATA_FRAME_FOOTER_2 0x0A               // 陀螺数据帧尾2
#define CMD_FRAME_HEAD 0x24                    // 指令帧头 $
#define CMD_FRAME_END 0x24                     // 指令帧尾 $
#define CMD_PAYLOAD_LENGTH 35                  // 指令载荷长度
#define SPI_DEVICE_ID XPAR_SPI_0_DEVICE_ID     // SPI设备ID号
#define SPI_IRPT_INTR XPAR_FABRIC_SPI_0_VEC_ID // SPI中断设备ID号
XSpi SpiInstance;                              // AXI_SPI实例
u8 recvTemp[236] = {0};
u8 recvCmdStatus = 0; // 接收的指令帧接收完成标记
u8 recvCmdValid = 0;  // 接收的指令帧有效无效标记

void set_recvCmd(u8 value)
{
    // Description: 设置接收指令收到与否的状态
    recvCmdStatus = value;
}

u8 get_recvCmd(void)
{
    // Description: 返回接收指令收到与否的状态
    return recvCmdStatus;
}

void set_CmdValid(u8 value)
{
    // Description: 设置命令有效无效状态 1有效 0无效
    recvCmdValid = value;
}

u8 get_CmdValid(void)
{
    // Description: 返回命令有效无效状态 1有效 0无效
    return recvCmdValid;
}

void spiWrite(u8 *send_temp, u8 length)
{
    // Description: SPI写入数据
    XSpi_SetSlaveSelect(&SpiInstance, 0x0001);
    XSpi_Transfer(&SpiInstance, send_temp, recvTemp, length);
}

void SpiIntrHandler(void *CallBackRef, u32 StatusEvent, u32 ByteCount)
{
    // Description: SPI中断处理函数
    /*****************************************************************************
    接收数据格式
        0xEB+0x90+gyroID+Type+Data+CRC高位+CRC低位+0x0D+0x0A
    帧头:0xEB 0x90
    gyroID :u8 陀螺ID号， 1为G1， 2为G2
    Type:类型，根据不同类型号执行不同功能
    Data:系统时间u32 调幅系数，频率字，调相，扫频，锁相，稳幅，正交抑制，力平衡
    CRC:u16 由高位和低位组成，用于校验数据是否传输异常
    帧尾 :0x0D 0x0A
    *******************************************************************************/

    // 判断接收到的数据中是否包含指令帧的帧头与帧尾
    if ((recvTemp[2] == CMD_FRAME_HEAD) && (recvTemp[41] == CMD_FRAME_END) && ((recvTemp[3] == '>') | (recvTemp[3] == '!')))
    {
        set_recvCmd(1);                               // 01位为帧头；11和12位为帧尾
        recvCrc = (recvTemp[39] << 8) | recvTemp[40]; // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫碉拷锟斤拷锟斤拷8bitCRC锟较筹拷CRC16校锟斤拷值
        // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟饺★拷锟絩ecv_data_packet锟斤拷
        for (u8 i = 0; i < CMD_PAYLOAD_LENGTH; i++)
        {
            recvCmdPayload[i] = recvTemp[i + 4]; // recvTemp锟斤拷应去锟斤拷锟斤拷锟斤拷帧头锟斤拷锟斤拷i+2
        }
    }
}

int spi_init()
{
    // Description: 初始化SPI设备
    int SPI_Status;
    XSpi_Config *ConfigPtr;
    ConfigPtr = XSpi_LookupConfig(SPI_DEVICE_ID);
    if (ConfigPtr == NULL)
    {
        return XST_DEVICE_NOT_FOUND;
    }
    XSpi *SpiInstancePtr = &SpiInstance;
    SPI_Status = XSpi_CfgInitialize(SpiInstancePtr, ConfigPtr, ConfigPtr->BaseAddress);
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    SPI_Status = XSpi_SelfTest(SpiInstancePtr);
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    if (SpiInstancePtr->SpiMode != XSP_STANDARD_MODE)
    {
        return XST_SUCCESS;
    }
    XScuGic *IntcInstancePtr = &Intc;

    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == IntcConfig)
    {
        return XST_FAILURE;
    }

    SPI_Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                                       IntcConfig->CpuBaseAddress);
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    XScuGic_SetPriorityTriggerType(IntcInstancePtr, SPI_IRPT_INTR, 0xA0, 0x3);

    //* Connect the device driver handler that will be called when an
    //* interrupt for the device occurs, the handler defined above performs
    //* the specific interrupt processing for the device.

    SPI_Status = XScuGic_Connect(IntcInstancePtr, SPI_IRPT_INTR,
                                 (Xil_InterruptHandler)XSpi_InterruptHandler,
                                 SpiInstancePtr);
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_SUCCESS;
    }
    XScuGic_Enable(IntcInstancePtr, SPI_IRPT_INTR);
    // Enable interrupts from the hardware
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, (void *)IntcInstancePtr);
    Xil_ExceptionEnable();
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    XSpi_SetStatusHandler(SpiInstancePtr, SpiInstancePtr, (XSpi_StatusHandler)SpiIntrHandler);
    SPI_Status = XSpi_SetOptions(SpiInstancePtr, XSP_MANUAL_SSELECT_OPTION | XSP_MASTER_OPTION);
    if (SPI_Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    XSpi_Start(SpiInstancePtr);
    return XST_SUCCESS;
}

/**** AXI-GPIO设备 ****/
#if defined(USE_HARDWARE_SPEC_SINGLE_GYRO_HV)
// 陀螺频率字初始值
#define G1_freword_init 1000 * 4294967295 / 50000000 // G1
// 各个同相正交调幅系数，驱动解调调相值初始值
#define G1_cha_sin_drive_phase_init 0x0000 // 0-0000 30-2000 45-2D41 60-376C 90-4000
#define G1_cha_cos_drive_phase_init 0x4000 // 0-4000 30-376C 45-2D41 60-2000 90-0000
#define G1_chb_sin_drive_phase_init 0x0000 // 0-0000 30-2000 45-2D41 60-376C 90-4000
#define G1_chb_cos_drive_phase_init 0x4000 // 0-4000 30-376C 45-2D41 60-2000 90-0000
#define G1_cha_sin_demod_phase_init 0x0000 // 0-0000 30-2000 45-2D41 60-376C 90-4000
#define G1_cha_cos_demod_phase_init 0x4000 // 0-4000 30-376C 45-2D41 60-2000 90-0000
#define G1_chb_sin_demod_phase_init 0x0000 // 0-0000 30-2000 45-2D41 60-376C 90-4000
#define G1_chb_cos_demod_phase_init 0x4000 // 0-4000 30-376C 45-2D41 60-2000 90-0000
// 陀螺同相幅值初始化
#define G1_cha_amp_i_init 0x0000
#define G1_chb_amp_i_init 0x0000

// 陀螺正交幅值初始化
#define G1_cha_amp_q_init 0x0000
#define G1_chb_amp_q_init 0x0000

// G1  GPIO接口结构作用
XGpio G1_freword;                    // axi_gpio0  	G1频率字
XGpio G1_amp_i_modulation;           // AXI_GPIO_8 	   G1同相驱动力调幅系数(sin)
XGpio G1_amp_q_modulation;           // AXI_GPIO_9 	G1正交驱动力调幅系数(cos)
XGpio G1_cha_drive_phase_modulation; // AXI_GPIO_13 	G1 A通道驱动调相
XGpio G1_chb_drive_phase_modulation; // AXI_GPIO_12 	G1 B通道驱动调相
XGpio G1_cha_demod_phase_modulation; // AXI_GPIO_11  	G1 A通道解调调相
XGpio G1_chb_demod_phase_modulation; // AXI_GPIO_10  	G1 B通道解调调相

XGpio G1_cha_demod_IQ; // AXI_GPIO_4  G1 A通道解调后IQ值
XGpio G1_chb_demod_IQ; // AXI_GPIO_5  G1 B通道解调后IQ值
XGpio G1_demod_amp;    // AXI_GPIO_6	G1 解调幅值
XGpio G1_demod_phase;  // AXI_GPIO_7	G1 解调相位
XGpio G1_ADC_data;     // AXI_GPIO_3	G1 ADC输出数据（调试用）
XGpio G1_NCO;          // AXI_GPIO_2	G1 NCO输出频率(调试用)

// XGpio for Whole Angle Mode; EC:energe control
XGpio EC_RS;           // AXI_GPIO_14	S R 
XGpio EC_Angel;        // AXI_GPIO_15	arctan(S/2R) 

void axi_gpio_init(void)
{
	// Description: GPIO初始化

    // Gyr1
    // axi_gpio0 G1_freword
    XGpio_Initialize(&G1_freword, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&G1_freword, 1, 0x00000000);
    XGpio_SetDataDirection(&G1_freword, 2, 0x0);
    XGpio_DiscreteWrite(&G1_freword, 1, G1_freword_init);

    // axi_gpio8 G1_amp_i_modulation
    XGpio_Initialize(&G1_amp_i_modulation, XPAR_AXI_GPIO_8_DEVICE_ID);
    XGpio_SetDataDirection(&G1_amp_i_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_amp_i_modulation, 2, 0x0000);
    XGpio_DiscreteWrite(&G1_amp_i_modulation, 2, G1_chb_amp_i_init); // CH2_Amp_init
    XGpio_DiscreteWrite(&G1_amp_i_modulation, 1, G1_cha_amp_i_init); // CH1_Amp_init

    // axi_gpio9 G1_amp_q_modulation
    XGpio_Initialize(&G1_amp_q_modulation, XPAR_AXI_GPIO_9_DEVICE_ID);
    XGpio_SetDataDirection(&G1_amp_q_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_amp_q_modulation, 2, 0x0000);
    XGpio_DiscreteWrite(&G1_amp_q_modulation, 2, G1_chb_amp_q_init); // CH2_QAmp_init
    XGpio_DiscreteWrite(&G1_amp_q_modulation, 1, G1_cha_amp_q_init); // CH1_QAmp_init

    // axi_gpio13 G1_cha_drive_phase_modulation
    XGpio_Initialize(&G1_cha_drive_phase_modulation, XPAR_AXI_GPIO_13_DEVICE_ID);
    XGpio_SetDataDirection(&G1_cha_drive_phase_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_cha_drive_phase_modulation, 2, 0x0000);

    XGpio_DiscreteWrite(&G1_cha_drive_phase_modulation, 2, G1_cha_sin_drive_phase_init);
    XGpio_DiscreteWrite(&G1_cha_drive_phase_modulation, 1, G1_cha_cos_drive_phase_init);

    // axi_gpio12 G1_chb_drive_phase_modulation
    XGpio_Initialize(&G1_chb_drive_phase_modulation, XPAR_AXI_GPIO_12_DEVICE_ID);
    XGpio_SetDataDirection(&G1_chb_drive_phase_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_chb_drive_phase_modulation, 2, 0x0000);

    XGpio_DiscreteWrite(&G1_chb_drive_phase_modulation, 2, G1_chb_sin_drive_phase_init);
    XGpio_DiscreteWrite(&G1_chb_drive_phase_modulation, 1, G1_chb_cos_drive_phase_init);

    // axi_gpio11 G1_cha_demod_phase_modulation
    XGpio_Initialize(&G1_cha_demod_phase_modulation, XPAR_AXI_GPIO_11_DEVICE_ID);
    XGpio_SetDataDirection(&G1_cha_demod_phase_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_cha_demod_phase_modulation, 2, 0x0000);
    XGpio_DiscreteWrite(&G1_cha_demod_phase_modulation, 2, G1_cha_sin_demod_phase_init);
    XGpio_DiscreteWrite(&G1_cha_demod_phase_modulation, 1, G1_cha_cos_demod_phase_init);

    // axi_gpio10 G1_chb_demod_phase_modulation
    XGpio_Initialize(&G1_chb_demod_phase_modulation, XPAR_AXI_GPIO_10_DEVICE_ID);
    XGpio_SetDataDirection(&G1_chb_demod_phase_modulation, 1, 0x0000);
    XGpio_SetDataDirection(&G1_chb_demod_phase_modulation, 2, 0x0000);
    XGpio_DiscreteWrite(&G1_chb_demod_phase_modulation, 2, G1_chb_sin_demod_phase_init);
    XGpio_DiscreteWrite(&G1_chb_demod_phase_modulation, 1, G1_chb_cos_demod_phase_init);

    // axi_gpio4 G1_cha_demod_IQ
    XGpio_Initialize(&G1_cha_demod_IQ, XPAR_AXI_GPIO_4_DEVICE_ID);
    XGpio_SetDataDirection(&G1_cha_demod_IQ, 1, 0xFFFFFFFF);
    XGpio_SetDataDirection(&G1_cha_demod_IQ, 2, 0xFFFFFFFF);

    // axi_gpio5 G1_chb_demod_IQ
    XGpio_Initialize(&G1_chb_demod_IQ, XPAR_AXI_GPIO_5_DEVICE_ID);
    XGpio_SetDataDirection(&G1_chb_demod_IQ, 1, 0xFFFFFFFF);
    XGpio_SetDataDirection(&G1_chb_demod_IQ, 2, 0xFFFFFFFF);

    // axi_gpio6 G1_demod_amp
    XGpio_Initialize(&G1_demod_amp, XPAR_AXI_GPIO_6_DEVICE_ID);
    XGpio_SetDataDirection(&G1_demod_amp, 1, 0xFFFFFFFF);
    XGpio_SetDataDirection(&G1_demod_amp, 2, 0xFFFFFFFF);

    // axi_gpio7 G1_demod_phase
    XGpio_Initialize(&G1_demod_phase, XPAR_AXI_GPIO_7_DEVICE_ID);
    XGpio_SetDataDirection(&G1_demod_phase, 1, 0xFFFFFFFF);
    XGpio_SetDataDirection(&G1_demod_phase, 2, 0xFFFFFFFF);

    // axi_gpio3 G1_ADC_data
    XGpio_Initialize(&G1_ADC_data, XPAR_AXI_GPIO_3_DEVICE_ID);
    XGpio_SetDataDirection(&G1_ADC_data, 1, 0xFFFF);
    XGpio_SetDataDirection(&G1_ADC_data, 2, 0xFFFF);

    // axi_gpio2 G1_NCO
    XGpio_Initialize(&G1_NCO, XPAR_AXI_GPIO_2_DEVICE_ID);
    XGpio_SetDataDirection(&G1_NCO, 1, 0xFFFF);
    XGpio_SetDataDirection(&G1_NCO, 2, 0xFFFF);

    // axi_gpio14 EC_RS
    XGpio_Initialize(&EC_RS, XPAR_AXI_GPIO_14_DEVICE_ID);
    XGpio_SetDataDirection(&EC_RS, 1, 0x00000000);
    XGpio_SetDataDirection(&EC_RS, 2, 0x00000000);
    XGpio_DiscreteWrite(&EC_RS, 1, 0x0000);
    XGpio_DiscreteWrite(&EC_RS, 2, 0x0000);
    
    // axi_gpio15 EC_Angel
    XGpio_Initialize(&EC_Angel, XPAR_AXI_GPIO_15_DEVICE_ID);
    XGpio_SetDataDirection(&EC_Angel, 1, 0xFFFFFFFF);
}

/*void write_gpio_debug(u8 value)
{
    // Description:  GPIO口值调试用
    XGpio_DiscreteWrite(&G1_freword, 2,value);
}*/

void G12_write_HVout(u16 data, u16 channel){
    return;
}
void G1_write_freword(u32 freword)
{
    // Description: 写入1号陀螺的频率字的值
    // AXIGPIO通道1 -> u32 freword: 频率字
    XGpio_DiscreteWrite(&G1_freword, 1, freword);
    usleep(1);
}

void G1_write_amp_i_modulation(u8 channel, short amp_i_modulation)
{
    // Description: 写入1号陀螺，同相驱动力调幅系数的值
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // short amp_i_modulation：同相驱动力调幅系数的值
    XGpio_DiscreteWrite(&G1_amp_i_modulation, channel, amp_i_modulation);
    usleep(1);
}

void G1_write_amp_q_modulation(u8 channel, short amp_q_modulation)
{
    // Description: 写入1号陀螺，正交驱动力调幅系数的值
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // short amp_q_modulation：正交驱动力调幅系数的值
    XGpio_DiscreteWrite(&G1_amp_q_modulation, channel, amp_q_modulation);
    usleep(1);
}

void G1_write_drive_phase_modulation(u8 channel, short sin, short cos)
{
    // Description: 写入1号陀螺，调整对应驱动调相器的相位
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // short sin:相位对应sin值，定点数格式：1位符号+1位整数+14位小数
    // short cos:相位对应cos值，定点数格式：1位符号+1位整数+14位小数
    if (channel == 1)
    {
        XGpio_DiscreteWrite(&G1_cha_drive_phase_modulation, 1, cos); // cos
        usleep(1);
        XGpio_DiscreteWrite(&G1_cha_drive_phase_modulation, 2, sin); // sin
        usleep(1);
    }
    else if (channel == 2)
    {
        XGpio_DiscreteWrite(&G1_chb_drive_phase_modulation, 1, cos); // cos
        usleep(1);
        XGpio_DiscreteWrite(&G1_chb_drive_phase_modulation, 2, sin); // sin
        usleep(1);
    }
    else
    {
    }
}

void G1_write_demod_phase_modulation(u8 channel, short sin, short cos)
{
    // Description: 写入1号陀螺，调整对应解调调相器的相位
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // short sin:相位对应sin值，定点数格式：1位符号+1位整数+14位小数
    // short cos:相位对应cos值，定点数格式：1位符号+1位整数+14位小数
    if (channel == 1)
    {
        XGpio_DiscreteWrite(&G1_cha_demod_phase_modulation, 1, cos); // cos
        usleep(1);
        XGpio_DiscreteWrite(&G1_cha_demod_phase_modulation, 2, sin); // sin
        usleep(1);
    }
    else if (channel == 2)
    {
        XGpio_DiscreteWrite(&G1_chb_demod_phase_modulation, 1, cos); // cos
        usleep(1);
        XGpio_DiscreteWrite(&G1_chb_demod_phase_modulation, 2, sin); // sin
        usleep(1);
    }
    else
    {
    }
}

s32 G1_read_demod_IQ(u8 channel, u32 Gyro_IQ)
{
    // Description: 读取1号陀螺解调后的IQ值
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // u32 Gyro_IQ: 陀螺IQ选择，1为读取Q，2为读取I
    // return: s32 G1解调后IQ值
    s32 temp = 0;
    if (channel == 1)
    {
        temp = XGpio_DiscreteRead(&G1_cha_demod_IQ, Gyro_IQ);
    }
    else if (channel == 2)
    {
        temp = XGpio_DiscreteRead(&G1_chb_demod_IQ, Gyro_IQ);
    }
    else
    {
    }
    return temp;
}

u32 G1_read_demod_amp(u8 channel)
{
    // Description: 读取1号陀螺解调后的幅值
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // return: s32 G1解调后的幅值
    s32 temp = 0;
    temp = XGpio_DiscreteRead(&G1_demod_amp, channel);
    return temp;
}

s32 G1_read_demod_phase(u8 channel)
{
    // Description: 读取1号陀螺解调后的相位
    // u8 channel: 陀螺通道号，1为A通道，2为B通道
    // return: s32 G1解调后的相位
    s32 temp = 0;
    temp = XGpio_DiscreteRead(&G1_demod_phase, channel);
    return temp;
}

void write_whole_angle_parameter(u8 channel, s32 RS_value)
{
    // Description: 写入全角模式的计算量
    // channel:  1通道写入R ，2通道为写入S
    XGpio_DiscreteWrite(&EC_RS,channel,RS_value);
    usleep(1);
}

s32 read_standing_wave_angle(void)
{
    // Description: 读取cordic计算的角度(需要除以2)
    // cordic输出定点数格式： 1位符号位，2位整数位，29位小数位
    // 输出范围： （-1，1）
    s32 temp = 0;
    temp = XGpio_DiscreteRead(&EC_Angel, 1);
    return temp;
}

#endif /* USE_HARDWARE_SPEC_xxx */
