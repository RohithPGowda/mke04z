#include "MKE02Z4.h"  // Device header
#define S4	(1 << 14)  // PTB6
#define LED1     (1 << 1)  // PTA1
#define PORTA_BASE  0x40049000
#define PORTA  ((PORT_Type *)  PORTA_BASE)
int main() {
    SIM->SCGC |= (1 << 7);          // Enable GPIOA clock
		SIM->SCGC |= (1<<8);
    PORTA->PUEL |= S4;
		GPIOA->PIDR &= ~S4;
		GPIOA->PDDR &= ~S4;     // Switch as input
    GPIOA->PDDR |= LED1;         // PTA7 as output
	while (1) {
			 if (!(GPIOA->PDIR & S4)){  // Button pressed (logic low)
					GPIOA->PDOR |= LED1;                     // Turn ON LED
				}
				else{
					GPIOA->PDOR &= ~LED1;      // Turn OFF LED
				}
   	 }
	return 0;
}
