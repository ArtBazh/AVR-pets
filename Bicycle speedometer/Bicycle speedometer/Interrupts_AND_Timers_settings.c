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



/* Настраиваем INT1 (PD3) и INT0 (PD2) внешние прерывания, для сигнала с датчика холла и переключения дисплея
    на отоборажение пробега за всё время соответственно. */
void External_interrupts_settings (void)
{
   DDRD &= ~((1<<3) | (1<<2));
   PORTD |= (1<<3) | (1<<2);
	  
   MCUCR |= (1<<1); // INT0 настраиваем по спаду.
   MCUCR &= ~(1<<0);// INT0 настраиваем по спаду.
   GICR |= (1<<6);  // INT0 разрешаем прерывания.
   
   MCUCR |= (1<<3); // INT1 настраиваем по спаду.
   MCUCR &= ~(1<<2);// INT1 настраиваем по спаду.
   GICR |= (1<<7);  // INT1 разрешаем прерывания.
}

// Настраиваем нулевой таймер-счетчик для включения зуммера через 60 минут бездействия.
void Timer_0_settings (void)
{
	TCCR0 = 0b00000101; // Делим частоту МК для этого счетчика на 1024.
	TCNT0 = 0;
	TIMSK |= (1<<TOIE0); // Разрешаем прерывания при переполнении.
}

	
	
// Настраиваем первый таймер-счетчик для замера времени между импульсами с датчика холла от колеса.
// Таймер переполнится через 33,55392 секунд, чего хватит за глаза.
// Одно число в регистре таймера равно 0,000512 секунды. 

void Timer_1_settings (void)
{
    TCCR1B = 0b00000101;  // Делим частоту МК для этого счетчика на 1024.
    TCNT1 = 0;
}

//Таймер для обнуления скорости через 8 секунд без движения (без сигналов с датчика на колесе)
void Timer_2_settings (void)
{
	TCCR2 |= (1<<2) | (1<<1) | (1<<0); // Делим частоту МК для этого счетчика на 1024.
	TCNT2 = 0;
	TIMSK |= (1<<6); // Разрешаем прерывания при переполнении.
}