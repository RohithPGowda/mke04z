7) PWM based LED brightness control
#include "MKE02Z4.h"  // Header for register definitions
#define PWM_CH        1            // FTM0 Channel 1
#define PWM_PIN_MASK  (1 << 1)     // PTA1
#define PWM_FREQ_HZ   1000         // PWM frequency = 1 kHz
#define BUS_CLK_HZ    20000000     // Assume 20 MHz bus clock

void delay_ms(unsigned int ms) {
    volatile unsigned int i;
    while (ms--) {
        for (i = 0; i < 4000; i++) {
            __asm("nop");
        }
    }
}

void pwm_init(void) {
    // Enable clocks to FTM0 and GPIOA
    SIM->SCGC |= SIM_SCGC_FTM0_MASK;     // Enable FTM0
		SIM->SCGC |= SIM_SCGC_FTM0_SHIFT;
    SIM->SCGC |= (1<<7);    // Enable PORTA
    // PTA1 -> Alt2 (FTM0_CH1)
    SIM->PINSEL &= ~SIM_PINSEL_FTM0PS1_MASK;   // Clear bits 3:2
    SIM->PINSEL |= SIM_PINSEL_FTM0PS1(0b10);   // Set Alt2 (FTM0_CH1)
		//SIM->PINSEL |= (0<<9);
		//FTM0->SC |= (0<<6)|();
		// Disable write protection
    //FTM0->MODE |= FTM_MODE_WPDIS_MASK;
    // Set counter range for 1 kHz PWM
    //FTM0->CNTIN = 0;
    FTM0->MOD = (BUS_CLK_HZ / PWM_FREQ_HZ) - 1;  // MOD = 19999 for 20 MHz
    // Edge-aligned PWM, high-true
    FTM0->CONTROLS[PWM_CH].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    // Initial duty cycle = 0%
    FTM0->CONTROLS[PWM_CH].CnV = 0;
    // Use system clock, no prescaler
    FTM0->SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
}
int main(void) {
   unsigned int duty = 0;
    int direction = 1;
    pwm_init();
    while (1) {
        // Set duty cycle (0–100%)
        FTM0->CONTROLS[PWM_CH].CnV = (FTM0->MOD * duty) / 100;
        delay_ms(10);  // Smooth fade delay
        // Fade logic
        if (direction) {
            duty++;
            if (duty >= 100) {
                duty = 100;
                direction = 0;
            }
        } else {
            duty--;
            if (duty == 0) {
                direction = 1;
            }
        }
    }
}

