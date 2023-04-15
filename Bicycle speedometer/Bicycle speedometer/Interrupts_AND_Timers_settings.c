//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

void PORTB_LCD_Settings (void);
void LCD_commands (unsigned char);
void LCD_datas (unsigned char);
void LCD_settings(void);
void LCD_lines (unsigned char, unsigned char);
void LCD_str (char words[]);
void External_interrupts_settings (void);
void Timer_0_settings (void);
void Timer_1_settings (void);
void Timer_2_settings (void);
void cleaning_LCD (void);

#define RS_0 PORTB &= ~(1<<2); // �������� �������.     (���������� � LCD_settings)
#define RS_1 PORTB |= (1<<2); // �������� ������.       (���������� � LCD_settings)
#define E_1 PORTB |= (1<<3); // ������������. �� �����. (���������� � LCD_settings)
#define E_0 PORTB &= ~(1<<3); // ������������.          (���������� � LCD_settings)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



/* ����������� INT1 (PD3) � INT0 (PD2) ������� ����������, ��� ������� � ������� ����� � ������������ �������
    �� ������������ ������� �� �� ����� ��������������. */
void External_interrupts_settings (void)
{
   DDRD &= ~((1<<3) | (1<<2));
   PORTD |= (1<<3) | (1<<2);
	  
   MCUCR |= (1<<1); // INT0 ����������� �� �����.
   MCUCR &= ~(1<<0);// INT0 ����������� �� �����.
   GICR |= (1<<6);  // INT0 ��������� ����������.
   
   MCUCR |= (1<<3); // INT1 ����������� �� �����.
   MCUCR &= ~(1<<2);// INT1 ����������� �� �����.
   GICR |= (1<<7);  // INT1 ��������� ����������.
}

// ����������� ������� ������-������� ��� ��������� ������� ����� 60 ����� �����������.
void Timer_0_settings (void)
{
	TCCR0 = 0b00000101; // ����� ������� �� ��� ����� �������� �� 1024.
	TCNT0 = 0;
	TIMSK |= (1<<TOIE0); // ��������� ���������� ��� ������������.
}

	
	
// ����������� ������ ������-������� ��� ������ ������� ����� ���������� � ������� ����� �� ������.
// ������ ������������ ����� 33,55392 ������, ���� ������ �� �����.
// ���� ����� � �������� ������� ����� 0,000512 �������. 

void Timer_1_settings (void)
{
    TCCR1B = 0b00000101;  // ����� ������� �� ��� ����� �������� �� 1024.
    TCNT1 = 0;
}

//������ ��� ��������� �������� ����� 8 ������ ��� �������� (��� �������� � ������� �� ������)
void Timer_2_settings (void)
{
	TCCR2 |= (1<<2) | (1<<1) | (1<<0); // ����� ������� �� ��� ����� �������� �� 1024.
	TCNT2 = 0;
	TIMSK |= (1<<6); // ��������� ���������� ��� ������������.
}