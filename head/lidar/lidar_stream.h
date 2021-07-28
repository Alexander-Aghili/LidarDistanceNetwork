#ifndef LSTREAM_H
#define LSTREAM_H

#include <iostream>
#include <termios.h>

int initializeLidarDevice(speed_t baudRate);
uint8_t serialGetchar(const int serial_port);
void serialFlush(const int serial_port); 
void serialClose(const int serial_port);

#endif