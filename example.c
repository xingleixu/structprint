/**
 * @file example.c
 * @brief struct_print.h 完整使用示例（C99/C11 双模式）
 * @author xingleixu@gmail.com
 * @date 2025-10-18
 * 
 * 本示例展示了如何使用 struct_print.h 工具打印STM32开发中的结构体
 * 
 * 编译选项：
 *   gcc -std=c99 example.c  → 使用 C99 两参数模式
 *   gcc -std=c11 example.c  → 同时支持 C99 和 C11 单参数模式
 * 
 * 测试内容：
 *   测试1-4: 演示 C99 两参数模式（兼容 C11）
 *   测试5:   演示 C11 单参数自动推导模式
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* ============================================================================
 *                   步骤1：定义您的打印函数
 * ============================================================================ */

/**
 * @brief 自定义打印函数示例
 * @note 本示例使用 my_printf，您可以使用任何名字
 *       只要函数签名为：void func(const char* format, ...)
 */
void my_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);  /* 本示例使用标准输出 */
    va_end(args);
}


/* ============================================================================
 *                   步骤2：配置并包含头文件
 * ============================================================================ */

/* 配置使用您的打印函数 */
#define STRUCT_PRINT_PRINTF my_printf

/* 启用结构体打印功能 */
#define STRUCT_PRINT_ENABLE

/* 包含核心头文件 */
#include "struct_print.h"

/* ============================================================================
 *                   STM32 环境配置参考
 * ============================================================================ */

/**
 * 方式1：使用已有的打印函数（推荐）
 * ================================
 * 如果您的项目已有 uart_printf、log_printf 等函数，直接配置即可
 * 
 * #define STRUCT_PRINT_PRINTF uart_printf
 * #define STRUCT_PRINT_ENABLE
 * #include "struct_print.h"
 */

/**
 * 方式2：创建 UART 输出函数
 * ================================
 * void uart_printf(const char* format, ...) {
 *     char buffer[256];
 *     va_list args;
 *     va_start(args, format);
 *     int len = vsnprintf(buffer, sizeof(buffer), format, args);
 *     va_end(args);
 *     HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
 * }
 * 
 * #define STRUCT_PRINT_PRINTF uart_printf
 * #define STRUCT_PRINT_ENABLE
 * #include "struct_print.h"
 */

/**
 * 方式3：使用 SWO/ITM 输出
 * ================================
 * void itm_printf(const char* format, ...) {
 *     char buffer[256];
 *     va_list args;
 *     va_start(args, format);
 *     int len = vsnprintf(buffer, sizeof(buffer), format, args);
 *     va_end(args);
 *     for (int i = 0; i < len; i++) {
 *         ITM_SendChar(buffer[i]);
 *     }
 * }
 * 
 * #define STRUCT_PRINT_PRINTF itm_printf
 * #define STRUCT_PRINT_ENABLE
 * #include "struct_print.h"
 */

/**
 * 方式4：使用标准 printf（如果已重定向到串口）
 * ================================
 * // 不定义 STRUCT_PRINT_PRINTF，默认使用 printf
 * #define STRUCT_PRINT_ENABLE
 * #include "struct_print.h"
 */


/* ============================================================================
 *                   定义测试用的结构体（保持原样定义）
 * ============================================================================ */

/* 常量定义 */
#define stCircuitCmdDataLenMax      32
#define stCircuitCmdDataMsgLenMax   512

/**
 * @brief 服务器命令结构体（示例）
 * @note 这是您提供的原始结构体定义，完全不需要修改
 */
typedef struct
{
    u8 type[stCircuitCmdDataLenMax+1];          /* 消息类型 */
    u8 ProtocolType;                            /* 协议类型  1：数码管协议 2：液晶屏协议 */
    u8 Imei[stCircuitCmdDataLenMax+1];          /* 设备唯一ID */
    u8 MsgType[stCircuitCmdDataLenMax+1];       /* 报文控制命令 */
    int MsgData;                                /* 报文实体：可为空 */
    u8 MsgDataString[512];                      /* 报文实体：字符串数据 */
    u32 MeterAdr;                               /* 控制485设备的地址 */
} stCircuitMqttCmdData;


/**
 * @brief 设备信息结构体（示例）
 */
typedef struct
{
    u8 device_id;
    u16 firmware_version;
    u32 serial_number;
    float temperature;
    double voltage;
} DeviceInfo;


/**
 * @brief 嵌套结构体示例 - 传感器数据
 */
typedef struct
{
    u16 sensor_id;
    s16 value;
    u8 status;
} SensorData;

/**
 * @brief 嵌套结构体示例 - 系统状态（包含嵌套结构体）
 */
typedef struct
{
    u32 timestamp;
    DeviceInfo device;      /* 嵌套结构体 */
    SensorData sensor;      /* 嵌套结构体 */
    u8 error_code;
} SystemStatus;


/* ============================================================================
 *                   步骤3：定义结构体描述符
 * ============================================================================ */

/**
 * 描述符定义方式：
 * 
 * 方式A：使用 descriptor_generator.html 在线工具生成（推荐）
 *        - 打开 descriptor_generator.html
 *        - 粘贴结构体定义
 *        - 选择生成模式（Complete/Append/Desc Only）
 *        - 复制生成的代码
 * 
 * 方式B：手动编写（本示例采用此方式）
 *        - 使用 BEGIN_STRUCT_DESC / END_STRUCT_DESC 宏
 *        - 为每个字段选择对应的 FIELD_XXX 宏
 *        - 命名规范：类型名_desc
 */

/* stCircuitMqttCmdData 的描述符 */
BEGIN_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)
    FIELD_STRING(stCircuitMqttCmdData, type),
    FIELD_U8(stCircuitMqttCmdData, ProtocolType),
    FIELD_STRING(stCircuitMqttCmdData, Imei),
    FIELD_STRING(stCircuitMqttCmdData, MsgType),
    FIELD_INT(stCircuitMqttCmdData, MsgData),
    FIELD_STRING(stCircuitMqttCmdData, MsgDataString),
    FIELD_U32(stCircuitMqttCmdData, MeterAdr)
END_STRUCT_DESC(stCircuitMqttCmdData, stCircuitMqttCmdData_desc)

/* DeviceInfo 的描述符 */
BEGIN_STRUCT_DESC(DeviceInfo, DeviceInfo_desc)
    FIELD_U8(DeviceInfo, device_id),
    FIELD_U16(DeviceInfo, firmware_version),
    FIELD_U32(DeviceInfo, serial_number),
    FIELD_FLOAT(DeviceInfo, temperature),
    FIELD_DOUBLE(DeviceInfo, voltage)
END_STRUCT_DESC(DeviceInfo, DeviceInfo_desc)

/* SensorData 的描述符 */
BEGIN_STRUCT_DESC(SensorData, SensorData_desc)
    FIELD_U16(SensorData, sensor_id),
    FIELD_S16(SensorData, value),
    FIELD_U8(SensorData, status)
END_STRUCT_DESC(SensorData, SensorData_desc)

/* SystemStatus 的描述符（包含嵌套结构体） */
BEGIN_STRUCT_DESC(SystemStatus, SystemStatus_desc)
    FIELD_U32(SystemStatus, timestamp),
    FIELD_STRUCT(SystemStatus, device, DeviceInfo_desc),
    FIELD_STRUCT(SystemStatus, sensor, SensorData_desc),
    FIELD_U8(SystemStatus, error_code)
END_STRUCT_DESC(SystemStatus, SystemStatus_desc)


/* ============================================================================
 *                   可选：配置 C11 单参数模式支持
 * ============================================================================ */

/**
 * @brief C11 _Generic 自动类型推导宏
 * @note 配置此宏后，可使用 STRUCT_PRINT(var) 单参数调用
 * @note 需要编译器支持 C11 标准
 * 
 * 使用方法：
 *   1. 定义 GET_STRUCT_DESC 宏，映射类型到描述符
 *   2. 使用 STRUCT_PRINT(var) 即可自动选择描述符
 * 
 * 或者使用 descriptor_generator.html 的 "C11 Complete" 模式自动生成
 */
#if STRUCT_PRINT_HAS_GENERIC
    #undef GET_STRUCT_DESC
    #define GET_STRUCT_DESC(var) _Generic((var), \
        stCircuitMqttCmdData: &stCircuitMqttCmdData_desc, \
        DeviceInfo: &DeviceInfo_desc, \
        SensorData: &SensorData_desc, \
        SystemStatus: &SystemStatus_desc, \
        default: NULL \
    )
#endif


/* ============================================================================
 *                   步骤4：使用 STRUCT_PRINT 打印结构体
 * ============================================================================ */

/**
 * @brief 测试1：基本结构体打印
 * @note 演示如何打印包含字符串、整数的基本结构体
 */
void test_basic_struct(void)
{
    printf("\n\n");
    printf("========================================\n");
    printf("测试1：基本结构体打印\n");
    printf("========================================\n\n");
    
    /* 创建并初始化结构体 */
    stCircuitMqttCmdData cmd;
    memset(&cmd, 0, sizeof(cmd));
    
    /* 填充数据 */
    strcpy((char*)cmd.type, "request");
    cmd.ProtocolType = 1;
    strcpy((char*)cmd.Imei, "862123456789012");
    strcpy((char*)cmd.MsgType, "CONNECT");
    cmd.MsgData = 12345;
    strcpy((char*)cmd.MsgDataString, "Hello STM32!");
    cmd.MeterAdr = 0x000000FF;
    
    /* 打印结构体 */
#if STRUCT_PRINT_HAS_GENERIC
    STRUCT_PRINT(cmd);  /* C11: 单参数，自动推导类型 */
#else
    STRUCT_PRINT(cmd, stCircuitMqttCmdData);  /* C99: 两参数 */
#endif
}

/**
 * @brief 测试2：浮点数类型打印
 * @note 演示 float 和 double 类型的打印效果
 */
void test_float_struct(void)
{
    printf("\n\n");
    printf("========================================\n");
    printf("测试2：包含浮点数的结构体\n");
    printf("========================================\n\n");
    
    /* 创建并初始化结构体 */
    DeviceInfo device;
    device.device_id = 5;
    device.firmware_version = 0x0102;  /* v1.2 */
    device.serial_number = 123456789;
    device.temperature = 25.6f;
    device.voltage = 3.3;
    
    /* 打印结构体 */
#if STRUCT_PRINT_HAS_GENERIC
    STRUCT_PRINT(device);  /* C11: 单参数 */
#else
    STRUCT_PRINT(device, DeviceInfo);  /* C99: 两参数 */
#endif
}

/**
 * @brief 测试3：嵌套结构体打印
 * @note 演示多层嵌套结构体的自动展开打印
 */
void test_nested_struct(void)
{
    printf("\n\n");
    printf("========================================\n");
    printf("测试3：嵌套结构体打印\n");
    printf("========================================\n\n");
    
    /* 创建并初始化嵌套结构体 */
    SystemStatus status;
    
    status.timestamp = 1697612345;
    
    /* 初始化嵌套的device字段 */
    status.device.device_id = 10;
    status.device.firmware_version = 0x0201;
    status.device.serial_number = 987654321;
    status.device.temperature = 28.5f;
    status.device.voltage = 5.0;
    
    /* 初始化嵌套的sensor字段 */
    status.sensor.sensor_id = 100;
    status.sensor.value = -273;  /* 有符号数 */
    status.sensor.status = 1;
    
    status.error_code = 0;
    
    /* 打印嵌套结构体 */
#if STRUCT_PRINT_HAS_GENERIC
    STRUCT_PRINT(status);  /* C11: 单参数 */
#else
    STRUCT_PRINT(status, SystemStatus);  /* C99: 两参数 */
#endif
}

/**
 * @brief 测试4：边界值和特殊情况
 * @note 演示空字符串、最大值、负数的打印效果
 */
void test_various_types(void)
{
    printf("\n\n");
    printf("========================================\n");
    printf("测试4：边界值和特殊情况\n");
    printf("========================================\n\n");
    
    /* 测试边界值 */
    stCircuitMqttCmdData cmd;
    memset(&cmd, 0, sizeof(cmd));
    strcpy((char*)cmd.type, "");  /* 空字符串 */
    cmd.ProtocolType = 255;       /* u8 最大值 */
    cmd.MsgData = -999;           /* 负数 */
    cmd.MeterAdr = 0xFFFFFFFF;    /* u32 最大值 */
    
    /* 打印边界值测试 */
#if STRUCT_PRINT_HAS_GENERIC
    STRUCT_PRINT(cmd);  /* C11: 单参数 */
#else
    STRUCT_PRINT(cmd, stCircuitMqttCmdData);  /* C99: 两参数 */
#endif
}

/**
 * @brief 测试5：C11 单参数模式（需要 C11 编译器）
 * @note 演示 STRUCT_PRINT(var) 单参数调用方式
 *       编译时需要加 -std=c11 或更高版本
 */
void test_c11_auto_mode(void)
{
#if STRUCT_PRINT_HAS_GENERIC
    printf("\n\n");
    printf("========================================\n");
    printf("测试5：C11 单参数自动推导模式\n");
    printf("========================================\n");
    printf("ℹ️  编译器支持 C11 _Generic，可使用单参数模式\n\n");
    
    /* 示例1：打印 DeviceInfo */
    DeviceInfo device;
    device.device_id = 88;
    device.firmware_version = 0x0305;
    device.serial_number = 999888777;
    device.temperature = 36.5f;
    device.voltage = 3.3;
    
    printf("【单参数调用示例】\n");
    printf("代码: STRUCT_PRINT(device);  // ← 只需一个参数！\n\n");
    
    /* ✨ C11 单参数调用 - 自动推导类型！ */
    STRUCT_PRINT(device);
    
    /* 示例2：打印 SensorData */
    SensorData sensor;
    sensor.sensor_id = 999;
    sensor.value = 1024;
    sensor.status = 0xFF;
    
    printf("\n\n【再次演示单参数调用】\n");
    printf("代码: STRUCT_PRINT(sensor);  // ← 自动识别为 SensorData 类型！\n\n");
    
    /* ✨ C11 单参数调用 */
    STRUCT_PRINT(sensor);
    
    printf("\n");
    printf("优势：\n");
    printf("  ✓ 不需要手动指定类型名\n");
    printf("  ✓ 编译时自动类型匹配\n");
    printf("  ✓ 代码更简洁易读\n");
    printf("  ✓ 重构时更安全（改变量类型会自动匹配新描述符）\n");
    
#else
    printf("\n\n");
    printf("========================================\n");
    printf("测试5：C11 单参数模式\n");
    printf("========================================\n");
    printf("⚠️  当前编译器不支持 C11 _Generic\n");
    printf("提示：使用 gcc -std=c11 或 clang -std=c11 编译\n");
    printf("      或继续使用 C99 两参数模式: STRUCT_PRINT(var, Type)\n");
#endif
}


/* ============================================================================
 *                          主函数
 * ============================================================================ */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf("  struct_print.h 演示程序\n");
    printf("  C语言结构体调试工具\n");
    printf("========================================\n");
    
    /* 显示当前编译模式 */
#if STRUCT_PRINT_HAS_GENERIC
    printf("  编译模式: C11 (支持单参数模式)\n");
#else
    printf("  编译模式: C99 (两参数模式)\n");
#endif
    printf("========================================\n");
    printf("\n");
    
    /* 运行所有测试 */
    test_basic_struct();
    test_float_struct();
    test_nested_struct();
    test_various_types();
    test_c11_auto_mode();  /* C11 单参数模式测试 */
    
    printf("\n\n");
    printf("========================================\n");
    printf("所有测试完成！\n");
    printf("========================================\n\n");
    
    printf("编译示例：\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("C99 模式（两参数）：\n");
    printf("  gcc -std=c99 -o example example.c\n");
    printf("\n");
    printf("C11 模式（支持单参数）：\n");
    printf("  gcc -std=c11 -o example example.c\n");
    printf("\n");
    printf("优化编译（Release）：\n");
    printf("  gcc -std=c11 -O2 -o example example.c\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    printf("使用说明：\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("1. 配置打印函数\n");
    printf("   #define STRUCT_PRINT_PRINTF your_printf_function\n");
    printf("\n");
    printf("2. 启用调试功能\n");
    printf("   #define STRUCT_PRINT_ENABLE\n");
    printf("\n");
    printf("3. 生成描述符\n");
    printf("   - 在线工具: descriptor_generator.html\n");
    printf("   - 手动编写: BEGIN_STRUCT_DESC / END_STRUCT_DESC\n");
    printf("\n");
    printf("4. 打印结构体\n");
    printf("   - C99 模式: STRUCT_PRINT(var, Type)\n");
    printf("   - C11 模式: STRUCT_PRINT(var)  [需配置 GET_STRUCT_DESC]\n");
    printf("\n");
    printf("5. Release 版本\n");
    printf("   - 不定义 STRUCT_PRINT_ENABLE，代码自动优化为空\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("\n");
    
    return 0;
}

