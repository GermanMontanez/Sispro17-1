#include <avr/io.h>
#include "MQ2.H"
#include "MQ2.c"
#include "adc_driver.h"
#include "adc_driver.c"
#include "uart.c"
#include "uart.h"
#include "LASER.h"
#include "LASER.c"

#define F_CPU 2000000UL

#define ADCPORT PORTA
#define USARTPORT PORTC
#define PWM PORTE
#define ESPERA 0
#define LEER 1
#define ACTIVAR 2
#define ENVIAR 3

char gas=255,luz=500;



int main(void)
{
	
	//USART.........................................................................................................................
	initUART();
	//PWM...........................................................................................................................
   	PWM.DIRSET = 0x1F; //define los pines como salidas
   	PWM.OUTSET = 0x1F; //pone en 0 los pines
   	
   	TCE0.CTRLB |= 0x03; //selecciona el modo SINGLESLOPE para la generacion de la señal
   	TCE0.CTRLB |= 0xF0; //habilita los canales al poner en 1 el bit CCxEN correspondiente
   	
   	TCE0.CTRLA |= 0x01; //define la preescalizacion del conteo a 1
   	TCE0.PER = 5000; //periodo= frecuenciaCPU + preescalizacion + frecuenciaPWM; frecuenciaPWM= 500hz
   	
   	
   	
   	TCE0.CCABUF= 2500; //define el ancho de pulso del canal A. CCABUF=PERIODO*CICLO_UTIL/100;ciclo_util=75%
   	TCE0.CCBBUF= 2500; //define el ancho del pulso del canal B.
   	TCE0.CCCBUF= 2500; //define el ancho del pulso del canal C.
   	TCE0.CCDBUF= 2500; // define el ancho del pulso del canal D.
   	
   	PORTE.DIR &=~ (0x10); // el pin se declara como salida
   	PORTE.DIR |=0x10;
   	PORTE.OUT |=(0x10);
	  // PORTE.OUT&=(0x10);
	 //ADC.............................................................................................................................
	
	 // init_laser();
	  ADCPORT.DIRCLR = 0x0F;//
	  ADC_CalibrationValues_Load(&ADCA);
	  ADCA.CTRLB = 0x04;
	  ADCA.REFCTRL = 0x00;
	  ADCA.SAMPCTRL = 0x3F;
	  ADCA.CH0.CTRL=0x01 ;
	  ADCA.CH0.MUXCTRL= 0x10;
	  ADCA.CTRLA |=0x01;
	  ADCA.CH0.CTRL|=0x80;
	 
	
	 
	   
	//TIMER...............................................................................................................................................
	TCC0.PER = 0x3D09; //El periodo se establece en 0x3D09 en decimal:15625
	TCC0.CTRLA = 0x05;  // Selección de preescalización : DIV64
	   	
		   
	int estado=LEER;
	int gas_activado=0,laser_activado=0;	  
	int cont_timer=0;
	int MQ2_flag=1;
	int boton0;	 
	int alarma_flag=0;  
    char res;
	char reci;
PORTE.OUT&=0x10;
//PORTE.OUT&=~(0x10);
	
	while (1) 
    {
if(laser_activado==1){
	PORTE.OUT&=0x10;
}
	

//sendChar(luz);

	boton0=PORTB.IN & 0x08;
	
	
	switch(estado){
		
		case ESPERA:
					
	if(boton0!=0 && alarma_flag==0){
			alarma_flag=1;
			estado=LEER;
		}

					

					if(alarma_flag==0){
						PORTE.OUTSET=0x40;
						TCE0.CCABUF=00;
						TCE0.CCBBUF=00;
						TCE0.CCCBUF=00;
						TCE0.CCDBUF=000;
						
						estado=ESPERA;
					}	
					break;
		case LEER: 
			if(MQ2_flag==1){
				if((ADCA.CH0.INTFLAGS & 0x01) !=0) //Asks if the ADC conversion is complete
				{
					ADCA.CH0.INTFLAGS |= 0x01; //Clears the ADC flag
					MQ2_result( &gas);//gets the ADC conversion result and changes the DC
					ADCA.CH0.MUXCTRL=0x00;
					ADCA.CH0.CTRL|=0x80; //Starts an ADC conversion
					MQ2_flag=0;
				}
			}
			else{
				
				if((ADCA.CH0.INTFLAGS & 0x01) !=0) //Asks if the ADC conversion is complete
				{
					ADCA.CH0.INTFLAGS |= 0x01; //Clears the ADC flag
					LASER_result( &luz);//gets the ADC conversion result and changes the DC
					ADCA.CH0.MUXCTRL=0x10;
					ADCA.CH0.CTRL|=0x80; //Starts an ADC conversion
					MQ2_flag=1;
				}

			}
		if(gas==3){
				gas_activado=1;
			
			}
			else{
				gas_activado=0;
			}
			if(luz==2){
				laser_activado=1;
			}
			else{
				laser_activado=0;
			}
			if(gas_activado==1 || laser_activado==1){
			estado=ACTIVAR;
			}
			else{
				estado=LEER;
			}
			
				PORTE.OUTSET=0x40;
				TCE0.CCABUF=00;
				TCE0.CCBBUF=2500;
				TCE0.CCCBUF=00;
				TCE0.CCDBUF=000;
				PORTE.OUT&=0x10;
				//PORTE.OUT&=~(0x10);

		break;
		case ACTIVAR:
		
			if(gas_activado==1 && laser_activado == 0){
					PORTE.OUTSET=0x40;
					TCE0.CCABUF=2500;
					TCE0.CCBBUF=2500;
					TCE0.CCCBUF=00;
					TCE0.CCDBUF=000;
					//PORTE.OUT&=0x10;
					PORTE.OUT&=~(0x10);
					
			}
			if(gas_activado==1 && laser_activado == 1){
				
				PORTE.OUTSET=0x40;
				TCE0.CCABUF=2500;
				TCE0.CCBBUF=00;
				TCE0.CCCBUF=00;
				TCE0.CCDBUF=3000;
				PORTE.OUT&=0x10;
				//PORTE.OUT&=~(0x10);
			}
			
			if(gas_activado==0 && laser_activado==1){
				
				PORTE.OUTSET=0x40;
				TCE0.CCABUF=2500;
				TCE0.CCBBUF=00;
				TCE0.CCCBUF=00;
				TCE0.CCDBUF=3000;
				//PORTE.OUT&=0x10;
				PORTE.OUT&=~(0x10);
			}
			
		
			
				estado=ENVIAR;
			
			
			break;
			
		case ENVIAR: 
		 res=(laser_activado*10)+gas_activado;
			//sendChar(res);
			
				if(boton0!=0 && alarma_flag==1){
					
					alarma_flag=0;
					laser_activado=0;
					gas_activado=0;
					gas=255;
					luz=2;
					estado=ESPERA;
					
				}
				
				
			break;
	default: estado=LEER;
	break;
	}	
	

	if(usart_receiveByte()=='L'){
		sendChar(laser_activado|0b00110000);
		sendChar(gas_activado|0b00110000);
	}
	
	
	
    }
}

