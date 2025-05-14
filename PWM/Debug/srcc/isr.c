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



//�ڲ���������
//uint8_t CreateFrame(uint8_t Data,uint8_t * buffer);

//======================================================================
//�жϷ���������ƣ�UART_USER_Handler
//����������UART_USE�����յ�һ���ֽڴ���
//��    �ܣ��յ�һ���ֽڣ�ֱ�ӷ��ظ��ֽ�
//��    ע�����뱾����󣬿�ʹ��uart_get_re_int�������ٽ����жϱ�־�ж�
//          ��1-��UART�����жϣ�0-û��UART�����жϣ�
//======================================================================
void UART_User_Handler(void)
{
	static uint8_t intTime = 0;
    uint8_t ch,flag;
    //uint8_t g_uart_recvBuf[3];
    //uint8_t *Buf=g_uart_recvBuf;
    //Buf[0] = 0;Buf[1] = 0;Buf[2] = 0;
    DISABLE_INTERRUPTS;               //�����ж�
    //-----------------------------------------------------------------------
    ch = uart_re1(UART_User, &flag);   //���ý���һ���ֽڵĺ�����������ж�λ

   	
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
		uart_send1(UART_User,ch);//�ط����յ����ֽ�
	}
   	
	ENABLE_INTERRUPTS;       //�����ж�
	
}


//�ڲ����ú���

//===========================================================================
//�������ƣ�CreateFrame

//===========================================================================
//uint8_t CreateFrame(uint8_t Data,uint8_t * buffer)
//{
//    uint8_t frameFlag=0;            //��֡״̬
//	for(int i=0; i<3 ;i++)
//	{
//		buffer[i]=Data-'0';
//		if(i==2) frameFlag=1;
//		uart_send1(UART_User,buffer[i]);
//	}
//    return frameFlag;                 //������֡״̬
//}

/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
