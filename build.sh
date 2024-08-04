#!/usr/bin/bash

avr-gcc -mmcu=attiny2313 -O2 -o main.o main.c

avr-objcopy -O ihex -j.text -j.data main.o main.hex

avr-size -C -x --mcu=attiny2313 main.o
