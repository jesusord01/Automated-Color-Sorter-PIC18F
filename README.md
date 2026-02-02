#  Automated Color Sorting Machine (PIC18F4550)

Este proyecto es un prototipo de automatización industrial diseñado para la clasificación de productos (caramelos) basado en su colorimetría RGB. El sistema utiliza un enfoque de **Sistemas Embebidos (Bare-Metal)** programado en C, gestionando sensores, actuadores y seguridad de acceso sin depender de un sistema operativo.

![Foto de tu máquina clasificadora](link-a-tu-foto.jpg)

##  Características Técnicas Destacadas

* **Arquitectura de Interrupciones:** Implementación de paradas de emergencia mediante interrupciones externas (`INT0`, `INT1`) para seguridad industrial, priorizando el detener la máquina sobre cualquier otro proceso.
* **Control de Acceso:** Sistema de Login vía UART/Terminal serial que bloquea la operación de la máquina hasta que un operario autorizado ingrese credenciales válidas.
* **Sensado Óptico:** Calibración y lectura de frecuencia del sensor TCS230 para determinar rangos RGB específicos (Rojo, Amarillo, Verde, Morado, Rosa).
* **Actuación PWM:** Control preciso de servomotores modificando el ciclo de trabajo (Duty Cycle) de los temporizadores internos para direccionar el producto al contenedor correcto.

##  Hardware y Tecnologías

* **MCU:** PIC18F4550 (Microchip).
* **Compilador:** XC8 (Lenguaje C Estándar).
* **Sensores:** TCS230 (Conversor Luz-a-Frecuencia).
* **HMI:** Pantalla LCD 16x2 y Terminal Serial (UART 9600 baudios).
* **Mecánica:** Estructura impresa en 3D (PLA).

##  Análisis del Código

El sistema opera como una **Máquina de Estados**. A continuación se muestra cómo se configura la seguridad por hardware (Interrupciones):

```c
// Configuración de Registros para Interrupciones (Seguridad)
void main() {
    // ...
    INTCONbits.INT0IE = 1;   // Habilita interrupción externa RB0 (Parada Emergencia 1)
    INTCON2bits.INTEDG0 = 1; // Disparo por flanco de subida
    
    INTCON3bits.INT1IE = 1;  // Habilita interrupción externa RB1 (Parada Emergencia 2)
    INTCONbits.GIE = 1;      // Habilita interrupciones globales
    
    // Bucle de bloqueo hasta autenticación
    while (!logged_in) login_loop(); 
    // ...
}

