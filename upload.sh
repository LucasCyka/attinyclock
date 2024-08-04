#!/usr/bin/bash

sudo avrdude -C /etc/avrdude.conf -cusbasp -p attiny2313 -v -U flash:w:main.hex:i
