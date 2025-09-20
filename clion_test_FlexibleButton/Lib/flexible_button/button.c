#include "button.h"
#include "main.h"
#include "stdio.h"
#include "string.h"


/**
 * @file    button.c
 * @brief   基于FlexibleButton库的按键管理模块实现
 * @details 本模块封装了FlexibleButton库，提供了3个按键的初始化、事件检测
 *          和队列消息发送功能。支持单击、双击、长按等多种按键事件。
 * @author  [Your Name]
 * @date    2024-01-01
 * @version V1.0
 *
 * @note    使用前需要：
 *          1. 创建按键扫描任务，定期调用flex_button_scan()
 *          2. 创建按键队列xButtonQueue
 *          3. 根据实际硬件修改GPIO读取函数
 */
/* ========================= 全局变量定义 ========================= */
/**
 * @brief 按键消息队列句柄
 * @note  用于存储按键事件，供其他任务读取处理
 */
QueueHandle_t xButtonQueue;
/**
 * @brief FlexibleButton按键对象数组
 * @note  支持3个物理按键，索引0-2分别对应K1、K2、K3
 */
flex_button_t btn[3];

// 🔹 硬件读电平   修改此处代码
uint8_t read_btn_k1(void *btn) { return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14); }
uint8_t read_btn_k2(void *btn) { return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);  }
uint8_t read_btn_k3(void *btn) { return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);  }

/* ========================= 事件回调函数 ========================= */
/**
 * @brief   按键事件统一回调函数
 * @details 当FlexibleButton检测到按键事件时调用此函数，
 *          将事件转换为自定义格式并发送到队列
 * @param   btn_obj 触发事件的按键对象指针
 * @note    🔹 此函数在按键扫描任务中被调用
 * @warning 如果在中断中调用flex_button_scan()，需要使用xQueueSendFromISR
 */
static void btn_event_cb(void *btn_obj)
{
    flex_button_t *p_btn = (flex_button_t *)btn_obj;
    ButtonMsg_t msg;

    /* 记录按键ID（0-2对应K1-K3） */
    msg.id = p_btn->id;
    /* 将FlexibleButton事件类型转换为自定义事件类型 */
    switch (p_btn->event) {
        case FLEX_BTN_PRESS_CLICK:          /* 单击事件 */
            msg.event = BTN_EVENT_SINGLE_CLICK;
            break;

        case FLEX_BTN_PRESS_DOUBLE_CLICK:   /* 双击事件 */
            msg.event = BTN_EVENT_DOUBLE_CLICK;
            break;

        case FLEX_BTN_PRESS_LONG_START:     /* 长按开始事件 */
            msg.event = BTN_EVENT_LONG_PRESS_START;
            break;

        case FLEX_BTN_PRESS_LONG_HOLD:      /* 长按保持事件 */
            msg.event = BTN_EVENT_LONG_PRESS_HOLD;
            break;

        default:                             /* 其他事件忽略 */
            return;
    }
    /* 发送消息到队列，不等待 */
    xQueueSend(xButtonQueue, &msg, 0);
    // 注意：如果是在中断里面调用，要换成xQueueSendFromISR
}

/* ========================= 初始化函数 ========================= */
/**
 * @brief   按键模块初始化函数
 * @details 配置3个按键的参数并注册到FlexibleButton库
 * @note    🔹 调用此函数前需要先初始化GPIO
 *
 * 时序参数说明：
 * - short_press_start_tick: 短按判定时间，超过此时间判定为短按
 * - long_press_start_tick:  长按判定时间，超过此时间判定为长按
 * - long_hold_start_tick:   长按保持时间，超过此时间持续触发保持事件
 */

void button_init(void)
{
    memset(btn, 0, sizeof(btn));

    btn[0].id = 0;                                              /* 按键ID */
    btn[0].usr_button_read = read_btn_k1;                       /* GPIO读取函数 */
    btn[0].pressed_logic_level = 0;                             /* 按下时的电平（0=低电平有效） */
    btn[0].cb = btn_event_cb;                                   /* 事件回调函数 */
    btn[0].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(300);   /* 短按阈值 300ms */
    btn[0].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);  /* 长按阈值 1000ms */
    btn[0].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);  /* 长按保持 2000ms */
    flex_button_register(&btn[0]);                              /* 注册到FlexibleButton */

    btn[1].id = 1;
     btn[1].usr_button_read = read_btn_k2;
    btn[1].pressed_logic_level = 0;
    btn[1].cb = btn_event_cb;
    btn[1].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(300);   // 短按阈值 300ms
    btn[1].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);     // 长按阈值 1000ms
    btn[1].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);     // 长按保持 2000ms
    flex_button_register(&btn[1]);

    btn[2].id = 2;
    btn[2].usr_button_read = read_btn_k3;
    btn[2].pressed_logic_level = 0;
    btn[2].cb = btn_event_cb;
    btn[2].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(300);   // 短按阈值 300ms
    btn[2].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);     // 长按阈值 1000ms
    btn[2].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);     // 长按保持 2000ms
    flex_button_register(&btn[2]);
}

