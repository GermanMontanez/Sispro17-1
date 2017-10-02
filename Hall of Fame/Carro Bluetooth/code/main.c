/*
 * PROYECTO FINAL.c
 *
 * Created: 11/04/2017 4:24:18 p. m.
 * Author : brayaN
 */ 
#include <avr/io.h>
# define F_CPU 2000000UL // define reloj
// librerias usadas
#include <util/delay.h>
#include "Funciones.c"
#include "adc_driver.h"
#include "adc_driver.c"
#include "uart.c"
#include "avr_compiler.h"

//Mascaras HC-SR04
#define ECHO 0x04
#define TRIGER 0x08

#define ADCPORT PORTB

//Mascaras para puente H
#define BRAZO_1  0x01
#define BRAZO_12 0x02
#define BRAZO_2  0x04
#define BRAZO_21 0x10

// Estados para el manejo de cada motor
typedef enum {
	UP, // moverse adelate
	DOWN, // moverse atras 
	TURN_LEFT, // giro a la izquierda
	TURN_RIGHT,// giro a la derecha
	NEUTRAL, // motores deshabilitados
	LATERALLEFT, // moverse lateral izquierdo
	LATERALRIGHT, // moverse lateral derecho
	PALA_UP, // mover pala hacia arriba
	PALA_DOWN, // mover pala hacia abajo
	AUTOMATIZACION // rutina para el robot
} DIRECTION_T;
 DIRECTION_T state = NEUTRAL; // inicializar variable tipo: DIRECTION_T
 
uint8_t pot_table[256] = 
{225,224,223,222,221,221,220,219,218,217,216,215,214,214,213,212,211,210,209,208,207,206,206,205,204,203,202,201,200,199,199,198,197,196,195
	,194,193,192,191,191,190,189,188,187,186,185,184,184,183,182,181,180,179,178,177,176,176,175,174,173,172,171,170,169,169,168,167,166,165,164
	,163,162,161,161,160,159,158,157,156,155,154,154,153,152,151,150,149,148,147,146,146,145,144,143,142,141,140,139,139,138,137,136,135,134,133
	,132,131,131,130,129,128,127,126,125,124,124,123,122,121,120,119,118,117,116,116,115,114,113,112,111,110,109,109,108,107,106,105,104,103,102
	,101,101,100,99,98,97,96,95,94,94,93,92,91,90,89,88,87,86,86,85,84,83,82,81,80,79,79,78,77,76,75,74,73,72,71,71,70,69,68,67,66,65,64,64,63,62
	,61,60,59,58,57,56,56,55,54,53,52,51,50,49,49,48,47,46,45,44,43,42,41,41,40,39,38,37,36,35,34,34,33,32,31,30,29,28,27,26,26,25,24,23,22,21,20
,19,19,18,17,16,15,14,13,12,11,11,10,9,8,7,6,5,4,4,3,2,1,0} ;//Function table for the potentiometer sensor
 
 //funciones
 
 /* ----------------------- Prototype of functions ---------------------- */
 void ADC_result(uint8_t *res); //Convierte el ADC a ciclo util
 void PWM_MOTOR(int per,int utilA,int utilB,int utilC,int utilD); //asigna PWM a los motores de la base
 // funciones para pasar float a string
 void reverse(char *str, int len); 
 int intToStr(int x, char str[], int d);
 void ftoa(float n, char *res, int afterpoint);
 // salto de linea 
 void Salto_de_linea(void);
 // funcion´para leer sensor ultrasonido
 int read_ultrasonic(void);
 
 //Variables HC-SR04
 char cont_hc[5]; int cont_int=0;

/* ---------------------------- Main ----------------------------------- */
 
int main (void)
{	
	//definicon de variables
	int numHerz=500; // frecuencia PWM
	int escaler=1; // prescalizacion
	char chanel1='A'; char chanel2='B'; char chanel3= 'C'; char chanel4= 'D';  // definir nombre de cada canal PWM
	int On=1; // activar canal
	int per = 0; // inicializar per
	int utilA = 0; int utilB = 0; int utilC = 0; int utilD = 0; // inicializaion ciclo util
	//--------------------------------------
	initPWM(); // iniciar PWM
	initUART(); // iniciar UART
	
	// Activar canales
	per = initSetper(numHerz,escaler);
	initEnableDisable(chanel1,On);
	initEnableDisable(chanel2,On);
	initEnableDisable(chanel3,On);
	initEnableDisable(chanel4,On);
	
	// declarar salidas y entradas 
	PORTB.DIR |= BRAZO_1;  PORTB.OUTCLR = BRAZO_1;
	PORTB.DIR |= BRAZO_12; PORTB.OUTCLR = BRAZO_12;
	PORTB.DIR |= BRAZO_2;  PORTB.OUTCLR = BRAZO_2;
	PORTB.DIR |= BRAZO_21; PORTB.OUTCLR = BRAZO_21;
	PORTA.DIRCLR = ECHO;
	PORTA.DIRSET = TRIGER;
	PORTA.DIRSET = 0x04;
	 
	char estado;
	//Variables HC-SR04
	float tiempo;
	float distancia=0;
	char Dis[5];
	char t[20];
	int limite=350; // equivale a 27 cm
	char Pot[5];
	
	
	//---------------------------------------Implementacion del ADC--------------------------------------------------------------
	/* ------------------ variable definition ------------------------ */
	uint8_t potentiometer = 0;

	//IOPORTS
	ADCPORT.DIRCLR = 0x20; //Pin6 del Puerto B como entrada 
	
	//ADC
	ADC_CalibrationValues_Load(&ADCB); //Moves calibration values from memory to CALL/CALH.
	ADCB.CTRLB = 0x04; //no limit current, unsigned, no freerun, 8 bit resolution
	ADCB.REFCTRL = 0x00; //internal reference voltage of 1 volt
	ADCB.SAMPCTRL = 0x3F; //Configurates the maximum sample time
	ADCB.CH0.CTRL=0x01 ; //single ended, gain factor x1
	ADCB.CH0.MUXCTRL= 0x01; //Selects pin 0 of PORTB as the ADC input (Potentiometer)
	ADCB.CTRLA |= 0x01; //Enables ADC
	ADCB.CH0.CTRL|=0x80; //Starts an ADC conversion
	
	while(1){
	//----------------leer potenci?metro-----------------------------
	if((ADCB.CH0.INTFLAGS & 0x01) !=0) //Asks if the ADC conversion is complete
	{
		ADCB.CH0.INTFLAGS |= 0x01; //Clears the ADC flag
		ADC_result( &potentiometer);//gets the ADC conversion result and changes the DC
		ADCB.CH0.CTRL|=0x80; //Starts an ADC conversion
		

	}  //if
	ftoa(potentiometer,&Pot,2); // pasar a string para visualizar en real term
	sendString(Pot); // enviar por serial el dato del potenciometro

	sendChar(0x0A); // salto de linea

		cont_int=read_ultrasonic(); // leer HC-SR04
		ftoa(cont_int,cont_hc,2); // pasar dato del sensor a string
		sendString(cont_hc); //enviar dato por serial 
		Salto_de_linea(); // salto de linea
		
	     // Control de estados
	     estado = usart_receiveByte(); // leer serial
		 if(estado == 'u'){state = NEUTRAL;} // motores desabilitados
         else if(estado == '1'){state = LATERALLEFT;} // moverse lateral izquierdo
	     else if(estado == '2'){state = LATERALRIGHT;} // moverse lateral derecho 
		 else if(estado == '3'){state = UP;} // moverse hacia adelante
		 else if(estado == '4'){state = TURN_LEFT; } // giro a la izquierda
		 else if(estado == '5'){state = DOWN;}// moverse hacia atras 
	     else if(estado == '6'){state = TURN_RIGHT;}// giro a la derecha
	     else if(estado == '7'){state = PALA_UP;}// mover pala hacia arriba
	     else if(estado == '8'){state = PALA_DOWN;} // mover pala hacia abajo
		 else if(estado == '9'){state = AUTOMATIZACION;} // rutina para el robot 
			 
	   if(cont_int<=limite){  // si llega al limite
			 utilA=52;  utilB=91;  utilC=91;  utilD=52; // pwm para hacer mover hacia adelante
			 PWM_MOTOR(per,utilA,utilB,utilC,utilD); //activa motores hacia adelante
			_delay_ms(1000); // duracion del movimiento
			state= NEUTRAL; // cambio de estado
		}	
	
	    // Maquina de estados
		switch(state){  
			 
			 case NEUTRAL: 
			    //carro quieto
				utilA=78;  utilB=78;  utilC=78;  utilD=78; 
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				PORTB.OUTCLR = BRAZO_1;
				PORTB.OUTCLR = BRAZO_12;
				PORTB.OUTCLR = BRAZO_2;
				PORTB.OUTCLR = BRAZO_21;
			 break;
			 
			 case UP:
			    // carro hacia adelante
			    utilA=55;  utilB=97;  utilC=97;  utilD=55;
			    PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case DOWN:
			      // carro hacia atras
				  utilA=91;  utilB=52;  utilC=52;  utilD=91;
				  PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case TURN_LEFT:
			      // carro girando a la izquierda
			      utilA=52;  utilB=52;  utilC=52;  utilD=52;
			      PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case TURN_RIGHT:
				   // carro girando a la derecha
			       utilA=91;  utilB=91;  utilC=91;  utilD=91;
			       PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case LATERALLEFT:
			 // carro desplazando a la izquierda
			 utilA=52;  utilB=52;  utilC=91;  utilD=91;
			 PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case LATERALRIGHT:
			 // carro desplazando a la derecha
			 utilA=91;  utilB=91;  utilC=52;  utilD=52;
			 PWM_MOTOR(per,utilA,utilB,utilC,utilD);
			 break;
			 
			 case PALA_UP:
			 // mover pala hacia arriba
			       PORTB.OUTSET = BRAZO_1;
			       PORTB.OUTCLR = BRAZO_12;
			       PORTB.OUTCLR = BRAZO_2;
			       PORTB.OUTSET = BRAZO_21;
			 break;
			
			case PALA_DOWN:
			// mover pala hacia abajo
				   PORTB.OUTCLR = BRAZO_1;
				   PORTB.OUTSET = BRAZO_12;
				   PORTB.OUTSET = BRAZO_2;
				   PORTB.OUTCLR = BRAZO_21;
			break;
			
			case AUTOMATIZACION:  //rutina
			//adelante
				utilA=52;  utilB=91;  utilC=91;  utilD=52;
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				_delay_ms(2000); 
				utilA=78;  utilB=78;  utilC=78;  utilD=78;
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				_delay_ms(500);
				
		   //sube pala	
				utilA=78;  utilB=78;  utilC=78;  utilD=78;
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				PORTB.OUTSET = BRAZO_1;
				PORTB.OUTCLR = BRAZO_12;
				PORTB.OUTCLR = BRAZO_2;
				PORTB.OUTSET = BRAZO_21;
				estado=3;
				_delay_ms(1000);
				PORTB.OUTCLR = BRAZO_1;
				PORTB.OUTCLR = BRAZO_12;
				PORTB.OUTCLR = BRAZO_2;
				PORTB.OUTCLR = BRAZO_21;
				
	      //baja pala
	            utilA=78;  utilB=78;  utilC=78;  utilD=78;
	            PWM_MOTOR(per,utilA,utilB,utilC,utilD);
	            PORTB.OUTCLR = BRAZO_1;
	            PORTB.OUTSET = BRAZO_12;
	            PORTB.OUTSET = BRAZO_2;
	            PORTB.OUTCLR = BRAZO_21;
				_delay_ms(1000);
				PORTB.OUTCLR = BRAZO_1;
				PORTB.OUTCLR = BRAZO_12;
				PORTB.OUTCLR = BRAZO_2;
				PORTB.OUTCLR = BRAZO_21;
		  //atras
				utilA=91;  utilB=52;  utilC=52;  utilD=91;
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				_delay_ms(2000);
		  //lateral izquierda
				utilA=52;  utilB=52;  utilC=91;  utilD=91;
				PWM_MOTOR(per,utilA,utilB,utilC,utilD);
				_delay_ms(1000);
							
			break;		
										
		}//Switch
		
	}  //while		
}/* Main */
	


void PWM_MOTOR(int per,int utilA,int utilB,int utilC,int utilD){
		// asignacion de pwm a cada motor de la base		initDutyCycle('A',per,utilA);    
	initDutyCycle('B',per,utilB);     
	initDutyCycle('C',per,utilC);     
	initDutyCycle('D',per,utilD);
		
}

//---------------FLOAT TO STRING-------------
// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}
void Salto_de_linea(void){
	sendChar(0x0A);sendChar(0x0D); // envia caracteres de para hacer un salto de linea
}


int read_ultrasonic(void){
	int count = 0; //contador para medir la distancia
    PORTA.OUTCLR = TRIGER;
    _delay_us(5); 
    PORTA.OUTSET = TRIGER; // activa el triger o microfono durante 10 us
	_delay_us(10);
	for (int i=0;i < 10000;i++){ // Checking port in loop 10,000 times

		if ((PORTA.IN & ECHO) != 0){	     // read PORTF pin 5 for echo pulse
			count++;	// count if still high count
		}
	}

	return count;
}
void ADC_result(uint8_t *res)
{  *res = pot_table[ADCB.CH0.RESL];  } // conversion ADC