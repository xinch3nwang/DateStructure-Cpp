//=====================================================================
//�ļ����ƣ�incapture.c
//���ܸ�Ҫ��incapture�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��STM32
//=====================================================================

#include "incapture.h"

//GPIO�ڻ���ַ���볣��������GPIO_ARR[0]~GPIO_ARR[5]��
extern GPIO_TypeDef * GPIO_ARR[] ;
		
static void timer_init2(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value);
static void tim_mux_val(uint16_t capNo,uint8_t* TIM_i,uint8_t* chl);
//===================================================================
//�������ƣ�incap_init
//���ܸ�Ҫ��incapģ���ʼ����
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//        clockFre��ʱ��Ƶ�ʣ���λ��Khz��ȡֵ��375��750��1500��3000��6000��
//                                                   12000��24000��48000
//        period�����ڣ���λΪ��������������������������ΧΪ1~65536
//        capmode�����벶׽ģʽ�������ء��½��ء�˫���أ����к궨�峣��ʹ��
//�������أ���
//ע�⣺          ��ΪGEC�и�����PWM�����벶׽����ͬһģ��ģ�ֻ��ͨ����ͬ������Ϊ��ֹ��ʹ�ö���
//          PWM�����벶׽ʱ��Ƶ�ʴ۸ģ���Ҫʹ��ʹ�õ���clockFre��period��������һ�¡�

//capNo  #define  INCAP_PIN0  GEC_10  //GEC_10��PTC_NUM|2��//UART0_TX
//       #define  INCAP_PIN1  GEC_8  //GEC_8��PTC_NUM|3��//UART0_RX
//===================================================================
void incapture_init(uint16_t capNo,uint32_t clockFre,uint16_t period,uint8_t capmode)
{
	GPIO_TypeDef *gpio_ptr;    //����gpio_ptrΪGPIO�ṹ������ָ��
	uint8_t port,pin;    //�����˿�port������pin����
	uint8_t TIM_i,chl;  //��tpmx_Chy��������timģ��š�ͨ������ʱ����
	uint32_t temp;       //��ʱ��żĴ������ֵ
	//gpio���Ž���
	port = (capNo>>8);     //�������Ķ˿�
	pin = capNo;           //�����������ź�
	
	tim_mux_val(capNo,&TIM_i,&chl);//�ɶ�Ӧģ��ţ�PTx_num|y)���ó�ʱ��ģ��ź�ͨ����
	timer_init2(TIM_i,clockFre,period);//ʱ��ģ���ʼ��

	//GPIO�Ĵ������Ÿ���
	gpio_ptr=GPIO_ARR[port];
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN_Msk<< (port * 1u);//ʹ��GPIOʱ��
	//����ģʽѡ������ѡ���ù���
    temp = gpio_ptr->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (pin * 2u));
	temp|=2<<(pin * 2u);
	gpio_ptr->MODER = temp;

	gpio_ptr->OTYPER &= ~(GPIO_OTYPER_OT0_Msk<<(pin * 1u));//�������
	gpio_ptr->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0<<(pin * 2u));//�����ٶ�
	gpio_ptr->PUPDR &= ~(GPIO_PUPDR_PUPD0<<(pin * 2u));//����
   //���Ÿ���ѡ���Ӧͨ������
    gpio_ptr->AFR[0]&=~(GPIO_AFRL_AFSEL0_Msk<< (pin * 4u));
    gpio_ptr->AFR[0]|=14<<(pin * 4u);
   
    if(TIM_i==15)
    {
        if(chl==1)
    	{
    	    TIM15->CCMR1&=~TIM_CCMR1_IC1F;//���˲���
    	    TIM15->CCMR1&=~TIM_CCMR1_IC1PSC;//��Ԥ��Ƶ��������������ÿ��⵽һ�����ر�ִ�в���
    	    TIM15->CCMR1|=TIM_CCMR1_CC1S_0;//CC1 ͨ������Ϊ���룬IC1 ӳ�䵽 TI1 ��
    	    //���벶׽�����趨CC1P��CC1NP���ʹ��
    	    if(capmode == CAP_UP)//�����ز�׽
    	    {
    	        TIM15->CCER&=~TIM_CCER_CC1P;
    		    TIM15->CCER&=~TIM_CCER_CC1NP;
    	     }
    	    else if(capmode == CAP_DOWN)//�½��ز�׽
    	    {
    		    TIM15->CCER|=TIM_CCER_CC1P;
    		    TIM15->CCER&=~TIM_CCER_CC1NP;
    	     }
    	    else if(capmode == CAP_DOUBLE)//˫���ز�׽
    	    {
    		    TIM15->CCER|=TIM_CCER_CC1P;
    		    TIM15->CCER|=TIM_CCER_CC1NP;
    	     }
            TIM15->CCER|=TIM_CCER_CC1E;//ʹ�ܲ��񣬾������Ƿ����ʵ�ʽ�������ֵ�������벶��/�ȽϼĴ��� 1(TIMx_CCR1)
        }
        if(chl==2)
        {
            TIM15->CCMR1&=~TIM_CCMR1_IC2F;//���˲���
    	    TIM15->CCMR1&=~TIM_CCMR1_IC2PSC;//��Ԥ��Ƶ��������������ÿ��⵽һ�����ر�ִ�в���
    	    TIM15->CCMR1|=TIM_CCMR1_CC2S_0;//CC2 ͨ������Ϊ���룬IC2 ӳ�䵽 TI2 ��
    	  //���벶׽�����趨CC2P��CC2NP���ʹ��
    	    if(capmode == CAP_UP)//�����ز�׽
    		{
    		    TIM15->CCER&=~TIM_CCER_CC2P;
    		    TIM15->CCER&=~TIM_CCER_CC2NP;
    		 }
    	    else if(capmode == CAP_DOWN)//�½��ز�׽
    	    {
    		    TIM15->CCER|=TIM_CCER_CC2P;
    		    TIM15->CCER&=~TIM_CCER_CC2NP;
    	     }
    	    else if(capmode == CAP_DOUBLE)//˫���ز�׽
    	    {
    		    TIM15->CCER|=TIM_CCER_CC2P;
    		    TIM15->CCER|=TIM_CCER_CC2NP;
    	     }
    		TIM15->CCER|=TIM_CCER_CC2E;//ʹ�ܲ��񣬾������Ƿ����ʵ�ʽ�������ֵ�������벶��/�ȽϼĴ���2(TIMx_CCR2)
		}
	}
}
//=====================================================================
//�������ƣ�incapture_value
//���ܸ�Ҫ����ȡ��ͨ���ļ�������ǰֵ
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//�������أ�ͨ���ļ�������ǰֵ
//=====================================================================
uint16_t get_incapture_value(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //��tapNo��������timģ��š�ͨ������ʱ����
    uint16_t cnt=0;
    tim_mux_val(capNo,&TIM_i,&chl);//����timģ��ź�ͨ����
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
//�������ƣ�cap_clear_flag
//���ܸ�Ҫ�������벶׽�жϱ�־λ��
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//�������أ���
//===================================================================
void cap_clear_flag(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //��capNo��������timģ��š�ͨ������ʱ����
    tim_mux_val(capNo,&TIM_i,&chl);
    if(TIM_i==15)
    {
    	if(chl==1)
    	{
        	TIM15->SR&=~TIM_SR_CC1IF;
        	TIM15->SR&=~TIM_SR_UIF;//������жϣ��Լӣ�
        }
        else if(chl==2)
        {
        	TIM15->SR&=~TIM_SR_CC2IF;
        	TIM15->SR&=~TIM_SR_UIF;//������жϣ��Լӣ�
        }
     }
}

//===================================================================
//�������ƣ�cap_get_flag
//���ܸ�Ҫ����ȡ���벶׽�жϱ�־λ��
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//�������أ����ص�ǰ�жϱ�־λ
//===================================================================
uint8_t cap_get_flag(uint16_t capNo)
{
    uint8_t TIM_i,chl,flag;  //��capNo��������timģ��š�ͨ������ʱ����
    flag=0;
    tim_mux_val(capNo,&TIM_i,&chl);   //����timģ��ź�ͨ����
    if(TIM_i==15)
    {
    	//���Լӣ�
    	if(chl==1)
        {
        	//���CC1IF=1��UIF=0�����������ж�
        	if((TIM15->SR&=TIM_SR_CC1IF)==TIM_SR_CC1IF &&(TIM15->SR & TIM_SR_UIF)!=TIM_SR_UIF)
            	flag=1;
            //���CC1IF=0��UIF=1,��������ж�
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
//�������ƣ�cap_enable_int
//���ܸ�Ҫ��ʹ�����벶׽�жϡ�
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//�������أ���
//===================================================================
void cap_enable_int(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //��capNo��������timģ��š�ͨ������ʱ����
    tim_mux_val(capNo,&TIM_i,&chl);   //����timģ��ź�ͨ����
    if(TIM_i==15)
    {
        if(chl==1)
        {
            TIM15->DIER|=TIM_DIER_CC1IE;//ʹ�ܲ����ж�
            TIM15->DIER|=TIM_DIER_UIE;//ʹ�ܸ����ж�(�Լ�)
            NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);    //���жϿ�����IRQ�ж�
         }
        else if(chl==2)
        {
         	TIM15->DIER|=TIM_DIER_CC2IE;//ʹ�ܲ����ж�
         	TIM15->DIER|=TIM_DIER_UIE;//ʹ�ܸ����ж�(�Լ�)
            NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);    //���жϿ�����IRQ�ж�
         }
     }

}

//===================================================================
//�������ƣ�cap_disable_int
//���ܸ�Ҫ����ֹ���벶׽�жϡ�
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//�������أ���
//===================================================================
void cap_disable_int(uint16_t capNo)
{
    uint8_t TIM_i,chl;  //��capNo��������timģ��š�ͨ������ʱ����
    tim_mux_val(capNo,&TIM_i,&chl);   //����timģ��ź�ͨ����
    if(TIM_i==15)
    {
        if(chl==1)
            {
                TIM15->DIER&=~TIM_DIER_CC1IE;    //��ֹ�����ж�
                NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);    //���жϿ�����IRQ�ж�
             }
        else if(chl==2)
            {
                TIM15->DIER&=~TIM_DIER_CC2IE;    //��ֹ�����ж�
                NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);    //���жϿ�����IRQ�ж�
             }
    }
}


//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�tpm_mux_val
//���ܸ�Ҫ������������tpmx_Chy���н������ó�����ģ�����ͨ���ţ�����TPM_CH0
//        ������PORT���ţ����������ŷ���mux_val����
//����˵����capNo�����벶׽ͨ���ţ�ʹ�ú궨��INCAP_PIN0��INCAP_PIN1������
//          TIM_i��TIMģ��ţ�chl����Ӧģ���ͨ��
//�������أ�gpio���üĴ�������ֵ
//=====================================================================
static void tim_mux_val(uint16_t capNo,uint8_t* TIM_i,uint8_t* chl)
{

    //����TIMģ��ź�ͨ����
    switch(capNo)
    {
        case ((0<<8)|2):*TIM_i =15;*chl=1;break;
        case ((0<<8)|3):*TIM_i =15;*chl=2;break;
        default:break;
    }

}
//===================================================================
//�������ƣ� tpm_timer_init
//���ܸ�Ҫ�� tpmģ���ʼ�������ü�����Ƶ��f�����������ʱ��MOD_Value��
//����˵���� TIM_i:TIMģ���
//           f����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//           MOD_Value����λ��ms����Χȡ���ڼ�����Ƶ���������λ����16λ��
//�������أ� ��
//==================================================================
static void timer_init2(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value)
{
    uint32_t clk_div;
	if(TIM_i==15)
	{
	    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;//ʹ�ܶ�ʱ����Ĭ��ѡ����ϵͳʱ�ӣ���48MHz
        clk_div=MCU_SYSTEM_CLK/(f*1000)-1;//��������ʱ��Ƶ��f���÷�Ƶϵ�� MCU_SYSTEM_CLK(48MHz)/clockFre(...kHz)
        TIM15->PSC = clk_div;//CNK=div*PSC
        TIM15->ARR = (uint16_t)(MCU_SYSTEM_CLK_KHZ/(clk_div+1)*MOD_Value-1);//�������ʱ��MOD_Value:period
        TIM15->EGR = TIM_EGR_UG;//��ʼ��������
        TIM15->CR1 |= TIM_CR1_CEN;//ʹ�ܼ�����
	}	  
}
