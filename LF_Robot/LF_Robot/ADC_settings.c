/*
 * ADC_settings.c
 */ 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

void PWM_1_settings (void);
void PWM_2_settings (void);
//void PORT_LCD_settings(void);
//void LCD_commands (unsigned char);
//void LCD_datas (unsigned char data);
//void LCD_settings (void);
//void LCD_lines (unsigned char x, unsigned char y);
//void LCD_str (char words[]);
//void cleaning_LCD (void);
void ADC_settings (void);
void ADC_channal_PC2 (void);
void ADC_channal_PC3 (void);

#define RS_1 PORTD |= (1<<2); // �������� ������
#define RS_0 PORTD &= ~(1<<2); // �������� ������
#define E_1  PORTD |= (1<<3); // �������������
#define E_0  PORTD &= ~(1<<3); // �������������
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC_settings (void)
{
	DDRC &= ~((1<<3) | (1<<2));
	
	ADCSRA |=(1<<ADEN); // �������� ���
	ADCSRA &= ~(1<<ADFR); // ����� ����������� ��������� 
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//������� ������������� 125 000 �� (16 ��� / 128) ��� ������� 16 ���
	
	ADMUX |=(1<<REFS1) | (1<<REFS0); // �����. ���. �������� ���������� 2,56 � � ������������� �� AREF

	ADMUX &= ~(1<<ADLAR); // �������������� ������������
	
}

void ADC_channal_PC2 (void)
{
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0));
	ADMUX |= (1<<MUX1);
	
	ADCSRA |= (1<<ADSC); // ������ ADC
	
	while ((ADCSRA & (1<<ADSC))); // ����������� ���� �������� ��������� ������ ADC
}

void ADC_channal_PC3 (void)
{
	ADMUX &= ~((1<<MUX3) | (1<<MUX2));
	ADMUX |= (1<<MUX1) | (1<<MUX0);
	
	ADCSRA |= (1<<ADSC); // ������ ADC
	
	while ((ADCSRA & (1<<ADSC))); // ����������� ���� �������� ��������� ������ ADC
}

