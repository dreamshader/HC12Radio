/*
 ***********************************************************************
 *
 *  hc12Radio.h - definitions for hc-12 wireless comm board
 *
 *  Copyright (C) 2018 Dreamshader (aka Dirk Schanz)
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

#ifndef _HC12_RADIO_H_
#define _HC12_RADIO_H_

#include "serialConnection.h"

#if defined(ARDUINO)

    #if ARDUINO > 22
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif

    #include "SoftwareSerial.h"

#else // NOT on Arduino platform

#if defined( __linux__ )
    #include <iostream>
    #include <string>
    #include <sys/types.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h> 
    #include <getopt.h>
#if defined(RASPBERRY)
    #include <pigpio.h>
#endif // defined(RASPBERRY)

#endif // defined( __linux__ )

#endif // ARDUINO

#ifdef __cplusplus
extern "C" {
#endif
 
using namespace std;

#ifndef HIGH
#define HIGH                        1
#endif
#
#ifndef LOW
#define LOW                         0
#endif

#define HC12_ERR_OK                 0
#define HC12_ERR_FAIL              -1
#define HC12_ERR_RESPONSE          -2
#define HC12_ERR_ARGS              -3
#define HC12_ERR_OP_MODE           -4
#define HC12_ERR_NULLP             -5

#define HC12_ERR_BAUD             -10
#define HC12_ERR_PARITY           -11
#define HC12_ERR_DATABITS         -12
#define HC12_ERR_STOPBITS         -13
#define HC12_ERR_HANDSHAKE        -14
#define HC12_ERR_TTMODE           -15
#define HC12_ERR_CHANNEL          -16
#define HC12_ERR_POWER            -17
#define HC12_ERR_RANGE            -18

#define HC12_ERR_INIT_PIGPIO      -30

#if defined(ARDUINO)
    #define IO_BUFFER_SIZE         64
    #define LOG_BUFFER_SIZE        60
#else // NOT on Arduino platform
//    #define IO_BUFFER_SIZE        128
    #define IO_BUFFER_SIZE         64
    #define LOG_BUFFER_SIZE       120
#endif // defined(ARDUINO)

#define HC12_INTERFACE_HW           2
#define HC12_INTERFACE_SW           4

#define HC12_NULLPIN               -1

#define HC12_TTMODE_FU1             1
#define HC12_TTMODE_FU2             2
#define HC12_TTMODE_FU3             3
#define HC12_TTMODE_FU4             4
#define HC12_MIN_TTMODE            HC12_TTMODE_FU1
#define HC12_MAX_TTMODE            HC12_TTMODE_FU4

#define HC12_BAUD_1200           1200
#define HC12_BAUD_2400           2400
#define HC12_BAUD_4800           4800
#define HC12_BAUD_9600           9600
#define HC12_BAUD_19200         19200
#define HC12_BAUD_38400         38400
#define HC12_BAUD_57600         57600
#define HC12_BAUD_115200       115200

#define HC12_PARITY_NONE           'N'
#define HC12_PARITY_ODD            'O'
#define HC12_PARITY_EVEN           'E'

#define HC12_STOPBITS_1             1
#define HC12_STOPBITS_2             2
#define HC12_STOPBITS_15            3

#define HC12_DATABITS_8             8
#define HC12_DATABITS_7             7
#define HC12_DATABITS_6             6
#define HC12_DATABITS_5             5

#define HC12_HANDSHAKE_HW          'H'
#define HC12_HANDSHAKE_SW          'S'
#define HC12_HANDSHAKE_NONE        'N'

#define HC12_SETPIN_CMD_MODE       LOW
#define HC12_SETPIN_TT_MODE        HIGH

#define HC12_POWERPIN_ON           HIGH
#define HC12_POWERPIN_OFF          LOW

#define HC12_MIN_CHANNEL            1
#define HC12_MAX_CHANNEL          127
#define HC12_FREQ_MIN_CHANNEL  433400
#define HC12_FREQ_STEP            400

#define HC12_CHAN_2_FREQ(c)   HC12_FREQ_MIN_CHANNEL+((c-1)*HC12_FREQ_STEP)
#define HC12_FREQ_2_CHAN(f)   (((f-HC12_FREQ_MIN_CHANNEL)/HC12_FREQ_STEP)+1)

#define HC12_POWER_M1_DBM           1
#define HC12_POWER_2_DBM            2
#define HC12_POWER_5_DBM            3
#define HC12_POWER_8_DBM            4
#define HC12_POWER_11_DBM           5
#define HC12_POWER_14_DBM           6
#define HC12_POWER_17_DBM           7
#define HC12_POWER_20_DBM           8
#define HC12_MIN_POWER             HC12_POWER_M1_DBM
#define HC12_MAX_POWER             HC12_POWER_20_DBM

#define HC12_OP_UNKNOWN_MODE       -1
#define HC12_OP_NO_MODE             0
#define HC12_OP_CMD_MODE            1
#define HC12_OP_MNU_MODE            2
#define HC12_OP_TT_MODE             3

#define HC12_DEFAULT_SET_PIN       HC12_NULLPIN
#define HC12_DEFAULT_POW_PIN       HC12_NULLPIN

#define HC12_DEFAULT_CHANNEL       HC12_MIN_CHANNEL
#define HC12_DEFAULT_TTMODE        HC12_TTMODE_FU3
#define HC12_DEFAULT_POWER         HC12_POWER_20_DBM

#define HC12_DEFAULT_DEVICE        "/dev/ttyUSB0"
#define HC12_DEFAULT_BAUD          HC12_BAUD_9600
#define HC12_DEFAULT_DATABIT       HC12_DATABITS_8
#define HC12_DEFAULT_PARITY        HC12_PARITY_NONE
#define HC12_DEFAULT_STOPBITS      HC12_STOPBITS_1
#define HC12_DEFAULT_HANDSHAKE     HC12_HANDSHAKE_NONE

#define HC12_DEFAULT_V_MAJOR       0
#define HC12_DEFAULT_V_MINOR       0
#define HC12_DEFAULT_V_PATCH       0
#define HC12_DEFAULT_INTERFACE     HC12_INTERFACE_SW
#define HC12_DEFAULT_OPMODE        HC12_OP_TT_MODE

#define HC12_CMD_CODE_NULL         0
#define HC12_CMD_CODE_TEST         20
#define HC12_CMD_TEST              "AT\n"
#define HC12_RSP_TEST              "OK"
#define HC12_ARGS_RSP_TEST          0
#define HC12_CMD_CODE_SET_DEFAULT  21
#define HC12_CMD_SET_DEFAULT       "AT+DEFAULT\n"
#define HC12_RSP_SET_DEFAULT       "OK+DEFAULT"
#define HC12_ARGS_RSP_SET_DEFAULT   0
#define HC12_CMD_CODE_SLEEP        22
#define HC12_CMD_SLEEP             "AT+SLEEP\n"
#define HC12_RSP_SLEEP             "OK+SLEEP"
#define HC12_ARGS_RSP_SLEEP         0
#define HC12_CMD_CODE_UPDATE       23
#define HC12_CMD_UPDATE            "AT+UPDATE\n"
#define HC12_RSP_UPDATE            ""
#define HC12_ARGS_RSP_UPDATE        0
#define HC12_CMD_CODE_SET_BAUD     24
#define HC12_CMD_SET_BAUD          "AT+B%d\n"
#define HC12_RSP_SET_BAUD          "OK+B%d"
#define HC12_ARGS_RSP_SET_BAUD      1
#define HC12_CMD_CODE_SET_CHANNEL  25
#define HC12_CMD_SET_CHANNEL       "AT+C%03d\n"
#define HC12_RSP_SET_CHANNEL       "OK+C%03d"
#define HC12_ARGS_RSP_SET_CHANNEL   1
#define HC12_CMD_CODE_SET_TTMODE   26
#define HC12_CMD_SET_TTMODE        "AT+FU%d\n"
#define HC12_RSP_SET_TTMODE        "OK+FU%d"
#define HC12_ARGS_RSP_SET_TTMODE    1
#define HC12_CMD_CODE_SET_POWER    27
#define HC12_CMD_SET_POWER         "AT+P%d\n"
#define HC12_RSP_SET_POWER         "OK+P%d"
#define HC12_ARGS_RSP_SET_POWER     1
#define HC12_CMD_CODE_SET_PARAM    28
#define HC12_CMD_SET_PARAM         "AT+DEFAULT\n"
#define HC12_RSP_SET_PARAM         "OK+DEFAULT"
#define HC12_ARGS_RSP_SET_PARAM     0
#define HC12_CMD_CODE_SET_SERIAL   29
#define HC12_CMD_SET_SERIAL        "AT+U%d%c%d\n"
#define HC12_RSP_SET_SERIAL        "OK+U%d%c%d"
#define HC12_ARGS_RSP_SET_SERIAL    3
#define HC12_CMD_CODE_GET_BAUD     30
#define HC12_CMD_GET_BAUD          "AT+RB\n"
#define HC12_RSP_GET_BAUD          "OK+B%d"
#define HC12_ARGS_RSP_GET_BAUD      1
#define HC12_CMD_CODE_GET_CHANNEL  31
#define HC12_CMD_GET_CHANNEL       "AT+RC\n"
#define HC12_RSP_GET_CHANNEL       "OK+RC%d"
#define HC12_ARGS_RSP_GET_CHANNEL   1
#define HC12_CMD_CODE_GET_TTMODE   32
#define HC12_CMD_GET_TTMODE        "AT+RF\n"
#define HC12_RSP_GET_TTMODE        "OK+FU%d"
#define HC12_ARGS_RSP_GET_TTMODE    1
#define HC12_CMD_CODE_GET_POWER    33
#define HC12_CMD_GET_POWER         "AT+RP\n"
#define HC12_RSP_GET_POWER         "OK+RP:%ddBm"
#define HC12_ARGS_RSP_GET_POWER     1
#define HC12_CMD_CODE_GET_PARAM    34
#define HC12_CMD_GET_PARAM         "AT+RX\n"
#define HC12_RSP_GET_PARAM         "OK+B%d\r\nOK+RC%d\r\nOK+RP:%ddBm\r\nOK+FU%d"
#define HC12_ARGS_RSP_GET_PARAM     4
#define HC12_CMD_CODE_GET_SERIAL   35
#define HC12_CMD_GET_SERIAL        "AT+DEFAULT\n"
#define HC12_RSP_GET_SERIAL        "OK+DEFAULT"
#define HC12_ARGS_RSP_GET_SERIAL    0
#define HC12_CMD_CODE_GET_VERSION  36
#define HC12_CMD_GET_VERSION       "AT+V\n"
#define HC12_RSP_GET_VERSION       "HC-12_V%c.%c"
#define HC12_ARGS_RSP_GET_VERSION   2
// www.hc01.com  HC-12_V2.4
// HC-12_V1.1

#define HC12_CMD_STATUS_REQUEST     9
#define HC12_CMD_STATUS_ACTIVE     90
#define HC12_CMD_STATUS_DONE       99
#define HC12_CMD_STATUS_FAILED   -100
#define HC12_CMD_STATUS_UNKNOWN   -99
#define HC12_CMD_RESPONSE_UNKNOWN -98

#define HC12_DUMP_SERIAL_PARAM      1
#define HC12_DUMP_FW_INFO           2
#define HC12_DUMP_HC12_PARAM        3

struct _hc12_serial_param {
#if defined(__linux__)
    char *device;
    int dev_fd;
    struct termios oldtio;
    struct termios rawtio;
#else // NOT defined(__linux__)
    #if defined(ARDUINO)
    HardwareSerial *pHPort;
    SoftwareSerial *pSPort;
    bool            isHWPort;
    #endif // defined(ARDUINO)
#endif // defined(__linux__)
unsigned int baud;
unsigned char databit;
unsigned char parity;
unsigned char stopbits;
#if defined(__linux__)
unsigned char handshake;
#endif // defined(__linux__)
};

struct _hc12_fw_info {
    char major;
    char minor;
    char patch;
};

struct _hc12_param {
    int setPin;
    int powerPin;
    int comChannel;
    int ttMode;
    int power;
    struct _hc12_serial_param serialParam;
    struct _hc12_fw_info hwInfo;
};

struct _hc12_power {
short pIndex;
short dbm;
short mW;
};

#if defined(__linux__)
void dumpSerialParam( struct _hc12_serial_param *pData );
void dumpFWInfo( struct _hc12_fw_info *pData );
void dumpHC12Param( struct _hc12_param *pData );
#endif // defined(__linux__)


class hc12Radio {

  protected:
    struct _hc12_power _powTable[HC12_MAX_POWER] = {
        {1, -1, 8},
        {2, 2, 16},
        {3, 5, 32},
        {4, 8, 63},
        {5, 11, 120},
        {6, 14, 250},
        {7, 17, 500},
        {8, 20, 1000}
    };

// 
// Serial.begin(speed) Serial.begin(speed, config)
// 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200
// SERIAL_5N1 _6N1 _7N1 _8N1 (the default)
// SERIAL_5N2 _6N2 _7N2 _8N2
// SERIAL_5E1 _6E1 _7E1 _8E1
// SERIAL_5E2 _6E2 _7E2 _8E2
// SERIAL_5O1 _6O1 _7O1 _8O1
// SERIAL_5O2 _6O2 _7O2 _8O2

    int                _status;
    int                _currentCommand;
    int                _commandStatus;
    int                _responseArgs;
    serialConnection*  _connection;

    struct _hc12_param _moduleParam;
    int8_t             _interfaceType;
    int8_t             _currOpMode;
    char               _ioBuffer[IO_BUFFER_SIZE];



  public:
#if defined(ARDUINO)
    hc12Radio(int setPin, HardwareSerial *port = NULL);
    hc12Radio(int setPin, SoftwareSerial *port = NULL);
    hc12Radio(int setPin, int powerPin, HardwareSerial *port = NULL);
    hc12Radio(int setPin, int powerPin, SoftwareSerial *port = NULL);
#else // NOT on Arduino platform
    hc12Radio(int setPin = HC12_DEFAULT_SET_PIN,
              int powerPin = HC12_DEFAULT_POW_PIN);
#endif // defined(ARDUINO)


    bool isValidBaud( uint32_t baud );
    bool isValidParity( char parity );
    bool isValidDatabits( int databits );
    bool isValidStopbits( int stopbits );
#if defined(__linux__)
    bool isValidHandshake( int handshake );
#endif // defined(__linux__)

    bool isValidTTMode( int mode );
    bool isValidPower( int power );
    bool isValidChannel( int channel );

    void dump( int what );

    int getResponse( void );
    int parseResponse( void );
    short powerDB2Mode( int powerDB );
    short powerMode2DB( int power );

    int sendRequest( void );
    int connect( struct _hc12_serial_param *pParam );
    int disconnect( void );

    void init( void );
    void reset( void );

    int enterCommandMode( void );
    int leaveCommandMode( void );

    int test( void );
    int setDefault( void );

    int goSleepMode( void );
    int goUpdateMode( void );

    int setBaud( uint32_t baud );
    int setComChannel( int chan );
    int setTTMode( int mode );
    int setTPower( int power );

// not applicable!
    int setParam( struct _hc12_param* pParam );

    int setSerialParam( int databits, char parity, int stopbits );

/* ********************* */

    int getBaud( void );
    int getComChannel( void );
    int getTTMode( void );
    int getTPower( void );
    int getParam( void );
    int getFWVersion( void );

    int getSerialParam( int *databits, char *parity, int *stopbits );


};


#ifdef __cplusplus
}
#endif

#endif // _HC12_RADIO_H_

