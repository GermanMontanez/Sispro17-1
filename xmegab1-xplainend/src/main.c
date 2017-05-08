/*
 * adcsample.c
 *
 * Created: 4/5/2017 10:44:21 AM
 * Author : germa
 */ 





#include <avr/io.h>
#include "adc_driver.h"
#include "uart.h"
#include "dht22.h"

#define ADCPORT PORTB
#define LEDPORT PORTB

uint8_t temp_table[256] ={125,125,125,125,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,110,109,108,108,107,106,106,105,104,104,103,102,102,101,100,100,99,98,98,97,96,96,95,94,94,93,92,92,91,90,90,89,89,88,88,87,87,86,86,85,85,84,83,83,82,81,80,80,79,79,78,78,77,77,76,76,75,75,74,74,73,73,72,72,71,71,70,70,69,69,68,68,68,67,67,66,66,65,65,65,64,64,63,63,63,62,62,61,61,61,60,60,59,59,59,58,58,57,57,57,56,56,56,55,55,54,54,54,53,53,53,52,52,51,51,51,50,50,50,49,49,49,48,48,48,47,47,47,46,46,46,45,45,44,44,43,43,43,42,42,42,41,41,41,41,40,40,39,39,38,38,38,38,37,37,36,36,36,35,35,35,34,34,34,33,33,32,32,31,31,31,30,30,29,29,29,28,28,27,27,27,26,26,25,25,25,24,24,23,23,22,22,22,21,21,20,20,19,19,18,18,17,17,16,16,15,14,14,13,13,12,11,11,10,10,9,8,8,7,6,6,5,4,3,3,2,1,0,0,0,0,0,0,0,0,0};
void ADC_result(uint8_t *res); //Converts the ADC result to 

int main(void)
{
	
	float mytemp=0.0;
	float myhum=0.0;
	int aux=0;
	

   
		char char1=0;
		char char2=0;
		char char3=0;
	initUART();
	
	uint8_t temp_result =0;
	//IOPORTS
	ADCPORT.DIRCLR = 0x0F; //Pin0 to pin3 of PORTB as inputs
	LEDPORT.DIRSET = 0xF0; //Pin4 to pin7 of PORTB as outputs
	LEDPORT.OUTSET = 0xF0; //4 leds OFF
	PORTA.PIN0CTRL|=0b00000000;
	//ADC
	ADC_CalibrationValues_Load(&ADCB); //Moves calibration values from memory to CALL/CALH.
	
	ADCB.CTRLB = 0x04; //no limit current, unsigned, no freerun, 8 bit resolution
	ADCB.REFCTRL = 0x00; //internal reference voltage of 1 volt
	ADCB.SAMPCTRL = 0x3F; //Configurates the maximum sample time
	ADCB.CH0.CTRL=0x01 ; //single ended, gain factor x1
	ADCB.CH0.MUXCTRL= 0x00; //Selects pin 0 of PORTB as the ADC input (temp sensor)
	ADCB.CTRLA |=0x01; //Enables ADC
    /* Replace with your application code */
	ADCB.CH0.CTRL|=0x80;
	
	
	{
		LEDPORT.OUTCLR = 0x20;
	}
    while (1) 
    {
		
		if((ADCB.CH0.INTFLAGS & 0x01) !=0) //Asks if the ADC conversion is complete
		{
			LEDPORT.OUTTGL=0x10;
			ADCB.CH0.INTFLAGS |= 0x01; //Clears the ADC flag
			ADC_result( &temp_result);//gets the ADC conversion result and changes the DC
// 			char1=(temp_result%10)|0b00110000;
// 			char2=((temp_result/10)%10)|0b00110000;;
// 			char3=((temp_result/100)%10)|0b00110000;;
// 			sendChar(char3);
// 			sendChar(char2);
// 			sendChar(char1);
// 			sendChar(0x0A);
			ADCB.CH0.CTRL|=0x80; //Starts an ADC conversion
			
		}
	
		if (usart_receiveByte()== 'D')
		{
			
			sendString("something");
			LEDPORT.OUTTGL=0x20;
		}
	
		if (dht_gettemperaturehumidity(&mytemp,&myhum)==0)
		{
// 			mytemp=mytemp*10;
// 			aux=(int)mytemp;
// 			char1=(aux%10)|0b00110000;
// 			char2=((aux/10)%10)|0b00110000;;
// 			char3=((aux/100)%10)|0b00110000;;
// 			sendChar('T');
// 			sendChar(char3);
// 			sendChar(char2);
// 			sendChar(char1);
// 			sendChar(0x0A);
// 			myhum=myhum*10;
// 			aux=(int)myhum;
// 			char1=(aux%10)|0b00110000;
// 			char2=((aux/10)%10)|0b00110000;;
// 			char3=((aux/100)%10)|0b00110000;;
// 			sendChar('H');
// 			sendChar(char3);
// 			sendChar(char2);
// 			sendChar(char1);
// 			sendChar(0x0A);
			LEDPORT.OUTTGL=0x40;
		}
		
		if (usart_receiveByte()=='T')
		{
			
			char1=(temp_result%10)|0b00110000;
			char2=((temp_result/10)%10)|0b00110000;;
			char3=((temp_result/100)%10)|0b00110000;;
			sendChar('T');
			sendChar(char3);
			sendChar(char2);
			sendChar(char1);
			
			mytemp=mytemp*10;
			aux=(int)mytemp;
			char1=(aux%10)|0b00110000;
			char2=((aux/10)%10)|0b00110000;;
			char3=((aux/100)%10)|0b00110000;;
			sendChar('T');
			sendChar(char3);
			sendChar(char2);
			sendChar(char1);
			
			myhum=myhum*10;
			aux=(int)myhum;
			char1=(aux%10)|0b00110000;
			char2=((aux/10)%10)|0b00110000;;
			char3=((aux/100)%10)|0b00110000;;
			sendChar('H');
			sendChar(char3);
			sendChar(char2);
			sendChar(char1);
			
			
		}
		
		
    }
}

void ADC_result(uint8_t *res)
{
	
		*res = temp_table[ADCB.CH0.RESL];
		
	
}



