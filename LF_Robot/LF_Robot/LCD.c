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

#define RS_1 PORTD |= (1<<2); // Передача данных
#define RS_0 PORTD &= ~(1<<2); // Передача команд
#define E_1  PORTD |= (1<<3); // Синхронизация
#define E_0  PORTD &= ~(1<<3); // Синхронизация
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void PORT_LCD_settings(void)
{
	DDRD |= (1<<7) | (1<<6) | (1<<5) | (1<<4) | (1<<3) | (1<<2);
	PORTD &= ~((1<<7) | (1<<6) | (1<<5) | (1<<4) | (1<<3) | (1<<2));
}

void LCD_commands (unsigned char command)
{
	PORTD = command;
	RS_0; // Передаем команду
	E_1;
	_delay_us(50);
	E_0;
	PORTD = (command<<4);
	RS_0; // Передаем команду
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

// Передача данных к LCD
void LCD_datas (unsigned char data)
{
	PORTD = data;
	RS_1; // Передаем команду
	E_1;
	_delay_us(50);
	E_0;
	PORTD = (data<<4);
	RS_1; // Передаем команду
	E_1;
	_delay_us(50);
	E_0;
	_delay_us(50);
}

void LCD_settings (void)
{
	_delay_ms(15); // Задержка при запуске ЖКД
	LCD_commands(0b00000011);
	_delay_ms(5);
	LCD_commands(0b00000011);
	_delay_us(100);
	LCD_commands(0b00000011);
	_delay_ms(1);
	LCD_commands(0b00000010);
	_delay_ms(1);
	LCD_commands(0b00101000); // 4 бита, 2 линии, шрифт 5*8
	_delay_ms(1);
	LCD_commands(0b00001100); // Включаем дисплей, не показываем курсор, не мигаем курсором
	_delay_ms(1);
	LCD_commands(0b00000001); // Очистка экрана
	_delay_ms(2);
	LCD_commands(0b00000110); // Инкрементировние курсора и сдвие его вправо
	_delay_ms(2);
}

// Выбор линии и позиции для вывода данных на дисплей.
void LCD_lines (unsigned char x, unsigned char y)
{
  // Если подаем в функцию значение (1, 'число') то пишем в первой строке, начиная с позиции 'число'.
  //  Если (2,'число') то пишем во второй строке, начиная также с позиции 'число'. 

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

void cleaning_LCD (void)
{
	LCD_lines (1, 0);
	LCD_str ("                ");
	LCD_lines (2, 0);
	LCD_str ("                ");
}
*/