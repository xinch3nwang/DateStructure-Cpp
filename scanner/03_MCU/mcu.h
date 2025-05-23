//======================================================================
//文件名称：mcu.h（mcu头文件）
//制作单位：SD-Arm(sumcu.suda.edu.cn)
//更新记录：20181201-20200627
//======================================================================
#ifndef MCU_H     //防止重复定义（MCU_H 开头）
#define MCU_H

//（1）【变动】包含芯片头文件
#include "stm32l4xx.h"
//（2）【固定】包含cpu头文件
#include "cpu.h"
//（3）【变动】MCU基本信息相关宏常数
//                            "1234567890123456789"
#define MCU_TYPE              "AHL-STM32L431-V3.6 "  //MCU型号（19字节）
//                            "123456789"
#define BIOS_TYPE             "20200908 "       //BIOS版本号（9字节）
#define MCU_SECTORSIZE        2048              //扇区大小（B）
#define MCU_SECTOR_NUM        128               //MCU扇区总数
#define MCU_STACKTOP          0x2000ffff        //栈顶地址（RAM最高地址）
#define MCU_FLASH_ADDR_START  0x08000000        //MCU的FLASH起始地址
#define MCU_FLASH_ADDR_LENGTH 0x00040000        //MCU的FLASH长度（128KB）
#define MCU_RAM_ADDR_START    0x20000000	    //MCU的RAM起始地址
#define MCU_RAM_ADDR_LENGTH   0x00010000	    //MCU的RAM长度（16KB）

//(6)定义系统使用的时钟频率
//【BIOS工程宏定义添加处】4
#define  MCU_SYSTEM_CLK        SystemCoreClock              // 芯片系统时钟频率(HZ)
#define  MCU_SYSTEM_CLK_KHZ    SystemCoreClock/1000         // 芯片系统时钟频率(KHz)
#define  MCU_BUS_CLK_KHZ      MCU_SYSTEM_CLK_KHZ/2         // 芯片总线时钟频率(KHz)
#define  MCU_SYSTEM_CLK_MS    (SystemCoreClock/ 1000)     // 1ms所需时钟频率(HZ)
#define  MCU_SYSTEM_CLK_US    (MCU_SYSTEM_CLK_MS / 1000)   // 1us所需时钟频率(HZ)

#endif    //防止重复定义（MCU_H 结尾）