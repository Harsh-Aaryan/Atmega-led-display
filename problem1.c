//problem 1 
// just a simple conversion 
// have to check all 
//test 
//harsh 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 0;

//intitiazlizae buttons 
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
    //z 
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

int main(void) {
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