//=====================================================================
//文件名称：incapture.c
//功能概要：incapture底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：STM32
//=====================================================================

#include "incapture.h"

//GPIO口基地址放入常数数据组GPIO_ARR[0]~GPIO_ARR[5]中
extern GPIO_TypeDef * GPIO_ARR[] ;
		
static void timer_init2(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value);
static void tim_mux_val(uint16_t capNo,uint8_t* TIM_i,uint8_t* chl);
//===================================================================
//函数名称：incap_init
//功能概要：incap模块初始化。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//        clockFre：时钟频率，单位：Khz，取值：375、750、1500、3000、6000、
//                                                   12000、24000、48000
//        period：周期，单位为个数，即计数器跳动次数，范围为1~65536
//        capmode：输入捕捉模式（上升沿、下降沿、双边沿），有宏定义常数使用
//函数返回：无
//注意：          因为GEC中给出的PWM和输入捕捉都是同一模块的，只是通道不同，所以为防止在使用多组
//          PWM和输入捕捉时，频率篡改，需要使得使用到的clockFre和period参数保持一致。

//capNo  #define  INCAP_PIN0  GEC_10  //GEC_10（PTC_NUM|2）//UART0_TX
//       #define  INCAP_PIN1  GEC_8  //GEC_8（PTC_NUM|3）//UART0_RX
//===================================================================
void incapture_init(uint16_t capNo,uint32_t clockFre,uint16_t period,uint8_t capmode)
{
	GPIO_TypeDef *gpio_ptr;    //声明gpio_ptr为GPIO结构体类型指针
	uint8_t port,pin;    //声明端口port、引脚pin变量
	uint8_t TIM_i,chl;  //由tpmx_Chy解析出的tim模块号、通道号临时变量
	uint32_t temp;       //临时存放寄存器里的值
	//gpio引脚解析
	port = (capNo>>8);     //解析出的端口
	pin = capNo;           //解析出的引脚号
	
	tim_mux_val(capNo,&TIM_i,&chl);//由对应模块号（PTx_num|y)）得出时钟模块号和通道号
	timer_init2(TIM_i,clockFre,period);//时钟模块初始化

	//GPIO寄存器引脚复用
	gpio_ptr=GPIO_ARR[port];
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN_Msk<< (port * 1u);//使能GPIO时钟
	//进行模式选择，引脚选择备用功能
    temp = gpio_ptr->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (pin * 2u));
	temp|=2<<(pin * 2u);
	gpio_ptr->MODER = temp;

	gpio_ptr->OTYPER &= ~(GPIO_OTYPER_OT0_Msk<<(pin * 1u));//推挽输出
	gpio_ptr->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0<<(pin * 2u));//设置速度
	gpio_ptr->PUPDR &= ~(GPIO_PUPDR_PUPD0<<(pin * 2u));//下拉
   //引脚复用选择对应通道功能
    gpio_ptr->AFR[0]&=~(GPIO_AFRL_AFSEL0_Msk<< (pin * 4u));
    gpio_ptr->AFR[0]|=14<<(pin * 4u);
   
    if(TIM_i==15)
    {
        if(chl==1)
    	{
    	    TIM15->CCMR1&=~TIM_CCMR1_IC1F;//无滤波器
    	    TIM15->CCMR1&=~TIM_CCMR1_IC1PSC;//无预分频器，捕获输入上每检测到一个边沿便执行捕获
    	    TIM15->CCMR1|=TIM_CCMR1_CC1S_0;//CC1 通道配置为输入，IC1 映射到 TI1 上
    	    //输入捕捉参数设定CC1P与CC1NP配合使用
    	    if(capmode == CAP_UP)//上升沿捕捉
    	    {
    	        TIM15->CCER&=~TIM_CCER_CC1P;
    		    TIM15->CCER&=~TIM_CCER_CC1NP;
    	     }
    	    else if(capmode == CAP_DOWN)//下降沿捕捉
    	    {
    		    TIM15->CCER|=TIM_CCER_CC1P;
    		    TIM15->CCER&=~TIM_CCER_CC1NP;
    	     }
    	    else if(capmode == CAP_DOUBLE)//双边沿捕捉
    	    {
    		    TIM15->CCER|=TIM_CCER_CC1P;
    		    TIM15->CCER|=TIM_CCER_CC1NP;
    	     }
            TIM15->CCER|=TIM_CCER_CC1E;//使能捕获，决定了是否可以实际将计数器值捕获到输入捕获/比较寄存器 1(TIMx_CCR1)
        }
        if(chl==2)
        {
            TIM15->CCMR1&=~TIM_CCMR1_IC2F;//无滤波器
    	    TIM15->CCMR1&=~TIM_CCMR1_IC2PSC;//无预分频器，捕获输入上每检测到一个边沿便执行捕获
    	    TIM15->CCMR1|=TIM_CCMR1_CC2S_0;//CC2 通道配置为输入，IC2 映射到 TI2 上
    	  //输入捕捉参数设定CC2P与CC2NP配合使用
    	    if(capmode == CAP_UP)//上升沿捕捉
    		{
    		    TIM15->CCER&=~TIM_CCER_CC2P;
    		    TIM15->CCER&=~TIM_CCER_CC2NP;
    		 }
    	    else if(capmode == CAP_DOWN)//下降沿捕捉
    	    {
    		    TIM15->CCER|=TIM_CCER_CC2P;
    		    TIM15->CCER&=~TIM_CCER_CC2NP;
    	     }
    	    else if(capmode == CAP_DOUBLE)//双边沿捕捉
    	    {
    		    TIM15->CCER|=TIM_CCER_CC2P;
    		    TIM15->CCER|=TIM_CCER_CC2NP;
    	     }
    		TIM15->CCER|=TIM_CCER_CC2E;//使能捕获，决定了是否可以实际将计数器值捕获到输入捕获/比较寄存器2(TIMx_CCR2)
		}
	}
}
//=====================================================================
//函数名称：incapture_value
//功能概要：获取该通道的计数器当前值
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//函数返回：通道的计数器当前值
//=====================================================================
uint16_t get_incapture_value(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //由tapNo解析出的tim模块号、通道号临时变量
    uint16_t cnt=0;
    tim_mux_val(capNo,&TIM_i,&chl);//解析tim模块号和通道号
    if(TIM_i==15)
    {
    	if(chl==1)
        	cnt=TIM15->CCR1;
        	
        else if(chl==2)
        	cnt=TIM15->CCR2;
        	
    }
    return cnt;
}

//===================================================================
//函数名称：cap_clear_flag
//功能概要：清输入捕捉中断标志位。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//函数返回：无
//===================================================================
void cap_clear_flag(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //由capNo解析出的tim模块号、通道号临时变量
    tim_mux_val(capNo,&TIM_i,&chl);
    if(TIM_i==15)
    {
    	if(chl==1)
    	{
        	TIM15->SR&=~TIM_SR_CC1IF;
        	TIM15->SR&=~TIM_SR_UIF;//清溢出中断（自加）
        }
        else if(chl==2)
        {
        	TIM15->SR&=~TIM_SR_CC2IF;
        	TIM15->SR&=~TIM_SR_UIF;//清溢出中断（自加）
        }
     }
}

//===================================================================
//函数名称：cap_get_flag
//功能概要：获取输入捕捉中断标志位。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//函数返回：返回当前中断标志位
//===================================================================
uint8_t cap_get_flag(uint16_t capNo)
{
    uint8_t TIM_i,chl,flag;  //由capNo解析出的tim模块号、通道号临时变量
    flag=0;
    tim_mux_val(capNo,&TIM_i,&chl);   //解析tim模块号和通道号
    if(TIM_i==15)
    {
    	//（自加）
    	if(chl==1)
        {
        	//如果CC1IF=1且UIF=0，则发生捕获中断
        	if((TIM15->SR&=TIM_SR_CC1IF)==TIM_SR_CC1IF &&(TIM15->SR & TIM_SR_UIF)!=TIM_SR_UIF)
            	flag=1;
            //如果CC1IF=0且UIF=1,则发生溢出中断
           	else if((TIM15->SR&TIM_SR_CC1IF)!=TIM_SR_CC1IF &&(TIM15->SR & TIM_SR_UIF)==TIM_SR_UIF)
           		flag=0;
         }
        else if(chl==2)
        {
            if((TIM15->SR&=TIM_SR_CC2IF)==TIM_SR_CC2IF &&(TIM15->SR & TIM_SR_UIF)!=TIM_SR_UIF)
            	flag=1;
            else if((TIM15->SR & TIM_SR_UIF)==TIM_SR_UIF)
           		flag=0;
         }
//        if(chl==1)
//        {
//        	if((TIM15->SR&=TIM_SR_CC1IF)==TIM_SR_CC1IF)
//            	flag=1;
//          else flag=0;
//         }
//        else if(chl==2)
//        {
//            if((TIM15->SR&=TIM_SR_CC2IF)==TIM_SR_CC2IF)
//            	flag=1;
//            else flag=0;
//         }
    }
    return flag;
}

//===================================================================
//函数名称：cap_enable_int
//功能概要：使能输入捕捉中断。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//函数返回：无
//===================================================================
void cap_enable_int(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //由capNo解析出的tim模块号、通道号临时变量
    tim_mux_val(capNo,&TIM_i,&chl);   //解析tim模块号和通道号
    if(TIM_i==15)
    {
        if(chl==1)
        {
            TIM15->DIER|=TIM_DIER_CC1IE;//使能捕获中断
            TIM15->DIER|=TIM_DIER_UIE;//使能更新中断(自加)
            NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);    //开中断控制器IRQ中断
         }
        else if(chl==2)
        {
         	TIM15->DIER|=TIM_DIER_CC2IE;//使能捕获中断
         	TIM15->DIER|=TIM_DIER_UIE;//使能更新中断(自加)
            NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);    //开中断控制器IRQ中断
         }
     }

}

//===================================================================
//函数名称：cap_disable_int
//功能概要：禁止输入捕捉中断。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//函数返回：无
//===================================================================
void cap_disable_int(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //由capNo解析出的tim模块号、通道号临时变量
    tim_mux_val(capNo,&TIM_i,&chl);   //解析tim模块号和通道号
    if(TIM_i==15)
    {
        if(chl==1)
            {
                TIM15->DIER&=~TIM_DIER_CC1IE;    //禁止捕获中断
                NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);    //关中断控制器IRQ中断
             }
        else if(chl==2)
            {
                TIM15->DIER&=~TIM_DIER_CC2IE;    //禁止捕获中断
                NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);    //关中断控制器IRQ中断
             }
    }
}


//------以下为内部函数------
//=====================================================================
//函数名称：tpm_mux_val
//功能概要：将传进参数tpmx_Chy进行解析，得出具体模块号与通道号（例：TPM_CH0
//        解析出PORT引脚，并根据引脚返回mux_val）。
//参数说明：capNo：输入捕捉通道号（使用宏定义INCAP_PIN0、INCAP_PIN1、…）
//          TIM_i：TIM模块号；chl：对应模块的通道
//函数返回：gpio复用寄存器传入值
//=====================================================================
static void tim_mux_val(uint16_t capNo,uint8_t* TIM_i,uint8_t* chl)
{

    //解析TIM模块号和通道号
    switch(capNo)
    {
        case ((0<<8)|2):*TIM_i =15;*chl=1;break;
        case ((0<<8)|3):*TIM_i =15;*chl=2;break;
        default:break;
    }

}
//===================================================================
//函数名称： tpm_timer_init
//功能概要： tpm模块初始化，设置计数器频率f及计数器溢出时间MOD_Value。
//参数说明： TIM_i:TIM模块号
//           f：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//           MOD_Value：单位：ms，范围取决于计数器频率与计数器位数（16位）
//函数返回： 无
//==================================================================
static void timer_init2(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value)
{
    uint32_t clk_div;
	if(TIM_i==15)
	{
	    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;//使能定时器，默认选择用系统时钟，即48MHz
        clk_div=MCU_SYSTEM_CLK/(f*1000)-1;//根据期望时钟频率f设置分频系数 MCU_SYSTEM_CLK(48MHz)/clockFre(...kHz)
        TIM15->PSC = clk_div;//CNK=div*PSC
        TIM15->ARR = (uint16_t)(MCU_SYSTEM_CLK_KHZ/(clk_div+1)*MOD_Value-1);//设置溢出时间MOD_Value:period
        TIM15->EGR = TIM_EGR_UG;//初始化计数器
        TIM15->CR1 |= TIM_CR1_CEN;//使能计数器
	}	  
}
