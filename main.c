//Program for a simple clock in attiny2313


#define F_CPU 1000000UL

#define A 128
#define B 64
#define C 32
#define D 16
#define E 8
#define F 4
#define G 2


#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

long millisecond = 0;


int seconds = 5;
int minutes = 0;
int hours   = 0;

volatile unsigned char numbers[10][8] = {
{A,B,C,D,E,F,0,0}, //0
{0,B,C,0,0,0,0,0}, //1
{A,B,0,D,E,0,G,0}, //2
{A,B,C,D,0,0,0,0}, //3
{0,B,C,0,0,F,G,0}, //4
{A,0,C,D,0,F,G,0}, //5
{A,0,C,D,E,F,G,0}, //6
{A,B,C,0,0,0,0,0}, //7
{A,B,C,D,E,F,G,0}, //8
{A,B,C,0,0,F,G,0}  //9

};

int main(){

     DDRB = 255;
     DDRD = 63;
     
     while(1){
          
          //****** SECONDS ******//
          //lsd
          PORTB = 254;
          PORTD |= (1 << PIND2);
          PORTD &= ~(1 << PIND3);

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[seconds % 10][i]);
               
          }
         _delay_ms(10);
          PORTD &= ~(1 << PIND2);
          PORTD |= (1 << PIND3);
          //msd
          PORTB = 254;
          
          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(seconds/10)][i]);
               
          }
          _delay_ms(10);




     }

}