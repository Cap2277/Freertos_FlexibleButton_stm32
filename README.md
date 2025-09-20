# FreeRTOS Button Library

基于FlexibleButton库的FreeRTOS按键管理模块，支持多种按键事件检测和队列消息机制。

## 📖 简介

本库是对FlexibleButton的FreeRTOS封装，提供了完整的按键事件处理方案，支持单击、双击、长按等多种按键操作。通过FreeRTOS队列机制实现按键事件的异步处理。

## ✨ 特性

- 🔘 支持多个按键同时管理（当前支持3个按键）
- 🖱️ 丰富的按键事件类型：
  - 单击（Single Click）
  - 双击（Double Click）  
  - 长按开始（Long Press Start）
  - 长按保持（Long Press Hold）
- ⚡ 基于FreeRTOS队列的异步事件处理
- ⏱️ 可配置的按键时序参数
- 🔧 易于移植和扩展
- 🛡️ 防抖处理

## 🛠️ 依赖库

- **FreeRTOS** - 实时操作系统
- **FlexibleButton** - 底层按键检测库
- **STM32 HAL** - 硬件抽象层（GPIO操作）

## 📁 文件结构

```
├── button.h          # 头文件，包含接口定义和数据结构
├── button.c          # 实现文件，按键初始化和事件处理
└── freertos.c        # FreeRTOS任务示例
```

## 🚀 快速开始

### 1. 硬件连接

```
按键1 (K1) -> PB14 (低电平有效)
按键2 (K2) -> PB1  (低电平有效)
按键3 (K3) -> PA0  (低电平有效)
```

### 2. 代码集成

#### 包含头文件
```c
#include "button.h"
#include "flexible_button.h"
```

#### 创建按键队列和任务
```c
void MX_FREERTOS_Init(void) {
    // 创建按键消息队列
    xButtonQueue = xQueueCreate(10, sizeof(ButtonMsg_t));
    
    // 创建按键扫描任务
    xTaskCreate(vButtonTask, "BtnScan", 128, NULL, 2, NULL);
    
    // 创建按键事件处理任务
    xTaskCreate(vButtonEventTask, "BtnEvent", 256, NULL, 1, NULL);
}
```

#### 按键扫描任务
```c
void vButtonTask(void *pvParameters) {
    button_init();  // 初始化按键
    
    for (;;) {
        flex_button_scan();  // 扫描按键状态
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms扫描周期
    }
}
```

#### 按键事件处理任务
```c
void vButtonEventTask(void *pvParameters) {
    ButtonMsg_t msg;
    
    for (;;) {
        if (xQueueReceive(xButtonQueue, &msg, portMAX_DELAY) == pdPASS) {
            switch (msg.event) {
                case BTN_EVENT_SINGLE_CLICK:
                    printf("Button %d Single Click\n", msg.id);
                    break;
                case BTN_EVENT_DOUBLE_CLICK:
                    printf("Button %d Double Click\n", msg.id);
                    break;
                case BTN_EVENT_LONG_PRESS_START:
                    printf("Button %d Long Press Start\n", msg.id);
                    break;
                case BTN_EVENT_LONG_PRESS_HOLD:
                    printf("Button %d Long Press Hold\n", msg.id);
                    break;
            }
        }
    }
}
```

## ⚙️ 配置说明

### 按键时序参数

```c
btn[0].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(300);   // 短按阈值: 300ms
btn[0].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);  // 长按阈值: 1000ms  
btn[0].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);  // 长按保持: 2000ms
```

### GPIO引脚修改

根据您的硬件连接，修改`button.c`中的GPIO读取函数：

```c
uint8_t read_btn_k1(void *btn) { 
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14); 
}
uint8_t read_btn_k2(void *btn) { 
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1); 
}
uint8_t read_btn_k3(void *btn) { 
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0); 
}
```

## 📊 数据结构

### 按键事件类型
```c
typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_PRESS_DOWN,
    BTN_EVENT_PRESS_UP, 
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_DOUBLE_CLICK,
    BTN_EVENT_LONG_PRESS_START,
    BTN_EVENT_LONG_PRESS_HOLD
} ButtonEvent_t;
```

### 按键消息结构
```c
typedef struct {
    uint8_t id;           // 按键ID (0-2)
    ButtonEvent_t event;  // 按键事件类型
} ButtonMsg_t;
```

## 🔧 API 接口

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `button_init()` | 初始化按键模块 | 无 | 无 |
| `vButtonTask()` | 按键扫描任务 | pvParameters | 无 |
| `vButtonEventTask()` | 按键事件处理任务 | pvParameters | 无 |

## 📈 系统要求

- **MCU**: STM32系列
- **RAM**: 最少2KB（用于FreeRTOS任务栈）
- **Flash**: 最少10KB
- **FreeRTOS版本**: V10.0+

## 🔨 移植指南

1. **修改GPIO读取函数**: 根据目标硬件修改`read_btn_kx()`函数
2. **调整按键数量**: 修改`btn`数组大小和初始化代码
3. **时序参数调整**: 根据按键特性调整防抖和检测时间
4. **队列大小**: 根据应用需求调整`xButtonQueue`的大小

## 📝 使用注意事项

1. **初始化顺序**: 必须先初始化GPIO，再调用`button_init()`
2. **扫描频率**: 建议10ms扫描一次，过快会增加CPU负担
3. **队列满处理**: 如果队列满了，新事件会被丢弃
4. **中断使用**: 如果在中断中调用扫描函数，需要使用`xQueueSendFromISR`

## 🐛 故障排除

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| 按键无响应 | GPIO未正确初始化 | 检查GPIO配置和引脚连接 |
| 事件重复触发 | 扫描频率过高 | 调整扫描周期至10-20ms |
| 队列溢出 | 事件处理太慢 | 增加队列大小或优化处理逻辑 |
| 长按失效 | 时序参数不当 | 调整长按阈值参数 |

## 📄 许可证

MIT License - 请参阅 [LICENSE](LICENSE) 文件了解详情。

## 🤝 贡献

欢迎提交Issue和Pull Request！

## 📧 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 [Issue](https://github.com/yourusername/freertos-button-library/issues)
- 发送邮件至：your.email@example.com

## 🔗 相关链接

- [FlexibleButton库](https://github.com/murphyzhao/FlexibleButton)
- [FreeRTOS官网](https://www.freertos.org/)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

---

⭐ 如果这个项目对您有帮助，请给个星标支持！
