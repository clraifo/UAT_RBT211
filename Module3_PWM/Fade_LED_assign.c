//Include necessary libraries
#include <avr/io.h>         // Standard AVR header, allows I/O
#include <avr/interrupt.h>  // Allows for use of interrupts
#define F_CPU 16000000UL	// Set the CPU clock speed to 16MHz
#include <util/delay.h>     // Allows for use of _delay_ms() function

// Define PWM values for fading LED
#define HALF_PWM 127        // 50% of max brightness
#define MAX_PWM 255		    // 100% of max brightness  
#define MIN_PWM 0		    // 0% of max brightness

// Declare volatile variables to be used in the interrupt service routines (ISRs)
volatile uint8_t fade = 0;         // Fade level of LED
volatile uint8_t direction = 0;    // Direction of fade (0=up, 1=down)
volatile uint8_t blink_state = 0;  // State of blink (0=off, 1=on)

// Function to initialize Timer0
void timer0_init()
{
    DDRD |= (1 << PD6) | (1 << PD5);                                        // Configure PD6 and PD5 as output
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);   // Set Timer0 to Phase Correct PWM Mode
    TCCR0B = (1 << CS00);                                                   // Set the clock without any prescaler
}

// Function to initialize Timer1
void timer1_init()
{
    TCCR1A = 0;                                         // Set Timer1 to Normal Mode
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  // Set Timer1 to CTC Mode and pre-scaling of 1024
    TIMSK1 = (1 << OCIE1A);                             // Enable interrupt when Timer1 matches OCR1A
    OCR1A = 15624;                                      // Set output compare register to generate a 1s delay
    sei();                                              // Enable global interrupts
}

// Main function
int main(void)
{
    timer0_init(); // Initialize Timer0
    timer1_init(); // Initialize Timer1

    while (1)       // Infinite loop
    {
        if (direction == 0) // If direction is up
        {
            if (fade < HALF_PWM)    // And fade level is less than 50%
                fade++;             // Increase fade level
            else
                direction = 1;      // Else, change direction to down
        }
        else // If direction is down
        {
            if (fade > 0)           // And fade level is more than 0
                fade--;             // Decrease fade level
            else
                direction = 0;      // Else, change direction to up
        }
        OCR0A = fade;               // Assign fade level to OCR0A (PD6)
        _delay_ms(10);              // Delay for 10 ms
    }
}

// ISR for Timer1 compare match
ISR(TIMER1_COMPA_vect)
{
    if (blink_state == 0)   // If LED is off
    {
        OCR0B = HALF_PWM;   // Turn it on at 50% brightness
        blink_state = 1;    // Set blink state to on
    }
    else // If LED is on
    {
        OCR0B = MIN_PWM;    // Turn it off
        blink_state = 0;    // Set blink state to off
    }
}
