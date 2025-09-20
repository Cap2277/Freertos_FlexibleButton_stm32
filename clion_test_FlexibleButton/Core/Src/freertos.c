/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Lib/OLED/oled.h"
#include "stdio.h"
#include "string.h"
#include "DHT11.h"
#include "semphr.h"
#include "bsp_ds18b20.h"
#include "usart.h"
#include "button.h"
#include "flexible_button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Bee_toggle() HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8)
#define Bee_ON() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,0)
#define Bee_OFF() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,1)
#define Red_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0)
#define Red_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t message[64];
uint8_t high_water_message[64];
uint32_t adc_value;
int count=0;
/* USER CODE END Variables */
osThreadId HIGHTaskHandle;
osMutexId tookenHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void HIGHTaskAPP(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  // 创建队列
  xButtonQueue = xQueueCreate(10, sizeof(ButtonMsg_t));

  // 创建任务
  xTaskCreate(vButtonTask, "BtnScan", 128, NULL, 2, NULL);
  xTaskCreate(vButtonEventTask, "BtnEvent", 256, NULL, 1, NULL);

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of tooken */
  osMutexDef(tooken);
  tookenHandle = osMutexCreate(osMutex(tooken));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of HIGHTask */
  osThreadDef(HIGHTask, HIGHTaskAPP, osPriorityHigh, 0, 200);
  HIGHTaskHandle = osThreadCreate(osThread(HIGHTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_HIGHTaskAPP */
/**
  * @brief  Function implementing the HIGHTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_HIGHTaskAPP */
void HIGHTaskAPP(void const * argument)
{
  /* USER CODE BEGIN HIGHTaskAPP */
  /* Infinite loop */
  for(;;)
  {
    Red_OFF();
    osDelay(200);
    Red_ON();
    osDelay(100);
  }
  /* USER CODE END HIGHTaskAPP */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
//   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//   if(hadc->Instance==ADC1) {
//     adc_value = HAL_ADC_GetValue(hadc); // 获取ADC转换结果
//
//     if (myBinarySem_ADCdataHandle != NULL) {
//       // 释放信号量，通知任务ADC数据已准备好
//       xSemaphoreGiveFromISR(myBinarySem_ADCdataHandle, &xHigherPriorityTaskWoken);
//       // 如果更高优先级的任务被唤醒，请求进行上下文切�??????????
//       portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
//   }
// }
// 🔹 扫描任务
void vButtonTask(void *pvParameters)
{
  button_init();

  for (;;) {
    flex_button_scan();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// 🔹 事件处理任务
void vButtonEventTask(void *pvParameters)
{
  ButtonMsg_t msg;
  char temp_message[32];
  for (;;) {
    if (xQueueReceive(xButtonQueue, &msg, portMAX_DELAY) == pdPASS) {
      switch (msg.event) {
        case BTN_EVENT_SINGLE_CLICK:
          sprintf(temp_message,"Button %d Single Click \n", msg.id);
          HAL_UART_Transmit(&huart1, (uint8_t *)temp_message, strlen(temp_message), 100);
          break;
        case BTN_EVENT_DOUBLE_CLICK:
          sprintf(temp_message,"Button %d Double Click\n", msg.id);
          HAL_UART_Transmit(&huart1, (uint8_t *)temp_message, strlen(temp_message), 100);
          break;
        case BTN_EVENT_LONG_PRESS_START:
          sprintf(temp_message,"Button %d Long Press Start\n", msg.id);
          HAL_UART_Transmit(&huart1, (uint8_t *)temp_message, strlen(temp_message), 100);

          break;
        case BTN_EVENT_LONG_PRESS_HOLD:
          sprintf(temp_message,"Button %d Long Press Hold \n", msg.id, msg.event);
          HAL_UART_Transmit(&huart1, (uint8_t *)temp_message, strlen(temp_message), 100);
          break;
        default:
          break;
      }
    }
  }
}

/* USER CODE END Application */

