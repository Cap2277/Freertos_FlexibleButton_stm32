#ifndef __BUTTON_H
#define __BUTTON_H

#include "flexible_button.h"
#include "cmsis_os.h"
#include "queue.h"

// 按键事件类型
typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_PRESS_DOWN,
    BTN_EVENT_PRESS_UP,
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_DOUBLE_CLICK,
    BTN_EVENT_LONG_PRESS_START,
    BTN_EVENT_LONG_PRESS_HOLD
} ButtonEvent_t;

// 消息结构体
typedef struct {
    uint8_t id;           // 按键ID
    ButtonEvent_t event;  // 按键事件
} ButtonMsg_t;

extern QueueHandle_t xButtonQueue;

void button_init(void);
void vButtonTask(void *pvParameters);
void vButtonEventTask(void *pvParameters);
/* ========================= 使用示例 ========================= */
/**
 * @code
 * // 1. 创建按键扫描任务
 * void button_scan_task(void *pvParameters) {
 *     while(1) {
 *         flex_button_scan();
 *         vTaskDelay(pdMS_TO_TICKS(BUTTON_SCAN_PERIOD_MS));
 *     }
 * }
 *
 * // 2. 创建按键处理任务
 * void button_process_task(void *pvParameters) {
 *     ButtonMsg_t msg;
 *     while(1) {
 *         if(xQueueReceive(xButtonQueue, &msg, portMAX_DELAY) == pdPASS) {
 *             switch(msg.id) {
 *                 case 0:  // K1按键
 *                     if(msg.event == BTN_EVENT_SINGLE_CLICK) {
 *                         // 处理K1单击
 *                     }
 *                     break;
 *             }
 *         }
 *     }
 * }
 *
 * // 3. 初始化
 * int main(void) {
 *     // 创建队列
 *     xButtonQueue = xQueueCreate(10, sizeof(ButtonMsg_t));
 *     // 初始化按键
 *     button_init();
 *     // 创建任务...
 * }
 * @endcode
 */
#endif
