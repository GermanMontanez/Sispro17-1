#include "UART-h.h"
#include <avr/io.h>


void initUART(void){
	
	PORTC.DIRSET=UART_PORT_Tx;
	PORTC.DIRCLR=UART_PORT_Rx;
	
	USARTC0.CTRLC=USART_CHSIZE_8BIT_gc;
	USARTC0.BAUDCTRLA= UART_9600BR;
	
	USARTC0.CTRLB|=USART_TXEN_bm | USART_RXEN_bm;
	
}


void sendChar(char c)
{
	
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTC0_DATA = c;
	
}


void sendString(char *text)
{
	while(*text)
	{
		sendChar(*text++);
	}
}


char usart_receiveByte()
{
	if( (USARTC0_STATUS & USART_RXCIF_bm) ){//Interesting DRIF didn't work.
	return USARTC0_DATA;
	}
	else{
		return 0;
	}
}