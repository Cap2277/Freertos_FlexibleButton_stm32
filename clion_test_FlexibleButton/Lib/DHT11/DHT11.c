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
/* ȫ�ֱ�������Ž������ */
float temperature = 0;
float humidity = 0;
void my_delay_us(uint16_t us){     //΢����ʱ
	uint16_t differ = 0xffff-us-5;				
	__HAL_TIM_SET_COUNTER(&htim1,differ);	//�趨TIM1��������ʼֵ
	HAL_TIM_Base_Start(&htim1);		//������ʱ��	
	
	while(differ < 0xffff-5){	//�ж�
		differ = __HAL_TIM_GET_COUNTER(&htim1);		//��ѯ�������ļ���ֵ
	}
	HAL_TIM_Base_Stop(&htim1);
}

/**
 * ************************************************************************
 * @brief ��DHT11����Ϊ�������ģʽ
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
 * @brief ��DHT11����Ϊ��������ģʽ
 * ************************************************************************
 */
static void DHT11_UP_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;	//����
	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief ��ȡ�ֽ�
 * @return temp
 * ************************************************************************
 */
uint8_t DHT11_ReadByte(void)
{
	uint8_t i, temp = 0;

	for (i = 0; i < 8; i++)
	{
		while (DHT11_ReadPin == 0);		// �ȴ��͵�ƽ����
		
		HAL_Delay(40);			//	��ʱ 40 ΢��
		
		if (DHT11_ReadPin == 1)
		{
			while (DHT11_ReadPin == 1);	// �ȴ��ߵ�ƽ����
			
			temp |= (uint8_t)(0X01 << (7 - i));			// �ȷ��͸�λ
		}
		else
		{
			temp &= (uint8_t)~(0X01 << (7 - i));
		}
	}
	return temp;
}

/**
 * @brief ��ȡ DHT11 ���ݣ��������
 * @return 0=ʧ��, 1=�ɹ�
 */
uint8_t DHT11_Read_Data(void)
{
    int i;
    uint64_t val = 0;
    int timeout = 0;
    uint8_t verify_num = 0;

    /* ��ʼ�ź� */
    DHT11_PP_OUT();
    DHT11_PULL_0;
    HAL_Delay(19);          // ��������18ms
    DHT11_PULL_1;
    my_delay_us(20);        // ���ߵȴ�20us

    DHT11_UP_IN();          // תΪ����ģʽ

    timeout = 5000;
    while ((!DHT11_ReadPin) && (timeout > 0)) timeout--;   // �ȴ��ߵ�ƽ����
    timeout = 5000;
    while ((DHT11_ReadPin) && (timeout > 0)) timeout--;    // �ȴ��͵�ƽ����

    /* ��ȡ40bit���� */
    for (i = 0; i < 40; i++)
    {
        timeout = 5000;
        while ((!DHT11_ReadPin) && (timeout > 0)) timeout--;   // �ȴ��͵�ƽ����

        my_delay_us(28);   // ������ʱ���ж�Ϊ1
        val <<= 1;
        if (DHT11_ReadPin) val |= 1;

        timeout = 5000;
        while ((DHT11_ReadPin) && (timeout > 0)) timeout--;   // �ȴ��ߵ�ƽ����
    }

    /* �ͷ����� */
    DHT11_PP_OUT();
    DHT11_PULL_1;

    /* ��� */
    uint8_t humi_int  = (val >> 32) & 0xFF;
    uint8_t humi_dec  = (val >> 24) & 0xFF;
    uint8_t temp_int  = (val >> 16) & 0xFF;
    uint8_t temp_dec  = (val >>  8) & 0xFF;
    uint8_t check_sum =  val        & 0xFF;

    /* У�� */
    verify_num = humi_int + humi_dec + temp_int + temp_dec;
    if (verify_num != check_sum)
    {
        return 0;   // У��ʧ��
    }
    else
    {
        humidity    = humi_int + humi_dec * 0.1f;
        temperature = temp_int + temp_dec * 0.1f;
        return 1;   // У��ɹ�
    }
}

/* ��ȡ�¶� */
float Get_temperature(void)
{
    return temperature;
}

/* ��ȡʪ�� */
float Get_humidity(void)
{
    return humidity;
}

