//=====================================================================
//文件名称：outcmp.c
//功能概要：pwm底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2019-11-21  V2.0
//适用芯片：STM32L4
//=====================================================================
#include "outcmp.h"

////GPIO口基地址放入常数数据组GPIO_ARR[0]~GPIO_ARR[5]中
//static const GPIO_TypeDef * GPIO_ARR[] =
//       {(GPIO_TypeDef *)GPIOA_BASE,(GPIO_TypeDef *)GPIOB_BASE,
//		(GPIO_TypeDef *)GPIOC_BASE,(GPIO_TypeDef *)GPIOD_BASE,
//		(GPIO_TypeDef *)GPIOE_BASE,(GPIO_TypeDef *)GPIOH_BASE};
       
//定时器模块2地址映射
TIM_TypeDef *OUTCMP_ARR[]={TIM2};
IRQn_Type OUTCMP_IRQ[]={TIM2_IRQn};

void outcmp_mux_val(uint16_t timx_Chy,uint8_t* TIM_i,uint8_t* chl);
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
void outcmp_init(uint16_t outcmpNo,uint32_t freq,uint32_t cmpPeriod,float cmpduty,uint8_t cmpmode)
{
//    GPIO_TypeDef * port_ptr;   //声明port_ptr为GPIO结构体类型指针
//    uint8_t port,pin;           //解析出的端口、引脚号临时变量
//    uint8_t mux_val;
    uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
    uint32_t mod;
    uint32_t clk_f;
    //防止越界
    if(cmpduty>100.0)  cmpduty=100.0;
    //1. gpio引脚解析
    //port = (outcmpNo>>8);    //解析出的端口
    //pin = outcmpNo;          //解析出的引脚号
    //2. 根据port，给局部变量port_ptr赋值,获得基地址
    //port_ptr = GPIO_ARR[port];  //获得GPIO模块相应口基地址
    //3. 获得解析的tim模块号和通道号
    outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //4.初始化TIM模块功能
    switch(TIM_i)
    {
    	case 0:
    	{
    	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	//1）使能定时器时钟
    	clk_f=MCU_SYSTEM_CLK /(freq*1000);
    	TIM2->PSC = clk_f-1;	//2）设置时钟频率
    	TIM2->ARR = (uint32_t)(MCU_SYSTEM_CLK_KHZ/(clk_f)*cmpPeriod-1);	//3）设置ARR寄存器
	    TIM2->CNT = 0;	//4）计数器清零
	    OUTCMP_ARR[TIM_i]->EGR |= TIM_EGR_UG_Msk;  //初始化定时器
	    break;
	    }
    }
    //5. 根据pin,指定该引脚功能为TIM的通道功能
    switch(outcmpNo)
    {
    	case ((0<<8)|5):
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
    //6. 输出比较模式的设定
    OUTCMP_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_CC3S; //设置通道3为输出
    OUTCMP_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE;   //输出预载除能
    OUTCMP_ARR[TIM_i]->CCER &=~TIM_CCER_CC3P;   //设置有效电平为高
    if(cmpmode == CMP_REV)        //翻转模式
    	OUTCMP_ARR[TIM_i]->CCMR2 |=	0x3UL<<TIM_CCMR2_OC3M_Pos;
    else if(cmpmode == CMP_LOW)   //强制低电平模式
        OUTCMP_ARR[TIM_i]->CCMR2 |=	0x2UL<<TIM_CCMR2_OC3M_Pos;
    else if(cmpmode == CMP_HIGH)  //强制高电平模式
    {
      OUTCMP_ARR[TIM_i]->CCMR2 |= 0x1UL<<TIM_CCMR2_OC3M_Pos;
    }
    //7. 输出比较占空比的设定
    mod=OUTCMP_ARR[TIM_i]->ARR+1;   //读ARR寄存器的值
    OUTCMP_ARR[TIM_i]->CCR3=(uint32_t)(mod*(cmpduty/100.0));
  
    OUTCMP_ARR[TIM_i]->CR1 |= TIM_CR1_CEN_Msk;//使能计数器
    OUTCMP_ARR[TIM_i]->CCER |=TIM_CCER_CC3E;  //通道３输出比较使能
   
    
}

//===================================================================
//函数名称：outcmp_enable_int
//功能概要：使能tim模块中断。
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：无
//===================================================================
void outcmp_enable_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
	OUTCMP_ARR[TIM_i]->DIER |= (0x1UL<<chl);
    //开TIM中断
    NVIC_EnableIRQ(OUTCMP_IRQ[TIM_i]);
}

//===================================================================
//函数名称：outcmp_disable_int
//功能概要：禁用tim模块中断。
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：无
//===================================================================
void outcmp_disable_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
	OUTCMP_ARR[TIM_i]->DIER &= ~(0x1UL<<chl);
    //关TIM中断
    NVIC_DisableIRQ(OUTCMP_IRQ[TIM_i]);
}

//===================================================================
//函数名称：outcmp_get_int
//功能概要：获取TIM模块中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint8_t outcmp_get_int(uint16_t outcmpNo)
{
  uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
  outcmp_mux_val(outcmpNo,&TIM_i,&chl);
  //获取TIM_i模块中断标志位
  if((OUTCMP_ARR[TIM_i]->SR & TIM_SR_UIF)==TIM_SR_UIF)
    return 1;
  else
    return 0;
}

//===================================================================
//函数名称：outcmp_chl_get_int
//功能概要：获取TIM通道中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint8_t outcmp_chl_get_int(uint16_t outcmpNo)
{
   uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
   outcmp_mux_val(outcmpNo,&TIM_i,&chl);
   //获取TIM_i模块chl通道中断标志位
   if((OUTCMP_ARR[TIM_i]->SR&(0x1UL<<chl))==(0x1UL<<chl))
       return 1;
   else
       return 0;
}

//===================================================================
//函数名称：outcmp_clear_int
//功能概要：清除TIM中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：清除中断标志位
//===================================================================
void outcmp_clear_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //清除TIM_i模块中断标志位
    BCLR(0,OUTCMP_ARR[TIM_i]->SR);
}

//===================================================================
//函数名称：outcmp_clear_chl_int
//功能概要：清除TIM通道中断标志
//参数说明：outcmpNo：模块号，使用gec.h中宏定义，例如OUTCMP_PIN0
//函数返回：清除TIM通道中断标志位
//===================================================================
void outcmp_clear_chl_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //由timx_Chy解析出的tim模块号、通道号临时变量
    outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //清除TIM_i模块chl通道中断标志位
    BCLR(chl,OUTCMP_ARR[TIM_i]->SR);
}

//------以下为内部函数------
//=====================================================================
//函数名称：outcmp_mux_val
//功能概要：将传进参数timx_Chy进行解析，得出具体模块号与通道号
//参数说明：timx_Chy：模块通道号
//
//函数返回：无
//=====================================================================
void  outcmp_mux_val(uint16_t timx_Chy,uint8_t* TIM_i,uint8_t* chl)
{
    //1.解析模块号和通道号
    switch(timx_Chy)
    {
        case OUTCMP_PIN0:*TIM_i =0;*chl=3;break;
        case OUTCMP_PIN1:*TIM_i =0;*chl=4;break;
        case OUTCMP_PIN2:*TIM_i =0;*chl=2;break;
        case OUTCMP_PIN3:*TIM_i =0;*chl=1;break;
        case OUTCMP_PIN4:*TIM_i =0;*chl=1;break;
        
    }
    
}
