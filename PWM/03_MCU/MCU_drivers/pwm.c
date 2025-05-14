//=====================================================================
//�ļ����ƣ�pwm.c
//���ܸ�Ҫ��pwm�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����2019-11-16  V2.0
//����оƬ��STM32L4
//=====================================================================
#include "includes.h"
#include "pwm.h"
//GPIO�ڻ���ַ���볣��������GPIO_ARR[0]~GPIO_ARR[5]��
//static const GPIO_TypeDef * GPIO_ARR[] =
//       {(GPIO_TypeDef *)GPIOA_BASE,(GPIO_TypeDef *)GPIOB_BASE,
//		(GPIO_TypeDef *)GPIOC_BASE,(GPIO_TypeDef *)GPIOD_BASE,
//		(GPIO_TypeDef *)GPIOE_BASE,(GPIO_TypeDef *)GPIOH_BASE};
//��ʱ��ģ��2��ַӳ��
TIM_TypeDef *PWM_ARR[]={TIM2};
//*****************************�ڲ�����******************************
//static void tim_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl);
void tim_mux_val(uint16_t pwmNo,uint8_t* TIM_i,uint8_t* chl);
void tim_timer_init1(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value);

//*****************************����ӿں���******************************

//=====================================================================
//�������ƣ�  pwm_init
//���ܸ�Ҫ��  pwm��ʼ������
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          clockFre��ʱ��Ƶ�ʣ���λ��hz��ȡֵ��375��750��1500��3000��6000��
//                                                   12000��24000��48000
//          period�����ڣ���λΪ��������������������������ΧΪ1~65536
//          duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//          align�����뷽ʽ ����ͷ�ļ��궨���������PWM_EDGEΪ���ض��䡣
//          pol�����ԣ���ͷ�ļ��궨���������PWM_PLUSΪ������
//�������أ�  ��
//ע�⣺          ��ΪGEC�и�����PWM�����벶׽����ͬһģ��ģ�ֻ��ͨ����ͬ������Ϊ��ֹ��ʹ�ö���
//          PWM�����벶׽ʱ��Ƶ�ʴ۸ģ���Ҫʹ��ʹ�õ���clockFre��period��������һ�¡�
//     ռ�ձȣ���ָ�ߵ�ƽ��һ����������ռ��ʱ�������
//=====================================================================
void pwm_init(uint16_t pwmNo,uint32_t clockFre,uint16_t period,double duty,
			                                   uint8_t align,uint8_t pol)
{
    uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
    uint32_t temp;
	//����ֹԽ��
	if(duty>100.0) duty=100.0;  
    //��1����ý�����timģ��ź�ͨ���Ų�����Ӧ�����Ž������Ÿ���
    tim_mux_val(pwmNo,&TIM_i,&chl);
    //��2��PWM���뷽ʽ���趨
    if(align == PWM_CENTER)//���Ķ���
    {
		tim_timer_init1(TIM_i,clockFre,period/2);
        PWM_ARR[TIM_i]->CR1 |= TIM_CR1_CMS;//�������Ķ���
        if(pol == PWM_PLUS)             //������    
		{
        	temp = (uint32_t)(duty*period/200);
            if(temp >= 65536)  temp=65535;
            switch(chl) 
            {
            	case 1:	//ͨ��1
            	{
                 	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                 	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //ѡ��PWMģʽ1
                 	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC1P_Msk;       //����Ϊ������
                 	PWM_ARR[TIM_i]->CCR1=temp;  //����ռ�ձ�
                 	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                 	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //ʹ��ch1Ԥװ�ؼĴ���
                 	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;		 //ʹ��PWM���
				 	//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      
				 	break;
             	}
            	case 2:   //ͨ��2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC2P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR2=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //ʹ��ch2Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;     //ʹ��PWM���
					break;
             	}
            	case 3:   //ͨ��3
           		{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC3P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR3=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //ʹ��ch3Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //ʹ��PWM���
					break;
            	}
				case 4:  //ͨ��4
				{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC4P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR4=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //ʹ��ch4Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //ʹ��PWM���
					break;
            	}
            }
		}
		else   //������
		{
			temp = (uint32_t)(period*(100-duty)/200);
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //ͨ��1
            	{
					PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC1P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR1=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //ʹ��ch1Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 2:   //ͨ��2
           		{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC2P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR2=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //ʹ��ch2Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 3:   //ͨ��3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC3P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR3=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //ʹ��ch3Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 4:  //ͨ��4
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC4P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR4=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //ʹ��ch4Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //ʹ��PWM���
					break;
				}
			}
		}
	}
	else                  //���ض���
	{
	    tim_timer_init1(TIM_i,clockFre,period);
        PWM_ARR[TIM_i]->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);//�������ϼ���ģʽ�����ض���
        //��3��PWM���Ե��趨��PWMռ�ձȵ��趨
        temp = PWM_ARR[TIM_i]->ARR + 1;  //�������ڣ�period��
        if(pol == PWM_PLUS)             //������    
        {
			temp = (uint32_t)(temp*duty/100);  
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //ͨ��1
            	{
					PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC1P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR1=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //ʹ��ch1Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 2:   //ͨ��2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC2P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR2=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //ʹ��ch2Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //ʹ��PWM���
					break;
             	}
            	case 3:   //ͨ��3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC3P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR3=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //ʹ��ch3Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 4:  //ͨ��4
				{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= ~TIM_CCER_CC4P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR4=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //ʹ��ch4Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //ʹ��PWM���
					break;
            	}
            
        	}
        }
        else   //������
        {
			temp = (uint32_t)(temp*(100-duty)/100);  
            if(temp >= 65536)  temp=65535;
            switch(chl)
            {
            	case 1: //ͨ��1
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC1P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR1=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC1PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC1PE_Msk;     //ʹ��ch1Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC1NP_Msk;      //ʹ��PWM���
					break;
             	}
            	case 2:   //ͨ��2
            	{
                	PWM_ARR[TIM_i]->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC2P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR2=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR1 &=~TIM_CCMR1_OC2PE_Msk;
                	PWM_ARR[TIM_i]->CCMR1 |=TIM_CCMR1_OC2PE_Msk;     //ʹ��ch2Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC2NP_Msk;      //ʹ��PWM���
					break;
             	}
            	case 3:   //ͨ��3
            	{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC3P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR3=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC3PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC3PE_Msk;     //ʹ��ch3Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC3NP_Msk;      //ʹ��PWM���
					break;
            	}
            	case 4:  //ͨ��4
           		{
                	PWM_ARR[TIM_i]->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |= 6 << TIM_CCMR2_OC4M_Pos; //ѡ��PWMģʽ1
                	PWM_ARR[TIM_i]->CCER &= TIM_CCER_CC4P_Msk;       //����Ϊ������
                	PWM_ARR[TIM_i]->CCR4=temp;  //����ռ�ձ�
                	PWM_ARR[TIM_i]->CCMR2 &=~TIM_CCMR2_OC4PE_Msk;
                	PWM_ARR[TIM_i]->CCMR2 |=TIM_CCMR2_OC4PE_Msk;     //ʹ��ch4Ԥװ�ؼĴ���
                	PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4E_Msk;
					//PWM_ARR[TIM_i]->CCER |= TIM_CCER_CC4NP_Msk;      //ʹ��PWM���
					break;
            	}
    		}
    	}
 	}
	PWM_ARR[TIM_i]->EGR |= TIM_EGR_UG_Msk;  //��ʼ����������
    PWM_ARR[TIM_i]->CR1 |= TIM_CR1_CEN_Msk | TIM_CR1_ARPE_Msk;//ʹ�ܶ�ʱ��
}

//=====================================================================
//�������ƣ�  pwm_update
//���ܸ�Ҫ��  timxģ��Chyͨ����PWM����
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//�������أ�  ��
//=====================================================================
void pwm_update(uint16_t pwmNo,double duty)
{
    uint8_t TIM_i,chl;  //��timx_Chy��������timģ��š�ͨ������ʱ����
    uint32_t period;
    // ��ֹԽ��
    if(duty>100.0)  duty=100.0;
    //1. ��ý�����tpmģ��ź�ͨ����
    tim_mux_val(pwmNo,&TIM_i,&chl);
    period=PWM_ARR[TIM_i]->ARR;
    //2. ����PWMͨ���Ĵ���ֵ
    switch(chl)
    {
    	case 1:PWM_ARR[TIM_i]->CCR1=(uint32_t)(period*duty/100);break;
    	case 2:PWM_ARR[TIM_i]->CCR2=(uint32_t)(period*duty/100);break;
    	case 3:PWM_ARR[TIM_i]->CCR3=(uint32_t)(period*duty/100);break;
    	case 4:PWM_ARR[TIM_i]->CCR4=(uint32_t)(period*duty/100);break;
    }
}

//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�tpm_mux_val
//���ܸ�Ҫ������������pwmNo���н������ó�����ģ�����ͨ���ţ�����PWM_PIN0
//         ������PORT����,������ָ������ΪPWM���ܣ���
//����˵����pwmNo��pwmģ��ţ���ͷ�ļ��ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//
//�������أ���
//=====================================================================
void tim_mux_val(uint16_t pwmNo,uint8_t* TIM_i,uint8_t* chl)
{ 
    //uint8_t port,pin;
    //1.����ģ��ź�ͨ����
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
//�������ƣ�  tim_timer_init1
//���ܸ�Ҫ��  timģ���ʼ�������ü�����Ƶ��f�����������ʱ��MOD_Value��
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          f����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//          MOD_Value����λ��������Χȡ���ڼ�����Ƶ���������λ����16λ��
//�������أ�  ��
//===================================================================
void tim_timer_init1(uint16_t TIM_i,uint32_t f,uint16_t MOD_Value)
{
    //�ֲ���������
    uint32_t clk_f;
    if(TIM_i==0)
    {
	    //��1��ʹ�ܶ�ʱ��ʱ��
	    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	    //��2������ʱ��Ƶ�� 
	    clk_f=MCU_SYSTEM_CLK /f;
	    TIM2->PSC = clk_f-1;
	    // ��3������������
	    TIM2->CNT = 0;
	    //��4���������ֵ
	    if(MOD_Value == 0)
	    {
	    	TIM2->ARR = 0; 
	    }
	    else
	    {
	    	TIM2->ARR = (uint32_t)(MOD_Value-1);  
	    }
		//��5�������ж�ʹ��
		//TIM2->DIER |= TIM_DIER_UIE_Msk;
	}
}
