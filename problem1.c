//problem 1 
// just a simple conversion 
// have to check all 
//test 
//harsh 

#include <avr/io.h>
#include <avr/interrupt.h>

 unsigned char count = 0;

//intitiazlizae buttons 
//look example notes 
//look previous code assembly 

void initialize_ports(void) {
    DDRA &= ~(1 << PA0);  // button1  -increment
    DDRA &= ~(1 << PA1);  // button2 - decrement
    DDRA &= ~(1 << PA2);  // button3 - initial Count
    DDRA &= ~(1 << PA3);  // button4 - intital 
    DDRE &= ~(1 << PE6);  // button5 - rest 

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

//if else works 

void display_count(unsigned char number) {
    
    //led 1
    if(number & 0x01) 
    PORTC |= (1 << PC0); 
    else 
    PORTC &= ~(1 << PC0);
    
    //led2
    if(number & 0x02)
     PORTD |= (1 << PD5); 
    else 
    PORTD &= ~(1 << PD5);

    //led3
    if(number & 0x04) 
    PORTE |= (1 << PE5); 
    else
     PORTE &= ~(1 << PE5);

    //led4
    if(number & 0x08)
     PORTD |= (1 << PD2); 
    else 
    PORTD &= ~(1 << PD2);

    //led5
    if(number & 0x10) 
    PORTD |= (1 << PD4);
     else 
     PORTD &= ~(1 << PD4);
}

int main(void) {
    initialize_ports();
  
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