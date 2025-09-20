#include "bsp_ds18b20.h"
#include "main.h"
#include "stdio.h"

// 微秒延时函数（需要你在工程里实现 SysTick 或 TIM 延时）
extern void my_delay_us(uint16_t us);


/******************************************************************
 * 函 数 名 称：DS18B20_GPIO_Init
 * 函 数 说 明：初始化 DS18B20 引脚
 * 返 回 值 ：0 - 检测到器件，1 - 未检测到
******************************************************************/
char DS18B20_GPIO_Init(void)
{
    // 先初始化为输出模式
    __HAL_RCC_GPIOB_CLK_ENABLE();
    DQ_OUT();
    return DS18B20_Check();
}

/******************************************************************
 * 从DS18B20读取一个字节
******************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i, dat = 0;
    for(i=0;i<8;i++)
    {
        DQ_OUT();
        DQ(0);
        my_delay_us(2);
        DQ(1);
        DQ_IN();
        my_delay_us(12);
        dat >>= 1;
        if(DQ_GET())
            dat |= 0x80;
        my_delay_us(50);
    }
    return dat;
}

/******************************************************************
 * 写一个字节到DS18B20
******************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        DQ_OUT();
        if(dat & 0x01) // 写1
        {
            DQ(0);
            my_delay_us(2);
            DQ(1);
            my_delay_us(60);
        }
        else // 写0
        {
            DQ(0);
            my_delay_us(60);
            DQ(1);
            my_delay_us(2);
        }
        dat >>= 1;
    }
}

/******************************************************************
 * 检测 DS18B20 是否存在
******************************************************************/
uint8_t DS18B20_Check(void)
{
    uint8_t timeout=0;

    DQ_OUT();
    DQ(0);
    my_delay_us(750);   // 拉低 750us
    DQ(1);
    my_delay_us(15);

    DQ_IN(); // 输入模式

    // 等待应答
    while(DQ_GET() && timeout<200)
    {
        timeout++;
        my_delay_us(1);
    }
    if(timeout >= 200) return 1;

    timeout = 0;
    while(!DQ_GET() && timeout<240)
    {
        timeout++;
        my_delay_us(1);
    }
    if(timeout >= 240) return 1;

    return 0;
}

/******************************************************************
 * 启动温度转换
******************************************************************/
void DS18B20_Start(void)
{
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);  // Skip ROM
    DS18B20_Write_Byte(0x44);  // Convert T
}

/******************************************************************
 * 读取温度
******************************************************************/
float DS18B20_GetTemperture(void)
{
    uint16_t temp;
    uint8_t dataL, dataH;
    float value;

    DS18B20_Start();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC); // Skip ROM
    DS18B20_Write_Byte(0xBE); // Read Scratchpad

    dataL = DS18B20_Read_Byte();
    dataH = DS18B20_Read_Byte();
    temp = (dataH<<8) | dataL;

    if(dataH & 0x80) // 负温度
    {
        temp = (~temp)+1;
        value = -((float)temp * 0.0625f);
    }
    else
    {
        value = (float)temp * 0.0625f;
    }
    return value;
}
