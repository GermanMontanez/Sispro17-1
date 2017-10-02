#ifndef Funciones_H
#define Funciones_H

void initPWM();

int initSetper(int numHerz,int escaler);
void initEnableDisable(char chanel , int OnOff);
void initDutyCycle(char chanel, int per, int util);


#endif
