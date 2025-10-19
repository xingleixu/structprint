# struct_print.h - C语言结构体调试打印工具

> **版本:** v1.0  
> **更新日期:** 2025-10-18  
> **作者:** xingleixu@gmail.com

一个轻量级、易用的C语言结构体调试打印工具，专为STM32等嵌入式开发场景设计。

## ✨ 特性

- ✅ **自动打印变量名和变量值** - 无需手动写printf，告别繁琐的调试代码
- ✅ **自动格式化输出** - 支持任意深度的嵌套结构体，层次清晰，一目了然
- ✅ **内存信息显示** - 显示内存地址、字段偏移量、十六进制数据，便于底层调试
- ✅ **智能类型识别** - 自动识别字符串、数组、嵌套结构体，无需额外标注
- ✅ **使用超简单** - 一行代码 `STRUCT_PRINT(var)` 完成打印
- ✅ **Release零开销** - Release版本完全优化为空，不占用任何Flash或RAM
- ✅ **C99/C11兼容** - 自动检测编译器特性，智能选择最佳实现方式
- ✅ **不改变结构体定义** - 完全保持原有代码风格，无侵入性
- ✅ **在线生成工具** - 提供可视化工具，一键自动生成描述符代码
- ✅ **高度可配置** - 灵活配置显示选项、打印函数、格式化参数
- ✅ **完全开源** - MIT许可证，可自由用于商业项目

## 🎯 为什么选择 struct_print.h？

| 对比项 | 传统 printf 方式 | struct_print.h |
|-------|----------------|----------------|
| **代码量** | 每个字段写一行 printf | 一行 STRUCT_PRINT() |
| **维护性** | 结构体变更需同步修改所有打印代码 | 只需重新生成描述符 |
| **可读性** | 输出格式混乱，难以阅读 | 自动格式化，层次清晰 |
| **内存信息** | 需要手动计算偏移量和地址 | 自动显示地址和偏移量 |
| **Release影响** | 需要手动删除或注释所有调试代码 | 自动优化为空，零开销 |
| **嵌套结构体** | 手动递归打印，代码复杂 | 自动递归展开 |
| **数组显示** | 需要循环打印 | 自动智能显示 |

## 📋 目录

- [✨ 特性](#特性)
- [🎯 为什么选择 struct_print.h？](#为什么选择-struct_printh)
- [🚀 快速开始](#快速开始)
- [📖 详细使用说明](#详细使用说明)
  - [编译器兼容性](#编译器兼容性)
  - [支持的数据类型](#支持的数据类型)
  - [数组和字符串的智能识别](#数组和字符串的智能识别)
  - [字段类型使用示例](#字段类型使用示例)
  - [嵌套结构体支持](#嵌套结构体支持)
- [🛠️ 描述符生成工具](#描述符生成工具)
- [⚙️ 配置选项](#配置选项)
- [🔧 STM32移植指南](#stm32移植指南)
- [📺 输出示例](#输出示例)
- [❓ 常见问题](#常见问题)
- [📝 完整示例](#完整示例)
- [📦 文件清单](#文件清单)
- [📄 许可证](#许可证)

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
                     /* 工具会自动检测 C11 _Generic 支持 */
```

## 📖 详细使用说明

### 编译器兼容性

本工具自动检测编译器对 C11 `_Generic` 特性的支持：

| 编译器环境 | 检测方式 | 宏定义 | 使用方式 |
|----------|---------|--------|----------|
| **C11 及以上** | `__STDC_VERSION__ >= 201112L` | `STRUCT_PRINT_HAS_GENERIC = 1` | `STRUCT_PRINT(var)` - 单参数 |
| **C99 及以下** | `__STDC_VERSION__ < 201112L` | `STRUCT_PRINT_HAS_GENERIC = 0` | `STRUCT_PRINT(var, Type)` - 两参数 |

**说明：**
- 工具会自动检测并选择合适的宏定义版本
- C11 单参数版本需要配置 `GET_STRUCT_DESC` 宏（由在线工具自动生成）
- C99 两参数版本无需额外配置，直接使用即可
- 大多数现代嵌入式编译器（如 ARM GCC 6.0+）都支持 C11

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

**类型别名支持：**

工具自动定义了 STM32 等嵌入式开发中常用的类型别名（如果系统未定义）：

```c
typedef uint8_t  u8;    // 无符号8位
typedef uint16_t u16;   // 无符号16位
typedef uint32_t u32;   // 无符号32位
typedef int8_t   s8;    // 有符号8位
typedef int16_t  s16;   // 有符号16位
typedef int32_t  s32;   // 有符号32位
```

这意味着您可以在结构体中自由使用 `u8`、`u16`、`u32` 等类型，无需担心兼容性问题。

### 数组和字符串的智能识别

工具提供了智能的数组和字符串处理：

**字符串自动识别：**
- 使用 `FIELD_STRING()` 明确定义为字符串的字段会按字符串显示
- 使用 `FIELD_ARRAY(struct, field, FIELD_TYPE_U8)` 定义的 `u8` 数组，如果内容是可打印字符，也会自动识别为字符串
- 字符串显示格式：`"Hello World"`，并附带十六进制内存显示

**数值数组显示：**
- 数值类型的数组（如 `u16[]`, `s32[]`）会显示为数组格式：`[1, 2, 3, 4, 5]`
- 如果数组元素超过 16 个，只显示前 16 个，后面显示 `...`
- 示例：`data: [10, 20, 30, 40, 50, ...]`

**字符串可打印性检测：**
工具会自动检测字符数组是否为有效字符串：
- 检查是否包含空终止符 `\0`
- 检查字符是否在可打印范围内（ASCII 0x20-0x7E）
- 如果不满足条件，即使是 `u8` 数组也会显示为数值数组

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

工具支持任意深度的结构体嵌套，会自动递归展开并以缩进方式显示层次关系。

**定义规则：**
1. **必须先定义内层结构体的描述符**，再定义外层的
2. 使用 `FIELD_STRUCT(外层类型, 字段名, 内层描述符)` 引用嵌套结构体
3. 嵌套结构体会自动递归打印，每层增加相应的缩进

**示例：**

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

/* 步骤1：先定义内层结构体描述符 */
BEGIN_STRUCT_DESC(SensorData, SensorData_desc)
    FIELD_U8(SensorData, sensor_id),
    FIELD_S16(SensorData, value)
END_STRUCT_DESC(SensorData, SensorData_desc)

/* 步骤2：再定义外层结构体描述符 */
BEGIN_STRUCT_DESC(SystemStatus, SystemStatus_desc)
    FIELD_U32(SystemStatus, timestamp),
    FIELD_STRUCT(SystemStatus, sensor, SensorData_desc)  /* 引用内层描述符 */
END_STRUCT_DESC(SystemStatus, SystemStatus_desc)

/* 步骤3：使用 */
SystemStatus status;
STRUCT_PRINT(status, SystemStatus);  /* C99: 传入类型名 */
```

**打印效果：**

```text
========================================
Struct: status [SystemStatus]
Address: 0x20001000
Size: 8 bytes
========================================
  [+0x0000] timestamp: 1697612345 (0x6534A019)
            └─ Memory: 19 A0 34 65

  [+0x0004] sensor:
  ========================================
  Struct: [SensorData]
  Address: 0x20001004
  Size: 4 bytes
  ========================================
    [+0x0000] sensor_id: 100 (0x64)
              └─ Memory: 64
    
    [+0x0001] value: -273 (0xFEEF)
              └─ Memory: EF FE
  ========================================
========================================
```

**支持多层嵌套：**
您可以嵌套任意多层，例如：结构体A 包含 结构体B，结构体B 包含 结构体C，工具会自动递归展开所有层级。

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

/* 数组作为字符串显示的最大长度（超过则显示为数值数组） */
#define STRUCT_PRINT_STRING_MAX_LEN     512

/* 十六进制内存显示的字节数（每个字段） */
#define STRUCT_PRINT_HEX_BYTES          16

/* 嵌套层级的缩进空格数 */
#define STRUCT_PRINT_INDENT_SPACES      2
```

**配置说明：**

- **STRUCT_PRINT_SHOW_ADDRESS**：控制是否显示结构体和字段的内存地址
  - 值为 `1` 时显示，`0` 时隐藏
  - 显示示例：`Address: 0x20000100`

- **STRUCT_PRINT_SHOW_OFFSET**：控制是否显示字段相对于结构体起始地址的偏移量
  - 值为 `1` 时显示，`0` 时隐藏
  - 显示示例：`[+0x0000] type: "request"`

- **STRUCT_PRINT_SHOW_HEX_MEMORY**：控制是否显示字段的十六进制内存数据
  - 值为 `1` 时显示，`0` 时隐藏
  - 显示示例：`└─ Memory: 72 65 71 75 65 73 74 00`

- **STRUCT_PRINT_STRING_MAX_LEN**：字符串最大显示长度
  - 超过此长度的字符数组将显示为数值数组而非字符串
  - 默认值：512 字节

- **STRUCT_PRINT_HEX_BYTES**：每个字段显示的十六进制字节数
  - 如果字段大小超过此值，将显示省略号
  - 默认值：16 字节

- **STRUCT_PRINT_INDENT_SPACES**：嵌套结构体的缩进空格数
  - 每增加一层嵌套，增加相应数量的空格
  - 默认值：2 个空格

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

### 步骤4：Release版本禁用（零开销）

在Release版本的编译选项中，不定义 `STRUCT_PRINT_ENABLE`，所有调试代码将被完全优化掉，**不会占用任何 Flash 或 RAM 空间**。

**工作原理：**
- 当 `STRUCT_PRINT_ENABLE` 未定义时，所有宏都会展开为空语句 `((void)0)`
- 编译器会完全优化掉这些空语句，不产生任何机器码
- 描述符定义也会变成空宏，不占用 Flash 空间
- 这意味着您可以在代码中保留所有 `STRUCT_PRINT()` 调用，而不必担心 Release 版本的性能和空间开销

**编译器配置示例：**

**Keil MDK：**
- Debug配置：Options for Target → C/C++ → Preprocessor Symbols → Define → 添加 `STRUCT_PRINT_ENABLE`
- Release配置：不添加该符号

**STM32CubeIDE (GCC)：**
- Debug配置：Project Properties → C/C++ Build → Settings → MCU GCC Compiler → Preprocessor → Defined symbols → 添加 `STRUCT_PRINT_ENABLE`
- Release配置：不添加该符号

**IAR EWARM：**
- Debug配置：Options → C/C++ Compiler → Preprocessor → Defined symbols → 添加 `STRUCT_PRINT_ENABLE`
- Release配置：不添加该符号

**编译验证：**
```bash
# Debug 版本（启用打印）
arm-none-eabi-gcc -DSTRUCT_PRINT_ENABLE -O2 -c main.c

# Release 版本（禁用打印，零开销）
arm-none-eabi-gcc -O2 -c main.c
```

## 📺 输出示例

运行 `example.c`，将看到类似以下的输出：

```text
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

### Q10: 打印操作会影响性能吗？

**A:** 在 Debug 模式下：
- 打印操作确实会消耗时间（主要是串口/输出的时间）
- 不建议在高频率中断或实时性要求极高的代码中使用
- 建议在初始化、配置变更、错误处理等低频场景使用

在 Release 模式下：
- 如果不定义 `STRUCT_PRINT_ENABLE`，**完全零开销**，不产生任何代码
- 这意味着您可以保留所有 `STRUCT_PRINT()` 调用，不影响 Release 版本性能

### Q11: 可以打印指针指向的结构体吗？

**A:** 可以。您需要解引用指针：
```c
MyStruct* ptr = get_struct_pointer();

/* C99 模式 */
STRUCT_PRINT(*ptr, MyStruct);

/* C11 模式 */
STRUCT_PRINT(*ptr);
```

或者使用临时变量：
```c
MyStruct temp = *ptr;
STRUCT_PRINT(temp, MyStruct);
```

### Q12: 支持结构体数组吗？

**A:** 需要手动遍历数组：
```c
DeviceInfo devices[10];

for (int i = 0; i < 10; i++) {
    printf("Device [%d]:\n", i);
    STRUCT_PRINT(devices[i], DeviceInfo);
}
```

未来版本可能会添加数组批量打印功能。

### Q13: 打印输出到文件而不是串口？

**A:** 可以。定义一个输出到文件的函数：
```c
void file_printf(const char* format, ...) {
    static FILE* fp = NULL;
    if (!fp) fp = fopen("debug.log", "a");
    
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);
    fflush(fp);
}

#define STRUCT_PRINT_PRINTF file_printf
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

### Q14: 如何只打印结构体的部分字段？

**A:** 有两种方法：
1. **创建部分描述符**：只在描述符中包含需要打印的字段
```c
BEGIN_STRUCT_DESC(MyStruct, MyStruct_partial_desc)
    FIELD_U8(MyStruct, id),        /* 只打印这两个字段 */
    FIELD_U32(MyStruct, status)
END_STRUCT_DESC(MyStruct, MyStruct_partial_desc)

struct_print("my_var", &my_var, &MyStruct_partial_desc);
```

2. **配置显示选项**：关闭不需要的信息（如十六进制内存）
```c
#define STRUCT_PRINT_SHOW_HEX_MEMORY 0  /* 关闭内存显示 */
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

```text
structprint/
├── struct_print.h              # 核心头文件（唯一需要包含的文件）
├── descriptor_generator.html   # 在线描述符生成工具（推荐使用）
├── example.c                   # 完整使用示例（C99/C11）
├── test_structs.h              # 测试用结构体定义
├── Makefile                    # 编译配置文件
├── LICENSE                     # MIT 许可证
└── README.md                   # 本文档（使用指南）
```

**核心文件说明：**
- **struct_print.h** - 唯一必需的文件，包含所有功能实现
- **descriptor_generator.html** - 强烈推荐使用的在线工具，自动生成描述符代码
- **example.c** - 演示如何在实际项目中使用，包含多种场景示例

## 📄 许可证

本项目采用 MIT 许可证，可自由用于个人和商业项目。

## 🔖 快速参考

### 宏定义速查表

**结构体描述符定义：**
```c
BEGIN_STRUCT_DESC(结构体类型, 描述符名称)
    字段定义列表...
END_STRUCT_DESC(结构体类型, 描述符名称)
```

**字段类型宏：**
| 宏 | 用途 | 示例 |
|---|------|------|
| `FIELD_U8(type, field)` | 无符号8位整数 | `FIELD_U8(MyStruct, id)` |
| `FIELD_U16(type, field)` | 无符号16位整数 | `FIELD_U16(MyStruct, version)` |
| `FIELD_U32(type, field)` | 无符号32位整数 | `FIELD_U32(MyStruct, serial)` |
| `FIELD_S8(type, field)` | 有符号8位整数 | `FIELD_S8(MyStruct, temp)` |
| `FIELD_S16(type, field)` | 有符号16位整数 | `FIELD_S16(MyStruct, offset)` |
| `FIELD_S32(type, field)` / `FIELD_INT` | 有符号32位整数 | `FIELD_INT(MyStruct, count)` |
| `FIELD_FLOAT(type, field)` | 单精度浮点数 | `FIELD_FLOAT(MyStruct, temperature)` |
| `FIELD_DOUBLE(type, field)` | 双精度浮点数 | `FIELD_DOUBLE(MyStruct, voltage)` |
| `FIELD_STRING(type, field)` | 字符串/字符数组 | `FIELD_STRING(MyStruct, name)` |
| `FIELD_ARRAY(type, field, elem_type)` | 数组 | `FIELD_ARRAY(MyStruct, data, FIELD_TYPE_U16)` |
| `FIELD_STRUCT(type, field, desc)` | 嵌套结构体 | `FIELD_STRUCT(MyStruct, device, DeviceInfo_desc)` |

**打印宏：**
```c
/* C99 模式（两参数） */
STRUCT_PRINT(变量名, 类型名);

/* C11 模式（单参数，需配置 GET_STRUCT_DESC） */
STRUCT_PRINT(变量名);
```

**配置宏：**
```c
#define STRUCT_PRINT_ENABLE              // 启用打印功能
#define STRUCT_PRINT_PRINTF my_printf     // 配置打印函数

#define STRUCT_PRINT_SHOW_ADDRESS    1    // 显示地址
#define STRUCT_PRINT_SHOW_OFFSET     1    // 显示偏移量
#define STRUCT_PRINT_SHOW_HEX_MEMORY 1    // 显示十六进制内存
#define STRUCT_PRINT_STRING_MAX_LEN  512  // 字符串最大长度
#define STRUCT_PRINT_HEX_BYTES       16   // 十六进制显示字节数
#define STRUCT_PRINT_INDENT_SPACES   2    // 缩进空格数
```

### 常用代码片段

**最小配置示例：**
```c
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"

BEGIN_STRUCT_DESC(MyStruct, MyStruct_desc)
    FIELD_U8(MyStruct, id),
    FIELD_U32(MyStruct, value)
END_STRUCT_DESC(MyStruct, MyStruct_desc)

MyStruct data;
STRUCT_PRINT(data, MyStruct);
```

**STM32 UART 输出示例：**
```c
void uart_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
}

#define STRUCT_PRINT_PRINTF uart_printf
#define STRUCT_PRINT_ENABLE
#include "struct_print.h"
```

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

如果这个工具对您有帮助，请给项目一个 ⭐ Star！

## 📮 联系方式

- **作者邮箱:** xingleixu@gmail.com
- **问题反馈:** 请提交 GitHub Issue
- **功能建议:** 欢迎通过 Issue 讨论

## 📊 项目统计

- **代码行数:** < 800 行（包含注释）
- **依赖库:** 仅依赖标准库（`stddef.h`, `stdint.h`, `string.h`, `stdio.h`）
- **支持平台:** 所有支持 C99/C11 的平台
- **测试平台:** STM32, ARM Cortex-M, x86/x64 Linux/Windows

---

**祝调试愉快！🎉**

> 如果在使用过程中遇到问题或有改进建议，欢迎随时联系！
