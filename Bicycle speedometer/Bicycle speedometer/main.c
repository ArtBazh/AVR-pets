// Bicycle speedometer.c

#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <math.h>

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

#define RS_0 PORTB &= ~(1<<2); // Передача комманд.     (Относиться к LCD_settings.с)
#define RS_1 PORTB |= (1<<2); // Передача данных.       (Относиться к LCD_settings.с)
#define E_1 PORTB |= (1<<3); // Синхонизация. По спаду. (Относиться к LCD_settings.с)
#define E_0 PORTB &= ~(1<<3); // Синхонизация.          (Относиться к LCD_settings.с)

double const HALF_WHEEL_LENGTH = 0.00102; // Пройденный путь между сигналами с датчика в километрах.

char str_buffer [16]; // Буффер.
double v = 0;
unsigned int v_comparison = 0;
double s = 0;
unsigned long buzzer_ON_counter = 0;
unsigned char zeroing_counter_for_speed_without_moving = 0;
double time_counter = 0;
unsigned char flag_1 = 0;
unsigned char flag_2 = 0;
unsigned char flag_3 = 0;
unsigned char flag_4 = 0;
unsigned long mileage_RAM_meters_to_EEPROM = 0;
unsigned long mileage_RAM_meters_intermidiate = 0;
unsigned long mileage_RAM_meters_to_LCD = 0;
unsigned long mileage_RAM_kilometers_to_LCD = 0;

unsigned long EEMEM mileage_EEPROM_meters = 0; // Пройденный путь за всё время в метрах в EEPROM.

// Ведет отсчет до 1 часа если спидометр включен и велик стоит, затем включает зуммер.
ISR (TIMER0_OVF_vect)
{
	buzzer_ON_counter++;
}

ISR (TIMER2_OVF_vect)
{
	zeroing_counter_for_speed_without_moving ++;
}

// Прерывания от нажатия кнопки, показывает пройденное за все время расстояние.
ISR (INT0_vect)
{
	cli ();
	zeroing_counter_for_speed_without_moving = 0; // Сбрасываем счетчик для обнуления скорости без движения.
	
	flag_2 ^= 1; // Устанавливаем/снимаем флаг, чтобы отобразить/скрыть общее расстояние на LCD.
	flag_4 = 36; // Устанавливаем флаг, чтобы при повторном нажатии отобразить снова скорость при отсутствии сигналось с колеса.
	
	// Вытаскиваем из EEPROM данные (в метрах) о пройденном пути за всё время.
	mileage_RAM_meters_to_LCD = eeprom_read_dword (&mileage_EEPROM_meters);

	// Переводим пройденный путь за все время в киллометры.
	mileage_RAM_kilometers_to_LCD = mileage_RAM_meters_to_LCD / 1000;
	
	sei ();
}

// Прерывания от датчика холла на колесе.
ISR(INT1_vect)
{
	time_counter = TCNT1; // Добываем время из показания таймера.
	TCNT1 = 0;
	
	// Переводим значения счетчика во время в секундах для 2 Мгц, затем переводим в часы.
	time_counter *= (0.000512 * 0.00028);
	
	
// Танецуем с бубном чтобы при первом сигнале от датчика с колеса таймер не насчитал нам скорости.
	if (flag_1==0)
	{
		flag_1 = 1;
		Timer_1_settings();
		return;
	}
	
	s += HALF_WHEEL_LENGTH;
	v = (HALF_WHEEL_LENGTH / time_counter); // Округлять смысла нет, так как snprintf сама это сделает за нас.
	
 // Округляем скорость и записываем в переменную целочисленного типа для дальнейшнего сравнения.
	v_comparison = (unsigned int) round(v);
	
/* flag_4 нужен чтобы отображать скорость на дисплее лишь около 1 раза в секунду 
   в зависимости от скорости движения, чтобы LCD не мерца как сумасшедший. */
	
	if (flag_4 >= 36) {flag_4 = 0;} // Сбрасываем флаг, если после предиущего цикла уже отобразили инфу на LCD.
		
	if (v <= 8) {flag_4 += 18;} 
	else if (v <= 24) {flag_4 += 9;} 
	else if (v <= 34) {flag_4 += 6;} 
	else {flag_4 += 4;}

   buzzer_ON_counter = 0; // Сбрасываем счетчик считающий до 60 минут для зуммера.
   
   flag_3 = 1; // Этот флаг запустит запись о пройденном пути в EEPROM в основном цикле.
   
   zeroing_counter_for_speed_without_moving = 0; // Сбрасываем счетчик для обнуления скорости без движения.
}


int main(void)
{	
	ACSR |= (1<<7); // Отключаем компаратор, ради экономии энергии (по умолчанию он включен).
	
	// Вытягиваеим записанные из EEPROM данные о пройденном пути за все время.
	mileage_RAM_meters_intermidiate = eeprom_read_dword (&mileage_EEPROM_meters);
	
	flag_4 = 36; // Взводим флаг, чтобы при запуске после приветствия отобразить нулевую скорость на LCD.
	
	PORTB_LCD_Settings ();
	LCD_settings ();
		
	// Настраиваем порты для зуммера.
	DDRD |=(1<<6) | (1<<5);
	PORTD &=~(1<<6) | (1<<5);
	
	// Приветствие.
	LCD_lines (1, 0);
	LCD_str ("Hello, Rita");
    LCD_lines (2, 0);
	LCD_str ("Good luck! (^_^)");
	_delay_ms(4000);
	cleaning_LCD ();
	
	sei (); // Разрешаем глобальные прерывания.	
	Timer_0_settings ();
	Timer_2_settings ();
	External_interrupts_settings();

while (1) 
{

/* Ждёт 60 минут, затем запускает 2-ва, надрывающихся в бесконечном цикле зуммера,
   бесцеремонно намекая моей сестре, что неплохо было бы выключить спидометр. */

		if (buzzer_ON_counter >= 27573)
		{   
			cli ();
			
			cleaning_LCD ();
			
			LCD_lines (1, 3);
			LCD_str ("PRESS RED");		
			LCD_lines (2, 1);
			LCD_str ("!!! BUTTON !!!");
			
			while(1) 
			
			{   PORTD |=(1<<6) | (1<<5);
			    _delay_ms(700);
				PORTD &=~((1<<6) | (1<<5));
				_delay_ms(300);
			}
		}
		
		
		// Ждёт 8 секунд и если сигналов с датчика нет, то обнуляет скорость.
		if (zeroing_counter_for_speed_without_moving>=61)
		{
			zeroing_counter_for_speed_without_moving = 0;
			v = 0;
			flag_4 = 36; // Взводим флаг, чтобы отобразить нулевую скорость на LCD.
		} 
		
		// Этот флаг устанавливает в прерывания от датчика с колеса и тут мы записываем новые данные о пути в EEPROM
		if (flag_3==1)
		{
			cli ();
			// Доплюсовываем к данным о пройденном за всё время пути, путь за промежуток между сигналов с датчика.
			mileage_RAM_meters_to_EEPROM = mileage_RAM_meters_intermidiate + (s*1000);
				
			// Записываем полученное на предидущем шаге в EEPROM.
			eeprom_write_dword (&mileage_EEPROM_meters, mileage_RAM_meters_to_EEPROM);
			sei ();
			
			// Сбрасываем флаг, чтобы не мучить EEPROM в перерывах между сигналами от датчика с колеса.
			flag_3 = 0; 
		}
		
		// Показываем весь путь из EEPROM.
		if (flag_2==1)
		{
			cleaning_LCD ();
						
			snprintf (str_buffer, sizeof(str_buffer), "Km = %.ld", mileage_RAM_kilometers_to_LCD);
			LCD_lines (1, 1);
			LCD_str (str_buffer);

			snprintf (str_buffer, sizeof(str_buffer), "M = %.ld", mileage_RAM_meters_to_LCD);
			LCD_lines (2, 1);
			LCD_str (str_buffer);
			
			while (flag_2==1)
			{_delay_us(10);} // Без этого не выйти из прерывания, бесконечный цикл тонет в сравнении и игнорирует внешнее прерывание
		}
		else if (flag_4 >= 36) // Каждые N (зависит от скорости) сигналов с датчика с колеса ообновляем инфу на LCD, иначе он рябит и нан его не возможно смотреть.
		{
			// Печатаем действущую скорость.
			snprintf (str_buffer, sizeof(str_buffer), "V = %.1lf km/h    ", v);
			LCD_lines (1, 1);
			LCD_str (str_buffer);

			// Печатаем пройденное за эту поездку расстояние.
			snprintf (str_buffer, sizeof(str_buffer), "S = %.3lf km    ", s);
			LCD_lines (2, 1);
			LCD_str (str_buffer);
		}

		

}
}

