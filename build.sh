#!/usr/bin/bash

avr-gcc -mmcu=attiny2313 -O2 -o hex/main.o source/main.c

avr-objcopy -O ihex -j.text -j.data hex/main.o hex/main.hex

avr-size -C -x --mcu=attiny2313 hex/main.o
