/*
 * USART.h
 *
 * Created: 2011-11-12 13:57:09
 *  Author: Piotrek
 */ 


#ifndef USART_H_
#define USART_H_


void USART_Transmit(char volatile data)
{
   while ( !( UCSRA & (1<<UDRE)) );         /* Wait for empty transmit buffer */ 
   UDR = data;
}

void USART_Init(uint16_t ubrr_value) 
{ 
   UBRRL = ubrr_value; 
   UBRRH = (ubrr_value>>8); 
   UCSRC = (1 << UCSZ1) | (1 << UCSZ0); 
   UCSRB=  1<<TXEN | 1<<RXEN | 1<<RXCIE;// | 1<<TXEN | 1<<RXCIE | 1<<TXCIE; 
}

void USART_TransmitString (char volatile *str) 
{ 
   while ((*str != 0) /*&& (*str != 10)*/) USART_Transmit (*str++); 
} 


#endif /* USART_H_ */