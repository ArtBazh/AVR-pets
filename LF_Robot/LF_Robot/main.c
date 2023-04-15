/*
 * LF_Robot.c
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

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

char str_buffer [16]; // Буффер.
int arr_filter_PC2 [7] = {0};
int arr_filter_PC3 [7] = {0};
int x = 666;
int y = 666;
int i = 0;
int j = 0;
double ADC_to_ERR_coeff = 0.19480519480519; // 440 = 1.1 В; 480 = 1.2 В
int PID_PROPORTIONAL_int = 0;
int PID_DERIVATIVE_int = 0;
int PID_U_t = 0;
int HELPER = 0;
int ERR_x = 0;
int ERR_x_priv = 0;
int ERR_y = 0;
int ERR_y_priv = 0;
char flag_for_start = 0;
char x_flag = 0;
char y_flag = 0;


void PWM_1_settings (void)
{
	DDRB |= (1<<1); // Выход PWM для таймера 1, OC1A
	TCCR1A |= (1<<COM1A1); // НЕинвертирующая ШИМ
	TCCR1A &= ~(1<<COM1A0); // НЕинвертирующая ШИМ
	/*
	TCCR1A |=  (1<<WGM10); // 8-bit Fast PWM
	TCCR1A &= ~(1<<WGM11); // 8-bit Fast PWM
	TCCR1B |= (1<<WGM12); // 8-bit Fast PWM
	TCCR1B &= ~(1<<WGM13); // 8-bit Fast PWM
	
	
	TCCR1B |= (1<<CS11); // Запуск таймера + делим частоту на 8
	TCCR1B &= ~((1<<CS12) | (1<<CS10)); // Запуск таймера + делим частоту на 8
	
	*/
	
	TCCR1A |=  (1<<WGM10); // 8-bit Phase Correct PWM
	TCCR1A &= ~(1<<WGM11); // 8-bit Phase Correct PWM
	TCCR1B &= ~(1<<WGM12); // 8-bit Phase Correct PWM
	TCCR1B &= ~(1<<WGM13); // 8-bit Phase Correct PWM
	
	//TCCR1B |= (1<<CS10); // Запуск таймера + не делим частоту
	//TCCR1B &= ~((1<<CS12) | (1<<CS11)); // Запуск таймера + не делим частоту
	TCCR1B |= (1<<CS11); // Запуск таймера + делим частоту на 8
	TCCR1B &= ~((1<<CS12) | (1<<CS10)); // Запуск таймера + делим частоту на 8
}

void PWM_2_settings (void)
{ 
	DDRB |= (1<<3); // Выход PWM для таймера 2, OS2
	TCCR2 |= (1<<COM21); // НЕинвертирующая PWM
	TCCR2 &= ~(1<<COM20); // НЕинвертирующая PWM

/*
	TCCR2 |= (1<<WGM21) | (1<<WGM20); //8-bit Fast PWM
	
	TCCR2 &= ~((1<<CS22) | (1<<CS20)); // Запуск таймера + делим частоту на 8
	TCCR2 |= (1<<CS21); // Запуск таймера + делим частоту на 8
*/

	TCCR2 |=  (1<<WGM20); //8-bit Phase correct PWM
	TCCR2 &= ~(1<<WGM21); //8-bit Phase correct PWM
	
	//TCCR2 &= ~((1<<CS22) | (1<<CS21)); // Запуск таймера + не делим частоту
	//TCCR2 |= (1<<CS20); // Запуск таймера + не делим частоту
	TCCR2 &= ~((1<<CS22) | (1<<CS20)); // Запуск таймера + делим частоту на 8
	TCCR2 |= (1<<CS21); // Запуск таймера + делим частоту на 8
}

int main(void)
{
	ACSR |= (1<<7); // Отключаем компаратор, ради экономии энергии (по умолчанию он включен).
	
	ADC_settings ();
	
	PWM_1_settings ();
	PWM_2_settings ();
	
	DDRD |= (1<<7); // Двигатель левый
	DDRB |= (1<<0); // Двигатель правый
	PORTD &= ~(1<<7); // Минус - движение вперед - Двигатель левый
	PORTB &= ~(1<<0); // Минус - движение вперед - Двигатель правый

    while (1) 
    {	
		/*
		PORTD &= ~(1<<7);
		PORTB &= ~(1<<0); 
		OCR1A = 255;
		OCR2 = 255;
		_delay_ms(5000);
		PORTD |= (1<<7);
		PORTB |= (1<<0);
		OCR1A = 127;
		OCR2 = 127;
		_delay_ms(5000);
		*/						  
		ADC_channal_PC2(); // x - Левый датчик - зеленый провод - PC2 - ADC2
		if (ADCSRA & (1<<ADIF))
		{			
			arr_filter_PC2[i] = ADC;
			i++;
			ADCSRA |= (1<<ADIF);
		}		
		
		ADC_channal_PC3();
		
		if (ADCSRA & (1<<ADIF))
		{
			arr_filter_PC3[j] = ADC; 
			j++;
			
			if (i>6) // Рассчёты последнего шага тут, чтобы не всрать время замера со второго датчика
			{
				x = 0;
				i = 0;
				for (int k = 0; k < 7; k++)
				{
					x = x + arr_filter_PC2[k] ;
				}
				x = x/7;
			}
			
			if (j>6)
			{
				y = 0;
				j = 0;	
				for (int n = 0; n < 7; n++)
				{
				y = y +	arr_filter_PC3[n];
				}
				y = y/7;
			}
			ADCSRA |= (1<<ADIF);
		}

		if ((x<60) && (y<60))
		{
			PORTD &= ~(1<<7); // Минус - движение вперед - Двигатель левый
			PORTB &= ~(1<<0); // Минус - движение вперед - Двигатель правый
			
			OCR1A = 180; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 180;// Правый двигатель - правая гусеница (вид спереди)
			flag_for_start = 1;
			x_flag = 0;
			y_flag = 0;
		}
		
		if ((flag_for_start==0) && ((x>60) && (y>60)))
		{
			OCR1A = 0; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 0;// Правый двигатель - правая гусеница (вид спереди)
		}
		
		if ((x>60) && (y<61))
		{
			PORTD &= ~(1<<7); // Минус - движение вперед - Двигатель левый
			PORTB &= ~(1<<0); // Минус - движение вперед - Двигатель правый
			
			ERR_x_priv = ERR_x;
			ERR_x = x - 60;
			x_flag = 1;
			
			PID_PROPORTIONAL_int = (int) round (2 * ADC_to_ERR_coeff * ERR_x);
			
			PID_DERIVATIVE_int = (int) round (1.5 * ADC_to_ERR_coeff * (ERR_x - ERR_x_priv));

			PID_U_t = PID_PROPORTIONAL_int + PID_DERIVATIVE_int;
			
			if (PID_U_t > 75) 
			{
				OCR1A = 255;
				OCR2 = 180 - PID_U_t;
			}
			else
			{
			OCR1A = 180 + PID_U_t; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 180 - PID_U_t; // Правый двигатель - правая гусеница (вид спереди)
			}
		}

		if ((y>60) && (x<61))
		{
			PORTD &= ~(1<<7); // Минус - движение вперед - Двигатель левый
			PORTB &= ~(1<<0); // Минус - движение вперед - Двигатель правый
			
			ERR_y_priv = ERR_y;
			ERR_y = y - 60;
			y_flag = 1;
			
			PID_PROPORTIONAL_int = (int) round (2 * ADC_to_ERR_coeff * ERR_y);
			
			PID_DERIVATIVE_int = (int) round (1.5 * ADC_to_ERR_coeff * (ERR_y - ERR_y_priv));
			
			PID_U_t = PID_PROPORTIONAL_int + PID_DERIVATIVE_int;
			
			if (PID_U_t > 75) 
			{
				OCR1A = 180 - PID_U_t;;
				OCR2 = 255;
			}
			else
			{
			OCR1A = 180 - PID_U_t; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 180 + PID_U_t;// Правый двигатель - правая гусеница (вид спереди)			
			}	

		}
		
		if ((x_flag == 1) && ((x>100) && (y>100)))
		{	
			PORTB |= (1<<0);
			PORTD &= ~(1<<7); // Запускем левый двигатель вперед, а правый назад
			
			OCR1A = 225; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 30; // Правый двигатель - правая гусеница (вид спереди)

			x_flag = 0;
			
			ERR_x = 0;
			ERR_x_priv = 0;
			ERR_y = 0;
			ERR_y_priv = 0;
		}
		if ((y_flag == 1) && ((x>100) && (y>100)))
		{
			PORTB &= ~(1<<0); // Запускем правый двигатель вперед, а левый назад
			PORTD |= (1<<7);
			
			OCR1A = 30; // Левый двигатель - левая гусеница (вид спереди)
			OCR2 = 225;// Правый двигатель - правая гусеница (вид спереди) 
			
			y_flag = 0;
			
			ERR_x = 0;
			ERR_x_priv = 0;
			ERR_y = 0;
			ERR_y_priv = 0;
		}
		
		
    } // While
} // Main

