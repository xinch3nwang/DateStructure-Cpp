//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//��ֲ���򣺡��̶���
//=====================================================================
#include "includes.h"

void User_SysFun(uint8_t ch);

//======================================================================
//�������ƣ�UART_User_Handler
//����������UART_User�����յ�һ���ֽڴ���
//��    ע�����뱾����󣬿�ʹ��uart_get_re_int�������ٽ����жϱ�־�ж�
//          ��1-��UART�����жϣ�0-û��UART�����жϣ�
//======================================================================


//�ڲ�����
void User_SysFun(uint8_t ch)
{
    //��1���յ���һ���ֽڲ�����֡
    if(gcRecvLen == 0)  gcRecvLen =useremuart_frame(ch,(uint8_t*)gcRecvBuf);
    //��2���ֽڽ�����֡���ж�gcRecvLen=0����Ϊ0����ʾ��֡��δ��ɣ�
    //     �´��յ�һ���ֽڣ��ټ�����֡
    if(gcRecvLen == 0) goto User_SysFun_Exit;
    //��3�����ˣ�gcRecvLen��0,��ʾ��֡��ɣ�gcRecvLenΪ֡�ĳ���,У�����кź���
    //     ����Flash�е���һ������ʼ��16�ֽڽ��бȽϣ�
    //     gcRecvBuf[16]������ת
    if(strncmp((char *)(gcRecvBuf),(char *)((MCU_SECTOR_NUM-1)*MCU_SECTORSIZE+
       MCU_FLASH_ADDR_START),16) != 0)
    {
        gcRecvLen = 0;         //�ָ�����״̬
        goto User_SysFun_Exit;
    }
    //��4�����ˣ������յ�����֡������űȽ�Ҳһ�£� ���������ֽ�gcRecvBuf[16]������ת
    //��ΪUser���ڳ�������������г������
    switch(gcRecvBuf[16])  //֡��ʶ
    {
        case 0:
            SYSTEM_FUNCTION((uint8_t *)(gcRecvBuf+17));
            gcRecvLen = 0;         //�ָ�����״̬
        break;
        default:
        break;
    }
User_SysFun_Exit:
    return;
}




//======================================================================
//�жϷ���������ƣ�UART_USER_Handler
//����������UART_USE�����յ�һ���ֽڴ���
//��    �ܣ��յ�һ���ֽڣ�ֱ�ӷ��ظ��ֽ�
//��    ע�����뱾����󣬿�ʹ��uart_get_re_int�������ٽ����жϱ�־�ж�
//          ��1-��UART�����жϣ�0-û��UART�����жϣ�
//======================================================================
void UART_User_Handler(void)
{
    uint8_t ch,flag;
    DISABLE_INTERRUPTS;               //�����ж�
    //-----------------------------------------------------------------------
    ch = uart_re1(UART_User, &flag);   //���ý���һ���ֽڵĺ�����������ж�λ
   
   	if(flag)
   	{
		uart_send1(UART_User,ch);//�ط����յ����ֽ�
	}
	ENABLE_INTERRUPTS;       //�����ж�
	
}

//=====================================================================
//�������ƣ�INCAP_USER_Handler�����벶׽�жϴ������
//����˵������
//�������أ���
//���ܸ�Ҫ����1��ÿ�β�׽�������ػ����½��ش����ó���
//       ��2��ÿ�δ��������ϴ���ǰ��׽������λ������
//=====================================================================
void INCAP_USER_Handler(void)
{
	uint16_t val;
	static uint16_t gcIntTime=0;
	//static uint8_t status=0;
	static uint8_t Count = 0;
	DISABLE_INTERRUPTS;     //�����ж�
	
	//------------------------------------------------------------------
	if(cap_get_flag(INCAP_USER))//�����ж�
	{
		//status = 1;
		val = get_incapture_value(INCAP_USER);
		gcIntTime++;
		if(gcIntTime%2){
			printf("1:%d:%d\n",val,Count);
			//printf("������ ��ʱ������ֵ:%d  ",val);
			//printf("���е�ƽ���������%d\n",Count);
			incapture_init(INCAP_USER,375,2000,CAP_DOWN);
			Count = 0; 
		}
		else{
			printf("0:%d:%d\n",val,Count);
			//printf("�½��� ��ʱ������ֵ:%d  ",val);
			//printf("�ߵ�ƽ���������%d\n",Count);
			gcIntTime=0;
			incapture_init(INCAP_USER,375,2000,CAP_UP);
			Count = 0; 
		}	
	}
	else//����ж�
	{
		Count++;
	}
//	else if(timer_get_int(15))
//	{
//		if(status) Count++;
//		Count++;
//		timer_clear_int(15);    //��ʱ�������־λ
//	}
	cap_clear_flag(INCAP_USER); //���ж�
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS;     //�����ж�
}

/*
//TIM15�������жϴ������
void TIM15_USER_Handler(void)
{
	uint16_t val;
	static uint16_t gcIntTime=0;
	DISABLE_INTERRUPTS;                        //�����ж�
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
			cap_clear_flag(INCAP_USER); //���ж�
		}
		else{
			Count++;
			printf("���������%d\n",Count);
			timer_clear_int(15);    //��ʱ�������־λ
		}
		
	}
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS;                        //�����ж�
}




//��ʱ���жϷ������
void TIMi_IRQHandler(void)
{
	uint8_t CAPTURE_STA=0; //���벶��״̬
	uint16_t CAPTURE_VAL;//���벶��ֵ
  	if((CAPTURE_STA&0x80)!=0x80)//��δ�ɹ�����
  	{
     	if (!cap_get_flag())
       	{
         	if(CAPTURE_STA&0x40)//�Ѿ����񵽸ߵ�ƽ��
           	{
             	if((CAPTURE_STA&0x3F)==0x3F)//�ߵ�ƽ̫����
                {
                  	CAPTURE_STA|=0x80;//��ǳɹ�������һ��
                  	CAPTURE_VAL=0xFFFF;
                 }
                 else CAPTURE_STA++;
          	}
     	}
 		if (cap_get_flag())//���������¼�
  		{
    		if(CAPTURE_STA&0x40) //����һ���½���
     		{
				  CAPTURE_STA|=0x80; //��ǳɹ�����һ��������
				  CAPTURE_VAL=get_incapture_value(INCAP_USER);
				  incapture_init(INCAP_USER,375,2000,CAP_UP); //����Ϊ�����ز���
       		}
  			else //��δ��ʼ,��һ�β���������
   			{
				  CAPTURE_STA=0; //���
				  CAPTURE_VAL=0;
				  TIM2CH1_CAPTURE_STA|=0x40; //��ǲ�����������
				  incapture_init(INCAP_USER,375,2000,CAP_DOWN); //����Ϊ�½��ز���
    		}
  		}
	}
	cap_clear_flag();
}*/



/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
