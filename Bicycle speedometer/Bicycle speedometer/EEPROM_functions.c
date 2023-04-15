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

#define RS_0 PORTB &= ~(1<<2); // Передача комманд.     (Относиться к LCD_settings)
#define RS_1 PORTB |= (1<<2); // Передача данных.       (Относиться к LCD_settings)
#define E_1 PORTB |= (1<<3); // Синхонизация. По спаду. (Относиться к LCD_settings)
#define E_0 PORTB &= ~(1<<3); // Синхонизация.          (Относиться к LCD_settings)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Запись данных в EEPROM.
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	// Ожидаем завершения предидущей записи.
	while(EECR & (1<<EEWE))
	;
	// Пишем значиние в регистр адреса и данных.
	EEAR = uiAddress;
	EEDR = ucData;
	
	// Разрешаем запись данных в EEPROM.
	EECR |= (1<<EEMWE);
	
	// Стартуем.
	EECR |= (1<<EEWE);
}

// Чтение данных из EEPROM.
unsigned char EEPROM_read(unsigned int uiAddress)
{
	// Ожидаем завершения предидущей записи.
	while(EECR & (1<<EEWE))
	;
	// Устанавливаем адрес EEPROM.
	EEAR = uiAddress;
	
	// Начало чтения данных из EEPROM.
	EECR |= (1<<EERE);
	
	// Возвращаем данные с регистра данных.
	return EEDR;
}