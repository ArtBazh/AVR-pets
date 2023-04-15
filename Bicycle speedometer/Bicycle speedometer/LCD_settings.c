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

#define RS_0 PORTB &= ~(1<<2); // Передача комманд.     (Относиться к LCD_settings)
#define RS_1 PORTB |= (1<<2); // Передача данных.       (Относиться к LCD_settings)
#define E_1 PORTB |= (1<<3); // Синхонизация. По спаду. (Относиться к LCD_settings)
#define E_0 PORTB &= ~(1<<3); // Синхонизация.          (Относиться к LCD_settings)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Настройки портов. 
void PORTB_LCD_Settings (void)
{
	DDRB |= (1<<3) | (1<<2);
	PORTB &= ~((1<<3) | (1<<2));
	DDRC |= (1<<3) | (1<<2) | (1<<1) | (1<<0);
	PORTC &= ~((1<<3) | (1<<2) | (1<<1) | (1<<0));
}

// Передача комманд к LCD.
void LCD_commands (unsigned char command)
{
	unsigned char byte_higher = (command>>4);
	unsigned char byte_lower = (command);
	PORTC = byte_higher;
	RS_0; // Говорим что передаем комманду.
	E_1;
	_delay_us(50);
	E_0;
	PORTC = byte_lower;
	RS_0; // Говорим что передаем комманду.
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

// Передача данных к LCD.
void LCD_datas (unsigned char data)
{
	unsigned char byte_higher = (data>>4);
	unsigned char byte_lower = (data);
	PORTC = byte_higher;
	RS_1; // Говорим что передаем данные.
	E_1;
	_delay_us(50);
	E_0;
	PORTC = byte_lower;
	RS_1; // Говорим что передаем данные.
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

// Стартовые настройки LCD.
void LCD_settings(void)
{
	_delay_ms(15);            // Задержка при запуске LCD.
	LCD_commands (0b00000011);
	_delay_ms(5);
	LCD_commands (0b00000011);
	_delay_us(100);
	LCD_commands (0b00000011);
	_delay_ms(1);
	LCD_commands (0b00000010);
	_delay_ms(1);
	LCD_commands(0b00101100); // 4-bit передача данных, задействуем обе строчки дисплея, шрифт 5*11.
	_delay_ms(1);
	LCD_commands(0b00001100); // Включаем дисплей, не включаем курсор и не мигаем им.
	_delay_ms(1);
	LCD_commands(0b00000001); // Очищаем дисплей.
	_delay_ms(2);
	LCD_commands(0b00000110); // Инкрементируем курсор и двигаем его вправо.
	_delay_ms(2);
}

// Выбор линии и позиции для вывода данных на дисплей.
void LCD_lines (unsigned char x, unsigned char y)
{
/* Если подаем в функцию значение (1, 'число') то пишем в первой строке, начиная с позиции 'число'.
   Если (2,'число') то пишем во второй строке, начиная также с позиции 'число'. */

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

// Выводим строку на дисплей через символьный массив.
void LCD_str (char words[])
{
	for (unsigned char z = 0; words[z] != '\0'; z++)
	{
		LCD_datas(words[z]);
	}
}

// Очистка дисплея.
void cleaning_LCD (void)
{
	LCD_lines (1, 0);
	LCD_str ("                ");
	LCD_lines (2, 0);
	LCD_str ("                ");
}
