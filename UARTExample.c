/********************************************************************************
Date: 11/24/2017
Version: 1.0.0
License: USE AT YOUR OWN RISK

-------------------------------------
- UART Library based on Patrick Hood-Daniel's examples at the NewbieHack website.
  https://www.newbiehack.com/
  https://www.youtube.com/watch?v=IFPPMpuVAJM
- This is also in line with the code examples found in the Atmel 328p datasheet.
********************************************************************************/

#define F_CPU 16000000UL
#define ODD 1
#define NONE 0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

volatile unsigned char receivedByte;

void initializeUART(int baud, char AsyncDoubleSpeed, char DataBitSize, char Parity, char StopBits )
{
	uint16_t UBBRValue = lrint(( (F_CPU) / (16L * baud) ) - 1);
	if(AsyncDoubleSpeed == 1) (UCSR0A = (1 << U2X0)); //setting the U2X for double speed
	UBRR0H = (unsigned char) (UBBRValue >> 8);		  //set the upper part of the baud
	UBRR0L = (unsigned char) UBBRValue;				  //set the lower part of the baud
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);			  //enable the transmitter and receiver
	UCSR0B |= (1 << RXCIE0);					      //Enable interrupt
	if(DataBitSize == 6) UCSR0C |= (1 << UCSZ00);     //6-bit data length
	if(DataBitSize == 7) UCSR0C |= (2 << UCSZ00);     //7-bit data length
	if(DataBitSize == 8) UCSR0C |= (3 << UCSZ00);     //8-bit data length
	if(DataBitSize == 9) UCSR0C |= (7 << UCSZ00);     //9-bit data length
	if(Parity == NONE) UCSR0C |= (1 << UPM01);        //Sets parity to NONE - 0
	if(Parity == ODD) UCSR0C |= (3 << UPM00);         //Set parity to ODD - 1
	if(StopBits == 2) UCSR0C = (1 << USBS0);          //Set 2 stop bits
}

void transmitByte(unsigned char data)
{
	while (! (UCSR0A & (1 << UDRE0)) );				  //Wait until the Transmitter is ready
	UDR0 = data;
}

unsigned char receiveByte(void)
{
	while ( !(UCSR0A & (1 << RXC0)) );				  //Wait for the RXC0 to not have 0
	return UDR0;									  //return received data to main loop
}									  

	void printString(const char myString[])
	{
		uint8_t i = 0;
		while (myString[i]) {
			transmitByte(myString[i]);
			i++;
		}
	}

int main(void)
{
	DDRB |= (1 << 5);
	initializeUART(9600, 0, 8, NONE, 1);
	sei();
	
	while(1)
	{

		_delay_ms(1000);
		printString("hello world\n");
		
	}
}

ISR(USART_RX_vect)
{
	receivedByte = UDR0;
	PORTB ^= (1 << 5);
}
