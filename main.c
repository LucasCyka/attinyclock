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
#include<avr/interrupt.h>

long long milliseconds = 0;

int seconds = 0;
int minutes = 30;
int hours   = 14;

const int muxDelay = 0;

volatile unsigned char numbers[10][8] = {
{A,B,C,D,E,F,0,0}, //0
{0,B,C,0,0,0,0,0}, //1
{A,B,0,D,E,0,G,0}, //2
{A,B,C,D,0,0,G,0}, //3
{0,B,C,0,0,F,G,0}, //4
{A,0,C,D,0,F,G,0}, //5
{A,0,C,D,E,F,G,0}, //6
{A,B,C,0,0,0,0,0}, //7
{A,B,C,D,E,F,G,0}, //8
{A,B,C,0,0,F,G,0}  //9

};

void clear(){
     PORTB = 254;
     _delay_ms(5);
}

//too imprecise
//TODO: check CTC with interrupts
void init_timer(){
     TCCR1B |= (1 << WGM12); //sets timer on CTC mode

     OCR1A = 15624; //1 second @1MHz and prescale of 64. Top value that will set flag OCF1A and OCF1B 

     TCCR1B |= ((1 << CS11) | (1 << CS10)); //sets cpu clock as timer with prescaler of 64
 
}

void increase_time(){
     //TODO:
}

int main(){

     DDRB = 255;
     DDRD = 124;

     init_timer();

     while(1){

          if (TIFR & (1 << OCF1A)){ //tifr event flag
               seconds++;
               TIFR |= (1 << OCF1A); //clear and resets compare trigger

               if (seconds == 60){
                    seconds = 0;
                    increase_time();
               }

          }

          //******  SECONDS  ******//
          //lsd
          clear();

          PORTD |= (1 << PIND2);
          PORTD &= ~(1 << PIND3);

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[seconds % 10][i]);
               
          }
         //_delay_us(muxDelay);
          PORTD &= ~(1 << PIND2);
          PORTD |= (1 << PIND3);
          //msd
          clear();
          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(seconds/10)][i]);
               
          }
          //_delay_us(muxDelay);


          //******  MINUTES  ******//
          //lsd
          clear();

          PORTD &= ~(1 << PIND3);
          PORTD |= (1 << PIND4);
          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[minutes % 10][i]);
               
          }

          //_delay_us(muxDelay);

          clear();
          PORTD &= ~(1 << PIND4);
          PORTD |= (1 << PIND5);

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(minutes / 10)][i]);
               
          }

          //_delay_us(muxDelay);

          //******  HOURS  ******//
          //lsd
          clear();
          PORTD &= ~(1 << PIND5);
          PORTD |= (1 << PIND6);

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[hours % 10][i]);
               
          }

          //_delay_us(muxDelay);

          //msd
          clear();
          PORTD &= ~(1 << PIND6);
          PORTB|= (1 << PINB0);

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(hours / 10)][i]);
               
          }

          //_delay_us(muxDelay);
          PORTB &= ~(1 << PINB0);

     }


}