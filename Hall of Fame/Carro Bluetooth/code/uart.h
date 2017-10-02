#ifndef UART_H
#define UART_H

#define UART_PORT_Tx 0x08
#define UART_PORT_Rx 0x04
#define UART_9600BR  12

void initUART(void);
void sendChar(char c);
void sendString(char *text);
char usart_receiveByte();

#endif