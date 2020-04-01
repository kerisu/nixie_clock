#include "stm32f10x.h"
#include "DS3231.h"
#include "delay.h"

void DS3231_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
}

void IIC_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void IIC_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;

	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*I2C��ʼ*/
void DS3231_IIC_Start(void)
{
	IIC_SDA_OUT();	

	SDA_H();
	delay_us(2);	
	SCL_H();
	delay_us(2);		
	SDA_L();
	delay_us(2);	
	SCL_L();
	delay_us(2);
}
/*I2Cֹͣ*/
void DS3231_IIC_Stop(void)
{
	IIC_SDA_OUT();	

	SDA_L();	
	delay_us(2);
	SCL_L();	
	delay_us(2);
	SDA_H();
	delay_us(2);
}
/*I2C����Ӧ��*/
void DS3231_IIC_Ack(u8 a)
{
	IIC_SDA_OUT();	

	if(a)	
	SDA_H();
	else	
	SDA_L();

	delay_us(2);
	SCL_H();	
	delay_us(2);
	SCL_L();
	delay_us(2);

}
/*I2Cд��һ���ֽ�*/
u8 DS3231_IIC_Write_Byte(u8 dat)
{
	u8 i;
	u8 iic_ack=0;	

	IIC_SDA_OUT();	

	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)	
		SDA_H();
		else	
		SDA_L();
			
		delay_us(2);
		SCL_H();
	    delay_us(2);
		SCL_L();
		dat<<=1;
	}

	SDA_H();	//�ͷ�������

	IIC_SDA_IN();	//���ó�����

	delay_us(2);
	SCL_H();
	delay_us(2);
	
	iic_ack |= IN_SDA();	//����Ӧ��λ
	SCL_L();
	return iic_ack;	//����Ӧ���ź�
}
/*I2C��ȡһ���ֽ�*/
u8 DS3231_IIC_Read_Byte(void)
{
	u8 i;
	u8 x=0;

	SDA_H();	//������������Ϊ�ߵ�ƽ

	IIC_SDA_IN();	//���ó�����

	for(i = 0;i < 8;i++)
	{
		x <<= 1;	//�������ݣ���λ��ǰ

		delay_us(2);
		SCL_H();	//ͻ��
		delay_us(2);
		
		if(IN_SDA())	x |= 0x01;	//�յ��ߵ�ƽ

		SCL_L();
		delay_us(2);
	}	//���ݽ������

	SCL_L();

	return x;	//���ض�ȡ��������
}


 /******************************************************************************
* @ File name --> ds3231.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 02 - 01 - 2014
* @ Brief     --> �߾���ʼ��оƬDS3231��������
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.
* @ Author    -->
* @ Date      -->
* @ Revise    -->
*
******************************************************************************/

/******************************************************************************
                               ������ʾʱ���ʽ
                         Ҫ�ı���ʾ�ĸ�ʽ���޸Ĵ�����
******************************************************************************/

u8 Display_Time[9] = {0x30,0x30,0x3a,0x30,0x30,0x3a,0x30,0x30,0x00};	
					//ʱ����ʾ����   ��ʽ  00:00:00

u8 Display_Date[14] = {0x32,0x30,0x31,0x33,0x2f,0x31,0x30,0x2f,0x32,0x30,0x20,0x37,0x57,0x00};
					//������ʾ����   ��ʽ  2013/10/20 7W

/******************************************************************************
                               ������صı�������
******************************************************************************/

Time_Typedef TimeValue;	//����ʱ������ָ��


u8 Time_Buffer[7];	//ʱ���������ݻ���


/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ���д��һ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
*                   dat��Ҫд�������
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void DS3231_Write_Byte(u8 REG_ADD,u8 dat)
{
	DS3231_IIC_Start();
	if(!(DS3231_IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
	{
		DS3231_IIC_Write_Byte(REG_ADD);
		DS3231_IIC_Write_Byte(dat);	//��������
	}
	DS3231_IIC_Stop();
}
/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ�����ȡһ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
* Output        --> none
* Reaturn       --> ��ȡ���ļĴ�������ֵ 
******************************************************************************/
u8 DS3231_Read_Byte(u8 REG_ADD)
{
	u8 ReData;
	DS3231_IIC_Start();
	if(!(DS3231_IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
	{
		DS3231_IIC_Write_Byte(REG_ADD);	//ȷ��Ҫ�����ļĴ���
		DS3231_IIC_Start();	//��������
		DS3231_IIC_Write_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
		ReData = DS3231_IIC_Read_Byte();	//��ȡ����
		DS3231_IIC_Ack(1);	//���ͷ�Ӧ���źŽ������ݴ���
	}
	DS3231_IIC_Stop();
	return ReData;
}
/******************************************************************************
* Function Name --> DS3231��ʱ�������Ĵ���������д�����ݻ��߶�ȡ����
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
*                   *WBuff��д�����ݻ���
*                   num��д����������
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Operate_Register(u8 REG_ADD,u8 *pBuff,u8 num,u8 mode)
{
	u8 i;
	if(mode)	//��ȡ����
	{
		DS3231_IIC_Start();
		if(!(DS3231_IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
		{
			DS3231_IIC_Write_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
			DS3231_IIC_Start();	//��������
			DS3231_IIC_Write_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
			for(i = 0;i < num;i++)
			{
				*pBuff = DS3231_IIC_Read_Byte();	//��ȡ����
				if(i == (num - 1))	DS3231_IIC_Ack(1);	//���ͷ�Ӧ���ź�
				else DS3231_IIC_Ack(0);	//����Ӧ���ź�
				pBuff++;
			}
		}
		DS3231_IIC_Stop();	
	}
	else	//д������
	{		 	
		DS3231_IIC_Start();
		if(!(DS3231_IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
		{
			DS3231_IIC_Write_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
			for(i = 0;i < num;i++)
			{
				DS3231_IIC_Write_Byte(*pBuff);	//д������
				pBuff++;
			}
		}
		DS3231_IIC_Stop();
	}
}
/******************************************************************************
* Function Name --> DS3231��ȡ����д��ʱ����Ϣ
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> *pBuff��д�����ݻ���
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_ReadWrite_Time(u8 mode)
{
	u8 Time_Register[8];	//����ʱ�仺��
	
	if(mode)	//��ȡʱ����Ϣ
	{
		DS3231_Operate_Register(Address_second,Time_Register,7,1);	//�����ַ��0x00����ʼ��ȡʱ����������
		
		/******�����ݸ��Ƶ�ʱ��ṹ���У��������������******/
		TimeValue.second = ((Time_Register[0] & Shield_secondBit)>>4) * 10 
											+((Time_Register[0] & Shield_secondBit)&0x0f) ;	//������
		
		TimeValue.minute = ((Time_Register[1] & Shield_secondBit)>>4) * 10 
											+((Time_Register[1] & Shield_secondBit)&0x0f) ;//��������
		
		TimeValue.hour   = ((Time_Register[2] & Shield_secondBit)>>4) * 10 
											+((Time_Register[2] & Shield_secondBit)&0x0f) ;//Сʱ����
		
		TimeValue.week   = ((Time_Register[3] & Shield_secondBit)>>4) * 10 
											+((Time_Register[3] & Shield_secondBit)&0x0f) ;	//��������
		
		TimeValue.date   = ((Time_Register[4] & Shield_secondBit)>>4) * 10 
											+((Time_Register[4] & Shield_secondBit)&0x0f) ;//������
		
		TimeValue.month  = ((Time_Register[5] & Shield_secondBit)>>4) * 10 
											+((Time_Register[5] & Shield_secondBit)&0x0f) ;//������
		
		TimeValue.year   = ((Time_Register[6] & Shield_secondBit)>>4) * 10 
											+((Time_Register[6] & Shield_secondBit)&0x0f) ;//������
	}
	else
	{
		/******��ʱ��ṹ���и������ݽ���******/
		Time_Register[0] = ((TimeValue.second / 10) << 4 )|TimeValue.second % 10;	//��
		Time_Register[1] = ((TimeValue.minute / 10) << 4 )|TimeValue.minute % 10;	//����
		Time_Register[2] = (((TimeValue.hour | Hour_Mode24) / 10) << 4 )|(TimeValue.hour | Hour_Mode24) % 10;	//Сʱ
		Time_Register[3] = ((TimeValue.week / 10) << 4 )|TimeValue.week % 10;	//����
		Time_Register[4] = ((TimeValue.date / 10) << 4 )|TimeValue.date % 10;	//��
		Time_Register[5] = ((TimeValue.month / 10) << 4 )|TimeValue.month % 10;	//��
		Time_Register[6] = ((TimeValue.year / 10) << 4 )|TimeValue.year % 10;	//��
		
		DS3231_Operate_Register(Address_second,Time_Register,7,0);	//�����ַ��0x00����ʼд��ʱ����������
	}
}
/******************************************************************************
* Function Name --> ʱ��������ʼ��
* Description   --> none
* Input         --> *TimeVAL��RTCоƬ�Ĵ���ֵָ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Time_Init(Time_Typedef *TimeVAL)
{	
	//ʱ����������
	Time_Buffer[0] = ((TimeVAL->second / 10) << 4 )|TimeVAL->second % 10;	//��
	Time_Buffer[1] = ((TimeVAL->minute / 10) << 4 )|TimeVAL->minute % 10;	//����
	Time_Buffer[2] = (((TimeVAL->hour | Hour_Mode24) / 10) << 4 )|(TimeVAL->hour | Hour_Mode24) % 10;	//Сʱ
	Time_Buffer[3] = ((TimeVAL->week / 10) << 4 )|TimeVAL->week % 10;	//����
	Time_Buffer[4] = ((TimeVAL->date / 10) << 4 )|TimeVAL->date % 10;	//��
	Time_Buffer[5] = ((TimeVAL->month / 10) << 4 )|TimeVAL->month % 10;	//��
	Time_Buffer[6] = (u8)((TimeVAL->month / 10) << 4 )|TimeVAL->month % 10;	//��
	
	DS3231_Operate_Register(Address_second,Time_Buffer,7,0);	//���루0x00����ʼд��7������
	DS3231_Write_Byte(Address_control, OSC_Enable);
	DS3231_Write_Byte(Address_control_status, Clear_OSF_Flag);
}
/******************************************************************************
* Function Name --> DS3231��⺯��
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> 0: ����
*                   1: ������������Ҫ��ʼ��ʱ����Ϣ
******************************************************************************/
u8 DS3231_Check(void)
{
	if(DS3231_Read_Byte(Address_control_status) & 0x80)  //����ֹͣ������
	{
		return 1;  //�쳣
	}
	else if(DS3231_Read_Byte(Address_control) & 0x80)  //���� EOSC����ֹ��
	{
		return 1;  //�쳣
	}
	else	return 0;  //����
}
/******************************************************************************
* Function Name --> ʱ���������ݴ�����
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void Time_Handle(void)
{
	/******************************************************
	                   ��ȡʱ��������Ϣ
	******************************************************/
	
	DS3231_ReadWrite_Time(1);	//��ȡʱ����������
	
	/******************************************************
	            ʱ����Ϣת��ΪASCII������ַ�
	******************************************************/
	
	Display_Time[6] = (TimeValue.second / 10) + 0x30;
	Display_Time[7] = (TimeValue.second % 10) + 0x30;	//Second

	Display_Time[3] = (TimeValue.minute / 10) + 0x30;
	Display_Time[4] = (TimeValue.minute % 10) + 0x30;	//Minute

	Display_Time[0] = (TimeValue.hour / 10) + 0x30;
	Display_Time[1] = (TimeValue.hour % 10) + 0x30;	//Hour 

	Display_Date[8] = (TimeValue.date / 10) + 0x30;
	Display_Date[9] = (TimeValue.date % 10) + 0x30;	//Date

	Display_Date[5] = (TimeValue.month / 10) + 0x30;
	Display_Date[6] = (TimeValue.month % 10) + 0x30;	//Month

	Display_Date[0] = '2';
	Display_Date[1] = '0';
	Display_Date[2] = (TimeValue.year / 10) + 0x30;
	Display_Date[3] = (TimeValue.year % 10) + 0x30;	//Year

	Display_Date[11] = (TimeValue.week % 10) + 0x30;	//week

}
/******************************************************************************
* Function Name --> ��ȡоƬ�¶ȼĴ���
* Description   --> �¶ȼĴ�����ַΪ0x11��0x12�������Ĵ���Ϊֻ��
* Input         --> none
* Output        --> *Temp�������¶���ʾ�ַ�����
* Reaturn       --> none
******************************************************************************/
#include "stdio.h"
void DS3231_Read_Temp(u8 *Temp)
{
	u8 temph,templ;
	float temp_dec;

	temph = DS3231_Read_Byte(Address_temp_MSB);	//��ȡ�¶ȸ�8bits
	templ = DS3231_Read_Byte(Address_temp_LSB) >> 6;	//��ȡ�¶ȵ�2bits

	//�¶�ֵת��
	if(temph & 0x80)	//�ж��¶�ֵ������
	{	//���¶�ֵ
		temph = ~temph;	//��λȡ��
		templ = ~templ + 0x01;	//��λȡ����1
		Temp[0] = 0x2d;	//��ʾ��-��
	}
	else	Temp[0] = 0x20;	//���¶Ȳ���ʾ���ţ���ʾ������0x2b

	//С�����ּ��㴦��
	temp_dec = (float)templ * (float)0.25;	//0.25��ֱ���
	temp_dec += temph;
	
//	//�������ּ��㴦��
//	temph = temph & 0x70;	//ȥ������λ
//	Temp[1] = temph % 1000 / 100 + 0x30;	//��λ
//	Temp[2] = temph % 100 / 10 + 0x30;	//ʮλ
//	Temp[3] = temph % 10 + 0x30;	//��λ
//	Temp[4] = 0x2e;	//.

//	//С�����ִ���
//	Temp[5] = (u8)(temp_dec * 10) + 0x30;	//С�����һλ
//	Temp[6] = (u8)(temp_dec * 100) % 10 + 0x30;	//С������λ

//	if(Temp[1] == 0x30)	Temp[1] = 0x20;	//��λΪ0ʱ����ʾ
//	if(Temp[2] == 0x30)	Temp[2] = 0x20;	//ʮλΪ0ʱ����ʾ
//	
//	Temp[7] = '\0';
	sprintf(Temp,"%2.2f",temp_dec);
}

