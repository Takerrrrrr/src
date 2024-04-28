/*Origin Author: Hugo*/
#include "service.h"
#define SPI_COM
int main()
{
    u8 tick_1ms = 0;

    axi_gpio_init();	//GPIO初始化
    timer_init();		//定时器初始化
    spi_init();    		//SPI初始化

    serviceInit();

    while(1){
        tick_1ms = get_tickStatus();//获取1ms标志
        //每1ms间隔进入1次
        if(tick_1ms == 1){
            set_tickFlag(0);			//清定时标志
            serviceCommunication();				//通信业务
            serviceGyro();				//陀螺业务
        }
    }
}
