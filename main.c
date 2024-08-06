//Program for a simple clock in attiny2313


#define F_CPU 4000000UL

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

int seconds = 0;
int minutes = 30;
int hours   = 14;

int modeSeconds = 0;
int modeMinutes = 0;
int modeHours = 0;

char onModeSeconds = -1;
char onModeMinutes = -1;
char onModeHours = -1;

//for debounce
int buttonLastPressed = -1;

//for blinking display
char lastBlinkControl = 0;
int  blinkingElapsed = 0;

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

     OCR1A = 15624; //1 second @1MHz and prescale of 256. Top value that will set flag OCF1A and OCF1B 

     TIMSK |= (1 << OCIE1A); //output compare A match interrupt

     sei(); //enable global interrupts

     TCCR1B |= (1 << CS12); //sets cpu clock as timer with prescaler of 64
 
}

ISR(TIMER1_COMPA_vect){
     if(onModeSeconds == 1 || onModeHours == 1 || onModeMinutes == 1){return;}

     seconds++;
}

void increase_time(){
     minutes++;
     if (minutes == 60){
          minutes =0;
          hours++;
     }

     if (hours == 24){
          hours = 0;
     }
}

void blink_segments(volatile uint8_t *PORT,uint8_t PIN){
     if(lastBlinkControl == 0){
          *PORT &= ~(1 << PIN);
          blinkingElapsed++;

          if (blinkingElapsed >= 10){
               lastBlinkControl = 1;
          }
     }else if(lastBlinkControl == 1){
          blinkingElapsed--;

          if(blinkingElapsed <= 0){
               lastBlinkControl = 0;
          }
     }
}

int main(){

     DDRB = 255;
     DDRD = 124;

     //buttons pull up
     PORTD |= ((1 << PIND0) | (1 << PIND1));
     //D0 = mode
     //D1 = set


     init_timer();

     while(1){
          

          if (~(PIND) & (1 << PIND0) && onModeSeconds == -1 && onModeMinutes == -1 && onModeHours == -1 && buttonLastPressed == -1){
               //enter setting mode
               modeSeconds = seconds;
               modeMinutes = minutes;
               modeHours = hours;
               onModeSeconds = 1;

               buttonLastPressed = 0;
               
          }

          if (~(PIND) & (1 << PIND0) && onModeSeconds == 1 && buttonLastPressed == -1){
               //on setting mode and pressed mode again, change minutes
               onModeSeconds = -1;
               onModeMinutes = 1;

               buttonLastPressed = 0;
          }

          if (~(PIND) & (1 << PIND0) && onModeMinutes == 1 && buttonLastPressed == -1){
               //on setting mode and pressed a third time, change hours
               onModeSeconds = -1;
               onModeMinutes = -1;
               onModeHours = 1;
               
               buttonLastPressed = 0;
          }

          if (~(PIND) & (1 << PIND0) && onModeHours == 1 && buttonLastPressed == -1){
               //on setting mode and pressed a fourth time, exit settings mode
               onModeSeconds = -1;
               onModeMinutes = -1;
               onModeHours = -1;
               
               buttonLastPressed = 0;
          }

          if(~(PIND) & (1 << PIND1) && (onModeSeconds == 1 || onModeMinutes == 1 || onModeHours == 1) && buttonLastPressed == -1){
               //set button on setting's mode
               if(onModeSeconds == 1){ modeSeconds++;}
               if(onModeMinutes == 1){ modeMinutes++;}
               if(onModeHours == 1){ modeHours++;}

               if (modeSeconds == 60) {modeSeconds = 0 ;}
               if (modeMinutes == 60) {modeMinutes = 0 ;}
               if (modeHours == 24)   {modeHours = 0 ;}

               buttonLastPressed = 0;
          }

          if(onModeSeconds != -1 || onModeMinutes != -1 || onModeHours != -1){
               seconds = modeSeconds;
               minutes = modeMinutes;
               hours = modeHours;
          }

          //update time 
          if(seconds == 60){
               seconds = 0;
               increase_time();
          }

          //******  SECONDS  ******//
          //lsd
          clear();
          PORTD |= (1 << PIND2);
          PORTD &= ~(1 << PIND3);

          if(onModeSeconds == 1){
               blink_segments(&PORTD,PIND2);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[seconds % 10][i]);
               
          }
         //_delay_us(muxDelay);
          PORTD &= ~(1 << PIND2);
          PORTD |= (1 << PIND3);
          //msd
          clear();

          if(onModeSeconds == 1){
               blink_segments(&PORTD,PIND3);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(seconds/10)][i]);
               
          }
          //_delay_us(muxDelay);


          //******  MINUTES  ******//
          //lsd
          clear();

          PORTD &= ~(1 << PIND3);
          PORTD |= (1 << PIND4);

          if(onModeMinutes == 1){
               blink_segments(&PORTD,PIND4);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[minutes % 10][i]);
               
          }

          //_delay_us(muxDelay);

          //msd
          clear();
          PORTD &= ~(1 << PIND4);
          PORTD |= (1 << PIND5);

          if(onModeMinutes == 1){
               blink_segments(&PORTD,PIND5);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(minutes / 10)][i]);
               
          }

          //_delay_us(muxDelay);

          //******  HOURS  ******//
          //lsd
          clear();
          PORTD &= ~(1 << PIND5);
          PORTD |= (1 << PIND6);

          if(onModeHours == 1){
               blink_segments(&PORTD,PIND6);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[hours % 10][i]);
               
          }

          //_delay_us(muxDelay);

          //msd
          clear();
          PORTD &= ~(1 << PIND6);
          PORTB|= (1 << PINB0);

          if(onModeHours == 1){
               blink_segments(&PORTB,PINB0);
          }

          for (int i =0; i< 8; i++){
               PORTB &= ~(numbers[(int)floor(hours / 10)][i]);
               
          }

          //_delay_us(muxDelay);
          PORTB &= ~(1 << PINB0);

          if(buttonLastPressed >= 5){
               buttonLastPressed = -1;
          }
          if(buttonLastPressed >= 0) {buttonLastPressed++;}


     }

     return 0;
}