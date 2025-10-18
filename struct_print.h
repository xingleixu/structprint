/**
 * @file struct_print.h
 * @brief C语言结构体调试打印工具 - STM32嵌入式专用
 * @author xingleixu@gmail.com
 * @version 1.0
 * @date 2025-10-18
 * 
 * @description
 * 这是一个轻量级的C语言结构体调试工具，专为STM32等嵌入式环境设计。
 * 主要功能：
 *   1. 自动打印结构体变量名和变量值
 *   2. 自动格式化输出，支持多级嵌套
 *   3. 显示内存地址、偏移量、十六进制数据
 *   4. 通过宏控制，可在Release版本中完全禁用
 *   5. 不改变原有结构体定义方式
 * 
 * @usage
 * 1. 包含此头文件
 * 2. 使用 descriptor_generator.html 自动生成结构体描述符
 * 3. 调用 STRUCT_PRINT(变量) 进行打印（C11）或 STRUCT_PRINT(变量, 类型) (C99)
 * 
 * @note
 * - 在Debug模式下定义 STRUCT_PRINT_ENABLE 宏以启用
 * - 描述符表存储在Flash中，不占用RAM
 * - 可通过 STRUCT_PRINT_PRINTF 宏指定打印函数（默认为 printf）
 */

#ifndef __STRUCT_PRINT_H
#define __STRUCT_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 *                              配置区域
 * ============================================================================ */

/**
 * @brief 启用结构体打印功能
 * @note 在需要调试时定义此宏，Release版本不定义即可完全禁用
 */
// #define STRUCT_PRINT_ENABLE

/**
 * @brief 配置选项
 */
#ifdef STRUCT_PRINT_ENABLE

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

#endif /* STRUCT_PRINT_ENABLE */


/* ============================================================================
 *                            标准库和类型定义
 * ============================================================================ */

#include <stddef.h>     /* offsetof, size_t */
#include <stdint.h>     /* uint8_t, uint16_t, uint32_t */
#include <string.h>     /* memset, strlen */
#include <stdio.h>      /* sprintf (如果支持) */

/* STM32 常用类型别名 */
#ifndef u8
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
#endif


/* ============================================================================
 *                            编译器特性检测
 * ============================================================================ */

/**
 * @brief 检测 C11 _Generic 支持
 * @note 用于自动选择单参数或两参数版本
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define STRUCT_PRINT_HAS_GENERIC 1  /* C11 或更高版本 */
#else
    #define STRUCT_PRINT_HAS_GENERIC 0  /* C99 或更低版本 */
#endif


/* ============================================================================
 *                            字段类型枚举
 * ============================================================================ */

#ifdef STRUCT_PRINT_ENABLE

/**
 * @brief 字段类型枚举
 * @note 用于描述结构体中每个字段的数据类型
 */
typedef enum {
    FIELD_TYPE_U8,          /**< 无符号8位整数 uint8_t / u8 */
    FIELD_TYPE_U16,         /**< 无符号16位整数 uint16_t / u16 */
    FIELD_TYPE_U32,         /**< 无符号32位整数 uint32_t / u32 */
    FIELD_TYPE_S8,          /**< 有符号8位整数 int8_t / s8 */
    FIELD_TYPE_S16,         /**< 有符号16位整数 int16_t / s16 */
    FIELD_TYPE_S32,         /**< 有符号32位整数 int32_t / s32 */
    FIELD_TYPE_FLOAT,       /**< 单精度浮点数 float */
    FIELD_TYPE_DOUBLE,      /**< 双精度浮点数 double */
    FIELD_TYPE_ARRAY,       /**< 数组类型 */
    FIELD_TYPE_STRING,      /**< 字符串类型（字符数组） */
    FIELD_TYPE_STRUCT,      /**< 嵌套结构体类型 */
} FieldType;


/* ============================================================================
 *                            描述符数据结构
 * ============================================================================ */

/* 前向声明 */
struct StructDescriptor_t;

/**
 * @brief 字段描述符结构
 * @note 描述结构体中单个字段的元数据信息
 */
typedef struct {
    const char* name;                           /**< 字段名称 */
    FieldType type;                             /**< 字段类型 */
    size_t offset;                              /**< 字段在结构体中的字节偏移量 */
    size_t size;                                /**< 字段大小（字节）*/
    size_t array_count;                         /**< 数组元素个数（非数组为0） */
    const struct StructDescriptor_t* nested_desc; /**< 嵌套结构体的描述符指针 */
} FieldDescriptor;

/**
 * @brief 结构体描述符结构
 * @note 描述整个结构体的元数据信息
 */
typedef struct StructDescriptor_t {
    const char* struct_name;                    /**< 结构体名称 */
    size_t struct_size;                         /**< 结构体总大小（字节）*/
    size_t field_count;                         /**< 字段数量 */
    const FieldDescriptor* fields;              /**< 字段描述符数组指针 */
} StructDescriptor;


/* ============================================================================
 *                            辅助宏定义
 * ============================================================================ */

/**
 * @brief 开始定义结构体描述符
 * @param struct_type 结构体类型名
 * @param desc_name 描述符变量名
 * 
 * @example
 * BEGIN_STRUCT_DESC(MyStruct, my_struct_desc)
 *     FIELD_U8(MyStruct, field1),
 *     FIELD_U32(MyStruct, field2),
 * END_STRUCT_DESC(MyStruct, my_struct_desc)
 */
#define BEGIN_STRUCT_DESC(struct_type, desc_name) \
    static const FieldDescriptor desc_name##_fields[] = {

/**
 * @brief 定义u8类型字段
 */
#define FIELD_U8(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_U8, \
        offsetof(struct_type, field_name), \
        sizeof(u8), \
        0, \
        NULL \
    }

/**
 * @brief 定义u16类型字段
 */
#define FIELD_U16(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_U16, \
        offsetof(struct_type, field_name), \
        sizeof(u16), \
        0, \
        NULL \
    }

/**
 * @brief 定义u32类型字段
 */
#define FIELD_U32(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_U32, \
        offsetof(struct_type, field_name), \
        sizeof(u32), \
        0, \
        NULL \
    }

/**
 * @brief 定义s8类型字段
 */
#define FIELD_S8(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_S8, \
        offsetof(struct_type, field_name), \
        sizeof(s8), \
        0, \
        NULL \
    }

/**
 * @brief 定义s16类型字段
 */
#define FIELD_S16(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_S16, \
        offsetof(struct_type, field_name), \
        sizeof(s16), \
        0, \
        NULL \
    }

/**
 * @brief 定义s32/int类型字段
 */
#define FIELD_S32(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_S32, \
        offsetof(struct_type, field_name), \
        sizeof(s32), \
        0, \
        NULL \
    }

/**
 * @brief 定义int类型字段（别名）
 */
#define FIELD_INT(struct_type, field_name) FIELD_S32(struct_type, field_name)

/**
 * @brief 定义float类型字段
 */
#define FIELD_FLOAT(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_FLOAT, \
        offsetof(struct_type, field_name), \
        sizeof(float), \
        0, \
        NULL \
    }

/**
 * @brief 定义double类型字段
 */
#define FIELD_DOUBLE(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_DOUBLE, \
        offsetof(struct_type, field_name), \
        sizeof(double), \
        0, \
        NULL \
    }

/**
 * @brief 定义字符串类型字段（字符数组，自动识别为字符串）
 * @param struct_type 结构体类型
 * @param field_name 字段名
 */
#define FIELD_STRING(struct_type, field_name) \
    { \
        #field_name, \
        FIELD_TYPE_STRING, \
        offsetof(struct_type, field_name), \
        sizeof(((struct_type*)0)->field_name[0]), \
        sizeof(((struct_type*)0)->field_name) / sizeof(((struct_type*)0)->field_name[0]), \
        NULL \
    }

/**
 * @brief 定义数组类型字段
 * @param struct_type 结构体类型
 * @param field_name 字段名
 * @param element_type 数组元素类型（FIELD_TYPE_xxx）
 */
#define FIELD_ARRAY(struct_type, field_name, element_type) \
    { \
        #field_name, \
        element_type, \
        offsetof(struct_type, field_name), \
        sizeof(((struct_type*)0)->field_name[0]), \
        sizeof(((struct_type*)0)->field_name) / sizeof(((struct_type*)0)->field_name[0]), \
        NULL \
    }

/**
 * @brief 定义嵌套结构体类型字段
 * @param struct_type 结构体类型
 * @param field_name 字段名
 * @param nested_desc 嵌套结构体的描述符
 */
#define FIELD_STRUCT(struct_type, field_name, nested_desc) \
    { \
        #field_name, \
        FIELD_TYPE_STRUCT, \
        offsetof(struct_type, field_name), \
        sizeof(((struct_type*)0)->field_name), \
        0, \
        &nested_desc \
    }

/**
 * @brief 结束结构体描述符定义
 * @param struct_type 结构体类型名
 * @param desc_name 描述符变量名
 */
#define END_STRUCT_DESC(struct_type, desc_name) \
    }; \
    static const StructDescriptor desc_name = { \
        #struct_type, \
        sizeof(struct_type), \
        sizeof(desc_name##_fields) / sizeof(FieldDescriptor), \
        desc_name##_fields \
    };


/* ============================================================================
 *                        调试输出函数配置
 * ============================================================================ */

/**
 * @brief 配置打印函数名称
 * @note 用户可以通过定义此宏来指定自己的打印函数
 *       只需要保证函数签名为：void func(const char* format, ...)
 * 
 * @example 方式1：在包含头文件之前定义
 * #define STRUCT_PRINT_PRINTF uart_printf
 * #include "struct_print.h"
 * 
 * @example 方式2：在编译参数中定义
 * gcc -DSTRUCT_PRINT_PRINTF=my_custom_printf ...
 * 
 * @example 方式3：使用默认的 printf
 * // 不定义 STRUCT_PRINT_PRINTF，将使用标准 printf
 * 
 * @note 您的打印函数实现示例（使用 STM32 HAL）：
 * void uart_printf(const char* format, ...) {
 *     char buffer[256];
 *     va_list args;
 *     va_start(args, format);
 *     vsnprintf(buffer, sizeof(buffer), format, args);
 *     va_end(args);
 *     HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
 * }
 */
#ifndef STRUCT_PRINT_PRINTF
#define STRUCT_PRINT_PRINTF printf  /* 默认使用标准 printf */
#endif


/* ============================================================================
 *                        内部辅助函数
 * ============================================================================ */

/**
 * @brief 检查字符数组是否为可打印字符串
 * @param data 数据指针
 * @param length 数据长度
 * @return 1表示是字符串，0表示不是
 */
static inline int is_printable_string(const u8* data, size_t length) {
    size_t i;
    int null_found = 0;
    
    if (length == 0) return 0;
    
    /* 检查是否包含空字符 */
    for (i = 0; i < length; i++) {
        if (data[i] == '\0') {
            null_found = 1;
            break;
        }
    }
    
    /* 如果没有空字符，检查所有字符是否可打印 */
    if (!null_found) {
        for (i = 0; i < length; i++) {
            if (data[i] < 0x20 || data[i] > 0x7E) {
                return 0; /* 包含不可打印字符 */
            }
        }
        return 1;
    }
    
    /* 如果有空字符，检查空字符之前的字符是否可打印 */
    for (i = 0; i < length && data[i] != '\0'; i++) {
        if (data[i] < 0x20 || data[i] > 0x7E) {
            return 0;
        }
    }
    
    return (i > 0); /* 至少有一个可打印字符 */
}

/**
 * @brief 打印缩进空格
 * @param indent_level 缩进层级
 */
static inline void print_indent(int indent_level) {
    int i;
    for (i = 0; i < indent_level * STRUCT_PRINT_INDENT_SPACES; i++) {
        STRUCT_PRINT_PRINTF(" ");
    }
}

/**
 * @brief 打印十六进制内存数据
 * @param data 数据指针
 * @param length 数据长度
 * @param max_bytes 最多显示的字节数
 * @param indent_level 缩进层级
 */
static inline void print_hex_memory(const u8* data, size_t length, size_t max_bytes, int indent_level) {
#if STRUCT_PRINT_SHOW_HEX_MEMORY
    size_t i;
    size_t bytes_to_show = (length < max_bytes) ? length : max_bytes;
    
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("        └─ Memory: ");
    
    for (i = 0; i < bytes_to_show; i++) {
        STRUCT_PRINT_PRINTF("%02X ", data[i]);
        if ((i + 1) % 16 == 0 && (i + 1) < bytes_to_show) {
            STRUCT_PRINT_PRINTF("\n");
            print_indent(indent_level);
            STRUCT_PRINT_PRINTF("                   ");
        }
    }
    
    if (length > max_bytes) {
        STRUCT_PRINT_PRINTF("...");
    }
    STRUCT_PRINT_PRINTF("\n");
#endif
}

/**
 * @brief 打印单个字段的值
 * @param field 字段描述符
 * @param struct_base 结构体基地址
 * @param indent_level 缩进层级
 */
static void print_field_value(const FieldDescriptor* field, const void* struct_base, int indent_level);

/**
 * @brief 打印结构体（递归）
 * @param var_name 变量名
 * @param struct_data 结构体数据指针
 * @param desc 结构体描述符
 * @param indent_level 缩进层级
 */
static void struct_print_internal(const char* var_name, const void* struct_data, 
                                   const StructDescriptor* desc, int indent_level) {
    size_t i;
    
    if (struct_data == NULL || desc == NULL) {
        STRUCT_PRINT_PRINTF("Error: NULL pointer!\n");
        return;
    }
    
    /* 打印结构体头部信息 */
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("========================================\n");
    
    print_indent(indent_level);
    if (var_name != NULL && var_name[0] != '\0') {
        STRUCT_PRINT_PRINTF("Struct: %s [%s]\n", var_name, desc->struct_name);
    } else {
        STRUCT_PRINT_PRINTF("Struct: [%s]\n", desc->struct_name);
    }
    
#if STRUCT_PRINT_SHOW_ADDRESS
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("Address: 0x%08X\n", (unsigned int)(uintptr_t)struct_data);
#endif
    
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("Size: %u bytes\n", (unsigned int)desc->struct_size);
    
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("========================================\n");
    
    /* 打印每个字段 */
    for (i = 0; i < desc->field_count; i++) {
        const FieldDescriptor* field = &desc->fields[i];
        
        print_indent(indent_level);
        
#if STRUCT_PRINT_SHOW_OFFSET
        STRUCT_PRINT_PRINTF("  [+0x%04X] ", (unsigned int)field->offset);
#else
        STRUCT_PRINT_PRINTF("  ");
#endif
        
        STRUCT_PRINT_PRINTF("%s: ", field->name);
        
        /* 打印字段值 */
        print_field_value(field, struct_data, indent_level);
        
        /* 字段之间空行（嵌套结构体除外） */
        if (field->type != FIELD_TYPE_STRUCT && i < desc->field_count - 1) {
            STRUCT_PRINT_PRINTF("\n");
        }
    }
    
    print_indent(indent_level);
    STRUCT_PRINT_PRINTF("========================================\n");
}

/**
 * @brief 打印单个字段的值（实现）
 * @param field 字段描述符
 * @param struct_base 结构体基地址
 * @param indent_level 缩进层级
 */
static void print_field_value(const FieldDescriptor* field, const void* struct_base, int indent_level) {
    const void* field_addr = (const u8*)struct_base + field->offset;
    size_t i;
    
    /* 处理数组类型 */
    if (field->array_count > 0 && field->type != FIELD_TYPE_STRUCT) {
        /* 字符串类型 */
        if (field->type == FIELD_TYPE_STRING || 
            (field->type == FIELD_TYPE_U8 && is_printable_string((const u8*)field_addr, field->array_count))) {
            STRUCT_PRINT_PRINTF("\"%s\"\n", (const char*)field_addr);
            print_hex_memory((const u8*)field_addr, field->array_count, STRUCT_PRINT_HEX_BYTES, indent_level);
        }
        /* 数值数组 */
        else {
            STRUCT_PRINT_PRINTF("[");
            size_t max_show = (field->array_count > 16) ? 16 : field->array_count;
            
            for (i = 0; i < max_show; i++) {
                const u8* elem_addr = (const u8*)field_addr + i * field->size;
                
                switch (field->type) {
                    case FIELD_TYPE_U8:
                        STRUCT_PRINT_PRINTF("%u", *(const u8*)elem_addr);
                        break;
                    case FIELD_TYPE_U16:
                        STRUCT_PRINT_PRINTF("%u", *(const u16*)elem_addr);
                        break;
                    case FIELD_TYPE_U32:
                        STRUCT_PRINT_PRINTF("%u", (unsigned int)*(const u32*)elem_addr);
                        break;
                    case FIELD_TYPE_S8:
                        STRUCT_PRINT_PRINTF("%d", *(const s8*)elem_addr);
                        break;
                    case FIELD_TYPE_S16:
                        STRUCT_PRINT_PRINTF("%d", *(const s16*)elem_addr);
                        break;
                    case FIELD_TYPE_S32:
                        STRUCT_PRINT_PRINTF("%d", (int)*(const s32*)elem_addr);
                        break;
                    default:
                        STRUCT_PRINT_PRINTF("?");
                        break;
                }
                
                if (i < max_show - 1) {
                    STRUCT_PRINT_PRINTF(", ");
                }
            }
            
            if (field->array_count > max_show) {
                STRUCT_PRINT_PRINTF(", ...");
            }
            
            STRUCT_PRINT_PRINTF("]\n");
            print_hex_memory((const u8*)field_addr, field->array_count * field->size, STRUCT_PRINT_HEX_BYTES, indent_level);
        }
        return;
    }
    
    /* 处理单一值类型 */
    switch (field->type) {
        case FIELD_TYPE_U8:
            STRUCT_PRINT_PRINTF("%u (0x%02X)\n", *(const u8*)field_addr, *(const u8*)field_addr);
            print_hex_memory((const u8*)field_addr, 1, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_U16:
            STRUCT_PRINT_PRINTF("%u (0x%04X)\n", *(const u16*)field_addr, *(const u16*)field_addr);
            print_hex_memory((const u8*)field_addr, 2, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_U32:
            STRUCT_PRINT_PRINTF("%u (0x%08X)\n", (unsigned int)*(const u32*)field_addr, (unsigned int)*(const u32*)field_addr);
            print_hex_memory((const u8*)field_addr, 4, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_S8:
            STRUCT_PRINT_PRINTF("%d (0x%02X)\n", *(const s8*)field_addr, *(const u8*)field_addr);
            print_hex_memory((const u8*)field_addr, 1, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_S16:
            STRUCT_PRINT_PRINTF("%d (0x%04X)\n", *(const s16*)field_addr, *(const u16*)field_addr);
            print_hex_memory((const u8*)field_addr, 2, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_S32:
            STRUCT_PRINT_PRINTF("%d (0x%08X)\n", (int)*(const s32*)field_addr, (unsigned int)*(const u32*)field_addr);
            print_hex_memory((const u8*)field_addr, 4, STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
            
        case FIELD_TYPE_FLOAT: {
            float val = *(const float*)field_addr;
            STRUCT_PRINT_PRINTF("%.6f\n", val);
            print_hex_memory((const u8*)field_addr, sizeof(float), STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
        }
            
        case FIELD_TYPE_DOUBLE: {
            double val = *(const double*)field_addr;
            STRUCT_PRINT_PRINTF("%.6f\n", val);
            print_hex_memory((const u8*)field_addr, sizeof(double), STRUCT_PRINT_HEX_BYTES, indent_level);
            break;
        }
            
        case FIELD_TYPE_STRUCT:
            if (field->nested_desc != NULL) {
                STRUCT_PRINT_PRINTF("\n");
                struct_print_internal("", field_addr, field->nested_desc, indent_level + 1);
            } else {
                STRUCT_PRINT_PRINTF("<nested struct, no descriptor>\n");
            }
            break;
            
        default:
            STRUCT_PRINT_PRINTF("<unknown type>\n");
            break;
    }
}


/* ============================================================================
 *                        用户API接口
 * ============================================================================ */

/**
 * @brief 打印结构体（内部函数）
 * @param var_name 变量名（字符串）
 * @param struct_data 结构体数据指针
 * @param desc 结构体描述符指针
 * 
 * @note 用户请使用 STRUCT_PRINT 宏，不要直接调用此函数
 */
static inline void struct_print(const char* var_name, const void* struct_data, const StructDescriptor* desc) {
    struct_print_internal(var_name, struct_data, desc, 0);
}

/**
 * @brief 自动选择描述符的辅助宏（C11 版本）
 * @param var 变量
 * @return 对应类型的描述符指针
 * 
 * @note 此宏使用 C11 _Generic 实现类型到描述符的自动映射
 * @note 由描述符生成器自动生成，用户无需手动定义
 * 
 * @example 在生成的描述符文件中会包含类似定义：
 * #define GET_STRUCT_DESC(var) _Generic((var), \
 *     DeviceInfo: &DeviceInfo_desc, \
 *     SystemStatus: &SystemStatus_desc, \
 *     default: NULL \
 * )
 */
#ifndef GET_STRUCT_DESC
#define GET_STRUCT_DESC(var) NULL  /* 未定义时返回 NULL */
#endif

/* ============================================================================
 *                    智能自动打印宏（兼容 C99/C11）
 * ============================================================================ */

#if STRUCT_PRINT_HAS_GENERIC

/**
 * @brief 打印结构体的宏（C11 单参数版本）
 * @param var 变量名
 * 
 * @note 使用 _Generic 自动选择对应的描述符
 * @note 需要包含自动生成的描述符文件（包含 GET_STRUCT_DESC 定义）
 * @note 要求编译器支持 C11 _Generic 特性
 * 
 * @example
 * #include "descriptors.h"  // 包含自动生成的描述符
 * DeviceInfo device;
 * STRUCT_PRINT(device);   // ✨ 只需一个参数！
 */
#define STRUCT_PRINT(var) \
    struct_print(#var, &(var), GET_STRUCT_DESC(var))

#else

/**
 * @brief 打印结构体的宏（C99 两参数版本）
 * @param var 变量名
 * @param type 结构体类型名
 * 
 * @note C99 环境下使用，需要手动指定类型名
 * @note 自动拼接类型名和 _desc 后缀
 * 
 * @example
 * #include "descriptors.h"
 * DeviceInfo device;
 * STRUCT_PRINT(device, DeviceInfo);  // 需要两个参数
 */
#define STRUCT_PRINT(var, type) \
    struct_print(#var, &(var), &type##_desc)

#endif /* STRUCT_PRINT_HAS_GENERIC */


#else /* STRUCT_PRINT_ENABLE 未定义 */

/* ============================================================================
 *                    Release模式 - 所有函数编译为空
 * ============================================================================ */

/* 空的宏定义，编译后不产生任何代码 */
#define BEGIN_STRUCT_DESC(struct_type, desc_name)
#define FIELD_U8(struct_type, field_name)
#define FIELD_U16(struct_type, field_name)
#define FIELD_U32(struct_type, field_name)
#define FIELD_S8(struct_type, field_name)
#define FIELD_S16(struct_type, field_name)
#define FIELD_S32(struct_type, field_name)
#define FIELD_INT(struct_type, field_name)
#define FIELD_FLOAT(struct_type, field_name)
#define FIELD_DOUBLE(struct_type, field_name)
#define FIELD_STRING(struct_type, field_name)
#define FIELD_ARRAY(struct_type, field_name, element_type)
#define FIELD_STRUCT(struct_type, field_name, nested_desc)
#define END_STRUCT_DESC(struct_type, desc_name)

/* STRUCT_PRINT 支持可变参数（C99/C11 兼容）*/
#if STRUCT_PRINT_HAS_GENERIC
    #define STRUCT_PRINT(var) ((void)0)
#else
    #define STRUCT_PRINT(var, type) ((void)0)
#endif

#endif /* STRUCT_PRINT_ENABLE */


#ifdef __cplusplus
}
#endif

#endif /* __STRUCT_PRINT_H */

