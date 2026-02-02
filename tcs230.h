/* #include "tcs230.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 13/05/2020
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 8000000

#define OUT_DIR TRISDbits.TRISD0
#define S2_DIR TRISDbits.TRISD1
#define S3_DIR TRISDbits.TRISD2

#define OUT_PIN PORTDbits.RD0
#define S2_PIN LATDbits.LD1
#define S3_PIN LATDbits.LD2

#define CHANNEL_R 0x01
#define CHANNEL_G 0x02
#define CHANNEL_B 0x03

void TCS230_Init(void);
unsigned int TCS230_Get_Value(short channel);
