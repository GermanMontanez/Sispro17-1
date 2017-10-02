#include "Funciones.h"

//----------------------------------------------------
void initPWM(void){
	
	PORTE.DIRSET = 0x0F;//Enable pin 0,1 y 2 as Outputs
	PORTE.OUTSET = 0x0F; //set pin 0,1 y 2 ZERO.(Segun martha)
	TCE0.CTRLB |=0x03; // set singelslope
	TCE0.CTRLA |=0x01; // set prescaler div 1
	
}
//-------------------------------------------------
int initSetper(int numHerz , int escaler){
	int per=0;
	per = (2000000/escaler/numHerz);
	TCE0.PER = per;
	return per;
}

//--------------------------------------------------
void initDutyCycle(char chanel, int per, int util){
	
	if (chanel == 'A')     { TCE0.CCABUF = (per/100)*util;  }
	else if (chanel == 'B'){ TCE0.CCBBUF = (per/100)*util;  }
	else if (chanel == 'C'){ TCE0.CCCBUF = (per/100)*util;  }
	else if (chanel == 'D'){ TCE0.CCDBUF = (per/100)*util;  }
}

//-------------------------------------------------

#define MOTOR_1  0b00010000
#define MOTOR_2  0b00100000
#define MOTOR_3  0b01000000
#define MOTOR_4  0x80

void initEnableDisable(char chanel , int OnOff){
	
	if (chanel == 'A'){
		if ( OnOff == 1 ){ TCE0.CTRLB |= MOTOR_1; }
		else if( OnOff == 0 ){ TCE0.CTRLB &= ~MOTOR_1; }
	}
	if (chanel == 'B'){
		if ( OnOff == 1 ) { TCE0.CTRLB |= MOTOR_2 ; }
		else if ( OnOff == 0 ) { TCE0.CTRLB &= ~MOTOR_2 ; }
	}
	if (chanel == 'C'){
		if ( OnOff == 1 ) { TCE0.CTRLB |= MOTOR_3 ; }
		else if ( OnOff == 0 ) { TCE0.CTRLB &= ~MOTOR_3 ; }
	}
	if (chanel == 'D'){
		if ( OnOff == 1 ) { TCE0.CTRLB |= MOTOR_4; }
		else if ( OnOff == 0 ) { TCE0.CTRLB &= ~MOTOR_4; }
	}
}




