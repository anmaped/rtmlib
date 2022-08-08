#!/bin/sh

arm-none-eabi-as -c -mthumb -mlittle-endian -mcpu=cortex-m4 startup.S -o startup.o
arm-none-eabi-gcc -c -mthumb -mlittle-endian -mcpu=cortex-m4 test.c -o test.o
arm-none-eabi-ld -T ./mps2_m3.ld startup.o test.o -o test.elf
