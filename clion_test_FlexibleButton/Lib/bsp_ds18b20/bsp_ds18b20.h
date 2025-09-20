#ifndef _BSP_DS18B20_H_
#define _BSP_DS18B20_H_

#include "stm32f1xx_hal.h"

// DS18B20 接口定义
#define DS18B20_GPIO_PORT    GPIOA
#define DS18B20_GPIO_PIN     GPIO_PIN_1

// DQ 输出模式（推挽输出）
#define DQ_OUT()  do { \
GPIO_InitTypeDef GPIO_InitStruct = {0}; \
GPIO_InitStruct.Pin = DS18B20_GPIO_PIN; \
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
GPIO_InitStruct.Pull = GPIO_NOPULL; \
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
HAL_GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStruct); \
} while(0)

// DQ 输入模式（上拉输入）
#define DQ_IN()  do { \
GPIO_InitTypeDef GPIO_InitStruct = {0}; \
GPIO_InitStruct.Pin = DS18B20_GPIO_PIN; \
GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
GPIO_InitStruct.Pull = GPIO_PULLUP; \
HAL_GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStruct); \
} while(0)

// 读DQ电平
#define DQ_GET()   HAL_GPIO_ReadPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

// 写DQ电平
#define DQ(x)      HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, (x)?GPIO_PIN_SET:GPIO_PIN_RESET)


// DS18B20 函数声明
char DS18B20_GPIO_Init(void);
uint8_t DS18B20_Check(void);
void DS18B20_Start(void);
float DS18B20_GetTemperture(void);
uint8_t DS18B20_Read_Byte(void);
void DS18B20_Write_Byte(uint8_t dat);

#endif
