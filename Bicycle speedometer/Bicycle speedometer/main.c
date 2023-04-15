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

#define RS_0 PORTB &= ~(1<<2); // �������� �������.     (���������� � LCD_settings.�)
#define RS_1 PORTB |= (1<<2); // �������� ������.       (���������� � LCD_settings.�)
#define E_1 PORTB |= (1<<3); // ������������. �� �����. (���������� � LCD_settings.�)
#define E_0 PORTB &= ~(1<<3); // ������������.          (���������� � LCD_settings.�)

double const HALF_WHEEL_LENGTH = 0.00102; // ���������� ���� ����� ��������� � ������� � ����������.

char str_buffer [16]; // ������.
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

unsigned long EEMEM mileage_EEPROM_meters = 0; // ���������� ���� �� �� ����� � ������ � EEPROM.

// ����� ������ �� 1 ���� ���� ��������� ������� � ����� �����, ����� �������� ������.
ISR (TIMER0_OVF_vect)
{
	buzzer_ON_counter++;
}

ISR (TIMER2_OVF_vect)
{
	zeroing_counter_for_speed_without_moving ++;
}

// ���������� �� ������� ������, ���������� ���������� �� ��� ����� ����������.
ISR (INT0_vect)
{
	cli ();
	zeroing_counter_for_speed_without_moving = 0; // ���������� ������� ��� ��������� �������� ��� ��������.
	
	flag_2 ^= 1; // �������������/������� ����, ����� ����������/������ ����� ���������� �� LCD.
	flag_4 = 36; // ������������� ����, ����� ��� ��������� ������� ���������� ����� �������� ��� ���������� ��������� � ������.
	
	// ����������� �� EEPROM ������ (� ������) � ���������� ���� �� �� �����.
	mileage_RAM_meters_to_LCD = eeprom_read_dword (&mileage_EEPROM_meters);

	// ��������� ���������� ���� �� ��� ����� � ����������.
	mileage_RAM_kilometers_to_LCD = mileage_RAM_meters_to_LCD / 1000;
	
	sei ();
}

// ���������� �� ������� ����� �� ������.
ISR(INT1_vect)
{
	time_counter = TCNT1; // �������� ����� �� ��������� �������.
	TCNT1 = 0;
	
	// ��������� �������� �������� �� ����� � �������� ��� 2 ���, ����� ��������� � ����.
	time_counter *= (0.000512 * 0.00028);
	
	
// �������� � ������ ����� ��� ������ ������� �� ������� � ������ ������ �� �������� ��� ��������.
	if (flag_1==0)
	{
		flag_1 = 1;
		Timer_1_settings();
		return;
	}
	
	s += HALF_WHEEL_LENGTH;
	v = (HALF_WHEEL_LENGTH / time_counter); // ��������� ������ ���, ��� ��� snprintf ���� ��� ������� �� ���.
	
 // ��������� �������� � ���������� � ���������� �������������� ���� ��� ������������ ���������.
	v_comparison = (unsigned int) round(v);
	
/* flag_4 ����� ����� ���������� �������� �� ������� ���� ����� 1 ���� � ������� 
   � ����������� �� �������� ��������, ����� LCD �� ����� ��� �����������. */
	
	if (flag_4 >= 36) {flag_4 = 0;} // ���������� ����, ���� ����� ���������� ����� ��� ���������� ���� �� LCD.
		
	if (v <= 8) {flag_4 += 18;} 
	else if (v <= 24) {flag_4 += 9;} 
	else if (v <= 34) {flag_4 += 6;} 
	else {flag_4 += 4;}

   buzzer_ON_counter = 0; // ���������� ������� ��������� �� 60 ����� ��� �������.
   
   flag_3 = 1; // ���� ���� �������� ������ � ���������� ���� � EEPROM � �������� �����.
   
   zeroing_counter_for_speed_without_moving = 0; // ���������� ������� ��� ��������� �������� ��� ��������.
}


int main(void)
{	
	ACSR |= (1<<7); // ��������� ����������, ���� �������� ������� (�� ��������� �� �������).
	
	// ����������� ���������� �� EEPROM ������ � ���������� ���� �� ��� �����.
	mileage_RAM_meters_intermidiate = eeprom_read_dword (&mileage_EEPROM_meters);
	
	flag_4 = 36; // ������� ����, ����� ��� ������� ����� ����������� ���������� ������� �������� �� LCD.
	
	PORTB_LCD_Settings ();
	LCD_settings ();
		
	// ����������� ����� ��� �������.
	DDRD |=(1<<6) | (1<<5);
	PORTD &=~(1<<6) | (1<<5);
	
	// �����������.
	LCD_lines (1, 0);
	LCD_str ("Hello, Rita");
    LCD_lines (2, 0);
	LCD_str ("Good luck! (^_^)");
	_delay_ms(4000);
	cleaning_LCD ();
	
	sei (); // ��������� ���������� ����������.	
	Timer_0_settings ();
	Timer_2_settings ();
	External_interrupts_settings();

while (1) 
{

/* ��� 60 �����, ����� ��������� 2-��, ������������� � ����������� ����� �������,
   ������������ ������� ���� ������, ��� ������� ���� �� ��������� ���������. */

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
		
		
		// ��� 8 ������ � ���� �������� � ������� ���, �� �������� ��������.
		if (zeroing_counter_for_speed_without_moving>=61)
		{
			zeroing_counter_for_speed_without_moving = 0;
			v = 0;
			flag_4 = 36; // ������� ����, ����� ���������� ������� �������� �� LCD.
		} 
		
		// ���� ���� ������������� � ���������� �� ������� � ������ � ��� �� ���������� ����� ������ � ���� � EEPROM
		if (flag_3==1)
		{
			cli ();
			// ������������� � ������ � ���������� �� �� ����� ����, ���� �� ���������� ����� �������� � �������.
			mileage_RAM_meters_to_EEPROM = mileage_RAM_meters_intermidiate + (s*1000);
				
			// ���������� ���������� �� ���������� ���� � EEPROM.
			eeprom_write_dword (&mileage_EEPROM_meters, mileage_RAM_meters_to_EEPROM);
			sei ();
			
			// ���������� ����, ����� �� ������ EEPROM � ��������� ����� ��������� �� ������� � ������.
			flag_3 = 0; 
		}
		
		// ���������� ���� ���� �� EEPROM.
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
			{_delay_us(10);} // ��� ����� �� ����� �� ����������, ����������� ���� ����� � ��������� � ���������� ������� ����������
		}
		else if (flag_4 >= 36) // ������ N (������� �� ��������) �������� � ������� � ������ ���������� ���� �� LCD, ����� �� ����� � ��� ��� �� �������� ��������.
		{
			// �������� ���������� ��������.
			snprintf (str_buffer, sizeof(str_buffer), "V = %.1lf km/h    ", v);
			LCD_lines (1, 1);
			LCD_str (str_buffer);

			// �������� ���������� �� ��� ������� ����������.
			snprintf (str_buffer, sizeof(str_buffer), "S = %.3lf km    ", s);
			LCD_lines (2, 1);
			LCD_str (str_buffer);
		}

		

}
}

