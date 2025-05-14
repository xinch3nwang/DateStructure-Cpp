//=====================================================================
//文件名称：isr.c（中断处理程序源文件）
//框架提供：SD-ARM（sumcu.suda.edu.cn）
//版本更新：20170801-20191020
//功能描述：提供中断处理程序编程框架
//移植规则：【固定】
//=====================================================================
#include "includes.h"

void User_SysFun(uint8_t ch);
//======================================================================
//程序名称：UART_User_Handler
//触发条件：UART_User串口收到一个字节触发
//备    注：进入本程序后，可使用uart_get_re_int函数可再进行中断标志判断
//          （1-有UART接收中断，0-没有UART接收中断）
//======================================================================


//内部函数
void User_SysFun(uint8_t ch)
{
    //（1）收到的一个字节参与组帧
    if(gcRecvLen == 0)  gcRecvLen =useremuart_frame(ch,(uint8_t*)gcRecvBuf);
    //（2）字节进入组帧后，判断gcRecvLen=0？若为0，表示组帧尚未完成，
    //     下次收到一个字节，再继续组帧
    if(gcRecvLen == 0) goto User_SysFun_Exit;
    //（3）至此，gcRecvLen≠0,表示组帧完成，gcRecvLen为帧的长度,校验序列号后（与
    //     根据Flash中倒数一扇区开始的16字节进行比较）
    //     gcRecvBuf[16]进行跳转
    if(strncmp((char *)(gcRecvBuf),(char *)((MCU_SECTOR_NUM-1)*MCU_SECTORSIZE+
       MCU_FLASH_ADDR_START),16) != 0)
    {
        gcRecvLen = 0;         //恢复接收状态
        goto User_SysFun_Exit;
    }
    //（4）至此，不仅收到完整帧，且序号比较也一致， 根据命令字节gcRecvBuf[16]进行跳转
    //若为User串口程序更新命令，则进行程序更新
    switch(gcRecvBuf[16])  //帧标识
    {
        case 0:
            SYSTEM_FUNCTION((uint8_t *)(gcRecvBuf+17));
            gcRecvLen = 0;         //恢复接收状态
        break;
        default:
        break;
    }
User_SysFun_Exit:
    return;
}



//内部函数声明
//uint8_t CreateFrame(uint8_t Data,uint8_t * buffer);

//======================================================================
//中断服务程序名称：UART_USER_Handler
//触发条件：UART_USE串口收到一个字节触发
//功    能：收到一个字节，直接返回该字节
//备    注：进入本程序后，可使用uart_get_re_int函数可再进行中断标志判断
//          （1-有UART接收中断，0-没有UART接收中断）
//======================================================================
void UART_User_Handler(void)
{
	static uint8_t intTime = 0;
    uint8_t ch,flag;
    //uint8_t g_uart_recvBuf[3];
    //uint8_t *Buf=g_uart_recvBuf;
    //Buf[0] = 0;Buf[1] = 0;Buf[2] = 0;
    DISABLE_INTERRUPTS;               //关总中断
    //-----------------------------------------------------------------------
    ch = uart_re1(UART_User, &flag);   //调用接收一个字节的函数，清接收中断位

   	
   	if(flag) 
   	{
   		if(intTime==0){
   			in_duty=0.0+(ch-'0')*100;
   			intTime++;
   		}
   		else if(intTime==1){
   			in_duty+=(ch-'0')*10;
   			intTime++;
   		}
   		else if(intTime==2){
   			in_duty+=(ch-'0');
   			intTime=0;
   			pwm_update(PWM_USER,in_duty);
   		}
		uart_send1(UART_User,ch);//回发接收到的字节
	}
   	
	ENABLE_INTERRUPTS;       //开总中断
	
}


//内部调用函数

//===========================================================================
//函数名称：CreateFrame

//===========================================================================
//uint8_t CreateFrame(uint8_t Data,uint8_t * buffer)
//{
//    uint8_t frameFlag=0;            //组帧状态
//	for(int i=0; i<3 ;i++)
//	{
//		buffer[i]=Data-'0';
//		if(i==2) frameFlag=1;
//		uart_send1(UART_User,buffer[i]);
//	}
//    return frameFlag;                 //返回组帧状态
//}

/*
 知识要素：
 1.本文件中的中断处理函数调用的均是相关设备封装好的具体构件，在更换芯片
 时，只需保证设备的构件接口一致，即可保证本文件的相关中断处理函数不做任何
 更改，从而达到芯片无关性的要求。
 */
