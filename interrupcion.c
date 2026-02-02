
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


volatile bool emergency_active = false; // Bandera para el modo de emergencia (volatile es importante para variables compartidas entre ISR y main)
bool maquinaEncendida = false; // Variable para controlar el estado de la máquina por serial
char estado; // Almacena el dato recibido por el puerto serial
char buffer[20]; //Cadena para almacenar los valores de color en formato de texto que se enviarán al LCD.
unsigned int vr, vg, vb; //Variables enteras para guardar los valores de los canales de color rojo, verde y azul que el sensor de color detecta.



// --- Rutina de Servicio de Interrupción (ISR) ---
void __interrupt() MiISR() // Función de interrupción general
{
    // --- Interrupción por Paro de Emergencia (RB0 - INT0) ---
    if(INTCONbits.INT0IF == 1) // Verifica si ha ocurrido el evento INT0
    {
        // Activa el modo de emergencia
        emergency_active = true;

        // Muestra mensaje y enciende LEDs de emergencia
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Paro Emergencia");
        
        LATBbits.LB7 = 1; // LED de emergencia principal
        LATAbits.LA4 = 1; // Otros LEDs de indicación
        LATAbits.LA5 = 1;
        LATAbits.LA3 = 1;
       LATAbits.LA2 = 1;
        

        INTCONbits.INT0IF = 0; // Limpia el flag de la interrupción INT0
    }

    // --- Interrupción por Reinicio (RB1 - INT1) ---
    if(INTCON3bits.INT1IF == 1) // Verifica si ha ocurrido el evento INT1
    {
        // Desactiva el modo de emergencia
        emergency_active = false;
        
        // Apaga todos los LEDs de emergencia inmediatamente
        LATBbits.LB7 = 0; // Apaga el LED de emergencia principal (corregido de LC7 a LB7)
        LATAbits.LA4 = 0;
        LATAbits.LA5 = 0;
        LATAbits.LA3 = 0;
        LATAbits.LA2 = 0;
        

        // Opcional: Limpiar LCD y mostrar mensaje de reinicio
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Sistema Reiniciado");
        __delay_ms(500); // Pequeño retraso para que se vea el mensaje

        INTCON3bits.INT1IF = 0; // Limpia el flag de la interrupción INT1
    }
}