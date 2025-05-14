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




//======================================================================
//中断服务程序名称：UART_USER_Handler
//触发条件：UART_USE串口收到一个字节触发
//功    能：收到一个字节，直接返回该字节
//备    注：进入本程序后，可使用uart_get_re_int函数可再进行中断标志判断
//          （1-有UART接收中断，0-没有UART接收中断）
//======================================================================
void UART_User_Handler(void)
{
    uint8_t ch,flag;
    DISABLE_INTERRUPTS;               //关总中断
    //-----------------------------------------------------------------------
    ch = uart_re1(UART_User, &flag);   //调用接收一个字节的函数，清接收中断位
   
   	if(flag)
   	{
		uart_send1(UART_User,ch);//回发接收到的字节
	}
	ENABLE_INTERRUPTS;       //开总中断
	
}

//=====================================================================
//函数名称：INCAP_USER_Handler（输入捕捉中断处理程序）
//参数说明：无
//函数返回：无
//功能概要：（1）每次捕捉到上升沿或者下降沿触发该程序；
//       （2）每次触发都会上传当前捕捉到的上位机程序
//=====================================================================
void INCAP_USER_Handler(void)
{
	uint16_t val;
	static uint16_t gcIntTime=0;
	//static uint8_t status=0;
	static uint8_t Count = 0;
	DISABLE_INTERRUPTS;     //关总中断
	
	//------------------------------------------------------------------
	if(cap_get_flag(INCAP_USER))//捕获中断
	{
		//status = 1;
		val = get_incapture_value(INCAP_USER);
		gcIntTime++;
		if(gcIntTime%2){
			printf("1:%d:%d\n",val,Count);
			//printf("上升沿 此时计数器值:%d  ",val);
			//printf("空闲电平溢出次数：%d\n",Count);
			incapture_init(INCAP_USER,375,2000,CAP_DOWN);
			Count = 0; 
		}
		else{
			printf("0:%d:%d\n",val,Count);
			//printf("下降沿 此时计数器值:%d  ",val);
			//printf("高电平溢出次数：%d\n",Count);
			gcIntTime=0;
			incapture_init(INCAP_USER,375,2000,CAP_UP);
			Count = 0; 
		}	
	}
	else//溢出中断
	{
		Count++;
	}
//	else if(timer_get_int(15))
//	{
//		if(status) Count++;
//		Count++;
//		timer_clear_int(15);    //清时钟溢出标志位
//	}
	cap_clear_flag(INCAP_USER); //清中断
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS;     //关总中断
}

/*
//TIM15计数器中断处理程序
void TIM15_USER_Handler(void)
{
	uint16_t val;
	static uint16_t gcIntTime=0;
	DISABLE_INTERRUPTS;                        //关总中断
	//------------------------------------------------------------------
	static uint8_t Count = 0;
	if(timer_get_int(15))
	{
		if(cap_get_flag(INCAP_USER))
		{
			Count = 0; 
			val = get_incapture_value(INCAP_USER);
			gcIntTime++;
			if(gcIntTime%2){
				printf("1:%d\n",val);
				incapture_init(INCAP_USER,375,2000,CAP_DOWN); 
			}
			else{
				printf("0:%d\n",val);
				gcIntTime=0;
				incapture_init(INCAP_USER,375,2000,CAP_UP); 
			}
			cap_clear_flag(INCAP_USER); //清中断
		}
		else{
			Count++;
			printf("溢出次数：%d\n",Count);
			timer_clear_int(15);    //清时钟溢出标志位
		}
		
	}
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS;                        //开总中断
}




//定时器中断服务程序
void TIMi_IRQHandler(void)
{
	uint8_t CAPTURE_STA=0; //输入捕获状态
	uint16_t CAPTURE_VAL;//输入捕获值
  	if((CAPTURE_STA&0x80)!=0x80)//还未成功捕获
  	{
     	if (!cap_get_flag())
       	{
         	if(CAPTURE_STA&0x40)//已经捕获到高电平了
           	{
             	if((CAPTURE_STA&0x3F)==0x3F)//高电平太长了
                {
                  	CAPTURE_STA|=0x80;//标记成功捕获了一次
                  	CAPTURE_VAL=0xFFFF;
                 }
                 else CAPTURE_STA++;
          	}
     	}
 		if (cap_get_flag())//发生捕获事件
  		{
    		if(CAPTURE_STA&0x40) //捕获到一个下降沿
     		{
				  CAPTURE_STA|=0x80; //标记成功捕获到一次上升沿
				  CAPTURE_VAL=get_incapture_value(INCAP_USER);
				  incapture_init(INCAP_USER,375,2000,CAP_UP); //设置为上升沿捕获
       		}
  			else //还未开始,第一次捕获上升沿
   			{
				  CAPTURE_STA=0; //清空
				  CAPTURE_VAL=0;
				  TIM2CH1_CAPTURE_STA|=0x40; //标记捕获到了上升沿
				  incapture_init(INCAP_USER,375,2000,CAP_DOWN); //设置为下降沿捕获
    		}
  		}
	}
	cap_clear_flag();
}*/



/*
 知识要素：
 1.本文件中的中断处理函数调用的均是相关设备封装好的具体构件，在更换芯片
 时，只需保证设备的构件接口一致，即可保证本文件的相关中断处理函数不做任何
 更改，从而达到芯片无关性的要求。
 */
