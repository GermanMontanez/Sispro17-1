

#include <avr/io.h>
#include "PWM-h.h"

void initPWM(int freq){
	
	PORTE.DIRSET|=0x0F;				//Poner pines 0, 1 , 2 ,3 y 4 como Salidas
	PORTE.OUTSET|=0x0F;				//Poner pines 0,1 y 2 ,3, 4 en alto
	
	TCE0.CTRLA|=PRESCALERPWM;		//Poner prescaler en modo div 1
	TCE0.CTRLB|=0x03;				//Poner modo "singleslope"
	
	
	
	SetPer(freq);
	

	
	
}

void SetPer(int freq){
	
	//int aux=0;
	//aux=((2000000)/freq);
	TCE0.PER=((2000000)/freq); // De acuerdo a la formula PER = Frec-Reloj/Preescalizacion/Frec-PWM.
	
	
}

void SetDC(char canal,float DC){ //Duty Cycle
	
	if(canal=='A')						
	TCE0.CCABUF=(TCE0.PER)*(DC/100);	//Configurar ciclo util en el canal "A"
	if(canal=='B')
	TCE0.CCBBUF=((TCE0.PER)*(DC/100));	//Configurar ciclo util en el canal "B"
	if(canal=='C')
	TCE0.CCCBUF=(TCE0.PER)*(DC/100);	//Configurar ciclo util en el canal "C"
	if(canal=='D')
	TCE0.CCDBUF=(TCE0.PER)*(DC/100);	//Configurar ciclo util en el canal "D"
}

void Endis(char canal, char enable){
	
	if(canal=='A'&&enable==ENABLEPWM){		
		TCE0.CTRLB|=0x10;				//Activar el canal "A"
		}
	else if(canal=='A'&&enable==DISABLEPWM){
		TCE0.CTRLB&=~0x10;				//Desactivar el canal "A"
			}
	
	if(canal=='B'&&enable==ENABLEPWM){
		TCE0.CTRLB|=0x20;				//Activar el canal "B"
	}
	
	else if(canal=='B'&&enable==DISABLEPWM){
			TCE0.CTRLB&=~0x20;			//Desactivar el canal "B"
	}
			
	
	if(canal=='C'&&enable==ENABLEPWM){
		TCE0.CTRLB|=0x40;				//Activar el canal "C"
	}
	else if(canal=='C'&&enable==DISABLEPWM){
			TCE0.CTRLB&=~0x40;			//Desactivar el canal "C"
	}
	if(canal=='D'&&enable==ENABLEPWM){
		TCE0.CTRLB|=0x80;				//Activar el canal "D"
	}
	else if(canal=='D'&&enable==DISABLEPWM){
		TCE0.CTRLB&=~0x80;				//Desactivar el canal "A"
		
}
}