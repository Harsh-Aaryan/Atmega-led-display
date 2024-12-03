#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

// Message to display (8 characters)
const char message[] = "COMPENG3";
volatile uint8_t current_pos = 0;
volatile uint8_t is_playing = 0;
volatile uint8_t fast_mode = 0;

// Frequencies for notes (in Hz)
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

// LED patterns for characters
uint8_t getLEDPattern(char c) {
	switch (c) {
		case 'C': return 0b00000111;  // Pattern for 'C'
		case 'O': return 0b00111111;  // Pattern for 'O'
		case 'M': return 0b00110110;  // Pattern for 'M'
		case 'P': return 0b01110011;  // Pattern for 'P'
		case 'E': return 0b01111001;  // Pattern for 'E'
		case 'N': return 0b00110111;  // Pattern for 'N'
		case 'G': return 0b01111111;  // Pattern for 'G'
		case '3': return 0b01101111;  // Pattern for '3'
		default: return 0x00;
	}
}

void initialize_ports(void) {
	// Initialize button inputs
	DDRA &= ~(1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3);  // Buttons as inputs
	PORTA |= (1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3);  // Enable pull-ups

	// Initialize LED outputs (using PORTD for 8 LEDs)
	DDRD = 0xFF;  // All PORTD pins as outputs
	
	// Initialize speaker output
	DDRB |= (1 << PB7);  // Speaker on PB7
}

void initialize_timer(void) {
	// Timer1 for message timing
	TCCR1B |= (1 << WGM12);  // CTC mode
	OCR1A = 15624;  // 0.5s at 16MHz with prescaler 1024
	TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
	
	// Timer0 for sound generation
	TCCR0A |= (1 << WGM01);  // CTC mode
}

void display_character(char c) {
	uint8_t pattern = getLEDPattern(c);
	PORTD = pattern;  // Display on PORTD LEDs
}

void play_note(uint16_t frequency) {
	if(frequency == 0) {
		TCCR0B = 0; // Stop timer
		PORTB &= ~(1 << PB7);
		return;
	}
	
	uint16_t ocr = F_CPU / (2 * 256 * frequency) - 1;
	OCR0A = ocr;
	TCCR0B = (1 << CS02);  // Start timer with prescaler 256
}

// Timer1 interrupt for character display and sound timing
ISR(TIMER1_COMPA_vect) {
	if(is_playing) {
		display_character(message[current_pos]);
		play_note(notes[current_pos]);
		
		current_pos++;
		if(current_pos >= 8) {
			current_pos = 0;
		}
		
		// Update timer for speed
		if(fast_mode) {
			OCR1A = 7812;  // 0.25s
			} else {
			OCR1A = 15624; // 0.5s
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
	
	while(1) {
		// Check Start button (PA0)
		if(!(PINA & (1 << PA0))) {
			is_playing = 1;
			while(!(PINA & (1 << PA0))); // Wait for button release
		}
		
		// Check Stop button (PA1)
		if(!(PINA & (1 << PA1))) {
			is_playing = 0;
			play_note(0);
			while(!(PINA & (1 << PA1)));
		}
		
		// Check Speed button (PA2)
		if(!(PINA & (1 << PA2))) {
			fast_mode = !fast_mode;
			while(!(PINA & (1 << PA2)));
		}
		
		// Check Reset button (PA3)
		if(!(PINA & (1 << PA3))) {
			current_pos = 0;
			is_playing = 0;
			fast_mode = 0;
			play_note(0);
			while(!(PINA & (1 << PA3)));
		}
	}
	
	return 0;
}