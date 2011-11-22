# copyright - 2011 INKA - HTW Berlin

CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
#CFLAGS=-c -Wall
CFLAGS=-c 
LDFLAGS=-framework IOKit -framework Cocoa

all: workshop

workshop: Workshop.o SerialPortSample.o RfidControl.o
	$(CC) $(LDFLAGS) SerialDevice.o RfidControl.o Workshop.o -o workshop

SerialPortSample.o: SerialDevice.c
	$(CC) $(CFLAGS) SerialDevice.c

RfidControl.o: RfidControl.cpp
	$(CC) $(CFLAGS) RfidControl.cpp

workshop.o: Workshop.cpp
	$(CC) $(CFLAGS) Workshop.cpp



clean:
	rm -rf *o workshop
