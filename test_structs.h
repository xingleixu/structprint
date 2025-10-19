/**
 * @file test_structs.h
 * @brief 测试结构体定义文件
 * @note 此文件用于测试 descriptor_generator.html 工具的解析功能
 */

#ifndef __TEST_STRUCTS_H
#define __TEST_STRUCTS_H

#include <stdint.h>

/* 类型别名 */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;

/* 常量定义 */
#define stCircuitCmdDataLenMax      32
#define stCircuitCmdDataMsgLenMax   512

/**
 * @brief 服务器命令结构体（用户提供的原始示例）
 */
typedef struct
{
    u8 type[stCircuitCmdDataLenMax+1];          // 消息类型，request：服务器请求，reply：单片机回复；post：单片机上报
    u8 ProtocolType;                            // 协议类型  1：数码管协议 2：液晶屏协议
    u8 Imei[stCircuitCmdDataLenMax+1];          // 设备唯一ID
    u8 MsgType[stCircuitCmdDataLenMax+1];       // 报文控制命令
    int MsgData;                                // 报文实体：可为空
    u8 MsgDataString[512];                      // 报文实体：字符串数据
    u32 MeterAdr;                               // 控制485设备的地址
} stCircuitMqttCmdData;

/**
 * @brief 设备信息结构体
 */
typedef struct
{
    u8 device_id;               /* 设备ID */
    u16 firmware_version;       /* 固件版本 */
    u32 serial_number;          /* 序列号 */
    float temperature;          /* 温度 */
    double voltage;             /* 电压 */
} DeviceInfo;

/**
 * @brief 传感器数据结构体
 */
typedef struct
{
    u16 sensor_id;              /* 传感器ID */
    s16 value;                  /* 传感器值（有符号）*/
    u8 status;                  /* 状态 */
} SensorData;

/**
 * @brief 系统状态结构体（包含嵌套结构体）
 */
typedef struct
{
    u32 timestamp;              /* 时间戳 */
    DeviceInfo device;          /* 设备信息（嵌套）*/
    SensorData sensor;          /* 传感器数据（嵌套）*/
    u8 error_code;              /* 错误码 */
} SystemStatus;

/**
 * @brief 配置参数结构体
 */
typedef struct
{
    u8 mode;                    /* 工作模式 */
    u16 interval;               /* 采样间隔 */
    u32 timeout;                /* 超时时间 */
    s32 offset;                 /* 偏移量（有符号）*/
    float gain;                 /* 增益 */
    u8 enable;                  /* 使能标志 */
} ConfigParams;

#endif /* __TEST_STRUCTS_H */

