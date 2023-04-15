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

#define RS_1 PORTD |= (1<<2); // Передача данных
#define RS_0 PORTD &= ~(1<<2); // Передача команд
#define E_1  PORTD |= (1<<3); // Синхронизация
#define E_0  PORTD &= ~(1<<3); // Синхронизация
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC_settings (void)
{
	DDRC &= ~((1<<3) | (1<<2));
	
	ADCSRA |=(1<<ADEN); // Включаем АЦП
	ADCSRA &= ~(1<<ADFR); // Режим дискретного измерения 
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//Частота дискретизации 125 000 Гц (16 млн / 128) при частоте 16 МГц
	
	ADMUX |=(1<<REFS1) | (1<<REFS0); // Внутр. ист. опорного напряжения 2,56 В с конденсатором на AREF

	ADMUX &= ~(1<<ADLAR); // Правостороннее выравнивание
	
}

void ADC_channal_PC2 (void)
{
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0));
	ADMUX |= (1<<MUX1);
	
	ADCSRA |= (1<<ADSC); // Запуск ADC
	
	while ((ADCSRA & (1<<ADSC))); // Бесконечный цикл ожидания окончания работы ADC
}

void ADC_channal_PC3 (void)
{
	ADMUX &= ~((1<<MUX3) | (1<<MUX2));
	ADMUX |= (1<<MUX1) | (1<<MUX0);
	
	ADCSRA |= (1<<ADSC); // Запуск ADC
	
	while ((ADCSRA & (1<<ADSC))); // Бесконечный цикл ожидания окончания работы ADC
}

