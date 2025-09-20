/**
 * ************************************************************************
 * 
 * @file My_DHT11.h
 * @author zxr
 * @brief DHT11 ��ʪ�ȴ������������޽ṹ��汾��
 * 
 * ************************************************************************
 * @copyright Copyright (c) 2024
 * ************************************************************************
 */
#ifndef __MY_DHT11_H
#define __MY_DHT11_H

#include "stm32f1xx_hal.h"

/* ------------------- DHT11 ���Ŷ��� ------------------- */
#define DHT11_PORT         GPIOA
#define DHT11_PIN          GPIO_PIN_1

#define DHT11_PULL_1       HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET)
#define DHT11_PULL_0       HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET)

#define DHT11_ReadPin      HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)

/* ------------------- �������� ------------------- */

/**
 * @brief ΢�뼶��ʱ����
 * @param us ��ʱʱ�䣨΢�룩
 */
void my_delay_us(uint16_t us);

/**
 * @brief ��ȡ DHT11 ���ݣ��¶Ⱥ�ʪ�ȣ�
 * @return 1=�ɹ�, 0=ʧ��
 */
uint8_t DHT11_Read_Data(void);

/**
 * @brief ��ȡ�¶�
 * @return �¶�ֵ����������
 */
float Get_temperature(void);

/**
 * @brief ��ȡʪ��
 * @return ʪ��ֵ����������
 */
float Get_humidity(void);

#endif /* __MY_DHT11_H */
