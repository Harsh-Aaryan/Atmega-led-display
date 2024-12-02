// Problem 2 - Message Display/Tune Design

/*
 * Below is just stuff copied over from problem 1, I specifically wanted to use the port initialization function, but I copied everything over.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 0;

// Initializing Buttons
void initialize_ports(void) {
    DDRA &= ~(1 << PA0);  // button1  - Increment
    DDRA &= ~(1 << PA1);  // button2 - Decrement
    DDRA &= ~(1 << PA2);  // button3 - Set Initial Count
    DDRA &= ~(1 << PA3);  // button4 - Increment Initial
    DDRE &= ~(1 << PE6);  // button5 - Reset

	//pull up 
    PORTA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);
    PORTE |= (1 << PE6);

    // led outputs 
    //check with diagrram and project 1 
    DDRC |= (1 << PC0);   // led1
    DDRD |= (1 << PD5);   // led2
    DDRE |= (1 << PE5);   // led3
    DDRD |= (1 << PD2);   // led4
    DDRD |= (1 << PD4);   // led5
}

void display_count(uint8_t number) {
    
    if(number & 0x01) PORTC |= (1 << PC0); else PORTC &= ~(1 << PC0);
    if(number & 0x02) PORTD |= (1 << PD5); else PORTD &= ~(1 << PD5);
    if(number & 0x04) PORTE |= (1 << PE5); else PORTE &= ~(1 << PE5);
    if(number & 0x08) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
    if(number & 0x10) PORTD |= (1 << PD4); else PORTD &= ~(1 << PD4);
}

uint8_t main(void) {
    initialize_ports();
    uint8_t temp_count = 0;
//functions
//maybe create 2 files?
//no 1 better 
//functions 
//while loop
    while(1) {
        //incre
        if(!(PINA & (1 << PA0))) {
            if(count < 28) count++;
            else count = 0;
            while(!(PINA & (1 << PA0))); 
        }

        // decrement 
        if(!(PINA & (1 << PA1))) {
            if(count > 0) count--;
            else count = 27;
            while(!(PINA & (1 << PA1)));
        }

        // reset 
        if(!(PINE & (1 << PE6))) {
            count = 0;
            while(!(PINE & (1 << PE6))); 
        }

        // display current 
        display_count(count);
    }
    return 0;
}

/*
 * Below is the stuff I got from AI. It gives a good outline of the main functions that need to exist as well as ways to execute them
 * I'd say the biggest problems with it are the fact the LEDs/buttoms do not seem to be initialized here (or correctly at least), as well as the sound functions being pretty bare
*/

// Message array with 8 characters (can be any combination of symbols, numbers, or alphabetic characters)
char message[] = "HELLO123";  // 8 characters

// Notes for the tune, one per character in the message
uint8_t tune[] = {60, 62, 64, 65, 67, 69, 71, 72};  // Example MIDI notes (Middle C, D, E, F, G, A, B, C')

void displayCharacter(char c) {
    // This function should map the character `c` to a specific LED pattern
    // Assume we have a function `getLEDPattern()` to get the corresponding LED pattern.
    uint8_t ledPattern = getLEDPattern(c);
    setLEDs(ledPattern);  // This function controls the LEDs based on the pattern
}

uint8_t getLEDPattern(char c) {
    // For simplicity, return dummy patterns based on character
    switch (c) {
        case 'H': return 0x01;  // LED pattern for 'H'
        case 'E': return 0x02;  // LED pattern for 'E'
        case 'L': return 0x04;  // LED pattern for 'L'
        case 'O': return 0x08;  // LED pattern for 'O'
        case '1': return 0x10;  // LED pattern for '1'
        case '2': return 0x20;  // LED pattern for '2'
        case '3': return 0x40;  // LED pattern for '3'
        default: return 0x00;   // Default pattern
    }
}

void setLEDs(uint8_t pattern) {
    // Function to control LEDs; depends on the Simon board's hardware interface
    // For example, assuming an 8-bit register controlling 8 LEDs:
    PORTB = pattern;  // Assuming PORTB controls the LEDs
}

void playTune(uint8_t note) {
    // Function to play a tune. Here we assume a function `playNote()` plays a specific frequency.
    playNote(note);
}

void playNote(uint8_t note) {
    // This function generates a tone for the note (using PWM or a timer)
    // Note values should correspond to frequencies; e.g., 60 = Middle C, 62 = D, etc.
    setFrequency(note);  // Assume this function sets the frequency for the speaker
    delay(500);  // Play note for 500ms (duration of one character display)
    stopTone();  // Stop the tone after the note finishes
}

void setFrequency(uint8_t note) {
    // This would set the frequency of the sound using a PWM or a timer
    // For example, using a timer to generate a frequency corresponding to the MIDI note value
}

#define BUTTON_START 0x01  // Button 1: Start
#define BUTTON_PAUSE 0x02  // Button 2: Pause/Stop
#define BUTTON_DOUBLE_SPEED 0x04  // Button 3: Double Speed
#define BUTTON_RESET 0x08  // Button 4: Reset

uint8_t speedFactor = 1;  // Normal speed (0.5s per character)
uint8_t isRunning = 0;  // State variable to track if message is running

void checkButtons() {
    uint8_t buttonState = readButtons();  // Read button states
    if (buttonState & BUTTON_START) {
        startMessage();
    }
    if (buttonState & BUTTON_PAUSE) {
        pauseMessage();
    }
    if (buttonState & BUTTON_DOUBLE_SPEED) {
        doubleSpeed();
    }
    if (buttonState & BUTTON_RESET) {
        resetMessage();
    }
}

uint8_t readButtons() {
    // This function reads the button states from GPIO pins
    return PIND & 0x0F;  // Example: Read first 4 bits of PIND for button presses
}

void displayMessage() {
    for (int i = 0; i < 8; i++) {
        if (!isRunning) break;  // If the message is paused, exit loop
        
        displayCharacter(message[i]);
        playTune(tune[i]);
        delay(500 * speedFactor);  // Delay based on speed factor (0.5s or 0.25s)
    }
}

void delay(int ms) {
    // Simple delay function (use a timer or loop for actual implementation)
    for (int i = 0; i < ms; i++) {
        _NOP();  // No operation (just waste time)
    }
}
