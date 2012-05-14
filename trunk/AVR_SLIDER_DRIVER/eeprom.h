/*
 * eeprom.h
 *
 * Created: 2012-03-13 21:06:21
 *  Author: Piotrek
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
uint16_t eeprom_adc_max_value       __attribute__((section(".eeprom"))) = 50000; 
uint16_t eeprom_adc_half_value       __attribute__((section(".eeprom"))) = 25000; 




#endif /* EEPROM_H_ */