//
///*
 //* AVR_SLIDER_DRIVER.c
 //*
 //* Created: 2012-02-02 20:51:15
 //*  Author: Piotrek
 //*/ 
//
//#include <avr/eeprom.h>
//#include "USART.h"
//#include <util/delay.h>
//#include <avr/interrupt.h>
//#include "eeprom.h"
//
//#define F_CPU 8000000UL
//
//#define P_OUT_STEP	PD3
//#define P_OUT_DIR	PD2
//#define P_OUT_MS1	PD6
//#define P_OUT_MS2	PD5
//#define P_OUT_MS3	PD4
//#define C_PIN		PB0        // koñcówka kondensatora
//#define R_PIN		PB2		 // koncowka rezystora
//
//#define SET_DIR_LEFT			PORTD &= ~(1 << P_OUT_DIR)
//#define SET_DIR_RIGHT			PORTD |= (1 << P_OUT_DIR)
//#define SET_STEPPER_STOP		TCCR0B = 0x00
//#define SET_STEPPER_START		TCCR0B = (1<<CS02)|(0<<CS01)|(1<<CS00)
//#define SET_TIMER1_STOP			TCCR1B = 0x00
//#define SET_TIMER1_START		TCCR1B = (1<<ICNC1) | (1<<ICES1) | (1<<CS10);
//#define SET_TIMER1_CLEAR		TCNT1 = 0x00
//#define SET_COMPARATOR_DISABLE	ACSR = (1<<ACD)
//#define SET_COMPARATOR_ENABLE	ACSR = (1<<ACIC);
//#define _BV(n)					(1 << n)
//
//volatile uint8_t AdcBusy = 0;		// do sprawdzania zajêtoœci przetwornika
//volatile uint32_t AdcMaxVal = 0;	// wartosc czytana z eeprom po kalibracji 2 bajt
//volatile uint32_t AdcValue = 0;		// przechowuje przetworzon¹ wartoœæ
//volatile uint32_t AdcHalfValue = 0;	// przechowuje po³owê wartoœci max
//volatile uint8_t ACTUAL_RELEASE_TIMER_VALUE = 140;
//volatile uint8_t rt_to_ah;
//
//
//void AdcStart(void)        
//{
  //AdcBusy = 1;                
  //SET_TIMER1_STOP;
  //SET_TIMER1_CLEAR;			// wyzeruj timer 1
  //DDRB |= (1<<C_PIN);		// ustaw liniê kondenstaora (- komparatora) jako wyjœcie
  //_delay_us(100);			// i czekaj na roz³adowanie kondensator
  //SET_COMPARATOR_ENABLE;
  //DDRB &= ~(1<<C_PIN);		// ustaw liniê kondenstaora (- komparatora) jako wejœcie
  //PORTB |= (1<<R_PIN);      // ³adujemy kondensator
  //SET_TIMER1_START;			// START LICZNIKA
  //while(AdcBusy);			// czekaj na przerwanie od przechwytywania lub przepe³nienia
//}
//
//ISR (TIMER1_CAPT_vect)    // przerwanie od przechwytywania licznika 1
//{
  //AdcValue = ICR1;            // odczytaj wartoœæ z rejestru przechwytuj¹cego
  //SET_TIMER1_STOP;			   //TIMER1 STOP
  //PORTB &= ~(1<<R_PIN);        //wylaczamy ³adowanie kondensatora
  //AdcBusy = 0;        
//}
//
//ISR (TIMER1_OVF_vect)        // przerwanie od przepe³nienia licznika 1
//{
  //SET_TIMER1_STOP;
  //AdcValue = 0;                // wpisz wartoœæ 0
  //AdcBusy = 0; 
//}
//
//ISR (TIMER0_OVF_vect)
//{
	//TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
	//PORTD ^= (1<<P_OUT_STEP);	
//}
//
//uint8_t WyliczPredkosc(uint32_t _val)
//{
	//volatile uint8_t ret = 0;
	//ret = (rt_to_ah * _val) * 10;
	//return ret;
//}
//
//void UstawPredkosc()
//{
	//volatile uint32_t val;
	//if(AdcValue > AdcHalfValue)
	//{
		//val = AdcValue;
		//val -= AdcHalfValue;
	//}
	//else
	//{
		//val = AdcHalfValue;
		//val -= AdcValue;
	//}
	//ACTUAL_RELEASE_TIMER_VALUE = WyliczPredkosc(val);
	//TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
	//SET_STEPPER_START;
//}
//
//
//int main(void)
//{
	//DDRB= (1<<R_PIN) | (1<<PB7);
	//PORTB = 0x00;
	//TIMSK = (1<<TOIE0) | (1<<TOIE1) | (1<<ICIE1);
	//TCCR1A = 0;
	//ACSR = (1<<ACIC); 
	//
	//DDRD = 0xFF;
	//PORTD = 0x00;
	//PORTD |= (1<<P_OUT_MS1);
	//PORTD |= (1<<P_OUT_MS2);
	//PORTD |= (0<<P_OUT_MS3);
	//PORTD |= (0<<P_OUT_STEP);
	//PORTD |= (1<<P_OUT_DIR);//P
	//
	//SET_STEPPER_STOP;
	//
	////reading eeprom settings
	//AdcMaxVal = eeprom_read_word(&eeprom_adc_max_value);
	//AdcHalfValue = eeprom_read_word(&eeprom_adc_half_value);
	//sei();
	//
	//if(((PINB & _BV(PB7)) >> PB7) == 1)
	//{
		//_delay_ms(50);
		//while(((PINB & _BV(PB7)) >> PB7) == 1)
		//{
			//AdcStart();
		//}
		//AdcMaxVal = AdcValue;
		//cli();
		//eeprom_write_word(&eeprom_adc_max_value, AdcMaxVal);
		//eeprom_busy_wait();
		//AdcHalfValue = AdcMaxVal >> 1;
		//eeprom_write_word(&eeprom_adc_half_value, AdcHalfValue);
		//eeprom_busy_wait();
		//sei();
	//}
	//rt_to_ah = (25000 / AdcHalfValue);
	//TCNT0 = ACTUAL_RELEASE_TIMER_VALUE;
	//SET_DIR_LEFT;
	//
	//
    //while(1)
	//{
		//_delay_ms(50);	
		//
		//AdcStart();
		//if(AdcValue <= (AdcHalfValue - 80))
		//{
			//SET_DIR_LEFT;
			//UstawPredkosc();
		//}
		//else if(AdcValue >= (AdcHalfValue + 80))
		//{
			//SET_DIR_RIGHT;
			//UstawPredkosc();
		//}
		//else
		//{
			//SET_STEPPER_STOP;
		//}
	//}
//}