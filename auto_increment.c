#include "MKE02Z4.h" // Device header
// Segment control bit definitions
#define LA ((uint32_t)1 << 19)  // PTC3
#define LB ((uint32_t)1 << 18)  // PTC2
#define LC ((uint32_t)1 << 31)  // PTD7
#define LD ((uint32_t)1 << 30)  // PTD6
#define LE ((uint32_t)1 << 29)  // PTD5
#define LF ((uint32_t)1 << 7)   // PTE7
#define LG ((uint32_t)1 << 17)  // PTC1
#define LH ((uint32_t)1 << 13)  // PTB3
#define DIGIT1 ((uint32_t)1 << 24) // PTA24 — Ones place control
#define DIGIT2 ((uint32_t)1 << 25) // PTA25 — Tens place control
// Your provided SegmentPattern structure and digit_map
typedef struct {
    uint32_t portA;
    uint32_t portB;
} SegmentPattern;
const SegmentPattern digit_map[10] = {
    {LA | LB | LC | LD | LE, LF},  // 0
    {LB | LC, 0},                  // 1
    {LA | LB | LD | LE | LG, 0},   // 2
    {LA | LB | LC | LD | LG, 0},   // 3
    {LB | LC | LG, LF},            // 4
    {LA | LC | LD | LG, LF},       // 5
    {LA | LC | LD | LE | LG, LF},  // 6
    {LA | LB | LC, 0},             // 7
    {LA | LB | LC | LD | LE | LG, LF}, // 8
    {LA | LB | LC | LD | LG, LF}   // 9
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
// Function to display a single digit on a single display
void display_single_digit(uint8_t value) {
    clear_segments();
    GPIOA->PDOR |= digit_map[value].portA;
    GPIOB->PDOR |= digit_map[value].portB;
}
int main() {
    uint8_t count = 0;
    uint8_t direction = 1; // 1 for incrementing, 0 for decrementing
    SIM->SCGC |= (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11); // Enable GPIO A–E
    // Set all segment and digit control lines as output
    GPIOA->PDDR |= LA | LB | LC | LD | LE | LG | LH | DIGIT1 | DIGIT2;
    GPIOB->PDDR |= LF;
    // Remove button pin configurations as they are no longer needed
    // GPIOB->PDDR &= ~(S4);
    // GPIOA->PDDR &= ~(S1);
     // Main loop for display multiplexing and counter logic
    while (1) {
        // --- Multiplexing Logic (Displaying the number) ---
        uint8_t tens = count / 10;
        uint8_t ones = count % 10;
        // Display tens digit
        display_single_digit(tens);
        GPIOA->PDOR |= DIGIT2; // Enable tens display
        delay_ms(2);
        GPIOA->PDOR &= ~DIGIT2; // Disable tens display

        // Display ones digit
        display_single_digit(ones);
        GPIOA->PDOR |= DIGIT1; // Enable ones display
        delay_ms(2);
        GPIOA->PDOR &= ~DIGIT1; // Disable ones display
          // --- Counter Logic (Happens periodically, not on every display refresh) ---
        // The original delay of 200ms can be controlled here.
        // For a smoother multiplexing, we need a separate timekeeping mechanism.
        // Let's use a counter instead of a long delay to allow multiplexing to run.
        static uint16_t refresh_counter = 0;
        if (refresh_counter > 25) { // Roughly 25 * 4ms = 100ms, adjust as needed
            if (direction) {
                count++;
                if (count > 99) {
                    count = 99;
                    direction = 0;
                }
            } else {
                count--;
                if (count < 0) {
                    count = 0;
                    direction = 1;
                }
            }
            refresh_counter = 0;
        } else {
            refresh_counter++;
        }
    }

    return 0;
} 
