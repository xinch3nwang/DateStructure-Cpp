//===========================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��GPIO�ײ���������Դ�ļ�
//��Ȩ���У�SD-Arm(sumcu.suda.edu.cn)
//�汾���£�20181201-20200221
//оƬ���ͣ�STM32
//===========================================================================

#include "gpio.h"

//GPIO�ڻ���ַ���볣��������GPIO_ARR[0]~GPIO_ARR[5]��
GPIO_TypeDef * GPIO_ARR[] =
       {(GPIO_TypeDef *)GPIOA_BASE,(GPIO_TypeDef *)GPIOB_BASE,
		(GPIO_TypeDef *)GPIOC_BASE,(GPIO_TypeDef *)GPIOD_BASE,
		(GPIO_TypeDef *)GPIOE_BASE,(GPIO_TypeDef *)GPIOH_BASE};

//====������չ�ж�IRQ�Ŷ�Ӧ��====
IRQn_Type table_irq_exti[7] = {EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn,
		EXTI3_IRQn, EXTI4_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn};

//�ڲ���������
void gpio_get_port_pin(uint16_t port_pin,uint8_t* port,uint8_t* pin);

//=====================================================================
//�������ƣ�gpio_init
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//         dir�����ŷ���0=���룬1=���,�������ŷ���궨�壩
//         state���˿����ų�ʼ״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ����ʼ��ָ���˿�������ΪGPIO���Ź��ܣ�������Ϊ�������������������
//         ��ָ����ʼ״̬�ǵ͵�ƽ��ߵ�ƽ
//=====================================================================
void gpio_init(uint16_t port_pin, uint8_t dir, uint8_t state)
{
	GPIO_TypeDef *gpio_ptr;    //����gpio_ptrΪGPIO�ṹ������ָ��
	uint8_t port,pin;    //�����˿�port������pin����
	uint32_t temp;       //��ʱ��żĴ������ֵ
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

	//ʹ����ӦGPIOʱ��
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN<<(port * 1u));

	//��GPIOģʽ�Ĵ�����Ӧ����λ
	temp = gpio_ptr->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (pin * 2u));

	if(dir == 1)  //����Ϊ�������
	{
		temp |= (GPIO_OUTPUT << (pin * 2u));
		gpio_ptr->MODER = temp;
		gpio_set(port_pin,state);    //����gpio_set�������趨���ų�ʼ״̬
	}
	else  //����Ϊ��������
	{
		temp |= (GPIO_INPUT << (pin * 2u));
		gpio_ptr->MODER = temp;
	}
}

//=====================================================================
//�������ƣ�gpio_set
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//          state��ϣ�����õĶ˿�����״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ���ʱ���������趨����״̬
//=====================================================================
void gpio_set(uint16_t port_pin, uint8_t state)
{
	//�ֲ���������
	GPIO_TypeDef *gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port,pin;        //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

	//����state�����ö�Ӧ����״̬
	if(1 == state)    //�ߵ�ƽ(�����Ŷ�Ӧ��λ�Ĵ�����1)
		gpio_ptr->BSRR = (uint32_t)(1u<<pin);
	else              //�͵�ƽ(�����Ŷ�Ӧ���üĴ�����1)
		gpio_ptr->BRR = (uint32_t)(1u<<pin);
}

//=====================================================================
//�������ƣ�gpio_get
//�������أ�ָ���˿����ŵ�״̬��1��0��
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ����������ȡָ������״̬
//=====================================================================
uint8_t gpio_get(uint16_t port_pin)
{
	//�ֲ���������
	GPIO_TypeDef *gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port,pin;        //�����˿�port������pin����
	uint32_t temp;
	uint8_t value;
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

	//���״̬�Ĵ�����ֵ
	temp = gpio_ptr->MODER;

	if( (temp & (2u<<(pin*2)))>>(pin*2) == 1u )//GPIO���
	{
		//��ȡOutput data�Ĵ�����Ӧ���ŵ�ֵ
		temp = gpio_ptr->ODR;
		if((temp & (1u<<pin)) != 0x00u)
			value = 1;
		else
			value = 0;
	}
	else                                       //GPIO����
	{
		//��ȡInput data�Ĵ�����Ӧ���ŵ�ֵ
		temp = gpio_ptr->IDR;
		if((temp & (1u<<pin)) != 0x00u)
			value = 1;
		else
			value = 0;
	}

	return value;
}

//=====================================================================
//�������ƣ�gpio_reverse
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ���ʱ����������ת����״̬
//=====================================================================
void gpio_reverse(uint16_t port_pin)
{
	//�ֲ���������
	GPIO_TypeDef *gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port,pin;        //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

    //�ж��������״̬
	if ((gpio_ptr->ODR & (1u<<pin)) != 0x00u)
		//�ߵ�ƽ����תΪ�͵�ƽ
		gpio_ptr->BRR = (uint32_t)(1u<<pin);
	else
		//�͵�ƽ����תΪ�ߵ�ƽ
		gpio_ptr->BSRR = (uint32_t)(1u<<pin);
}

//=====================================================================
//�������ƣ�gpio_pull
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       pullselect������/������PULL_DOWN=������PULL_UP=������
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ��������������������/����
//=====================================================================
void gpio_pull(uint16_t port_pin, uint8_t pullselect)
{
	GPIO_TypeDef *gpio_ptr;    //����gpio_ptrΪGPIO�ṹ������ָ��
	uint8_t port,pin;    //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

	//�趨Ϊ�������״̬
	gpio_ptr->OSPEEDR |= (uint32_t)(HIGH_SPEED << (pin * 2u));

	if(1 == pullselect)
		//�������ߵ�ƽ
		gpio_ptr->PUPDR |= (uint32_t)(PULL_UP << (pin * 2u));
	else
		//�������͵�ƽ
		gpio_ptr->PUPDR |= (uint32_t)(PULL_DOWN << (pin * 2u));
}

//=====================================================================
//�������ƣ�gpio_enable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//          irqtype�������ж����ͣ��ɺ궨��������ٴ��о����£�
//                  RISING_EDGE  9      //�����ش���
//                  FALLING_EDGE 10     //�½��ش���
//                  DOUBLE_EDGE  11     //˫���ش���
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ�����������������жϣ���
//          �����жϴ���������
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//         KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//         KL36оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        STM32L433оƬ��PORTA��PORTB��PORTC��PORTH�ھ���GPIO���жϹ���,��EXIT(��չ
//                     �ж�)�����ã���ֻ���������ش������½��ش�����˫���ش������������ж�����
//=====================================================================
void gpio_enable_int(uint16_t port_pin,uint8_t irqtype)
{
	uint8_t port,pin;    //�����˿�port������pin����
	uint32_t temp;       //��ʱ��żĴ������ֵ
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);

	//ʹ��SYSCFGʱ��
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//���ö�Ӧ���ŵ���չ�ж�
	temp = SYSCFG->EXTICR[pin >> 2u];
	temp &= ~(0x0FuL << (4u * (pin & 0x03u)));
	temp |= (port << (4u * (pin & 0x03u)));
	SYSCFG->EXTICR[pin >> 2u] = temp;

	//�����Ӧ��չ�ж��е�����
	EXTI->IMR1 |= (uint32_t)(1u<<pin);

	//�������ػ�˫���ش���,ʹ�������������ش���
	if(RISING_EDGE == irqtype || DOUBLE_EDGE == irqtype)
		EXTI->RTSR1 |= (uint32_t)(1u<<pin);

	//���½��ػ�˫���ش���,ʹ���������½��ش���
	if(FALLING_EDGE == irqtype || DOUBLE_EDGE == irqtype)
		EXTI->FTSR1 |= (uint32_t)(1u<<pin);

	if(pin >= 0 && pin <= 4)
		//���жϿ�����IRQ�ж�(EXTIx,x=0-4)
		NVIC_EnableIRQ(table_irq_exti[pin]);
	else if(pin >= 5 && pin <= 9)
		//���жϿ�����IRQ�ж�(EXTI9_5)
		NVIC_EnableIRQ(table_irq_exti[5]);
	else
		//���жϿ�����IRQ�ж�(EXTI15_10)
		NVIC_EnableIRQ(table_irq_exti[6]);
}

//=====================================================================
//�������ƣ�gpio_disable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ���������ر������ж�
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//         KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//         KL36оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        STM32L433оƬ��PORTA��PORTB��PORTC��PORTH�ھ���GPIO���жϹ���,��EXIT(��չ
//                     �ж�)�����ã���ֻ���������ش������½��ش�����˫���ش������������ж�����
//=====================================================================
void gpio_disable_int(uint16_t port_pin)
{
	uint8_t port,pin;    //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);

	//��ֹSYSCFGʱ��
	RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;

	//���Ӧ���ŵ���չ�ж�
	SYSCFG->EXTICR[pin >> 2u] &= ~(0x0FuL << (4u * (pin & 0x03u)));

	//���ζ�Ӧ��չ�ж��е�����
	EXTI->IMR1 &= (uint32_t)~(1u<<pin);

	//��ֹ�������½��ش���
	EXTI->RTSR1 &= (uint32_t)~(1u<<pin);
	EXTI->FTSR1 &= (uint32_t)~(1u<<pin);

	if(pin >= 0 && pin <= 4)
		//���жϿ�����IRQ�ж�(EXTIx,x=0-4)
		NVIC_DisableIRQ(table_irq_exti[pin]);
	else if(pin >= 5 && pin <= 9)
		//���жϿ�����IRQ�ж�(EXTI9_5)
		NVIC_DisableIRQ(table_irq_exti[5]);
	else
		//���жϿ�����IRQ�ж�(EXTI15_10)
		NVIC_DisableIRQ(table_irq_exti[6]);

}

//=====================================================================
//�������ƣ�gpio_drive_strength
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       control���������ŵ�����������LOW_SPEED=����,MSDIUM_SPEED=����
//               HIGH_SPEED=����,VERY_HIGH_SPEED=������
//���ܸ�Ҫ������������������ָ�����������������ĳ�������һ����mA��λ����
//        ��������������5mA,����������18mA���������ű�����Ϊ�������ʱ,
//        �����ŵ�����������������
//=====================================================================
void gpio_drive_strength(uint16_t port_pin, uint8_t control)
{
	GPIO_TypeDef *gpio_ptr;    //����gpio_ptrΪGPIO�ṹ������ָ��
	uint8_t port,pin;    //�����˿�port������pin����
//	uint32_t temp;       //��ʱ��żĴ������ֵ
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);
	//����port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	if(7 == port) //GPIOH
		gpio_ptr = GPIO_ARR[port-2];
	else
		gpio_ptr = GPIO_ARR[port];

	//�������ŵ���������
	gpio_ptr->OSPEEDR |= (uint32_t)(control << (pin * 2u));
}

//=====================================================================
//�������ƣ�gpio_get_int
//�������أ�����GPIO�жϱ�־��1��0��1��ʾ������GPIO�жϣ�0��ʾ����û��GPIO�жϡ�
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ,��ȡ�жϱ�־��
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//        KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        KL36оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        STM32L433оƬ��PORTA��PORTB��PORTC��PORTH�ھ���GPIO���жϹ���,��EXIT(��չ
//                     �ж�)�����ã���ֻ���������ش������½��ش�����˫���ش������������ж�����
//=====================================================================
uint8_t gpio_get_int(uint16_t port_pin)
{
	uint8_t port,pin;    //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);

	//��ȡGPIO�жϱ�־
	if((EXTI->PR1 & (1u << pin)) == (1u << pin))
		return 1;
	else
		return 0;
}

//=====================================================================
//�������ƣ�gpio_clear_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)���磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ,����жϱ�־��
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//         KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//         KL36оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        STM32L433оƬ��PORTA��PORTB��PORTC��PORTH�ھ���GPIO���жϹ���,��EXIT(��չ
//                     �ж�)�����ã���ֻ���������ش������½��ش�����˫���ش������������ж�����
//=====================================================================
void gpio_clear_int(uint16_t port_pin)
{
	uint8_t port,pin;    //�����˿�port������pin����
	//���ݴ������port_pin���������˿������ŷֱ𸳸�port,pin
	gpio_get_port_pin(port_pin,&port,&pin);

	//��GPIO�жϱ�־
	EXTI->PR1 |= (1u << pin);
}

//=====================================================================
//�������ƣ�gpio_clear_allint
//�������أ���
//����˵������
//���ܸ�Ҫ��������ж˿ڵ�GPIO�ж�
//ע          �⣺ KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//         KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//         KL36оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//        STM32L433оƬ��PORTA��PORTB��PORTC��PORTH�ھ���GPIO���жϹ���,��EXIT(��չ
//                     �ж�)�����ã���ֻ���������ش������½��ش�����˫���ش������������ж�����
//=====================================================================
void gpio_clear_allint(void)
{
	EXTI->PR1 |= 0xFFFF;//pin��ȡ0-15
}


//----------------------����Ϊ�ڲ�������Ŵ�-----------------
//=====================================================================
//�������ƣ�gpio_get_port_pin
//�������أ���
//����˵����port_pin���˿ں�|���źţ��磺(PTB_NUM)|(9) ��ʾΪB��9�Žţ�
//       port���˿ںţ���ָ����������
//       pin:���źţ�0~15��ʵ��ȡֵ��оƬ���������ž���������ָ����������
//���ܸ�Ҫ������������port_pin���н������ó�����˿ں������źţ��ֱ�ֵ��port��pin,���ء�
//       ������(PTB_NUM)|(9)����ΪPORTB��9��������ֱ�ֵ��port��pin����
//=====================================================================
void gpio_get_port_pin(uint16_t port_pin,uint8_t* port,uint8_t* pin)
{
	*port = (port_pin>>8);
	*pin = port_pin;
}
