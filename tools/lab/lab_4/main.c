#define FOSC 1000000                       // Clock Speed
#define USART_BAUDRATE 4800
#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <avr/delay.h>
#include <stdlib.h>

enum direction {FORWARD, BACKWARD};

void init_wheels() {
	// In back right wheel
	DDRC |= (1 << PC2);
	DDRC |= (1 << PC1);

	// In back left wheel
	DDRD |= (1 << PD2);
	DDRB |= (1 << PB1);

	// *******************************************************

	// Enabler for back right wheels
	DDRB |= (1 << PB3);

	// Enabler for back left wheels
	DDRD |= (1 << PD5);

	// *******************************************************

	// In front right wheel
	DDRC |= (1 << PC0);
	DDRB |= (1 << PB2);

	// In front left wheel
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB5);

	// *******************************************************

	// Enabler for front right wheels
	DDRD |= (1 << PD6);

	// Enabler for front left wheels
	DDRD |= (1 << PD3);

	// Set Timer0 in Fast PWM counting, with a prescaler 0
	// The OC0A pin is cleared on match.
	// Non Inverted PWM output mode
	TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
	TCCR0B = (1 << CS00);

	TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2B1);
	TCCR2B = (1 << CS20);
}

/*
 * Moves front left wheel. Takes direction as argumet, 1 = forward, 0 = backward
 */
void fr_right(enum direction dir, uint8_t duty) {
	switch(dir) {
	case FORWARD:
		PORTB |= (1 << PB4);
		PORTB &= ~(1 << PB5);
		break;
	case BACKWARD:
		PORTB |= (1 << PB5);
		PORTB &= ~(1 << PB4);
		break;
	default:
		PORTB &= ~(1 << PB5);
		PORTB &= ~(1 << PB4);
	}

	OCR0A = duty;
}

/*
 * Moves front right wheel. Takes direction as argumet, 1 = forward, 0 = backward
 */
void fr_left(enum direction dir, uint8_t duty) {
	switch(dir) {
	case FORWARD:
		PORTB |= (1 << PB2);
		PORTC &= ~(1 << PC0);
		break;
	case BACKWARD:
		PORTC |= (1 << PC0);
		PORTB &= ~(1 << PB2);
		break;
	default:
		PORTB &= ~(1 << PB2);
		PORTC &= ~(1 << PC0);
	}

	OCR2B = duty;
}

/*
 * Moves back left wheel. Takes direction as argumet, 1 = forward, 0 = backward
 */
void bk_left(enum direction dir, uint8_t duty) {
	switch(dir) {
	case FORWARD:
		PORTB &= ~(1 << PB1);
		PORTD |= (1 << PD2);
		break;
	case BACKWARD:
		PORTD &= ~(1 << PD2);
		PORTB |= (1 << PB1);
		break;
	default:
		PORTB &= ~(1 << PB1);
		PORTD &= ~(1 << PD2);
	}

	OCR0B = duty;
}

/*
 * Moves back right wheel. Takes direction as argumet, 1 = forward, 0 = backward
 */
void bk_right(enum direction dir, uint8_t duty) {

	switch(dir) {
	case FORWARD:
		PORTC |= (1 << PC1);
		PORTC &= ~(1 << PC2);
		break;
	case BACKWARD:
		PORTC |= (1 << PC2);
		PORTC &= ~(1 << PC1);
		break;
	default:
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC2);
	}

	OCR2A = duty;
}

void set_wheels_dir(){
	fr_left(FORWARD, 255);
	fr_right(FORWARD, 255);
	bk_right(FORWARD, 255);
	bk_left(FORWARD, 255);
}

//***************************  USART  *****************************

void init_USART() {
	/*Set baud rate */
	UBRR0H  = (BAUD_PRESCALE >> 8);
	UBRR0L  = BAUD_PRESCALE;
	// Enable receiver and transmitter
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	// Set frame: 8data, 1 stp
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}
char receiveChar() {
	// Wait until data is received
	while (!(UCSR0A & (1 << RXC0))) {}
	// Return the data from the RX buffer
	return UDR0;
}

void receiveMessage(char* message) {
	int index = 0;
	char c = receiveChar();
	while (isdigit(c) || c == ',' || c == '-') {
		message[index++] = c;
		c = receiveChar();
	}
	message[index] = '\0';
}

void sendChar(char c) {
	// Wait until buffer is empty
	while (!(UCSR0A & (1 << UDRE0))) {}
	// Return the data from the RX buffer
	UDR0 = c;
}

void sendMessage(char msg[]) {
	for (int i = 0; msg[i]; i++) {
		sendChar(msg[i]);
	}
}

int ctoi(char d)
{
	char str[2];

	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}

/*
 *
 */
int parseInt(char* integer_str) {
	int sign = 1;
	int length = strlen(integer_str);
	int i = 0;
	if (integer_str[0] == '-') {
		sign = -1;
		i = 1;
	}
	int input_int = 0;
	if (length >= 3) {
		input_int = 1;
	}
	for (i; i < length; i++) {
		input_int += ctoi(integer_str[i]) * pow(10, length - 1 - i);
	}
	return input_int*sign;
}

void loop() {
	int duty = 0;
	while(1) {

		int length = 128;
		char message[length + 1];
		receiveMessage(message);

		char* token;
		char* string;
		char* tofree;

		string = strdup(message);
		if (string != NULL) {
			int count = 1;
			tofree = string;
			while ((token = strsep(&string, ",")) != NULL)
			{
				enum direction rotation = FORWARD;
				duty = parseInt(token);
				if (duty < 0) {
					rotation = BACKWARD;
					duty = -1 * duty;
				}
				if (duty >= 0 && duty <= 255) {
					switch(count) {
					case 1: fr_left(rotation, duty);
					break;
					case 2: fr_right(rotation, duty);
					break;
					case 3: bk_left(rotation, duty);
					break;
					case 4: bk_right(rotation, duty);
					break;
					}
					count++;
				}

			}
			free(tofree);
		}
	}
}

int main() {
	init_USART();
	init_wheels();
	set_wheels_dir();
	loop();
	return 0;
}
