//=====================================================================
//�ļ����ƣ�outcmp.c
//���ܸ�Ҫ��pwm�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����2019-11-21  V2.0
//����оƬ��STM32L4
//=====================================================================
#include "outcmp.h"

////GPIO�ڻ���ַ���볣��������GPIO_ARR[0]~GPIO_ARR[5]��
//static const GPIO_TypeDef * GPIO_ARR[] =
//       {(GPIO_TypeDef *)GPIOA_BASE,(GPIO_TypeDef *)GPIOB_BASE,
//		(GPIO_TypeDef *)GPIOC_BASE,(GPIO_TypeDef *)GPIOD_BASE,
//		(GPIO_TypeDef *)GPIOE_BASE,(GPIO_TypeDef *)GPIOH_BASE};
       
//��ʱ��ģ��2��ַӳ��
TIM_TypeDef *OUTCMP_ARR[]={TIM2};
IRQn_Type OUTCMP_IRQ[]={TIM2_IRQn};

void outcmp_mux_val(uint16_t timx_Chy,uint8_t* TIM_i,uint8_t* chl);
//===================================================================
//�������ƣ�outcmp_init
//���ܸ�Ҫ��outcmpģ���ʼ����
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//        freq����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000(Ƶ�ʾ�������)
//        cmpPeriod����λ��ms����Χȡ���ڼ�����Ƶ���������λ����16λ��
//        comduty������Ƚϵ�ƽ��תλ��ռ�����ڵı�����0.0~100.0��Ӧ0%~100%
//        cmpmode������Ƚ�ģʽ����ת��ƽ��ǿ�Ƶ͵�ƽ��ǿ�Ƹߵ�ƽ��,�к궨�峣��ʹ��
//�������أ���
//===================================================================
void outcmp_init(uint16_t outcmpNo,uint32_t freq,uint32_t cmpPeriod,float cmpduty,uint8_t cmpmode)
{
//    GPIO_TypeDef * port_ptr;   //����port_ptrΪGPIO�ṹ������ָ��
//    uint8_t port,pin;           //�������Ķ˿ڡ����ź���ʱ����
//    uint8_t mux_val;
    uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
    uint32_t mod;
    uint32_t clk_f;
    //��ֹԽ��
    if(cmpduty>100.0)  cmpduty=100.0;
    //1. gpio���Ž���
    //port = (outcmpNo>>8);    //�������Ķ˿�
    //pin = outcmpNo;          //�����������ź�
    //2. ����port�����ֲ�����port_ptr��ֵ,��û���ַ
    //port_ptr = GPIO_ARR[port];  //���GPIOģ����Ӧ�ڻ���ַ
    //3. ��ý�����timģ��ź�ͨ����
    outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //4.��ʼ��TIMģ�鹦��
    switch(TIM_i)
    {
    	case 0:
    	{
    	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	//1��ʹ�ܶ�ʱ��ʱ��
    	clk_f=MCU_SYSTEM_CLK /(freq*1000);
    	TIM2->PSC = clk_f-1;	//2������ʱ��Ƶ��
    	TIM2->ARR = (uint32_t)(MCU_SYSTEM_CLK_KHZ/(clk_f)*cmpPeriod-1);	//3������ARR�Ĵ���
	    TIM2->CNT = 0;	//4������������
	    OUTCMP_ARR[TIM_i]->EGR |= TIM_EGR_UG_Msk;  //��ʼ����ʱ��
	    break;
	    }
    }
    //5. ����pin,ָ�������Ź���ΪTIM��ͨ������
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
    //6. ����Ƚ�ģʽ���趨
    OUTCMP_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_CC3S; //����ͨ��3Ϊ���
    OUTCMP_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE;   //���Ԥ�س���
    OUTCMP_ARR[TIM_i]->CCER &=~TIM_CCER_CC3P;   //������Ч��ƽΪ��
    if(cmpmode == CMP_REV)        //��תģʽ
    	OUTCMP_ARR[TIM_i]->CCMR2 |=	0x3UL<<TIM_CCMR2_OC3M_Pos;
    else if(cmpmode == CMP_LOW)   //ǿ�Ƶ͵�ƽģʽ
        OUTCMP_ARR[TIM_i]->CCMR2 |=	0x2UL<<TIM_CCMR2_OC3M_Pos;
    else if(cmpmode == CMP_HIGH)  //ǿ�Ƹߵ�ƽģʽ
    {
      OUTCMP_ARR[TIM_i]->CCMR2 |= 0x1UL<<TIM_CCMR2_OC3M_Pos;
    }
    //7. ����Ƚ�ռ�ձȵ��趨
    mod=OUTCMP_ARR[TIM_i]->ARR+1;   //��ARR�Ĵ�����ֵ
    OUTCMP_ARR[TIM_i]->CCR3=(uint32_t)(mod*(cmpduty/100.0));
  
    OUTCMP_ARR[TIM_i]->CR1 |= TIM_CR1_CEN_Msk;//ʹ�ܼ�����
    OUTCMP_ARR[TIM_i]->CCER |=TIM_CCER_CC3E;  //ͨ��������Ƚ�ʹ��
   
    
}

//===================================================================
//�������ƣ�outcmp_enable_int
//���ܸ�Ҫ��ʹ��timģ���жϡ�
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ���
//===================================================================
void outcmp_enable_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
	OUTCMP_ARR[TIM_i]->DIER |= (0x1UL<<chl);
    //��TIM�ж�
    NVIC_EnableIRQ(OUTCMP_IRQ[TIM_i]);
}

//===================================================================
//�������ƣ�outcmp_disable_int
//���ܸ�Ҫ������timģ���жϡ�
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ���
//===================================================================
void outcmp_disable_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
	OUTCMP_ARR[TIM_i]->DIER &= ~(0x1UL<<chl);
    //��TIM�ж�
    NVIC_DisableIRQ(OUTCMP_IRQ[TIM_i]);
}

//===================================================================
//�������ƣ�outcmp_get_int
//���ܸ�Ҫ����ȡTIMģ���жϱ�־
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ��жϱ�־ 1=���жϲ���;0=���жϲ���
//===================================================================
uint8_t outcmp_get_int(uint16_t outcmpNo)
{
  uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
  outcmp_mux_val(outcmpNo,&TIM_i,&chl);
  //��ȡTIM_iģ���жϱ�־λ
  if((OUTCMP_ARR[TIM_i]->SR & TIM_SR_UIF)==TIM_SR_UIF)
    return 1;
  else
    return 0;
}

//===================================================================
//�������ƣ�outcmp_chl_get_int
//���ܸ�Ҫ����ȡTIMͨ���жϱ�־
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ��жϱ�־ 1=���жϲ���;0=���жϲ���
//===================================================================
uint8_t outcmp_chl_get_int(uint16_t outcmpNo)
{
   uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
   outcmp_mux_val(outcmpNo,&TIM_i,&chl);
   //��ȡTIM_iģ��chlͨ���жϱ�־λ
   if((OUTCMP_ARR[TIM_i]->SR&(0x1UL<<chl))==(0x1UL<<chl))
       return 1;
   else
       return 0;
}

//===================================================================
//�������ƣ�outcmp_clear_int
//���ܸ�Ҫ�����TIM�жϱ�־
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ�����жϱ�־λ
//===================================================================
void outcmp_clear_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
	outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //���TIM_iģ���жϱ�־λ
    BCLR(0,OUTCMP_ARR[TIM_i]->SR);
}

//===================================================================
//�������ƣ�outcmp_clear_chl_int
//���ܸ�Ҫ�����TIMͨ���жϱ�־
//����˵����outcmpNo��ģ��ţ�ʹ��gec.h�к궨�壬����OUTCMP_PIN0
//�������أ����TIMͨ���жϱ�־λ
//===================================================================
void outcmp_clear_chl_int(uint16_t outcmpNo)
{
	uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
    outcmp_mux_val(outcmpNo,&TIM_i,&chl);
    //���TIM_iģ��chlͨ���жϱ�־λ
    BCLR(chl,OUTCMP_ARR[TIM_i]->SR);
}

//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�outcmp_mux_val
//���ܸ�Ҫ������������timx_Chy���н������ó�����ģ�����ͨ����
//����˵����timx_Chy��ģ��ͨ����
//
//�������أ���
//=====================================================================
void  outcmp_mux_val(uint16_t timx_Chy,uint8_t* TIM_i,uint8_t* chl)
{
    //1.����ģ��ź�ͨ����
    switch(timx_Chy)
    {
        case OUTCMP_PIN0:*TIM_i =0;*chl=3;break;
        case OUTCMP_PIN1:*TIM_i =0;*chl=4;break;
        case OUTCMP_PIN2:*TIM_i =0;*chl=2;break;
        case OUTCMP_PIN3:*TIM_i =0;*chl=1;break;
        case OUTCMP_PIN4:*TIM_i =0;*chl=1;break;
        
    }
    
}
