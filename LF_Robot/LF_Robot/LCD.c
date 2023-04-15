/*
 * LCD.c
 */ 
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

void PWM_1_settings (void);
void PWM_2_settings (void);
void PORT_LCD_settings(void);
void LCD_commands (unsigned char);
void LCD_datas (unsigned char data);
void LCD_settings (void);
void LCD_lines (unsigned char x, unsigned char y);
void LCD_str (char words[]);
void cleaning_LCD (void);
void ADC_settings (void);
void ADC_channal_PC2 (void);
void ADC_channal_PC3 (void);

#define RS_1 PORTD |= (1<<2); // �������� ������
#define RS_0 PORTD &= ~(1<<2); // �������� ������
#define E_1  PORTD |= (1<<3); // �������������
#define E_0  PORTD &= ~(1<<3); // �������������
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void PORT_LCD_settings(void)
{
	DDRD |= (1<<7) | (1<<6) | (1<<5) | (1<<4) | (1<<3) | (1<<2);
	PORTD &= ~((1<<7) | (1<<6) | (1<<5) | (1<<4) | (1<<3) | (1<<2));
}

void LCD_commands (unsigned char command)
{
	PORTD = command;
	RS_0; // �������� �������
	E_1;
	_delay_us(50);
	E_0;
	PORTD = (command<<4);
	RS_0; // �������� �������
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

// �������� ������ � LCD
void LCD_datas (unsigned char data)
{
	PORTD = data;
	RS_1; // �������� �������
	E_1;
	_delay_us(50);
	E_0;
	PORTD = (data<<4);
	RS_1; // �������� �������
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

void LCD_settings (void)
{
	_delay_ms(15); // �������� ��� ������� ���
	LCD_commands(0b00000011);
	_delay_ms(5);
	LCD_commands(0b00000011);
	_delay_us(100);
	LCD_commands(0b00000011);
	_delay_ms(1);
	LCD_commands(0b00000010);
	_delay_ms(1);
	LCD_commands(0b00101000); // 4 ����, 2 �����, ����� 5*8
	_delay_ms(1);
	LCD_commands(0b00001100); // �������� �������, �� ���������� ������, �� ������ ��������
	_delay_ms(1);
	LCD_commands(0b00000001); // ������� ������
	_delay_ms(2);
	LCD_commands(0b00000110); // ���������������� ������� � ����� ��� ������
	_delay_ms(2);
}

// ����� ����� � ������� ��� ������ ������ �� �������.
void LCD_lines (unsigned char x, unsigned char y)
{
  // ���� ������ � ������� �������� (1, '�����') �� ����� � ������ ������, ������� � ������� '�����'.
  //  ���� (2,'�����') �� ����� �� ������ ������, ������� ����� � ������� '�����'. 

	unsigned char coordinate = 0;

	if (x == 1) 
	{
		coordinate = y | 0b10000000;
		LCD_commands(coordinate);
	} 
	if (x == 2)
	{
		coordinate = (y+0x40) | 0b10000000;
		LCD_commands(coordinate);
    }
}

// ������� ������ �� ������� ����� ���������� ������.
void LCD_str (char words[])
{
	for (unsigned char z = 0; words[z] != '\0'; z++)
	{
		LCD_datas(words[z]);
	}
}

void cleaning_LCD (void)
{
	LCD_lines (1, 0);
	LCD_str ("                ");
	LCD_lines (2, 0);
	LCD_str ("                ");
}
*/