#ifndef SERVOO_H
#define SERVOO_H
#define _XTAL_FREQ 8000000
#include <stdint.h>


#include <pic18f4550.h>
#include <stdio.h>
#include <math.h>
#include "configuracion.h" // Assuming config.h is a project-specific configuration file
#include <xc.h>

// --- Defines for PWM Configuration ---
#define MINTHR                      8000
#define RESOLUTION                  488

#define InternalOsc_8MHz            8000000
#define InternalOsc_4MHz            4000000
#define InternalOsc_2MHz            2000000
#define InternalOsc_1MHz            1000000
#define InternalOsc_500KHz          500000
#define InternalOsc_250KHz          250000
#define InternalOsc_125KHz          125000
#define InternalOsc_31KHz           31000

#define Timer2Prescale_1            1
#define Timer2Prescale_4            4
#define Timer2Prescale_16           16

// --- Function Prototypes ---
void PWM_Init(void);
int setPeriodTo(unsigned long FPWM);
void SetDutyCycleTo1(float Duty_cycle, int Period); // Para el primer servomotor (CCP1)
void SetDutyCycleTo2(float Duty_cycle, int Period); // Para el segundo servomotor (CCP2)
void delay(unsigned int val);



#endif  // SERVO_H