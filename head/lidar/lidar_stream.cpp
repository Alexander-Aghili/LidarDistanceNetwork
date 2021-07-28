
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "../log/log.h"
#include "lidar_stream.h"

/*
 * 
 * initializes the communication port between Lidar device and raspberry pi
 * 
 * 
 * @param baudRate is speed_t struct which just sets the baud rate of the UART communication. Ex. B115200
 * @return serial_port the port number of the connection
 */
int initializeLidarDevice(speed_t baudRate) {

    struct termios options;         //Options for the serial port connections
    int status, serial_port;        

    serial_port = open ("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

    if (serial_port == -1) {
        writeToLog("FATAL ERROR: COULD NOT OPEN SERIAL PORT /dev/ttyS0");
        return -1;
    }

    fcntl(serial_port, F_SETFL, O_RDWR); //Runs commands on serial_port

    //Get and modify current serial options
    tcgetattr (serial_port, &options);

    cfmakeraw   (&options) ;
    cfsetispeed (&options, baudRate) ;
    cfsetospeed (&options, baudRate) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

    tcsetattr (serial_port, TCSANOW, &options) ; //Set modify options

    ioctl (serial_port, TIOCMGET, &status);

    status |= TIOCM_DTR ;
    status |= TIOCM_RTS ;

    ioctl (serial_port, TIOCMSET, &status);

    usleep (10000) ;	// 10mS

    return serial_port;
}


/*
 * 
 * @param serial_port 
 * @returns int 8 bits long
 */
uint8_t serialGetchar(const int serial_port) {
    uint8_t byte; //8 bit byte

    if (read(serial_port, &byte, 1) != 1) {
        return -1;
    }
    
    return byte;

}


/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serialFlush (const int serial_port)
{
  tcflush (serial_port, TCIOFLUSH) ;
}


/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose (const int serial_port)
{
  close (serial_port) ;
}



