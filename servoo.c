#include "servoo.h"



// Note: _XTAL_FREQ is usually defined in a global configuration or main.c if specific to the main application.
// If it's universally needed by the library functions for __delay_ms, etc., you might include it here or in the header.
// For now, assuming it's handled elsewhere or not strictly needed for these functions' definitions.
// #define _XTAL_FREQ 8000000L // This can be here or in main.c, depending on usage.

void PWM_Init() /* Initialize PWM */
{
    TRISCbits.TRISC2 = 0; /* Set CCP1 pin (RC2) as output for PWM out */
    CCP1CON = 0x0C;       /* Set PWM mode for CCP1 */


    TRISCbits.TRISC1 = 0; /* Set CCP2 pin (RC1) as output for PWM out */
    CCP2CON = 0x0C;       /* Set PWM mode for CCP2 */
}

int setPeriodTo(unsigned long FPWM) /* Set period */
{
    int clockSelectBits, TimerPrescaleBits;
    int TimerPrescaleValue;
    float period;
    unsigned long FOSC, _resolution = RESOLUTION;

    if (FPWM < MINTHR)    {TimerPrescaleBits = 2; TimerPrescaleValue = Timer2Prescale_16;}
    else                  {TimerPrescaleBits = 0; TimerPrescaleValue = Timer2Prescale_1;}

    if (FPWM > _resolution)            {clockSelectBits = 7; FOSC = InternalOsc_8MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 6; FOSC = InternalOsc_4MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 5; FOSC = InternalOsc_2MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 4; FOSC = InternalOsc_1MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 3; FOSC = InternalOsc_500KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 2; FOSC = InternalOsc_250KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 1; FOSC = InternalOsc_125KHz;}
    else                                  {clockSelectBits = 0; FOSC = InternalOsc_31KHz;}

    period = ((float)FOSC / (4.0 * (float)TimerPrescaleValue * (float)FPWM)) - 1.0;
    period = round(period);

    OSCCON = ((clockSelectBits & 0x07) << 4) | 0x02;
    PR2 = (int)period;
    T2CON = TimerPrescaleBits;
    TMR2 = 0;
    T2CONbits.TMR2ON = 1; /* Turn ON Timer2 */
    return (int)period;
}

void SetDutyCycleTo1(float Duty_cycle, int Period) // Para CCP1
{
    int PWM10BitValue;

    PWM10BitValue = 4.0 * ((float)Period + 1.0) * (Duty_cycle/100.0);
    CCPR1L = (PWM10BitValue >> 2);
    CCP1CON = ((PWM10BitValue & 0x03) << 4) | 0x0C;
}

void SetDutyCycleTo2(float Duty_cycle, int Period) // Para CCP2
{
    int PWM10BitValue;

    PWM10BitValue = 4.0 * ((float)Period + 1.0) * (Duty_cycle/100.0);
    CCPR2L = (PWM10BitValue >> 2);
    CCP2CON = ((PWM10BitValue & 0x03) << 4) | 0x0C;
}


void delay(unsigned int val)
{
    unsigned int i,j;
    for(i=0;i<val;i++)
        for(j=0;j<10;j++);
}