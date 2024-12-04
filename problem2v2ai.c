#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

// Variable initialization
const char message[] = "COMPENG3"; // 8 character message we want to display (COMPENG3)
volatile uint8_t current_pos = 0; // Current character we're displaying
volatile uint8_t is_playing = 0; // If the timer should be active or not
volatile uint8_t fast_mode = 0; // If the timer is fast or not

// Frequencies for notes in Hz. We'll use the C Major scale for simplicity.
const uint16_t notes[] = {
	262,  // C4
	294,  // D4
	330,  // E4
	349,  // F4
	392,  // G4
	440,  // A4
	494,  // B4
	523   // C5
};

// LED patterns for characters. Leaving some notes here for possible value changes.
uint8_t getLEDPattern(char c) {
	switch (c) {
		case 'C': return 0b00000111;  // Pattern for 'C'. Current value is '7'. We could represent it with the UTF-8 value of '67' or '01000011'
		case 'O': return 0b00111111;  // Pattern for 'O'. Current value is '63'. We could represent it with the UTF-8 value of '79' or '01001111'
		case 'M': return 0b00110110;  // Pattern for 'M'. Current value is '54'. We could represent it with the UTF-8 value of '77' or '01001101'
		case 'P': return 0b01110011;  // Pattern for 'P'. Current value is '115'. We could represent it with the UTF-8 value of '80' or '01010000'
		case 'E': return 0b01111001;  // Pattern for 'E'. Current value is '121'. We could represent it with the UTF-8 value of '69' or '01000101'
		case 'N': return 0b00110111;  // Pattern for 'N'. Current value is '55'. We could represent it with the UTF-8 value of '78' or '01001110'
		case 'G': return 0b01111111;  // Pattern for 'G'. Current value is '127'. We could represent it with the UTF-8 value of '71' or '01000111'
		case '3': return 0b01101111;  // Pattern for '3'. Current value is '111'. We could represent it with the UTF-8 value of '51' or '00110011'
		default: return 0x00;
	}
}

void initialize_ports(void) {
	// Initialize button inputs
	DDRA &= ~(1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3);  // Buttons as inputs
	PORTA |= (1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3);  // Enable pull-ups

	// Initialize LED outputs using PORTD
	DDRD = 0xFF;  // All PORTD pins as outputs
	
	// Initialize speaker output
	DDRE |= (1 << PE4);  // Speaker on PE4
}

void initialize_timer(void) {
	// Use Timer1 for message timing
	TCCR1B |= (1 << WGM12);  // CTC mode
	OCR1A = 15624;  // 0.5s at 16MHz with prescaler 1024
	TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
	
	// Use Timer0 for the sound generation
	TCCR0A |= (1 << WGM01);  // CTC mode
}

void display_character(char c) {
	uint8_t pattern = getLEDPattern(c); // Get and store the LED pattern
	PORTD = pattern;  // Display on the LEDs
}

void play_note(uint16_t frequency) {
	if(frequency == 0) {
		TCCR0B = 0; // Stop timer
		PORTE &= ~(1 << PE4);
		return;
	}
	
	uint16_t ocr = F_CPU / (2 * 256 * frequency) - 1;
	OCR0A = ocr;
	TCCR0B = (1 << CS02);  // Start timer with prescaler 256
}

// Timer1 interrupt for the character display and the sound generation
ISR(TIMER1_COMPA_vect) {
	if(is_playing) {
		display_character(message[current_pos]);
		play_note(notes[current_pos]);
		
		current_pos++;
		if(current_pos >= 8) {
			current_pos = 0; // Loop back to 0 if at position 8
		}
		
		// Check if we need to update the timer speed
		if(fast_mode) {
			OCR1A = 7812;  // 0.25s at 16MHz with prescaler 1024
			} else {
			OCR1A = 15624; // 0.5s at 16MHz with prescaler 1024
		}
	}
}

// Main program
int main(void) {
	initialize_ports();
	initialize_timer();
	
	// Start Timer1 with 1024 prescaler
	TCCR1B |= (1 << CS12) | (1 << CS10);
	
	sei();  // Enable global interrupts
	
	// Infinite loop to listen for button input
	while(1) {
		// Check Start button (PA0 or SW1)
		if(!(PINA & (1 << PA0))) {
			is_playing = 1; // Start timer
			while(!(PINA & (1 << PA0))); // Wait for button release
		}
		
		// Check Stop button (PA1 or SW2)
		if(!(PINA & (1 << PA1))) {
			is_playing = 0; // Stop timer
			play_note(0); // Stop sound
			while(!(PINA & (1 << PA1)));
		}
		
		// Check Speed button (PA2 or SW3)
		if(!(PINA & (1 << PA2))) {
			fast_mode = !fast_mode; // Toggle timer speed
			while(!(PINA & (1 << PA2)));
		}
		
		// Check Reset button (PA3 or SW4)
		if(!(PINA & (1 << PA3))) {
			current_pos = 0; // Reset positon
			is_playing = 0; // Stop timer
			fast_mode = 0; // Set timer to normal speed
			play_note(0); // Stop sound
			while(!(PINA & (1 << PA3)));
		}
	}
	
	return 0;
}