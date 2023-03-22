Appendix - Source Code
NOTE: Heads up some of the code is commented out since certain functionality was not implemented due to time. Most of the commented out code, when not a regular comment, belongs to the ultrasonic sensor, this was intended for obstacle avoidance but the sensor could not make any readings. 


/*
 * FinalProject_Embedded_System.c
 *
 * Created: 11/25/2022 2:56:59 PM
 * Author : fitwa
 * 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/io.h>
#include <util/delay.h>
#include "hc-sr04.h"

#define F_CPU 16000000UL
#define FOSC 16000000 //clock speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


float distance = 0.00;
char str[20]; //create an empty string to store number
char voltage_buffer[50];

/*Serial communication initialization*/
void USART_Init(unsigned int ubrr)
{
	/*Set the baud rate*/
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	
	/*Enable receiver and transmitter*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/*Only 1 stop bit but 8 datat No parity*/
	UCSR0C = (0<<USBS0)|(3<<UCSZ00);
	
}



unsigned char USART_Receive()
{
	/* Wait for data to be received */
	
	while ( !(UCSR0A & (1<<RXC0)) )
	{
		/*double distance = ultra_distance();
		char str[10];
		sprintf(str, "%d", (int)distance);
		
		usart_print(str);
		if(distance < 10.00 && distance > 0.93)
		{
			turnRight();
			distance=ultra_distance();
			
			if(distance>=10.00)
			{
				stop();
				
			}
		}
		*/
		
		
		distance = get_dist();
		_delay_ms(100);
		//sprintf(str, "%f", distance);//make the number into string using sprintf function
		//dtostrf(get_dist(),4,3,str);
		//sprintf(voltage_buffer, "v=%s V \n", str);
		
		if(distance <= 10.00) //still needs some work, the get distance didnot work as expted
		{
			turnRight();
		}
		
		//this to help trcp
		if(distance > 10.00 && distance < 40.00) //track the person make sure the person in range of 10 and 40
		{
			moveForward();
		}
		else
		{
			stop();
		}
		
		
		dtostrf(distance, 2, 2, str);/* distance to string */
		strcat(str, " cm   \n");	/* Concat unit i.e.cm */
		//_delay_ms(100);
		//_delay_ms(100);
		
		//usart_print(str);
		

	}
	/* Get and return received data from buffer */
	return UDR0;
}


char* Usart_Read()
{
	char data[20];
	int i=0;
	while(i<5)
	{
		data[i]=USART_Receive();
		i+=1;
	}
	return data;
}


void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void usart_print(char data[])
{
	int i=0;
	while(data[i]!=NULL)
	{
		USART_Transmit(data[i]);
		i=i+1;
	}
}

/*Interrupt setup */
//initializing interrupt pin for ECHO on Ultrasonic :
/*void initInterruptPins(void){
	//for below code: Both the use of PCMSK1 and PCIE1 is because it's in group 1 out of 0, 1 and 2 ;-)
	DDRC   &= ~(1 << PINC0); //ECHO ON PC0
	PCMSK1  =  (1 << PCINT8); //PCMSK1 =  (1 << PCINT8),PCMSK1 |=  (1 << PCINT8); "=" is used instead of "|=" to hard set hard set PCMSK1 to avoid worrying about the previous state in PCMSK1
	PCICR  |=  (1 << PCIE1);
	ADCSRA |=  (1 << ADEN);
	
}
*/



/*device movement methods*/
void moveForward()
{
	stop();
	frontLeftForward();
	frontRightForward();
	backLeftForward();
    backRightForward();
}
void moveBackwards()
{
	stop();
	
	frontLeftBackward();
	frontRightBackward();
	backRightBackward();
	backLeftBackward();
}


/*manipulating individual wheels */
void frontLeftForward()////////////////////////
{
	PORTD |=  (1<<PORTD7);
	PORTD &= ~  (1<<PORTD6);
}

void frontLeftBackward()
{
	PORTD &= ~  (1<<PORTD7);
	PORTD |=  (1<<PORTD6);
}


void backLeftForward()/////////////////////////
{
	PORTB |=   (1<<PORTB2);
	PORTB &= ~ (1<<PORTB1);
}


void backLeftBackward()
{
	PORTB &= ~ (1<<PORTB2);
	PORTB |=  (1<<PORTB1);
}

void frontRightForward()//////////////////////////
{
	PORTD |=   (1<<PORTD3);
	PORTD &= ~ (1<<PORTD2);
}

void frontRightBackward()
{
	PORTD &= ~  (1<<PORTD3);
	PORTD |=  (1<<PORTD2);
}


void backRightForward() /////////////
{
	PORTD |=  (1<<PORTD5);
	PORTD &= ~ (1<<PORTD4);
}

void backRightBackward()
{
	PORTD &= ~ (1<<PORTD5);
	PORTD |=  (1<<PORTD4);
}


void turnRight()
{
	//frontRightBackward();
	frontLeftForward();
	backLeftForward();
	backRightBackward();
}

void turnLeft()
{
	stop();
	frontRightForward();
	backRightForward();
	//frontLeftBackward();
	backLeftBackward();
}




/*stopping functionality*/
void stop()
{
	//PORTB &= ~ 0b11110;
	PORTB &= ~ 0b110;
	PORTD &= ~ 0b11111100;
}

/*change angle*/
/*void changeAngle()
{
	/* 1. Set Fast PWM mode 14: set WGM11, WGM12, WGM13 to 1 */
	/* 3. Set pre-scaler of 8 */
	/* 4. Set Fast PWM non-inverting mode */
	/*TCCR1A |= (1 << WGM11) | (1 << COM1A1);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);*/
	
	/* 2. Set ICR1 register: PWM period */
	/*ICR1 = 39999;*/
	
	
//}

int main(void)
{
	USART_Init(MYUBRR);
	init_ultrasonic_sensor();
	
	//DDRB |= 0B011110;
	DDRB |= 0B110;
	//DDRB |= 0b0100;
	DDRD |= 0B11111110;
	
	
	
	PORTB &= ~  0b00000000;
	PORTD &= ~  0b00000000;
	
	//sei();									/* Enable global interrupt */
	//TIMSK1 = (1 << TOIE1);					/* Enable Timer1 overflow interrupts */
	//TCCR1A = 0;								/* Set all bit to zero Normal operation */
	
	char command=NULL;
	/* Offset for correction */
	//int offset = 800;
	

	
	
	
	while (1) {
		
		command= USART_Receive();
		
		
		
		
		
		if(command=='f')
		{
			moveForward();
			//USART_Transmit(command);
			
			
		}
		
		if(command=='b' )
		{
			moveBackwards();
			
		}
		
		if(command=='l' )
		{
			turnLeft();
			
		}
		
		if(command=='r' )
		{
			turnRight();
			
		}
		
		if(command=='s' )
		{
			stop();
			
		}


		
	}
}
