#ifndef __NIXIEAPP_H
#define __NIXIEAPP_H


typedef enum
{
	WeekUp, 			//�ⲿ����
	Sleep,				//����ģʽ
	Normal, 			//��������ģʽ
	Close,				//�ر���ʾģʽ
	SetMenu,			//���ò˵�
	SetDate,			//��������
	SetTime,			//����ʱ��
	
}StatusTypedef;//ȫ��״̬��״̬���Ͷ���


void Nixie_Setup(void); //Ӧ������
void Nixie_Loop(void);	//Ӧ����ѯ
void Nixie_Interrupt(void);//Ӧ���ж� ÿ1msһ��

#endif /*__NIXIE_H*/
