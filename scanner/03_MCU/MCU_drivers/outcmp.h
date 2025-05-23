//=====================================================================
//文件名称：outcmp.h
//功能概要：pwm底层驱动构件头文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2019-11-21  V2.0
//适用芯片：STM32L4
//=====================================================================
#ifndef OUTCMP_H
#define OUTCMP_H
#include "mcu.h"
#include "user.h"
//输出比较模式选择宏定义
#define CMP_REV   0   //翻转电平
#define CMP_LOW   1  //强制低电平
#define CMP_HIGH  2   //强制高电平


//输出比较通道号
#define  OUTCMP_PIN0  (PTA_NUM|5)   // CH1
#define  OUTCMP_PIN1  (PTB_NUM|3)   //CH2
#define  OUTCMP_PIN2  (PTB_NUM|10)  //CH3
#define  OUTCMP_PIN3  (PTB_NUM|11)  //CH4
#define  OUTCMP_PIN4  (PTA_NUM|15)  //CH5

//===================================================================
//函数名称：outcmp_init
//功能概要：outcmp模块初始化。
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//        freq：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000(频率决定精度)
//        cmpPeriod：单位：ms，范围取决于计数器频率与计数器位数（16位）
//        comduty：输出比较电平翻转位置占总周期的比例：0.0~100.0对应0%~100%
//        cmpmode：输出比较模式（翻转电平、强制低电平、强制高电平）,有宏定义常数使用
//函数返回：无
//===================================================================
void outcmp_init(uint16_t outcmpNo,uint32_t freq,uint32_t cmpPeriod,float cmpduty,uint8_t cmpmode);

//===================================================================
//函数名称：outcmp_enable_int
//功能概要：使能tim模块中断。
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：无
//===================================================================
void outcmp_enable_int(uint16_t outcmpNo);

//===================================================================
//函数名称：outcmp_disable_int
//功能概要：禁用tim模块中断。
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：无
//===================================================================
void outcmp_disable_int(uint16_t outcmpNo);

//===================================================================
//函数名称：outcmp_get_int
//功能概要：获取TIM模块中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint8_t outcmp_get_int(uint16_t outcmpNo);

//===================================================================
//函数名称：outcmp_chl_get_int
//功能概要：获取TIM通道中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint8_t outcmp_chl_get_int(uint16_t outcmpNo);

//===================================================================
//函数名称：outcmp_clear_int
//功能概要：清除TIM中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：清除中断标志位
//===================================================================
void outcmp_clear_int(uint16_t outcmpNo);

//===================================================================
//函数名称：outcmp_clear_chl_int
//功能概要：清除TIM通道中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：清除TIM通道中断标志位
//===================================================================
void outcmp_clear_chl_int(uint16_t outcmpNo);

#endif

//=====================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学嵌入式中心  0512-65214835  http://sumcu.suda.edu.cn