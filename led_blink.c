#include "MKE02Z4.h"  // Device header
#define LED_PIN  (1 << 7)  // PTA7

void delay_ms(unsigned int w) {
	volatile unsigned int delay, x;
	for (delay = w; delay > 0; delay--) {
		for (x = 0; x < 1000; x++);
	}
}

int main() {
	SIM->SCGC |= (1 << 7);                 
	GPIOA->PDDR |= LED_PIN;         

	while (1) {
		GPIOA->PDOR ^= LED_PIN;
		delay_ms(1000);
	}
	return 0;
}
