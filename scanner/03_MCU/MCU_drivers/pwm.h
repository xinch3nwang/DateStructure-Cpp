//=====================================================================
//文件名称：pwm.h
//功能概要：pwm底层驱动构件头文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2019-11-16  V2.0
//适用芯片：STM32
//=====================================================================

#ifndef _PWM_H_
#define _PWM_H_


//PWM对齐方式宏定义:边沿对齐、中心对齐
#define PWM_EDGE   0
#define PWM_CENTER 1
//PWM极性选择宏定义：正极性、负极性
#define PWM_PLUS   1
#define PWM_MINUS  0

//PWM通道号
#define  PWM_PIN0  (PTA_NUM|5)    //CH1
#define  PWM_PIN1  (PTB_NUM|3)    //CH2
#define  PWM_PIN2  (PTB_NUM|10)   //CH3
#define  PWM_PIN3  (PTB_NUM|11)   //CH4
#define  PWM_PIN4  (PTA_NUM|5)    //CH5
//=====================================================================
//函数名称：  pwm_init
//功能概要：  pwm初始化函数
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          clockFre：时钟频率，单位：hz，取值：375、750、1500、3000、6000、
//                                                   12000、24000、48000
//          period：周期，单位为个数，即计数器跳动次数，范围为1~65536
//          duty：占空比：0.0~100.0对应0%~100%
//          align：对齐方式 ，在头文件宏定义给出，如PWM_EDGE为边沿对其。
//          pol：极性，在头文件宏定义给出，如PWM_PLUS为正极性
//函数返回：  无
//注意：          因为GEC中给出的PWM和输入捕捉都是同一模块的，只是通道不同，所以为防止在使用多组
//          PWM和输入捕捉时，频率篡改，需要使得使用到的clockFre和period参数保持一致。
//=====================================================================
void pwm_init(uint16_t pwmNo,uint32_t clockFre,uint16_t period,double duty,
		                                       uint8_t align,uint8_t pol);

//=====================================================================
//函数名称：  pwm_update
//功能概要：  tpmx模块Chy通道的PWM更新
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          duty：占空比：0.0~100.0对应0%~100%
//函数返回：  无
//=====================================================================
void pwm_update(uint16_t pwmNo,double duty);


#endif
