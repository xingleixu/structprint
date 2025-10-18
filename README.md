# struct_print.h - C语言结构体调试打印工具

一个轻量级、易用的C语言结构体调试打印工具，专为STM32等嵌入式开发场景设计。

## ✨ 特性

- ✅ **自动打印变量名和变量值** - 无需手动写printf
- ✅ **自动格式化输出** - 支持多级嵌套结构体，缩进清晰
- ✅ **内存信息显示** - 显示地址、偏移量、十六进制数据
- ✅ **使用简单** - 一行代码完成打印
- ✅ **宏控制开关** - Release版本零开销，代码完全被优化掉
- ✅ **不改变结构体定义** - 保持原有代码风格
- ✅ **在线生成工具** - 自动生成描述符代码

## 📋 目录

- [快速开始](#快速开始)
- [详细使用说明](#详细使用说明)
- [Python脚本使用](#python脚本使用)
- [配置选项](#配置选项)
- [STM32移植指南](#stm32移植指南)
- [输出示例](#输出示例)
- [常见问题](#常见问题)

## 🚀 快速开始

### 1. 配置打印函数（可选）

```c
/* 方式1：使用自己已有的打印函数 */
#define STRUCT_PRINT_PRINTF uart_printf

/* 方式2：使用默认的 printf（如果不定义，默认使用标准 printf）*/
// 不定义 STRUCT_PRINT_PRINTF 即可

/* 启用调试打印 */
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

### 2. 实现您的打印函数（如果使用自定义函数）

```c
/* 示例：STM32 HAL库串口输出 */
void uart_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
}
```

### 3. 定义结构体（保持原样）

```c
typedef struct {
    u8 type[32];
    u8 ProtocolType;
    u8 Imei[32];
    int MsgData;
    u32 MeterAdr;
} stCircuitMqttCmdData;
```

### 4. 创建描述符

#### 方式A：使用在线工具自动生成（推荐）⭐

打开 `descriptor_generator.html`，粘贴结构体定义，自动生成描述符代码。

#### 方式B：手动创建

```c
/* 注意：描述符命名规范为 结构体类型名_desc */
BEGIN_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)
    FIELD_STRING(stCircuitMqttCmdData, type),
    FIELD_U8(stCircuitMqttCmdData, ProtocolType),
    FIELD_STRING(stCircuitMqttCmdData, Imei),
    FIELD_INT(stCircuitMqttCmdData, MsgData),
    FIELD_U32(stCircuitMqttCmdData, MeterAdr)
END_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)
```

### 5. 使用打印

```c
stCircuitMqttCmdData cmd;
// ... 初始化数据 ...

/* C99 环境（需要指定类型名）*/
STRUCT_PRINT(cmd, stCircuitMqttCmdData);

/* C11 环境（自动推导类型）*/
STRUCT_PRINT(cmd);  /* 需要配置 GET_STRUCT_DESC 宏 */
```

## 📖 详细使用说明

### 支持的数据类型

| 宏定义 | 对应C类型 | 说明 |
|--------|----------|------|
| `FIELD_U8()` | `uint8_t`, `u8`, `unsigned char` | 无符号8位整数 |
| `FIELD_U16()` | `uint16_t`, `u16`, `unsigned short` | 无符号16位整数 |
| `FIELD_U32()` | `uint32_t`, `u32`, `unsigned int` | 无符号32位整数 |
| `FIELD_S8()` | `int8_t`, `s8`, `char` | 有符号8位整数 |
| `FIELD_S16()` | `int16_t`, `s16`, `short` | 有符号16位整数 |
| `FIELD_S32()` / `FIELD_INT()` | `int32_t`, `s32`, `int` | 有符号32位整数 |
| `FIELD_FLOAT()` | `float` | 单精度浮点数 |
| `FIELD_DOUBLE()` | `double` | 双精度浮点数 |
| `FIELD_STRING()` | `char[]`, `u8[]` | 字符串（字符数组）|
| `FIELD_ARRAY()` | 任意类型数组 | 数组类型 |
| `FIELD_STRUCT()` | 嵌套结构体 | 嵌套结构体 |

### 字段类型使用示例

```c
typedef struct {
    u8 id;                    // 无符号8位
    u16 version;              // 无符号16位
    u32 serial;               // 无符号32位
    int count;                // 有符号整数
    float temperature;        // 浮点数
    u8 name[32];              // 字符串
    u16 data[10];             // 数组
    DeviceInfo device;        // 嵌套结构体
} MyStruct;

BEGIN_STRUCT_DESC(MyStruct, MyStruct_desc)
    FIELD_U8(MyStruct, id),
    FIELD_U16(MyStruct, version),
    FIELD_U32(MyStruct, serial),
    FIELD_INT(MyStruct, count),
    FIELD_FLOAT(MyStruct, temperature),
    FIELD_STRING(MyStruct, name),
    FIELD_ARRAY(MyStruct, data, FIELD_TYPE_U16),
    FIELD_STRUCT(MyStruct, device, DeviceInfo_desc)
END_STRUCT_DESC(MyStruct, MyStruct_desc)
```

### 嵌套结构体支持

```c
/* 定义两个结构体 */
typedef struct {
    u8 sensor_id;
    s16 value;
} SensorData;

typedef struct {
    u32 timestamp;
    SensorData sensor;  /* 嵌套 */
} SystemStatus;

/* 先定义内层结构体描述符 */
BEGIN_STRUCT_DESC(SensorData, SensorData_desc)
    FIELD_U8(SensorData, sensor_id),
    FIELD_S16(SensorData, value)
END_STRUCT_DESC(SensorData, SensorData_desc)

/* 再定义外层结构体描述符 */
BEGIN_STRUCT_DESC(SystemStatus, SystemStatus_desc)
    FIELD_U32(SystemStatus, timestamp),
    FIELD_STRUCT(SystemStatus, sensor, SensorData_desc)  /* 引用内层描述符 */
END_STRUCT_DESC(SystemStatus, SystemStatus_desc)

/* 使用 */
SystemStatus status;
STRUCT_PRINT(status, SystemStatus);  /* C99: 传入类型名 */
```

## 🛠️ 描述符生成工具

### descriptor_generator.html - 在线描述符生成器（推荐）

提供了一个简单易用的 HTML 在线工具，无需安装 Python 环境。

### 使用方法

1. 在浏览器中打开 `descriptor_generator.html`
2. 粘贴您的结构体定义到输入框
3. 点击"生成描述符"按钮
4. 复制生成的描述符代码到您的项目中

### 工具特性

- ✅ 无需安装，浏览器直接使用
- ✅ 自动识别 `typedef struct` 和普通 `struct` 定义
- ✅ 支持各种基本类型（u8、u16、u32、int、float等）
- ✅ 自动识别数组类型
- ✅ 自动识别字符串（字符数组）
- ✅ 支持嵌套结构体
- ✅ 自动生成符合规范的描述符名称（类型名_desc）
- ✅ 实时预览生成结果

### 输入示例

```c
/* 输入框中粘贴 */
typedef struct {
    u8 type[32];
    u8 ProtocolType;
    int MsgData;
    u32 MeterAdr;
} stCircuitMqttCmdData;
```

### 输出示例

```c
/* 自动生成的描述符代码 */
BEGIN_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)
    FIELD_STRING(stCircuitMqttCmdData, type),
    FIELD_U8(stCircuitMqttCmdData, ProtocolType),
    FIELD_S32(stCircuitMqttCmdData, MsgData),
    FIELD_U32(stCircuitMqttCmdData, MeterAdr)
END_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)
```

## ⚙️ 配置选项

在 `struct_print.h` 中可以配置以下选项：

```c
/* 是否显示内存地址 */
#define STRUCT_PRINT_SHOW_ADDRESS       1

/* 是否显示字段偏移量 */
#define STRUCT_PRINT_SHOW_OFFSET        1

/* 是否显示内存十六进制数据 */
#define STRUCT_PRINT_SHOW_HEX_MEMORY    1

/* 十六进制内存显示的字节数 */
#define STRUCT_PRINT_HEX_BYTES          16

/* 嵌套层级的缩进空格数 */
#define STRUCT_PRINT_INDENT_SPACES      2
```

## 🔧 STM32移植指南

### 步骤1：添加文件到项目

将 `struct_print.h` 复制到项目的 `Inc` 或 `Core/Inc` 目录。

### 步骤2：配置打印函数

根据你的输出方式选择配置：

#### 方式A：使用已有的打印函数

如果您的项目中已经有打印函数（如 `uart_printf`），只需配置宏：

```c
/* main.c */
#define STRUCT_PRINT_PRINTF uart_printf  /* 使用您的函数 */
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

#### 方式B：创建新的UART输出函数

```c
#include <stdarg.h>
#include <stdio.h>
#include "usart.h"  /* 你的串口头文件 */

/* 实现您的打印函数 */
void my_uart_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
}

/* 配置使用此函数 */
#define STRUCT_PRINT_PRINTF my_uart_printf
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

#### 方式C：通过SWO/ITM输出

```c
void itm_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    for (int i = 0; i < len; i++) {
        ITM_SendChar(buffer[i]);
    }
}

#define STRUCT_PRINT_PRINTF itm_printf
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

#### 方式D：通过RTT输出（SEGGER J-Link）

```c
#include "SEGGER_RTT.h"

void rtt_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    SEGGER_RTT_Write(0, buffer, len);
}

#define STRUCT_PRINT_PRINTF rtt_printf
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

#### 方式E：使用标准 printf（如果已重定向）

```c
/* 如果已将 printf 重定向到串口，直接使用默认配置 */
#define STRUCT_PRINT_ENABLE  /* 不定义 STRUCT_PRINT_PRINTF */
#include "struct_print.h"
```

### 步骤3：在代码中使用

```c
/* main.c */
#define STRUCT_PRINT_PRINTF uart_printf  /* 配置打印函数 */
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"

/* 定义描述符（或使用 descriptor_generator.html 生成）*/
BEGIN_STRUCT_DESC(MyStruct, MyStruct_desc)
    /* ... 字段定义 ... */
END_STRUCT_DESC(MyStruct, MyStruct_desc)

int main(void) {
    HAL_Init();
    /* ... 初始化代码 ... */
    
    MyStruct data;
    /* ... 填充数据 ... */
    
    STRUCT_PRINT(data, MyStruct);  /* C99: 打印调试信息 */
    
    while(1) {
        /* ... */
    }
}
```

### 步骤4：Release版本禁用

在Release版本的编译选项中，不定义 `STRUCT_PRINT_ENABLE`，所有调试代码将被完全优化掉。

**Keil MDK示例：**
- Debug配置：C/C++ 预定义符号中添加 `STRUCT_PRINT_ENABLE`
- Release配置：不添加该符号

**STM32CubeIDE示例：**
- Debug配置：Project Properties → C/C++ Build → Settings → MCU GCC Compiler → Preprocessor → 添加 `STRUCT_PRINT_ENABLE`
- Release配置：不添加

## 📺 输出示例

运行 `example.c`，将看到类似以下的输出：

```
========================================
Struct: cmd [stCircuitMqttCmdData]
Address: 0x20000100
Size: 612 bytes
========================================
  [+0x0000] type: "request"
            └─ Memory: 72 65 71 75 65 73 74 00 00 00 00 00 00 00 00 00 

  [+0x0021] ProtocolType: 1 (0x01)
            └─ Memory: 01 

  [+0x0022] Imei: "862123456789012"
            └─ Memory: 38 36 32 31 32 33 34 35 36 37 38 39 30 31 32 00 

  [+0x0043] MsgType: "CONNECT"
            └─ Memory: 43 4F 4E 4E 45 43 54 00 00 00 00 00 00 00 00 00 

  [+0x0064] MsgData: 12345 (0x00003039)
            └─ Memory: 39 30 00 00 

  [+0x0068] MsgDataString: "Hello STM32!"
            └─ Memory: 48 65 6C 6C 6F 20 53 54 4D 33 32 21 00 00 00 00 

  [+0x0268] MeterAdr: 255 (0x000000FF)
            └─ Memory: FF 00 00 00 
========================================
```

## ❓ 常见问题

### Q1: 为什么需要手动定义描述符？

**A:** C语言没有反射机制，无法在运行时获取结构体字段信息。描述符表是编译时确定的元数据，用于指导打印引擎。不过我们提供了在线工具 `descriptor_generator.html` 自动生成描述符，大大减少了工作量。

### Q2: 描述符会占用多少内存？

**A:** 描述符使用 `static const` 定义，存储在Flash（只读存储器）中，不占用RAM。在Release版本中，如果不定义 `STRUCT_PRINT_ENABLE`，所有代码都会被编译器优化掉，完全不占用空间。

### Q3: 如果结构体改变了怎么办？

**A:** 有两种情况：
1. **使用在线工具生成**：在 `descriptor_generator.html` 中重新生成即可
2. **手动定义**：如果字段类型或名称写错，编译器会报错（使用了`offsetof`和`sizeof`，会进行类型检查）

### Q4: 支持联合体（union）吗？

**A:** 当前版本不直接支持联合体。建议将联合体当作固定类型（如字节数组）打印，或者为每种情况定义不同的描述符。

### Q5: 浮点数打印精度可以调整吗？

**A:** 可以。在 `struct_print.h` 的 `print_field_value()` 函数中，找到浮点数打印部分：
```c
STRUCT_PRINT_PRINTF("%.6f\n", val);  /* 修改 .6 为你需要的精度 */
```

### Q6: 可以只打印部分字段吗？

**A:** 可以。在描述符定义中，只包含你想打印的字段即可。或者创建多个描述符，根据需要选择使用。

### Q7: 在没有标准库的裸机环境怎么办？

**A:** `struct_print.h` 尽量减少了对标准库的依赖。如果你的环境不支持 `vsnprintf`，可以简化您的打印函数实现，或者自己实现一个简单的格式化函数。

### Q8: 如何在多线程/中断环境使用？

**A:** 需要确保您配置的打印函数输出是线程安全的。建议：
1. 加锁保护输出函数
2. 或者使用环形缓冲区，在主循环中输出
3. 不要在中断中打印大结构体（可能阻塞太久）

### Q9: 如何在 C11 环境下使用单参数版本？

**A:** C11 单参数版本 `STRUCT_PRINT(var)` 需要配置 `GET_STRUCT_DESC` 宏。建议使用 `descriptor_generator.html` 生成描述符时，勾选"完整模式"选项，或者手动定义：
```c
#define GET_STRUCT_DESC(var) _Generic((var), \
    MyStruct: &MyStruct_desc, \
    DeviceInfo: &DeviceInfo_desc, \
    default: NULL \
)
```

## 📝 完整示例

请参考 `example.c` 文件，包含了：
- ✅ 基本类型打印
- ✅ 数组和字符串打印
- ✅ 浮点数打印
- ✅ 嵌套结构体打印
- ✅ 各种边界情况测试

编译运行示例：
```bash
gcc -o example example.c
./example
```

## 📦 文件清单

```
structprintv2/
├── struct_print.h              # 核心头文件（唯一需要包含的文件）
├── descriptor_generator.html   # 在线描述符生成工具
├── example.c                   # 完整使用示例（C11/C99）
└── README.md                   # 本文档
```

## 📄 许可证

本项目采用 MIT 许可证，可自由用于个人和商业项目。

## 🤝 贡献

欢迎提交Issue和Pull Request！

## 📮 联系方式

如有问题或建议，请提交Issue。

---

**祝调试愉快！🎉**

