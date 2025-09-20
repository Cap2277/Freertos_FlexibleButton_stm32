/**
 * ************************************************************************
 * 
 * @file My_DHT11.c
 * @author zxr
 * @brief 
 * 
 * ************************************************************************
 * @copyright Copyright (c) 2024 zxr 
 * ************************************************************************
 */
#include "DHT11.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "tim.h"
/* 全局变量，存放解析结果 */
float temperature = 0;
float humidity = 0;
void my_delay_us(uint16_t us){     //微秒延时
	uint16_t differ = 0xffff-us-5;				
	__HAL_TIM_SET_COUNTER(&htim1,differ);	//设定TIM1计数器起始值
	HAL_TIM_Base_Start(&htim1);		//启动定时器	
	
	while(differ < 0xffff-5){	//判断
		differ = __HAL_TIM_GET_COUNTER(&htim1);		//查询计数器的计数值
	}
	HAL_TIM_Base_Stop(&htim1);
}

/**
 * ************************************************************************
 * @brief 将DHT11配置为推挽输出模式
 * ************************************************************************
 */
static void DHT11_PP_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief 将DHT11配置为上拉输入模式
 * ************************************************************************
 */
static void DHT11_UP_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;	//上拉
	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief 读取字节
 * @return temp
 * ************************************************************************
 */
uint8_t DHT11_ReadByte(void)
{
	uint8_t i, temp = 0;

	for (i = 0; i < 8; i++)
	{
		while (DHT11_ReadPin == 0);		// 等待低电平结束
		
		HAL_Delay(40);			//	延时 40 微秒
		
		if (DHT11_ReadPin == 1)
		{
			while (DHT11_ReadPin == 1);	// 等待高电平结束
			
			temp |= (uint8_t)(0X01 << (7 - i));			// 先发送高位
		}
		else
		{
			temp &= (uint8_t)~(0X01 << (7 - i));
		}
	}
	return temp;
}

/**
 * @brief 读取 DHT11 数据（立创风格）
 * @return 0=失败, 1=成功
 */
uint8_t DHT11_Read_Data(void)
{
    int i;
    uint64_t val = 0;
    int timeout = 0;
    uint8_t verify_num = 0;

    /* 起始信号 */
    DHT11_PP_OUT();
    DHT11_PULL_0;
    HAL_Delay(19);          // 拉低至少18ms
    DHT11_PULL_1;
    my_delay_us(20);        // 拉高等待20us

    DHT11_UP_IN();          // 转为输入模式

    timeout = 5000;
    while ((!DHT11_ReadPin) && (timeout > 0)) timeout--;   // 等待高电平到来
    timeout = 5000;
    while ((DHT11_ReadPin) && (timeout > 0)) timeout--;    // 等待低电平到来

    /* 读取40bit数据 */
    for (i = 0; i < 40; i++)
    {
        timeout = 5000;
        while ((!DHT11_ReadPin) && (timeout > 0)) timeout--;   // 等待低电平结束

        my_delay_us(28);   // 超过此时间判断为1
        val <<= 1;
        if (DHT11_ReadPin) val |= 1;

        timeout = 5000;
        while ((DHT11_ReadPin) && (timeout > 0)) timeout--;   // 等待高电平结束
    }

    /* 释放总线 */
    DHT11_PP_OUT();
    DHT11_PULL_1;

    /* 拆分 */
    uint8_t humi_int  = (val >> 32) & 0xFF;
    uint8_t humi_dec  = (val >> 24) & 0xFF;
    uint8_t temp_int  = (val >> 16) & 0xFF;
    uint8_t temp_dec  = (val >>  8) & 0xFF;
    uint8_t check_sum =  val        & 0xFF;

    /* 校验 */
    verify_num = humi_int + humi_dec + temp_int + temp_dec;
    if (verify_num != check_sum)
    {
        return 0;   // 校验失败
    }
    else
    {
        humidity    = humi_int + humi_dec * 0.1f;
        temperature = temp_int + temp_dec * 0.1f;
        return 1;   // 校验成功
    }
}

/* 获取温度 */
float Get_temperature(void)
{
    return temperature;
}

/* 获取湿度 */
float Get_humidity(void)
{
    return humidity;
}

