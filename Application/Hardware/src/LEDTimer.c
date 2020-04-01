#include "stm32f10x.h"
#include "LEDTimer.h"


void LEDTimerInit(void)
{  
  GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);// 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);   //ʹ��GPIO����ʱ��ʹ��
                                                                                                                                                                
																																							  //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 ; 											  //TIM_CH12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 														  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	TIM_TimeBaseStructure.TIM_Period = 7200; 																		  //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
	TIM_TimeBaseStructure.TIM_Prescaler =0; 																		  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 																	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  									//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 															//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
																																								//TIM2ͨͨ��һ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 														//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState= TIM_OutputState_Enable; 									//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 300; 																					//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 											//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  																		//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
																																								//TIM2ͨ����
  TIM_OCInitStructure.TIM_OutputState =TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 150;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM2,ENABLE);        																			//MOE �����ʹ��        
  TIM_ARRPreloadConfig(TIM2, ENABLE); 																					//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
  TIM_Cmd(TIM2, ENABLE);  																											//ʹ��TIM2
	
	TIM_SetCompare1(TIM2,0); 
	TIM_SetCompare2(TIM2,0); 
}
void LED_TIM3Init(void)  
{  

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //��ʱ�� TIM3 ʹ��
	//��ʱ�� TIM3 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 999; //�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 71; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  //�ڳ�ʼ�� TIM3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );  //����������ж�
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 0 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ� 3 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);   //�ܳ�ʼ�� NVIC �Ĵ���
	TIM_Cmd(TIM3, ENABLE);  //��ʹ�� TIM3
}  

void LED_HartAttackInit(void)
{
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_15;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_13;
	
	GPIO_Init(GPIOC,&gpio);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

uint8_t g_LED_DISPLAY_FLAG = 0;


void LEDFlash(void)
{
	static uint16_t pluse = 0;
	static uint16_t div = 0;
	static uint8_t  flag = 0;
	div++;
	if(div > 20)
	{
		if(flag == 0)
		{
			pluse++;
			if(pluse > 500)
			{
				flag = 1;
			}
		}
		else
		{
			pluse--;
			if(pluse < 2)
			{
				flag = 0;
			}
		}
		if(g_LED_DISPLAY_FLAG)
		{
			TIM_SetCompare1(TIM2,pluse); 
		}
		else
		{
			TIM_SetCompare1(TIM2,0); 
		}		
	}	
}

void LEDBreathing(void)
{
	static uint16_t pluse = 0;
	static uint16_t div = 0;
	static uint8_t  flag = 0;
	div++;
	if(div > 100)
	{
		if(flag == 0)
		{
			pluse++;
			if(pluse > 4800)
			{
				flag = 1;
			}
		}
		else
		{
			pluse--;
			if(pluse < 2)
			{
				flag = 0;
			}
		}
		if(g_LED_DISPLAY_FLAG)
		{
			TIM_SetCompare2(TIM2,pluse); 
		}
		else
		{
			TIM_SetCompare2(TIM2,0); 
		}
	}
}

void TIM3_IRQHandler(void) 
{    
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
	
	LEDFlash();
	LEDBreathing();
} 

void LED_Control(uint8_t cmd)
{
	if(cmd == ENABLE)
	{
		g_LED_DISPLAY_FLAG = 1;
	}
	else if(cmd == DISABLE)
	{
		g_LED_DISPLAY_FLAG = 0;
	}
}

void LED_HartCmd(uint8_t cmd)
{
	GPIOC->BSRR = GPIO_Pin_13 << 16 * cmd;
	GPIOB->BSRR = GPIO_Pin_15 << 16 * cmd;
}



















