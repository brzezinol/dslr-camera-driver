/*
 * AVR_SLIDER_DRIVER.c
 *
 * Created: 2012-02-02 20:51:15
 *  Author: Piotrek Brzeziński
 *  email: brzezinol@gmail.com
 */ 

#include <avr/eeprom.h>
#include "USART.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "eeprom.h"

#define F_CPU 4000000UL

#define SENS_TRANS_PIN PB3
#define SENS_RECV_PIN PB4

#define C_PIN		PB0      // koñcówka kondensatora
#define R_PIN		PB2		 // koncowka rezystora
#define P_PIN		PB1		 // koncowka pot

#define P_OUT_STEP	PD3
#define P_OUT_DIR	PD2
#define P_OUT_MS1	PD6
#define P_OUT_MS2	PD5
#define P_OUT_MS3	PD4

#define SET_DIR_LEFT			PORTD &= ~(1 << P_OUT_DIR)
#define SET_DIR_RIGHT			PORTD |= (1 << P_OUT_DIR)
#define SET_STEPPER_STOP		TCCR0B = 0x00
#define SET_STEPPER_START		TCCR0B = (1<<CS02)|(0<<CS01)|(0<<CS00)

#define SET_TIMER1_STOP			TCCR1B = 0x00
#define SET_TIMER1_START		TCCR1B = (1<<ICNC1) | (1<<ICES1) | (1<<CS10);
#define SET_TIMER1_CLEAR		TCNT1 = 0x00
#define SET_COMPARATOR_DISABLE	ACSR = (1<<ACD)
#define SET_COMPARATOR_ENABLE	ACSR = (1<<ACIC);
#define _BV(n)					(1 << n)

volatile uint8_t AdcBusy = 0;		// do sprawdzania zajêtoci przetwornika
volatile uint32_t AdcMaxVal = 0;	// wartosc czytana z eeprom po kalibracji 2 bajt
volatile uint32_t AdcValue = 0;		// przechowuje przetworzon¹ wartoæ
volatile uint32_t AdcAvgValue = 0;		// 
volatile uint32_t AdcAvgSum = 0;		// 
volatile unsigned char AdcCount = 0;
volatile uint32_t AdcHalfValue = 0;	// przechowuje po³owê wartoci max
//volatile uint32_t STEP_COUNT = 0;
volatile uint16_t ACTUAL_RELEASE_TIMER_VALUE = 0;
volatile char LAST_STOP_DIR = 255;
volatile char INITIAL_LOCK = 1;
volatile char CLEAR_WAIT = 0;

void AdcStart(void)        
{
  AdcBusy = 1;                
  SET_TIMER1_STOP;
  SET_TIMER1_CLEAR;			// wyzeruj timer 1
  SET_COMPARATOR_ENABLE;
  DDRB &= ~(1<<C_PIN);		// ustaw liniê kondenstaora (- komparatora) jako wejcie
  PORTB |= (1<<R_PIN);      // ³adujemy kondensator
  SET_TIMER1_START;			// START LICZNIKA
  while(AdcBusy);			// czekaj na przerwanie od przechwytywania lub przepe³nienia
}

ISR (TIMER1_CAPT_vect)    // przerwanie od przechwytywania licznika 1
{
  AdcValue = ICR1;            // odczytaj wartoæ z rejestru przechwytuj¹cego
  SET_TIMER1_STOP;			   //TIMER1 STOP
  PORTB &= ~(1<<R_PIN);        //wylaczamy ³adowanie kondensatora
  DDRB |= (1<<C_PIN);		// ustaw liniê kondenstaora (- komparatora) jako wyjcie
  _delay_us(10);			// i czekaj na roz³adowanie kondensator
  AdcBusy = 0;        
}

ISR (TIMER1_OVF_vect)        // przerwanie od przepe³nienia licznika 1
{
  SET_TIMER1_STOP;
  PORTB &= ~(1<<R_PIN);        //wylaczamy ³adowanie kondensatora
  DDRB |= (1<<C_PIN);		// ustaw liniê kondenstaora (- komparatora) jako wyjcie
  _delay_us(10);			// i czekaj na roz³adowanie kondensator
  AdcValue = 0;                // wpisz wartoæ 0
  AdcBusy = 0; 
}

ISR (TIMER0_OVF_vect)
{
	TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
	PORTD ^= (1<<P_OUT_STEP);	
	//if(((PINB & _BV(P_OUT_STEP)) >> P_OUT_STEP) == 1)
	//{
		//STEP_COUNT ++;
	//}
}

void liczPowyzejPolowy(uint16_t * r)
{
	volatile int16_t tmp = AdcAvgValue / 100;
	tmp *= 8;
	tmp -= 35;
	*r = tmp;
	//*r = 0;
}

void liczPonizejPolowy(uint16_t * r)
{
	volatile int16_t tmp = AdcAvgValue / 100;
	tmp *= -12;
	tmp += 235;
	*r = tmp;
	//*r = 0;
}

int main(void)
{
	//port B
	DDRB = (1<<R_PIN) | (1<<PB7) | (1<<SENS_TRANS_PIN);
	PORTB = 0x00;
	PORTB |= (1<<SENS_TRANS_PIN);
	
	//port D
	DDRD = 0x00;
	DDRD |= (1<<P_OUT_MS1);
	DDRD |= (1<<P_OUT_MS2);
	DDRD |= (1<<P_OUT_MS3);
	DDRD |= (1<<P_OUT_STEP);
	DDRD |= (1<<P_OUT_DIR);//P
	
	PORTD = 0x00;
	PORTD |= (1<<P_OUT_MS1);
	PORTD |= (1<<P_OUT_MS2);
	PORTD |= (1<<P_OUT_MS3);
	PORTD |= (0<<P_OUT_STEP);
	PORTD |= (1<<P_OUT_DIR);//P
	
	SET_STEPPER_STOP;
	//
	TIMSK = (1<<TOIE0) | (1<<TOIE1) | (1<<ICIE1);
	
	//reading eeprom settings
	AdcMaxVal = eeprom_read_word(&eeprom_adc_max_value);
	AdcHalfValue = eeprom_read_word(&eeprom_adc_half_value);
	
	//kalibracja
	if(((PINB & _BV(PB7)) >> PB7) == 1)
	{
		_delay_ms(50);
		sei();
		while(((PINB & _BV(PB7)) >> PB7) == 1)
		{
			AdcStart();
		}
		AdcMaxVal = AdcValue;
		AdcMaxVal -= 400; 
		cli();
		eeprom_write_word(&eeprom_adc_max_value, AdcMaxVal);
		eeprom_busy_wait();
		AdcHalfValue = AdcMaxVal >> 1;
		eeprom_write_word(&eeprom_adc_half_value, AdcHalfValue);
		eeprom_busy_wait();
	}
	sei();
					
	
	while(1)
	{
		//if(((PINB & _BV(SENS_RECV_PIN)) >> SENS_RECV_PIN) == 1)
		//{
			//if(CLEAR_WAIT == 0){
				//LAST_STOP_DIR = ((PORTB & _BV(P_OUT_DIR)) >> P_OUT_DIR);
				//_delay_ms(10);	
				//SET_STEPPER_STOP;
			//}
			//else
			//{
				//if(((PINB & _BV(SENS_RECV_PIN)) >> SENS_RECV_PIN) == 0)
				//{
					//_delay_ms(10);	
					//CLEAR_WAIT = 0;
				//}			
			//}
		//}			
		
			_delay_ms(10);	
			AdcStart();
		
			AdcAvgSum += AdcValue;
			AdcCount ++;
			if(AdcCount>=20)
			{
				AdcAvgValue = AdcAvgSum / 20;
				AdcCount = 0;
				AdcAvgSum = 0;
	    
				if(INITIAL_LOCK==1)
				{
					if((AdcAvgValue > (AdcHalfValue - 50)) && (AdcAvgValue < (AdcHalfValue + 50)))
					{
						INITIAL_LOCK = 0;
					}						
				}
		
				if(INITIAL_LOCK==0)
				{
					if(AdcAvgValue < (AdcHalfValue - 50))
					{
						if(LAST_STOP_DIR == 255)
						{
							liczPonizejPolowy(&ACTUAL_RELEASE_TIMER_VALUE);
							if(ACTUAL_RELEASE_TIMER_VALUE > 248)
							{
								ACTUAL_RELEASE_TIMER_VALUE = 248;
							}
							SET_DIR_LEFT;
							SET_STEPPER_START;
						}				
						else
						{
							if(LAST_STOP_DIR == 1)
							{
								SET_DIR_LEFT;
								LAST_STOP_DIR = 255;
								CLEAR_WAIT = 1;
							}
						}						
						//TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
					}
					else if(AdcAvgValue > (AdcHalfValue + 50))
					{
						if(LAST_STOP_DIR == 255)
						{
							liczPowyzejPolowy(&ACTUAL_RELEASE_TIMER_VALUE);
							if(ACTUAL_RELEASE_TIMER_VALUE > 248)
							{
								ACTUAL_RELEASE_TIMER_VALUE = 248;
							}
							SET_DIR_RIGHT;
							SET_STEPPER_START;
						}		
						else
						{
							if(LAST_STOP_DIR == 0)
							{
								SET_DIR_RIGHT;
								LAST_STOP_DIR = 255;
								CLEAR_WAIT = 1;
							}
						}					
						//TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
					}
					else
					{
						ACTUAL_RELEASE_TIMER_VALUE = 0;
						SET_STEPPER_STOP;
					}
				}
				else
				{
					ACTUAL_RELEASE_TIMER_VALUE = 0;
					SET_STEPPER_STOP;
				}
			}		
	}
}