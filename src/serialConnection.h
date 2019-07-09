/*
 ***********************************************************************
 *
 *  serialConnection.h - a class for simplifying access to a
 *                       serial line
 *
 *  Copyright (C) 2018/2019 Dreamshader (aka Dirk Schanz)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ***********************************************************************
 */


#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#if defined(ARDUINO)

    #if ARDUINO > 22
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif

    #include "Stream.h"

#else // NOT on Arduino platform

#if defined( __linux__ )
#include <iostream>
#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>
#include <fcntl.h>
#include <termios.h>
#endif // defined( __linux__ )

#endif // NOT on Arduino platform


#ifdef __cplusplus
extern "C" {
#endif
 
using namespace std;

#define DEFAULT_CONNECTIONTYPE    -1
#define DEFAULT_DEVICE            "/dev/ttyUSB0"
#define DEFAULT_BAUD           38400
#define DEFAULT_DATABIT            8
#define DEFAULT_STOPBITS           1
#define DEFAULT_PARITY            'N'
#define DEFAULT_HANDSHAKE         'N'

#define E_OK                       0
#define E_FAIL                    -1
#define E_BUFSPACE                -2

#define E_PARAM_BAUDRATE         -10
#define E_PARAM_DATABIT	         -11
#define E_PARAM_PARITY	         -12
#define E_PARAM_STOPBITS         -13
#define E_PARAM_HANDSHAKE        -14
#define E_PARAM_DEVICE           -15
#define E_PARAM_NULL             -16
#define E_PARAM_NOFD             -17
#define E_PARAM_LENGTH           -18
#define E_NULL_CONNECTION        -19
#define E_READ_TIMEOUT           -20

#define CONNECTION_TYPE_UART      'u'
#define CONNECTION_TYPE_I2C       'i'
#define CONNECTION_TYPE_SPI       's'

#define CONNTYPE_SET               1

#define INATERFACE_SET             2
#define SPEED_SET                  4
#define PARITY_SET                 8
#define DATABITS_SET              16
#define STOPBITS_SET              32
#define UART_PARAM_COMPLETE       63

#define I2CSDA_SET                 2
#define I2CSCL_SET                 4
#define I2CADDR_SET                8
#define I2C_PARAM_COMPLETE        15

#define SPIMOSI_SET                2
#define SPIMISO_SET                4
#define SPISCLK_SET                8
#define SPICS_SET                 16
#define SPI_PARAM_COMPLETE        31

// for usleep
#define MICROSECONDS               1
#define MILLISECONDS            1000

#define TIMEOUT_MS               200

class serialConnection {

    private:
#if defined( __linux__ )
        int16_t        dev_fd;
        struct termios newtio;
#else // defined( __linux__ )
        Stream *ioStream;
#endif // defined( __linux__ )

        bool           dataReceived;
        int8_t         connType;
        uint32_t       connFlags;

#if defined( __linux__ )
        char           *device;
#endif // defined( __linux__ )
        uint32_t       baud;
        int16_t        databits;
        int8_t         parity;
        int16_t        stopbits;
        int8_t         handshake;

    public:
        int            errorNum;



#if defined( __linux__ )
        serialConnection( char *devname = (char*) DEFAULT_DEVICE, 
                          uint32_t baud = DEFAULT_BAUD, 
                          int16_t databits = DEFAULT_DATABIT, 
                          int8_t parity = DEFAULT_PARITY, 
                          int16_t stopbits = DEFAULT_STOPBITS, 
                          int8_t handshake = DEFAULT_HANDSHAKE );
#else // NOT defined( __linux__ )
        serialConnection( Stream *ioStream, 
                          uint32_t baud = DEFAULT_BAUD, 
                          int16_t databits = DEFAULT_DATABIT, 
                          int8_t parity = DEFAULT_PARITY, 
                          int16_t stopbits = DEFAULT_STOPBITS, 
                          int8_t handshake = DEFAULT_HANDSHAKE );
#endif // defined( __linux__ )

        bool isValidBaud( uint32_t baud );
        bool isValidDatabits( int16_t databits );
        bool isValidParity( int8_t parity );
        bool isValidStopbits( int16_t stopbits );
        bool isValidHandshake( int8_t handshake );
        bool isValidDevice( char* pDeviceName );

#if defined( __linux__ )
        int set_termios( void );

        int setup( char *devname, uint32_t baud, int16_t databits,
                   int8_t parity, int16_t stopbits, int8_t handshake );

        int ser_open( char *devname, uint32_t baud, short databits, 
                  int8_t parity, int16_t stopbits, int8_t handshake );
#else // NOT defined( __linux__ )
        int setup( Stream *ioStream, uint32_t baud, int16_t databits,
                   int8_t parity, int16_t stopbits, int8_t handshake );

        int ser_open( Stream *ioStream, uint32_t baud, short databits, 
                  int8_t parity, int16_t stopbits, int8_t handshake );
#endif // defined( __linux__ )

        int ser_open( void );

        int ser_close( void );
        int readBuffer( char* pBuffer, int bufLen );
        int ser_write( char* pBuffer, int wrLen );
        int readline( char* pBuffer, int bufLen );

        void flushOutput( void );
        void flushInput( void );

};

#ifdef __cplusplus
}
#endif


#endif // _SERIAL_CONNECTION_H_


