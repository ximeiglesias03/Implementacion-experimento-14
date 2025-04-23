#include <xc.h>
#define _XTAL_FREQ 4000000

// =============== CONFIGURATION BITS ===============
#pragma config FOSC = XT        // Oscilador externo (XT)
#pragma config WDTE = OFF       // Watchdog desactivado
#pragma config PWRTE = ON       // Power-up Timer activado
#pragma config BOREN = ON       // Brown-out Reset activado
#pragma config LVP = OFF        // Low-Voltage Programming desactivado
#pragma config CPD = OFF        // Data Code Protection desactivado
#pragma config WRT = OFF        // Write Protection desactivado
#pragma config CP = OFF         // Code Protection desactivado

// =============== FUNCIÓN ADC ===============
unsigned int ADC_Read() {
    ADCON0bits.CHS = 0b0011;    // Canal AN3 (CHS3:CHS0 = 0011)
    ADCON0bits.ADON = 1;        // Encender ADC
    
    // Configuración de pines analógicos (PIC16F887 usa ANSEL/ANSELH)
    ANSEL = 0b00001000;          // AN3 como analógico, resto digital
    ANSELH = 0x00;               // Todos los pines AN8-AN13 como digitales
    
    ADCON1bits.ADFM = 1;        // Justificación derecha
    __delay_us(25);             // Tiempo de adquisición
    
    ADCON0bits.GO = 1;          // Iniciar conversión
    while (ADCON0bits.GO);      // Esperar fin de conversión
    
    return (unsigned int)((ADRESH << 8) | ADRESL); // Combinar resultados
}

// =============== PROGRAMA PRINCIPAL ===============
void main() {
    TRISC = 0x00;       // PORTC como salida (PWM)
    PORTC = 0x00;       // Inicializar en 0
    
    // Configurar PWM
    PR2 = 0x3F;         // Período para ~15.63 kHz (4MHz, prescaler 1:1)
    T2CON = 0b00000100; // Timer2 ON, prescaler 1:1
    CCP2CON = 0x0F;     // Modo PWM
    
    while (1) {
        unsigned int ADCResult = ADC_Read();
        
        // Ajustar ciclo de trabajo (evitar warning con casteo explícito)
        CCPR2L = (unsigned char)(ADCResult >> 2);      // 8 MSB
        CCP2CONbits.DC2B0 = (ADCResult & 0x01) ? 1 : 0; // Bit 0
        CCP2CONbits.DC2B1 = (ADCResult & 0x02) ? 1 : 0; // Bit 1
        
        __delay_ms(10);
    }
}
