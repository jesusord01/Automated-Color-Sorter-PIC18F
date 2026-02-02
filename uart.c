#include "uart.h"
/*Definicion de funciones*/
void Serial_begin(uint16_t baud){
   
   
    float temp;
    TRISC6=0;   /*Pin Tx salida*/
    TRISC7=1;   /*Pin Rx entrada*/
    temp = 12;
    SPBRG = (uint8_t)temp;
    TXSTA =0x20;
    RCSTA =0x90;       
}
uint8_t Serial_available(void){
    if(PIR1bits.RCIF == 1)
        return 1;
    else
        return 0;
}
void Serial_write(char caracter){
    while(TXIF == 0);
    TXREG = caracter;
}
char Serial_read(void){
    while(RCIF==0);     /*Esperar por la bandera de interupcion*/
    if(OERR){
        CREN = 0;
        NOP();
        CREN = 1;
    }
    return RCREG;
}
void Serial_print(char *s){
    while(*s!='\0'){
        Serial_write(*s);
        s++;
    }
}
void Serial_println(char *s){
    while(*s!='\0'){
        Serial_write(*s);
        s++;
    }
    Serial_write('\r');// '\r' o '\n' segun tipo de terminal
}