/*#ifndef PWM_H_
#define PWM_H_
*/
#define ENABLEPWM 1
#define DISABLEPWM 0
#define PRESCALERPWM 1

#define BOTON1 0b00000001

void initPWM(int freq);
void SetPer(int freq);
void SetDC(char canal,float DC);
void Endis(char canal, char enable);