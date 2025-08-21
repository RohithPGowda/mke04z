#include "MKE02Z4.h"  // Device header
// Segment control bit definitions
#define LA ((uint32_t)1 << 19)  // PTC3
#define LB ((uint32_t)1 << 18)  // PTC2
#define LC ((uint32_t)1 << 31)  // PTD7
#define LD ((uint32_t)1 << 30)  // PTD6
#define LE ((uint32_t)1 << 29)  // PTD5
#define LF ((uint32_t)1 << 7)   // PTE7 (GPIOB)
#define LG ((uint32_t)1 << 17)  // PTC1
#define LH ((uint32_t)1 << 13)  // PTB3
#define DIGIT1 ((uint32_t)1 << 24) // PTA24 — Ones place control
#define DIGIT2 ((uint32_t)1 << 25) // PTA25 — Tens place control
#define S4 (1<<14)
#define S1 (1<<6)
typedef struct {
    uint32_t portA;
    uint32_t portB;
} SegmentPattern;
const SegmentPattern digit_map[10] = {
    {LA | LB | LC | LD | LE, LF},                // 0
    {LB | LC, 0},                                 // 1
    {LA | LB | LD | LE | LG, 0},                  // 2
    {LA | LB | LC | LD | LG, 0},                  // 3
    {LB | LC | LG, LF},                           // 4
    {LA | LC | LD | LG, LF},                      // 5
    {LA | LC | LD | LE | LG, LF},                 // 6
    {LA | LB | LC, 0},                            // 7
    {LA | LB | LC | LD | LE | LG, LF},            // 8
    {LA | LB | LC | LD | LG, LF}                  // 9
};
void delay_ms(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms * 7000; i++) {
        __asm("nop");
    }
}
void clear_segments() {
    GPIOA->PDOR &= ~(LA | LB | LC | LD | LE | LG | LH);
    GPIOB->PDOR &= ~LF;
}
void display_number(uint8_t value) {
    uint8_t tens = value / 10;
    uint8_t ones = value % 10;
    // Display ones
    clear_segments();
    GPIOA->PDOR |= digit_map[ones].portA;
    GPIOB->PDOR |= digit_map[ones].portB;
    GPIOA->PDOR |= DIGIT1;
    delay_ms(2);
    GPIOA->PDOR &= ~DIGIT1;
    // Display tens
    clear_segments();
    GPIOA->PDOR |= digit_map[tens].portA;
    GPIOB->PDOR |= digit_map[tens].portB;
    GPIOA->PDOR |= DIGIT2;
    delay_ms(2);
    GPIOA->PDOR &= ~DIGIT2;
}
int main() {
    uint8_t count = 0;
    SIM->SCGC |= (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11); // Enable GPIO A–E
    // Set all segment and digit control lines as output
    GPIOA->PDDR |= LA | LB | LC | LD | LE | LG | LH | DIGIT1 | DIGIT2;
    GPIOB->PDDR |= LF;
		PORT->PUEL |=S4|S1;
		GPIOA->PIDR &= ~(S4 | S1);
		GPIOA->PDDR &= ~(S4 | S1);
    while (1) {
        	for (int i = 0; i < 100; i++) {
		if(!(GPIOA->PDIR & S4)){
            for (int j = 0; j < 50; j++) {  // Adjust inner loop for display refresh rate
                display_number(count);
            }
            count++;
		}
	if(!(GPIOA->PDIR & S1)){
		display_number(count);
	}
            if (count > 99) count = 0;
        }
				
    }
    return 0;
}
