#define _XTAL_FREQ 8000000
#include <xc.h>
#include <stdio.h>
#include <pic18f4550.h>
#include "lcd.h"                                    // Libreria de la pantalla lcd
#include "tcs230.h"                                 // Libreria del sensor de color TCS230
#include "configuracion.h"
#include "servoo.h"                                 //Libreria para los servos
#include <math.h>
#include <stdbool.h>             // Incluir para usar el tipo 'bool' interrupciones
#include "uart.h"
#include <stdint.h>  /*Libreria par alos enteros no estandar*/
#include <stdlib.h>
#include <string.h>


/* ???????????? Credenciales ???????????? */
const char USER_OK[]  = "72421037";
const char PASS_OK[]  = "1234";
const char USER_OK1[] = "71819898";
const char PASS_OK1[] = "1234";
const char USER_OK2[] = "74651362";
const char PASS_OK2[] = "1234";

/* ???????????? Buffers UART ???????????? */
#define MAX_LEN 16
static char userBuf[MAX_LEN + 1]; // Buffer para nombre de usuario
static char passBuf[MAX_LEN + 1];

/* ???????????? Flags globales ???????????? */
volatile bool emergency_active = false;  // // Se activa si ocurre una interrupción externa (INT0 o INT1)
bool maquinaEncendida = false;           // 
bool logged_in = false;                  // True tras credenciales correctas

/* ???????????? Prototipos ???????????? */
static void ports_init(void);
static void getString(char *buf, uint8_t maxLen, uint8_t echo);
static void login_loop(void);
static void clearBuffers(void);

/* ???????????? Variables del sorter ???????????? */
char  estado;            // Comandos por UART ('a', 'b', etc.)
char  buffer[20];        // Para sprintf hacia LCD
unsigned int vr, vg, vb; // Valores RGB del sensor



void main()
{  
    
    
    // --- Configuración de Pines y Periféricos ---
    TRISBbits.RB0 = 1; // Pin RB0 como entrada para INT0
    TRISBbits.RB1 = 1; // Pin RB1 como entrada para INT1

    // Configuración de Interrupciones Externas
    INTCONbits.INT0IE = 1; // Habilita la interrupción externa en RB0 (INT0)
    INTCONbits.INT0IF = 0; // Limpia el flag de la interrupción INT0
    INTCON2bits.INTEDG0 = 1; // Flanco de subida para INT0 (RB0)

    INTCON3bits.INT1IE = 1; // Habilita la interrupción externa en RB1 (INT1)
    INTCON3bits.INT1IF = 0; // Limpia el flag de la interrupción INT1
    INTCON2bits.INTEDG1 = 1; // Flanco de subida para INT1 (RB1)

    INTCONbits.GIE = 1; // Habilita las interrupciones globales
    INTCONbits.PEIE = 1; // Habilita las interrupciones de periféricos (necesario para INT1 en algunos PICs)

   
    int Period;
    Serial_begin(9600);
    Period = setPeriodTo(50);                       /* 50Hz PWM frequency (shared by both) */
    OSCCON  = 0x72;                                 // Configura la frecuencia del oscilador interno a 8 MHz
    ADCON1bits.PCFG = 0x0F;                         // Coloca todos los pines como digitales
    PWM_Init();                                     /* Initialize PWM for both CCP1 and CCP2 */
    Lcd_Init();                                     // Inicializa la pantalla LCD  
    TCS230_Init();                                  // Inicializa el sensor de color TCS230 
    TRISA         = 0x00;   

    /* --- Bucle de autenticación --- */
    while (!logged_in) login_loop(); //Bloqueamos el programa hasta que se ingrese correctamente 

    /* --- Bienvenida --- */
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Bienvenido!");
    __delay_ms(1500);
    Lcd_Clear();
    
    
    while(1)
        
    {
        // Si el modo de emergencia está activo, la máquina no realiza sus operaciones normales
        if (emergency_active) {
            // En este bloque, la máquina está en "paro de emergencia".

            __delay_ms(10); // Pequeño retraso para evitar un bucle muy rápido y consumir CPU innecesariamente
        } else {
            // --- Operación Normal de la Máquina ---
            // Verifica si hay datos disponibles para leer del puerto serial
    
if (Serial_available()) 
    { // Verifica si hay datos disponibles para leer
            estado = Serial_read();
            
    if (estado == 'a') {
        
    maquinaEncendida = true;
    Lcd_Clear();
    }       

}

if (maquinaEncendida){
        
    Period = setPeriodTo(50);   /* 50Hz PWM frequency (shared by both) */ 
    

     
    vr = TCS230_Get_Value(CHANNEL_R);           // Lee el valor del canal rojo
    vg = TCS230_Get_Value(CHANNEL_G);           // Lee el valor del canal verde
    vb = TCS230_Get_Value(CHANNEL_B);           // Lee el valor del canal azul

  

        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        sprintf(buffer, "R: %u ", vr);
        Lcd_Write_String(buffer);                   // Imprime el valor del canal rojo
        Lcd_Set_Cursor(2,1);
        sprintf(buffer, "G: %u ", vg);
        Lcd_Write_String(buffer);                   // Imprime el valor del canal verde
        Lcd_Set_Cursor(1,9);
        sprintf(buffer, "B: %u ", vb);
        Lcd_Write_String(buffer);                   // Imprime el valor del canal azul

        __delay_ms(100);


         if((vr>27.80 && vr<40) && (vg>48 && vg<58.5) && (vb>43.9 && vb<55.8))
     {
            

            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Color rojo");
           
            
            T1CON = 0x00;
        
          // Control del Servomotor 2 (conectado a RC1)
        SetDutyCycleTo2(10, Period);   /* 12% duty cycle 0 grados */
        delay(500); // Pequeño retraso para observar el movimiento
 
        // Control del Servomotor 1 (conectado a RC2)
        
        SetDutyCycleTo1(5, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
        
        
       // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(3, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento

  
        
    // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(2, Period);    /* 3% duty cycle 180 grados */
        delay(500); // Pequeño retraso para observar el movimiento

        T1CON = 0xF8;
        T1CONbits.TMR1ON = 1;

        
        }
        else if((vr>20 && vr<30) && (vg>24 && vg<34) && (vb>35 && vb<45))
        {

            
             Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Color amarillo");           
            
          
             T1CON = 0x00;
             
             
             
        // Control del Servomotor 2 (conectado a RC1)
        SetDutyCycleTo2(8, Period);    /* 3% duty cycle 45 grados */
        delay(500);

        // Control del Servomotor 1 (conectado a RC2)       
        SetDutyCycleTo1(5, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
 
       // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(3, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento

        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(2, Period);    /* 3% duty cycle 180 grados */
        delay(500); // Pequeño retraso para observar el movimiento
  
        T1CON = 0xF8;
        T1CONbits.TMR1ON = 1;
   
        }
        else if((vr>40 && vr<53) && (vg>38 && vg<50) && (vb>48 && vb<60))
        {

            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Color verde");           
            
        
         
        T1CON = 0x00;
    
        
        // Control del Servomotor 2 (conectado a RC1)
        SetDutyCycleTo2(6.5, Period);    /* 7% duty cycle  90 grados -7.6 */
        delay(500);   
     
        // Control del Servomotor 1 (conectado a RC2)      
        SetDutyCycleTo1(5, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
 
       // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(3, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento

        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(2, Period);    /* 3% duty cycle 180 grados */
        delay(500); // Pequeño retraso para observar el movimiento
        
        T1CON = 0xF8;
        T1CONbits.TMR1ON = 1;
        
        }
                else if((vr>20 && vr<22) && (vg> 52 && vg<58) && (vb>51 && vb<57))
        {
            

            
             Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Color Morado");           
             
             T1CON = 0x00;
             
        // Control del Servomotor 2 (conectado a RC1)
        SetDutyCycleTo2(5, Period);    /* 3% duty cycle 135 grados */
        delay(500);  
    
        // Control del Servomotor 1 (conectado a RC2)       
        SetDutyCycleTo1(5, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
        
       // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(3, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento

        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(2, Period);    /* 3% duty cycle 180 grados */
        delay(500); // Pequeño retraso para observar el movimiento
        
    
        T1CON = 0xF8;
        T1CONbits.TMR1ON = 1;   
        
        }
                else if((vr>22.50 && vr<32) && (vg>38 && vg<48) && (vb>37 && vb<49))
        {
            

            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Color Rosa");           
           
  
             T1CON = 0x00;
             
             
        // Control del Servomotor 2 (conectado a RC1)
        SetDutyCycleTo2(3.5, Period);    /* 3% duty cycle 180 grados */
        delay(500);  
    
        // Control del Servomotor 1 (conectado a RC2)
        
        SetDutyCycleTo1(5, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
        
        
       // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(3, Period);    /* 3% duty cycle  grados */
        delay(1000); // Pequeño retraso para observar el movimiento
        
        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(2, Period);    /* 3% duty cycle 180 grados */
        delay(500); // Pequeño retraso para observar el movimiento       
             
             
        T1CON = 0xF8;
        T1CONbits.TMR1ON = 1;
        }
        
        
        else                                        
        {
                 

        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(12.3, Period);   /* 12% duty cycle 0 grados */
        delay(1000); // Pequeño retraso para observar el movimiento      
        
        // Control del Servomotor 1 (conectado a RC2)
        SetDutyCycleTo1(7, Period);    /* 7% duty cycle  90 grados -7.6 */
        delay(500); // Pequeño retraso para observar el movimiento

}
}
}
}
}


/* **************************** LOGIN LOOP **************************** */
static void login_loop(void)
{
    clearBuffers();

    /* --- Usuario --- */
    Lcd_Clear(); Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Usuario:");
    Serial_print("Usuario:\r\n");
    getString(userBuf, MAX_LEN, 1);      // eco visible

    /* --- Password --- */
    Lcd_Clear(); Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Contrasena:");
    Serial_print("Contrasena:\r\n");
    getString(passBuf, MAX_LEN, 0);      // eco oculto

    /* --- Verificación --- */
    Lcd_Clear(); Lcd_Set_Cursor(2,1);
    if ( (!strcmp(userBuf,USER_OK)  && !strcmp(passBuf,PASS_OK))  ||
         (!strcmp(userBuf,USER_OK1) && !strcmp(passBuf,PASS_OK1))||
         (!strcmp(userBuf,USER_OK2) && !strcmp(passBuf,PASS_OK2)) )
    {
        Lcd_Write_String("Acceso correcto");
        Serial_println("Acceso correcto\r\n");
        logged_in = true;
    } else {
        Lcd_Write_String("Acceso denegado");
        Serial_println("Acceso denegado\r\n");
        __delay_ms(2000);
    }
}


/* ****************************** Utilidades ********************** */
static void getString(char *buf, uint8_t maxLen, uint8_t echo)
{
    uint8_t i = 0; char c;
    while (1)
    {
        c = Serial_read();                 // bloqueante
        if (c=='o' || c=='o') break;     // Enter
        if (i < maxLen) {
            buf[i++] = c; buf[i] = '\0';
            Lcd_Write_Char(echo ? c : '*');
        }
    }
}

static void clearBuffers(void)
{
    memset(userBuf,  0, sizeof(userBuf));
    memset(passBuf,  0, sizeof(passBuf));
}

static void ports_init(void)
{
    ADCON1 = 0x0F;  // todos digitales
    CMCON  = 0x07;  // comparadores off
    RS_DIR = 0; EN_DIR = 0;
    D4_DIR = D5_DIR = D6_DIR = D7_DIR = 0;
}

/*=================================================================*/