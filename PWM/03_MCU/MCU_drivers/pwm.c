//=====================================================================
//文件名称：pwm.c
//功能概要：pwm底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2019-11-16  V2.0
//适用芯片：STM32L4
//=====================================================================
#include "includes.h"
#include "pwm.h"
//GPIO口基地址放入常数数据组GPIO_ARR[0]~GPIO_ARR[5]中
//static const GPIO_TypeDef * GPIO_ARR[] =
//       {(GPIO_TypeDef *)GPIOA_BASE,(GPIO_TypeDef *)GPIOB_BASE,
//		(GPIO_TypeDef *)GPIOC_BASE,(GPIO_TypeDef *)GPIOD_BASE,
//		(GPIO_TypeDef *)GPIOE_BASE,(GPIO_TypeDef *)GPIOH_BASE};
//定时器模块2地址映射
TIM_TypeDef *PWM_ARR[]={TIM2};
//*****************************内部函数******************************
//static void tim_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl);
void tim_mux_val(uint16_t pwmNo,uint8_t* TIM_i,uint8_t* chl);
void tim_timer_init1(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value);

//*****************************对外接口函数******************************

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
//     占空比：是指高电平在一个周期中所占的时间比例。
//=====================================================================
void pwm_init(uint16_t pwmNo,uint32_t clockFre,uint16_t period,double duty,
			                                   uint8_t align,uint8_t pol)
{
    uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
    uint32_t temp;
	//（防止越界
	if(duty>100.0) duty=100.0;  
    //（1）获得解析的tim模块号和通道号并对相应的引脚进行引脚复用
    tim_mux_val(pwmNo,&TIM_i,&chl);
    //（2）PWM对齐方式的设定
    if(align == PWM_CENTER)//中心对齐
    {
		tim_timer_init1(TIM_i,clockFre,period/2);
        PWM_ARR[TIM_i]->CR1 |= TIM_CR1_CMS;//设置中心对齐
        if(pol == PWM_PLUS)             //正极性    
		{
        	temp = (uint32_t)(duty*period/200);
            if(temp >= 65536)  temp=65535;
            switch(chl) 
            {
            	case 1:	//通道1
            	{
                 	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                 	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //选择PWM模式1
                 	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC1P_Msk;       //设置为正极性
                 	PWM_ARR[TIM_i]->CCR1=temp;  //设置占空比
                 	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                 	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //使能ch1预装载寄存器
                 	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;		 //使能PWM输出
				 	//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      
				 	break;
             	}
            	case 2:   //通道2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC2P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR2=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //使能ch2预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;     //使能PWM输出
					break;
             	}
            	case 3:   //通道3
           		{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC3P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR3=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //使能ch3预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //使能PWM输出
					break;
            	}
				case 4:  //通道4
				{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC4P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR4=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //使能ch4预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //使能PWM输出
					break;
            	}
            }
		}
		else   //负极性
		{
			temp = (uint32_t)(period*(100-duty)/200);
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //通道1
            	{
					PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC1P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR1=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //使能ch1预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //使能PWM输出
					break;
            	}
            	case 2:   //通道2
           		{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC2P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR2=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //使能ch2预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //使能PWM输出
					break;
            	}
            	case 3:   //通道3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC3P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR3=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //使能ch3预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //使能PWM输出
					break;
            	}
            	case 4:  //通道4
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC4P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR4=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //使能ch4预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //使能PWM输出
					break;
				}
			}
		}
	}
	else                  //边沿对齐
	{
	    tim_timer_init1(TIM_i,clockFre,period);
        PWM_ARR[TIM_i]->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);//设置向上计数模式、边沿对齐
        //（3）PWM极性的设定和PWM占空比的设定
        temp = PWM_ARR[TIM_i]->ARR + 1;  //计算周期（period）
        if(pol == PWM_PLUS)             //正极性    
        {
			temp = (uint32_t)(temp*duty/100);  
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //通道1
            	{
					PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC1P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR1=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //使能ch1预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //使能PWM输出
					break;
            	}
            	case 2:   //通道2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC2P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR2=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //使能ch2预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //使能PWM输出
					break;
             	}
            	case 3:   //通道3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC3P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR3=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //使能ch3预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //使能PWM输出
					break;
            	}
            	case 4:  //通道4
				{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC4P_Msk;       //设置为正极性
                	PWM_ARR[TIM_i]->CCR4=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //使能ch4预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //使能PWM输出
					break;
            	}
            
        	}
        }
        else   //负极性
        {
			temp = (uint32_t)(temp*(100-duty)/100);  
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //通道1
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC1P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR1=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //使能ch1预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //使能PWM输出
					break;
             	}
            	case 2:   //通道2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC2P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR2=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //使能ch2预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //使能PWM输出
					break;
             	}
            	case 3:   //通道3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC3P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR3=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //使能ch3预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //使能PWM输出
					break;
            	}
            	case 4:  //通道4
           		{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //选择PWM模式1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC4P_Msk;       //设置为负极性
                	PWM_ARR[TIM_i]->CCR4=temp;  //设置占空比
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //使能ch4预装载寄存器
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //使能PWM输出
					break;
            	}
    		}
    	}
 	}
	PWM_ARR[TIM_i]->EGR |= TIM_EGR_UG_Msk;  //初始化计数器；
    PWM_ARR[TIM_i]->CR1 |= TIM_CR1_CEN_Msk | TIM_CR1_ARPE_Msk;//使能定时器
}

//=====================================================================
//函数名称：  pwm_update
//功能概要：  timx模块Chy通道的PWM更新
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          duty：占空比：0.0~100.0对应0%~100%
//函数返回：  无
//=====================================================================
void pwm_update(uint16_t pwmNo,double duty)
{
    uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
    uint32_t period;
    // 防止越界
    if(duty>100.0)  duty=100.0;
    //1. 获得解析的tpm模块号和通道号
    tim_mux_val(pwmNo,&TIM_i,&chl);
    period=PWM_ARR[TIM_i]->ARR;
    //2. 更新PWM通道寄存器值
    switch(chl)
    {
    	case 1:PWM_ARR[TIM_i]->CCR1=(uint32_t)(period*duty/100);break;
    	case 2:PWM_ARR[TIM_i]->CCR2=(uint32_t)(period*duty/100);break;
    	case 3:PWM_ARR[TIM_i]->CCR3=(uint32_t)(period*duty/100);break;
    	case 4:PWM_ARR[TIM_i]->CCR4=(uint32_t)(period*duty/100);break;
    }
}

//------以下为内部函数------
//=====================================================================
//函数名称：tpm_mux_val
//功能概要：将传进参数pwmNo进行解析，得出具体模块号与通道号（例：PWM_PIN0
//         解析出PORT引脚,并设置指定引脚为PWM功能）。
//参数说明：pwmNo：pwm模块号，在头文件的宏定义给出，如PWM_PIN0表示PWM0通道
//
//函数返回：无
//=====================================================================
void tim_mux_val(uint16_t pwmNo,uint8_t* TIM_i,uint8_t* chl)
{ 
    //uint8_t port,pin;
    //1.解析模块号和通道号
    switch(pwmNo)
    {
		case ((0<<8)|5):
		        *TIM_i =0;
		        *chl=1;
		        RCC->AHB2ENR |=RCC_AHB2ENR_GPIOAEN_Msk;
		        GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
				GPIOA->MODER |= 2<<GPIO_MODER_MODE5_Pos;
				GPIOA->OTYPER &= ~GPIO_OTYPER_OT5_Msk;
				GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5_Msk;
				GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;
				GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5_Msk;
				GPIOA->AFR[0] |= 1<<GPIO_AFRL_AFSEL5_Pos;
		break;
		case ((0<<8)|15):
		        *TIM_i =0;
		        *chl=1;
		        RCC->AHB2ENR |=RCC_AHB2ENR_GPIOAEN_Msk;
		        GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk;
				GPIOA->MODER |= 2<<GPIO_MODER_MODE5_Pos;
				GPIOA->OTYPER &= ~GPIO_OTYPER_OT15_Msk;
				GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED15_Msk;
				GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD15_Msk;
				GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL15_Msk;
				GPIOA->AFR[1] |= 1<<GPIO_AFRH_AFSEL15_Pos;
		break;
		case ((1<<8)|3):
		        *TIM_i =0;
		        *chl=2;
		        RCC->AHB2ENR |=RCC_AHB2ENR_GPIOBEN_Msk;
		        GPIOB->MODER &= ~GPIO_MODER_MODE3_Msk;
				GPIOB->MODER |= 2<<GPIO_MODER_MODE3_Pos;
				GPIOB->OTYPER &= ~GPIO_OTYPER_OT3_Msk;
				GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3_Msk;
				GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD3_Msk;
				GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL3_Msk;
				GPIOB->AFR[0] |= 1<<GPIO_AFRL_AFSEL3_Pos;
		break;
		case ((1<<8)|10):
		        *TIM_i =0;
		        *chl=3;
		        RCC->AHB2ENR |=RCC_AHB2ENR_GPIOBEN_Msk;
		        GPIOB->MODER &= ~GPIO_MODER_MODE10_Msk;
				GPIOB->MODER |= 2<<GPIO_MODER_MODE10_Pos;
				GPIOB->OTYPER &= ~GPIO_OTYPER_OT10_Msk;
				GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED10_Msk;
				GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD10_Msk;
				GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL10_Msk;
				GPIOB->AFR[1] |= 1<<GPIO_AFRH_AFSEL10_Pos;
		break;
		case ((1<<8)|11):
		       *TIM_i =0;
		       *chl=4;
		       RCC->AHB2ENR |=RCC_AHB2ENR_GPIOBEN_Msk;
		       GPIOB->MODER &= ~GPIO_MODER_MODE11_Msk;
			   GPIOB->MODER |= 2<<GPIO_MODER_MODE11_Pos;
			   GPIOB->OTYPER &= ~GPIO_OTYPER_OT11_Msk;
			   GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED11_Msk;
			   GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD11_Msk;
			   GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL11_Msk;
			   GPIOB->AFR[1] |= 1<<GPIO_AFRH_AFSEL11_Pos;
		break;
    }	
}

//===================================================================
//函数名称：  tim_timer_init1
//功能概要：  tim模块初始化，设置计数器频率f及计数器溢出时间MOD_Value。
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          f：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//          MOD_Value：单位个数：范围取决于计数器频率与计数器位数（16位）
//函数返回：  无
//===================================================================
void tim_timer_init1(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value)
{
    //局部变量声明
    uint32_t clk_f;
    if(TIM_i==0)
    {
	    //（1）使能定时器时钟
	    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	    //（2）设置时钟频率 
	    clk_f=MCU_SYSTEM_CLK /f;
	    TIM2->PSC = clk_f-1;
	    // （3）计数器清零
	    TIM2->CNT = 0;
	    //（4）设置溢出值
	    if(MOD_Value == 0)
	    {
	    	TIM2->ARR = 0; 
	    }
	    else
	    {
	    	TIM2->ARR = (uint32_t)(MOD_Value-1);  
	    }
		//（5）更新中断使能
		//TIM2->DIER |= TIM_DIER_UIE_Msk;
	}
}
