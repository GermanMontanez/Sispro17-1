/*
 * POYECTO FINAL SISPRO.c
 *
 * Created: 23/05/2017 21:32:57 p. m.
 * Author : Fabian y Nicolas
 */ 

#include <avr/io.h>
#include "adc_driver.h"
#include "avr_compiler.h"
#include "UART-h.h"
#include "PWM-h.h"



#define ADCPORT PORTA
#define LEDPORT PORTB
#define ADCL1 1
#define ADCL2 2				//Definición de mascaras 
#define ES1 3
#define ES2 4
#define BUSCAR 1

uint8_t Adc_alterno [256] =
{0,0,0,0,0,0,0,0,0,0,0,0,1.1,1.1,1.1,1.1,1.5,1.7,1.9,2.3,2.35,2.4,2.6,2.7,3.2,3.3,3.4,3.5,3.7,3.9,
4,4.2,4.4,4.6,4.8,4.9,5,5.4,5.6,5.7,5.9,6,6.3,6.5,6.9,7,7.3,7.5,7.6,7.7,7.9,8,8.2,8.4,8.6,8.8,9,
9.2,9.4,9.5,9.7,9.9,10.1,10.3,10.5,10.7,10.9,11.1,11.3,11.4,11.5,11.7,11.9,12,12.2,12.4,12.6,12.8,
13,13.2,13.4,13.5,13.6,13.7,13.8,13.9,13.9,14,14,14,14.1,14.1,14.1,14.2,14.2,14.2,14.2,14.3,14.3,
14.3,14.4,14.4,14.4,14.5,14.5,14.5,14.5,14.5,14.6,14.6,14.6,14.6,14.6,14.6,14.7,14.7,14.7,14.7,
14.7,14.7,14.8,14.8,14.8,14.8,14.8,14.8,14.8,14.8,14.9,14.9,15,52,52,51,51,51,50,50,50,49,49,49,
48,48,48,47,47,47,46,46,46,45,45,44,44,43,43,43,42,42,42,41,41,41,41,40,40,39,39,38,38,38,38,37,
37,36,36,36,35,35,35,34,34,34,33,33,32,32,31,31,31,30,30,29,29,29,28,28,27,27,27,26,26,25,25,25,
24,24,23,23,22,22,22,21,21,20,20,19,19,18,18,17,17,16,16,15,14,14,13,13,12,11,11,10,10,9,8,8,7,6,
6,5,4,3,3,2,1,0,0,0,0,0,0,0,0,0};



void ReadADC(int *estado, uint8_t *Fotocelda1, uint8_t *Fotocelda2, uint8_t *Panel, uint8_t *Bateria );
void RevisarSerial(char *Buff, uint8_t *Panel, uint8_t *Bateria);

int main(void)
{
	
    ADCPORT.DIRCLR=0b11111111; //Definición de los pines del puerto A 
	
	
	uint8_t Fotocelda1=0, Fotocelda2=0, Panel=0, Bateria=0;
	int estado=ADCL1, sistema=BUSCAR;							//Declaración de variables 
	float Limite1=10.5, Limite2=4.5;
	char Buff=0;
	float Porcen=0; 
	
	initUART();	
	initPWM(50);
	Endis('A',ENABLEPWM);
	
	
	
	ADC_CalibrationValues_Load(&ADCA);
	ADCA.CTRLB = 0x04; //Configurar modo "no limit current, unsigned, no freerun, 8 bit resolution2"
	ADCA.REFCTRL= 0x00;//Configurar la referencia de voltaje interno como 1.
	ADCA.SAMPCTRL=0x3F;//Configurar el máximmo tiempo de muestreo
	ADCA.CH0.CTRL=0x01;//Configurar modo single ended y gain factor
	ADCA.CH0.MUXCTRL=0x38;//Habilitar el pin 7 para leer
	ADCA.CTRLA|=0x01;// Habilitar el ADC
	
	ADCA.CH0.CTRL|=0x80;//Iniciar la conversion de ADC
	

	

    while (1) 
    {
		SetDC('A',Porcen);	//Poner en el canal A del PWM el pulso para girar el servomotor.
		if(Porcen>Limite1){
			Porcen=Limite1;	//Fijar límites de giro del servomotor
		}
		if(Porcen<Limite2){
			Porcen=Limite2;	//Fijar límites de giro del servomotor
		}
		
			switch(sistema){
		
				case BUSCAR :	ReadADC(&estado,&Fotocelda1,&Fotocelda2,&Panel,&Bateria);	//Leer valores de cada canal de ADC
								RevisarSerial(&Buff,&Panel,&Bateria);
				
				
								
								
								if(Fotocelda1>Fotocelda2){
									if((Fotocelda1-Fotocelda2)>1){
									Porcen=Porcen+0.005;	//Girar el servomotor
									}
									}
								if (Fotocelda2>Fotocelda1){
									if((Fotocelda2-Fotocelda1)>1){
										Porcen=Porcen-0.005;	//Girar el servomotor
									}
									}
								
								
			
			}
	}
	
		
		
	}
		
			
	
void ReadADC(int *estado, uint8_t *Fotocelda1, uint8_t *Fotocelda2, uint8_t *Panel, uint8_t *Bateria ){
		
		switch(*estado){
			
			case ADCL1:	
							if((ADCA.CH0.INTFLAGS & 0x01) !=0){
				
								ADCA.CH0.INTFLAGS|=0x01;	//Restablecer la bandera de conversión
								*Fotocelda1=ADCA.CH0.RESL;	//Guardar valor obtenido en la variable correspondiente
								//sendChar(*Fotocelda1);
								ADCA.CH0.MUXCTRL=0x30;		//Cambio de canal de lectura al pin 6 
								ADCA.CH0.CTRL|=0x80;		//Iniciar la conversión
								*estado=ADCL2;				//Cambio de estado
								}
			
			case ADCL2:		if((ADCA.CH0.INTFLAGS & 0x01) !=0){
				
								ADCA.CH0.INTFLAGS|=0x01;	//Restablecer la bandera de conversión
								*Fotocelda2=ADCA.CH0.RESL;	//Guardar valor obtenido en la variable correspondiente
								//sendChar(*Fotocelda2);
								ADCA.CH0.MUXCTRL=0x28;		//Cambio del canal de lectura al pin 5
								ADCA.CH0.CTRL|=0x80;		//Iniciar la conversión
								*estado=ES1;				//Cambio de estado
								}								
				
			case ES1 :		if((ADCA.CH0.INTFLAGS & 0x01) != 0){
				
								ADCA.CH0.INTFLAGS |= 0x01;	//Restablecer la bandera de conversión
								*Panel=ADCA.CH0.RESL;		//Guardar valor obtenido en la variable correspondiente
								//sendChar((Adc_alterno[*Panel])+2);	//Enviar por serial el voltaje del panel.
								ADCA.CH0.MUXCTRL= 0x18;		//Cambio el canal de lectura al pin 3
								ADCA.CH0.CTRL|=0x80;		//Iniciar la conversión 
								*estado=ES2;				//Cambiar de estado
			}
			
			
			case ES2 :		if((ADCA.CH0.INTFLAGS & 0x01) != 0){
				
								ADCA.CH0.INTFLAGS |= 0x01;	//Restablecer la bandera de conversión
								*Bateria=ADCA.CH0.RESL;		//Guardar valor obtenido en la variable correspondiente
								//sendChar((Adc_alterno[*Bateria])+2);	//Enviar por serial el voltaje de la bateria.
								ADCA.CH0.MUXCTRL= 0x38;		//Cambio el canal de lectura al pin 7
								ADCA.CH0.CTRL|=0x80;		//Iniciar la conversión
								*estado=ADCL1;				//Cambio de estado
								}
		
				
			}
			
	
}

void RevisarSerial(char *Buff, uint8_t *Panel, uint8_t *Bateria){
	
	*Buff=usart_receiveByte(); //Funcion para recibir serial
	
	
	if(*Buff=='T'){
		sendChar(Adc_alterno[*Panel]+2);
		
		sendChar(Adc_alterno[*Bateria]+2);
	}
	
}


