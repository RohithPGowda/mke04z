#include "MKE02Z4.h"
#include <stdint.h>

#define LED1        (1 << 1)  // PTA1
#define SWITCH_PIN  (1 << 6)  // PTA6
#define BAUD_RATE   9600
#define CORE_CLOCK  33554432  // 32.768 kHz * 1024 (FLL out)
#define BUS_CLOCK   (CORE_CLOCK / 2)  // SIM->BUSDIV = 1 ? /2
volatile unsigned int i;
volatile uint8_t received_char;
void delay_ms(unsigned int ms) {
    for (i = 0; i < ms * 1000; i++) {
        __asm("nop");
    }
}
void Clock_Init(void) {
    // SIM_BUSDIV_BUSDIV(1) ? Divide by 2
    SIM->BUSDIV = SIM_BUSDIV_BUSDIV(1);
    // FLL uses internal 32.768 kHz ref
    ICS->C1 = ICS_C1_IRCLKEN_MASK | ICS_C1_IREFS_MASK;
    // BDIV = 0 ? FLL output directly used
    ICS->C2 = 0x00;
    // Wait for FLL lock
    while (!(ICS->S & ICS_S_LOCK_MASK));
}
void UART0_init(void) {
    SIM->SCGC |= (1 << 7);  // Enable PORTA clock
    SIM->SCGC |= SIM_SCGC_UART0_MASK;
    // PTA2 = RX, PTA3 = TX
    SIM->PINSEL |= SIM_PINSEL_UART0PS_MASK;
    uint16_t sbr = BUS_CLOCK / (16 * BAUD_RATE);
    UART0->BDH = (sbr >> 8) & UART_BDH_SBR_MASK;
    UART0->BDL = sbr & UART_BDL_SBR_MASK;
    UART0->C1 = 0x00;  // 8N1
    UART0->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;  // Enable TX, RX
}

void UART0_send(uint8_t c) {
    while (!(UART0->S1 & UART_S1_TDRE_MASK));
    UART0->D = c;
    while (!(UART0->S1 & UART_S1_TC_MASK));
}

void GPIO_init(void) {
    SIM->SCGC |= (1 << 7);  // PORTA clock
    GPIOA->PDDR |= LED1;
    GPIOA->PCOR = LED1;
		GPIOA->PIDR &= ~SWITCH_PIN;
    GPIOA->PDDR &= ~SWITCH_PIN;
    PORT->PUEL |= SWITCH_PIN;
}

int main(void) {
    Clock_Init();
    GPIO_init();
    UART0_init();

    while (1) {
        if (!(GPIOA->PDIR & SWITCH_PIN)) {  // Switch pressed
            delay_ms(20);
            if (!(GPIOA->PDIR & SWITCH_PIN)) {
                UART0_send('5');
                while (!(GPIOA->PDIR & SWITCH_PIN));  // Wait release
            }
        }

        if (UART0->S1 & UART_S1_RDRF_MASK) {
            received_char = UART0->D;
            UART0_send(received_char);  // Echo back
            if (received_char != 'O') {
                GPIOA->PSOR = LED1;
            } else {
                GPIOA->PCOR = LED1;
            }
        }
    }
}
