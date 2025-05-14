//======================================================================
//�ļ����ƣ�user.h��userͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200830
//��Ҫ˵������1�������õ���ͷ�ļ���Ϊ�ﵽӦ�ó���Ŀ���ֲ�ԣ�����Ӳ��Ҫ��
//              оƬ�ĸ����ţ���Ҫ������궨�壬Ŀ����ʵ�ֶԾ���Ӳ������
//              ��̣������Ƕ�оƬ�����ű��
//          ��2���жϴ�����������ڴ˺궨�壬�Ա�isr.c����ֲ
//======================================================================
#ifndef USER_H   //��ֹ�ظ����壨USER_H ��ͷ��
#define USER_H

//��1�����̶����ļ�����
#include "printf.h"
#include "gpio.h"
#include "flash.h"
#include "emuart.h"
#include "timer.h"
#include "pwm.h"
#include "gec.h"
#include "incapture.h"
//��2�����䶯��ָʾ�ƶ˿ڼ����Ŷ��塪����ʵ��ʹ�õ����ŸĶ�
//ָʾ�ƶ˿ڼ����Ŷ���
#define  LIGHT_RED    (PTB_NUM|7)  //��ƣ���GEC_56��
#define  LIGHT_GREEN  (PTB_NUM|8)  //�̵ƣ���GEC_55��
#define  LIGHT_BLUE   (PTB_NUM|9)  //���ƣ���GEC_54��

//��״̬�궨�壨�������ư���Ӧ�������ƽ��Ӳ���ӷ�������
#define  LIGHT_ON       0    //����
#define  LIGHT_OFF      1    //�ư�

//��3�����䶯��UART����ģ�鶨��
#define UART_Debug   UART_3   //���ڳ�����£��޷���ʹ��
#define UART_User    UART_2   //�û����ڣ���-GND;��-TX;��-RX��

//��4�����䶯���жϷ������궨��
#define UART_User_Handler   USART2_IRQHandler  //�û������жϺ���
#define INCAP_USER   INCAP_PIN0  	//�û�INCAP���벶׽ GEC_10��PTA_NUM|2��
//#define OUTCMP_USER  OUTCMP_PIN2    //�û�OUTCMP����Ƚ� GEC_39

//��5�����Ķ�����ʱ������ģ�鶨��
#define PWM_USER   	 PWM_PIN2  	    //�û�PWM��� ��GEC39
#define TIMER_USER   TIMERB  //�û���ʱ��

#define TIMER_USER_Handler   TIM7_IRQHandler    //�û���ʱ���жϺ���
//#define TIM15_USER_Handler   TIM1_BRK_TIM15_IRQHandler    //����벶׽�жϺ���
#define INCAP_USER_Handler   TIM1_BRK_TIM15_IRQHandler    //�û����벶׽�жϺ���



#endif    //��ֹ�ظ����壨USER_H ��β��
