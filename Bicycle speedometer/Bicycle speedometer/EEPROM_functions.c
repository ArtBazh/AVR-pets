//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

void PORTB_LCD_Settings (void);
void LCD_commands (unsigned char);
void LCD_datas (unsigned char);
void LCD_settings(void);
void LCD_lines (unsigned char, unsigned char);
void LCD_str (char words[]);
void External_interrupts_settings (void);
void Timer_0_settings (void);
void Timer_1_settings (void);
void cleaning_LCD (void);

#define RS_0 PORTB &= ~(1<<2); // �������� �������.     (���������� � LCD_settings)
#define RS_1 PORTB |= (1<<2); // �������� ������.       (���������� � LCD_settings)
#define E_1 PORTB |= (1<<3); // ������������. �� �����. (���������� � LCD_settings)
#define E_0 PORTB &= ~(1<<3); // ������������.          (���������� � LCD_settings)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ������ ������ � EEPROM.
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	// ������� ���������� ���������� ������.
	while(EECR & (1<<EEWE))
	;
	// ����� �������� � ������� ������ � ������.
	EEAR = uiAddress;
	EEDR = ucData;
	
	// ��������� ������ ������ � EEPROM.
	EECR |= (1<<EEMWE);
	
	// ��������.
	EECR |= (1<<EEWE);
}

// ������ ������ �� EEPROM.
unsigned char EEPROM_read(unsigned int uiAddress)
{
	// ������� ���������� ���������� ������.
	while(EECR & (1<<EEWE))
	;
	// ������������� ����� EEPROM.
	EEAR = uiAddress;
	
	// ������ ������ ������ �� EEPROM.
	EECR |= (1<<EERE);
	
	// ���������� ������ � �������� ������.
	return EEDR;
}