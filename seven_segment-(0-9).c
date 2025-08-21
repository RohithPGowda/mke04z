#include "MKE02Z4.h"  // Device header

// Segment control bit definitions with correct casting
#define LA ((uint32_t)1 << 19)  // PTC3
#define LB ((uint32_t)1 << 18)  // PTC2
#define LC ((uint32_t)1 << 31)  // PTD7
#define LD ((uint32_t)1 << 30)  // PTD6
#define LE ((uint32_t)1 << 29)  // PTD5
#define LF ((uint32_t)1 << 7)   // PTE7 (GPIOB)
#define LG ((uint32_t)1 << 17)  // PTC1
#define LH ((uint32_t)1 << 13)  // PTB3

// Use GPIOA for PTA, PTB, PTC, PTD
#define GPIOC GPIOA
#define GPIOD GPIOA

typedef struct {
    uint32_t portA;
    uint32_t portB;
} SegmentPattern;

const SegmentPattern digit_map[10] = {
    {LA | LB | LC | LD | LE , LF},               // 0
    {LB | LC, 0},                                    // 1
    {LA | LB | LD | LE | LG, 0},                     // 2
    {LA | LB | LC | LD | LG, 0},                     // 3
    {LB | LC | LG, LF},                               // 4
    {LA | LC | LD | LG , LF},                     // 5
    {LA | LC | LD | LE | LF | LG, LF},               // 6
    {LA | LB | LC, 0},                               // 7
    {LA | LB | LC | LD | LE | LF | LG, LF},          // 8
    {LA | LB | LC | LD | LG | LF, LF}                // 9
};

void delay_ms(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms * 7000; i++) {
        __asm("nop");
    }
}

void display_digit(uint8_t value) {
    // Turn OFF all segments
    GPIOA->PDOR &= ~(LA | LB | LC | LD | LE | LG | LH);
    GPIOB->PDOR &= ~LF;

    // Output the digit segments
    GPIOA->PDOR |= digit_map[value].portA;
    GPIOB->PDOR |= digit_map[value].portB;

    // Enable digit (LH = PTB3)
    GPIOA->PDOR |= LH;
}

int main(void) {
    uint8_t count = 0;

    // Enable clocks for PORTB–PORTE
    SIM->SCGC |= (1 << 8)   // PORTB
              | (1 << 9)    // PORTC
              | (1 << 10)   // PORTD
              | (1 << 11);  // PORTE

    // Set segment pins as output
    GPIOA->PDDR |= LA | LB | LC | LD | LE | LG | LH;
    GPIOB->PDDR |= LF;

    // Enable digit control line (assuming PTA25 = display enable)
    GPIOA->PDDR |= ((uint32_t)1 << 25);  // Set PTA25 as output
    GPIOA->PDOR |= ((uint32_t)1 << 25);  // Set PTA25 high

    while (1) {
			
        display_digit(count);
        delay_ms(500);
        count = (count + 1) % 10;
    }

}
