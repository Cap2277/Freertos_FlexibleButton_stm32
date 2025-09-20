/**
 * ************************************************************************
 * 
 * @file My_DHT11.h
 * @author zxr
 * @brief DHT11 温湿度传感器驱动（无结构体版本）
 * 
 * ************************************************************************
 * @copyright Copyright (c) 2024
 * ************************************************************************
 */
#ifndef __MY_DHT11_H
#define __MY_DHT11_H

#include "stm32f1xx_hal.h"

/* ------------------- DHT11 引脚定义 ------------------- */
#define DHT11_PORT         GPIOA
#define DHT11_PIN          GPIO_PIN_1

#define DHT11_PULL_1       HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET)
#define DHT11_PULL_0       HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET)

#define DHT11_ReadPin      HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)

/* ------------------- 函数声明 ------------------- */

/**
 * @brief 微秒级延时函数
 * @param us 延时时间（微秒）
 */
void my_delay_us(uint16_t us);

/**
 * @brief 读取 DHT11 数据（温度和湿度）
 * @return 1=成功, 0=失败
 */
uint8_t DHT11_Read_Data(void);

/**
 * @brief 获取温度
 * @return 温度值（浮点数）
 */
float Get_temperature(void);

/**
 * @brief 获取湿度
 * @return 湿度值（浮点数）
 */
float Get_humidity(void);

#endif /* __MY_DHT11_H */
