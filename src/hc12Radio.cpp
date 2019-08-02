/*
 ***********************************************************************
 *
 *  hc12Radio.cpp - a class for hc-12 wireless comm board
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

#include "hc12Radio.h"


#define DEBUG_LEVEL_0         0
#define DEBUG_LEVEL_1         1

static int hc12DebugLevel = DEBUG_LEVEL_0;

char hc12LogBuffer[LOG_BUFFER_SIZE];

void doLog(int level)
{
    if( level >= hc12DebugLevel )
    {
        switch(level)
        {
            case DEBUG_LEVEL_0:
                break;
            case DEBUG_LEVEL_1:
#if defined(__linux__)
                fprintf(stderr, "%s", hc12LogBuffer);
#else
                Serial.print(hc12LogBuffer);
#endif // defined(__linux__)
                break;
        }
    }
}

#if defined(__linux__)

void dumpSerialParam( struct _hc12_serial_param *pData )
{
    if( pData != NULL )
    {
        fprintf(stderr, "\ndumpSerialParam\n");
        fprintf(stderr, "---------------\n");
        fprintf(stderr, "device ...: %s\n", pData->device );
        fprintf(stderr, "dev_fd ...: %d\n", pData->dev_fd );
        fprintf(stderr, "baud .....: %u\n", pData->baud );
        fprintf(stderr, "databit ..: %d\n", pData->databit );
        fprintf(stderr, "parity ...: %c\n", pData->parity );
        fprintf(stderr, "stopbits .: %d\n", pData->stopbits );
        fprintf(stderr, "handshake : %c\n", pData->handshake );
    }
    else
    {
        fprintf(stderr, "dumpSerialParam: NULLP\n");
    }
}

void dumpFWInfo( struct _hc12_fw_info *pData )
{
    if( pData != NULL )
    {
        fprintf(stderr, "\ndumpFWInfo\n");
        fprintf(stderr, "----------\n");
        fprintf(stderr, "major ....: %d\n", pData->major );
        fprintf(stderr, "minor ....: %d\n", pData->minor );
        fprintf(stderr, "patch ....: %d\n", pData->patch );
    }
    else
    {
        fprintf(stderr, "dumpFWInfo: NULLP\n");
    }
}

void dumpHC12Param( struct _hc12_param *pData )
{
    if( pData != NULL )
    {
        fprintf(stderr, "\ndumpHC12Param\n");
        fprintf(stderr, "-------------\n");
        fprintf(stderr, "setPin ...: %d\n", pData->setPin );
        fprintf(stderr, "powerPin .: %d\n", pData->powerPin );
        fprintf(stderr, "comChannel: %d\n", pData->comChannel );
        fprintf(stderr, "ttMode ...: %d\n", pData->ttMode );
        fprintf(stderr, "power ....: %d\n", pData->power );
    }
    else
    {
        fprintf(stderr, "dumpHC12Param: NULLP\n");
    }
}

#endif // defined(__linux__)


/*
 ***********************************************************************
 | bool hc12Radio::isValidBaud( uint32_t baud )
 |
 | check baudrate for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidBaud( uint32_t baud )
{
    bool retVal = false;

    switch( baud )
    {
        case HC12_BAUD_1200:
        case HC12_BAUD_2400:
        case HC12_BAUD_4800:
        case HC12_BAUD_9600:
        case HC12_BAUD_19200:
        case HC12_BAUD_38400:
        case HC12_BAUD_57600:
        case HC12_BAUD_115200:
            retVal = true;
            break;
        default:
            retVal = false;
            break;
    }

    return( retVal );
}

/*
 ***********************************************************************
 | bool hc12Radio::isValidParity( char parity )
 |
 | check parity for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidParity( char parity )
{
    bool retVal = false;

    switch( parity )
    {
        case HC12_PARITY_NONE:
        case HC12_PARITY_ODD:
        case HC12_PARITY_EVEN:
            retVal = true;
            break;
        default:
            retVal = false;
            break;
    }

    return( retVal );
}

/*
 ***********************************************************************
 | bool hc12Radio::isValidDatabits( int databits )
 |
 | check databits for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidDatabits( int databits )
{
    bool retVal = false;

    switch( databits )
    {
        case HC12_DATABITS_8:
        case HC12_DATABITS_7:
        case HC12_DATABITS_6:
        case HC12_DATABITS_5:
            retVal = true;
            break;
        default:
            retVal = false;
            break;
    }

    return( retVal );
}

/*
 ***********************************************************************
 | bool hc12Radio::isValidStopbits( int stopbits )
 |
 | check stopbits for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidStopbits( int stopbits )
{
    bool retVal = false;

    switch( stopbits )
    {
        case HC12_STOPBITS_1:
        case HC12_STOPBITS_2:
        case HC12_STOPBITS_15:
            retVal = true;
            break;
        default:
            retVal = false;
            break;
    }

    return( retVal );
}

#if defined(__linux__)
/*
 ***********************************************************************
 | bool hc12Radio::isValidHandshake( int stopbits )
 |
 | check handshake for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidHandshake( int handshake )
{
    bool retVal = false;

    switch( handshake )
    {
        case HC12_HANDSHAKE_HW:
        case HC12_HANDSHAKE_SW:
            retVal = false;
            break;
        case HC12_HANDSHAKE_NONE:
            retVal = true;
            break;
        default:
            retVal = false;
            break;
    }

    return( retVal );
}
#endif // defined(__linux__)

/*
 ***********************************************************************
 | bool hc12Radio::isValidTTMode( int mode )
 |
 | check transparent transmission mode for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidTTMode( int mode )
{
    bool retVal = false;

    if( mode >= HC12_MIN_TTMODE && mode <= HC12_MAX_TTMODE )
    {
        retVal = true;
    }

    return( retVal );
}

/*
 ***********************************************************************
 | bool hc12Radio::isValidPower( int power )
 |
 | check power mode for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidPower( int power )
{
    bool retVal = false;

    if( power >= HC12_MIN_POWER && power <= HC12_MAX_POWER )
    {
        retVal = true;
    }

    return( retVal );
}

/*
 ***********************************************************************
 | bool hc12Radio::isValidChannel( int channel )
 |
 | check power mode for validity
 | return true if valid otherwise false
 ***********************************************************************
*/
bool hc12Radio::isValidChannel( int channel )
{
    bool retVal = false;

    if( channel >= HC12_MIN_CHANNEL && channel <= HC12_MAX_CHANNEL )
    {
        retVal = true;
    }

    return( retVal );
}



#if defined(ARDUINO)

hc12Radio::hc12Radio(int setPin, HardwareSerial *port)
{
    _connection = new serialConnection(port);
    _moduleParam.setPin = setPin;
}

hc12Radio::hc12Radio(int setPin, SoftwareSerial *port)
{
    _connection = new serialConnection(port);
    _moduleParam.setPin = setPin;
}

hc12Radio::hc12Radio(int setPin, int powerPin, HardwareSerial *port)
{
    _connection = new serialConnection(port);
    _moduleParam.setPin = setPin;
    _moduleParam.powerPin = powerPin;
}

hc12Radio::hc12Radio(int setPin, int powerPin, SoftwareSerial *port)
{
    _connection = new serialConnection(port);
    _moduleParam.setPin = setPin;
    _moduleParam.powerPin = powerPin;
}

#else // NOT on Arduino platform

hc12Radio::hc12Radio(int setPin, int powerPin) 
{  
    _connection = new serialConnection(); 
    _moduleParam.setPin = setPin;
    _moduleParam.powerPin = powerPin;
    init();
}

#endif // defined(ARDUINO)



#if defined(__linux__)
/* 
 ------------------------------------------------------------------------------
 * void hc12Radio::dump( int what )
 *
 * dump structures
 ------------------------------------------------------------------------------
*/
void hc12Radio::dump( int what )
{
    switch( what )
    {
        case HC12_DUMP_SERIAL_PARAM:
            dumpSerialParam( &_moduleParam.serialParam );
            break;
        case HC12_DUMP_FW_INFO:
            dumpFWInfo( &_moduleParam.hwInfo );
            break;
        case HC12_DUMP_HC12_PARAM:
            dumpHC12Param( &_moduleParam );
            break;
        default:
            break;
    }
}

#endif // defined(__linux__)








/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::powerDB2Mode(int powerDB)
 *
 * convert dB value to power mode
 * returns the power mode for a given dB value or an error code
 ------------------------------------------------------------------------------
*/
short hc12Radio::powerDB2Mode(int powerDB)
{
    short retVal = HC12_ERR_FAIL;

    for( int i = 0; retVal < 0 && i < HC12_MAX_POWER; i++ )
    {
        if( _powTable[i].dbm == powerDB )
        {
            retVal = _powTable[i].pIndex;
        }
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::powerMode2DB(int power)
 *
 * convert power mode to dB value
 * returns a dB value for a given power mode or an error code
 ------------------------------------------------------------------------------
*/
short hc12Radio::powerMode2DB(int power)
{
    short retVal = HC12_ERR_RANGE;

    if( power < HC12_MAX_POWER && power >= 0 )
    {
        retVal = _powTable[power].dbm;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getResponse( void )
 *
 * read the response of the attached smart-TFT into _ioBuffer
 * returns the amount if characters read or an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getResponse( void )
{
    int retVal;

    if( _connection != NULL )
    {
        memset( _ioBuffer, '\0', IO_BUFFER_SIZE );
        retVal = _connection->readline( _ioBuffer,
                                      IO_BUFFER_SIZE-1 );
    }
    else
    {
        retVal = E_NULL_CONNECTION;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::parseResponse( void )
 *
 * parse the response of the attached board
 * returns parsing result
 ------------------------------------------------------------------------------
*/
#define NO_MORE_DATA    22
#define TRY_MORE_DATA   33

int hc12Radio::parseResponse( void )
{
    static uint32_t baud;
    static int channel;
    static int powerDB;
    static int ttMode;
    static int power;
    static char parity;
    static int stopbits;
    static int databits;
    static char major;
    static char minor;

    int retVal = NO_MORE_DATA;
    char *pResult;
    int parsedValues = 0;

    if( _connection != NULL )
    {
        if( (pResult = strcasestr( _ioBuffer, "OK")) != NULL )
        {
            parsedValues = 0;

            if(pResult[2] == '+' )
            {
                switch(pResult[3])
                {
                    case 'B':
sprintf(hc12LogBuffer, "OK+B ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = sscanf(pResult, HC12_RSP_GET_BAUD, 
                                         &baud);
                        if( parsedValues == HC12_ARGS_RSP_GET_BAUD )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'R':
sprintf(hc12LogBuffer, "OK+R ");
doLog(DEBUG_LEVEL_1);
                        switch(pResult[4])
                        {
                            case 'C':
sprintf(hc12LogBuffer, " C ...");
doLog(DEBUG_LEVEL_1);
                                parsedValues = sscanf(pResult, 
                                         HC12_RSP_GET_CHANNEL, &channel );
                                if( parsedValues == HC12_ARGS_RSP_GET_CHANNEL )
                                {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                                }
                                else
                                {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                                    _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                                }
                                break;
                            case 'P':
sprintf(hc12LogBuffer, " P ...");
doLog(DEBUG_LEVEL_1);
                                parsedValues = sscanf(pResult, 
                                         HC12_RSP_GET_POWER, &powerDB );
                                if( parsedValues == HC12_ARGS_RSP_GET_POWER )
                                {
                                    power = powerDB2Mode(powerDB);
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                                }
                                else
                                {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                                    _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                                }
                                break;
                            default:
sprintf(hc12LogBuffer, " ?[=%c] ...", pResult[4]);
doLog(DEBUG_LEVEL_1);
                                _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                                break;
                        }
                        break;
                    case 'F':
sprintf(hc12LogBuffer, "OK+F ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = sscanf(pResult, HC12_RSP_GET_TTMODE, 
                                         &ttMode );
                        if( parsedValues == HC12_ARGS_RSP_GET_TTMODE )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'C':
sprintf(hc12LogBuffer, "OK+C ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = sscanf(pResult, HC12_RSP_SET_CHANNEL, 
                                         &channel );
                        if( parsedValues == HC12_ARGS_RSP_SET_CHANNEL )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'D':
sprintf(hc12LogBuffer, "OK+D ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = strncmp( pResult, HC12_RSP_SET_DEFAULT,
                               strlen(HC12_RSP_SET_DEFAULT) );

                        if( parsedValues == HC12_ARGS_RSP_SET_DEFAULT )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'S':
sprintf(hc12LogBuffer, "OK+S ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = strncmp( pResult, HC12_RSP_SLEEP,
                               strlen(HC12_RSP_SLEEP) );
                        if( parsedValues == HC12_ARGS_RSP_SLEEP )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'P':
sprintf(hc12LogBuffer, "OK+P ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = sscanf(pResult, HC12_RSP_SET_POWER, 
                                         &power );
                        if( parsedValues == HC12_ARGS_RSP_SET_POWER )
                        {
                            powerDB = powerMode2DB(power);
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    case 'U':
sprintf(hc12LogBuffer, "OK+U ...");
doLog(DEBUG_LEVEL_1);
                        parsedValues = sscanf(pResult, HC12_RSP_SET_SERIAL, 
                                         &databits, &parity, &stopbits );
                        if( parsedValues == HC12_ARGS_RSP_SET_SERIAL )
                        {
sprintf(hc12LogBuffer, "match!\n");
doLog(DEBUG_LEVEL_1);
                        }
                        else
                        {
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                            _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                        }
                        break;
                    default:
sprintf(hc12LogBuffer, "OK+ ?[=%c] ", pResult[4]);
doLog(DEBUG_LEVEL_1);
                        _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
sprintf(hc12LogBuffer, "NO match!\n");
doLog(DEBUG_LEVEL_1);
                        break;

                }
//                retVal = TRY_MORE_DATA;
                retVal = NO_MORE_DATA;
            }
            else
            {
                // result of test command
                if( strncmp(pResult,HC12_RSP_TEST,strlen(HC12_RSP_TEST)) == 0 )
                {
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                else
                {
                    _commandStatus = HC12_CMD_RESPONSE_UNKNOWN;
                }
            }
        }
        else
        {
            if( (pResult = strcasestr( _ioBuffer, "HC-12")) != NULL )
            {
                parsedValues = sscanf(pResult, HC12_RSP_GET_VERSION, 
                                         &major, &minor );
                if( parsedValues == HC12_ARGS_RSP_GET_VERSION )
                {
                    _commandStatus = HC12_CMD_STATUS_DONE;
//                    _currentCommand = HC12_CMD_CODE_NULL;
                }
                else
                {
                    parsedValues = 0;
                    _commandStatus = HC12_CMD_STATUS_FAILED;
                }
                retVal = NO_MORE_DATA;
            }
            else
            {
// look here if application hangs
                while( retVal == E_BUFSPACE || retVal > 0 )
                {
                    retVal = _connection->readBuffer( _ioBuffer,
                          IO_BUFFER_SIZE-1 );
                };
    
                parsedValues = 0;
                _commandStatus = HC12_CMD_STATUS_FAILED;
                retVal = NO_MORE_DATA;
            }
        }

        _responseArgs += parsedValues;

        switch( _currentCommand )
        {
            case HC12_CMD_CODE_NULL:
                // oops ...
                break;
            case HC12_CMD_CODE_TEST:
                if( _responseArgs == HC12_ARGS_RSP_TEST )
                {
// _modulstatus = online/connected
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_DEFAULT:
                if( _responseArgs == HC12_ARGS_RSP_SET_DEFAULT )
                {
                    reset();
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SLEEP:
                if( _responseArgs == HC12_ARGS_RSP_SLEEP )
                {
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_UPDATE:
                if( _responseArgs == HC12_ARGS_RSP_UPDATE )
                {
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_BAUD:
                if( _responseArgs == HC12_ARGS_RSP_SET_BAUD )
                {
                    _moduleParam.serialParam.baud = baud;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_CHANNEL:
                if( _responseArgs == HC12_ARGS_RSP_SET_CHANNEL )
                {
                    _moduleParam.comChannel = channel;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_TTMODE:
                if( _responseArgs == HC12_ARGS_RSP_SET_TTMODE )
                {
                    _moduleParam.ttMode = ttMode;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_POWER:
                if( _responseArgs == HC12_ARGS_RSP_SET_POWER )
                {
                    _moduleParam.power = power;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_PARAM:
// not applicable!
                if( _responseArgs == HC12_ARGS_RSP_SET_PARAM )
                {
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_SET_SERIAL:
                if( _responseArgs == HC12_ARGS_RSP_SET_SERIAL )
                {
                    _moduleParam.serialParam.databit = databits;
                    _moduleParam.serialParam.parity = parity;
                    _moduleParam.serialParam.stopbits = stopbits;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_BAUD:
                if( _responseArgs == HC12_ARGS_RSP_GET_BAUD )
                {
                    _moduleParam.serialParam.baud = baud;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_CHANNEL:
                if( _responseArgs == HC12_ARGS_RSP_GET_CHANNEL )
                {
                    _moduleParam.comChannel = channel;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_TTMODE:
                if( _responseArgs == HC12_ARGS_RSP_GET_TTMODE )
                {
                    _moduleParam.ttMode = ttMode;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_POWER:
                if( _responseArgs == HC12_ARGS_RSP_GET_POWER )
                {
                    _moduleParam.power = power;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_PARAM:
                if( _responseArgs == HC12_ARGS_RSP_GET_PARAM )
                {
                    _moduleParam.serialParam.baud = baud;
                    _moduleParam.comChannel = channel;
                    _moduleParam.ttMode = ttMode;
                    _moduleParam.power = power;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_SERIAL:
                if( _responseArgs == HC12_ARGS_RSP_GET_SERIAL )
                {
                    _moduleParam.serialParam.databit = databits;
                    _moduleParam.serialParam.parity = parity;
                    _moduleParam.serialParam.stopbits = stopbits;
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            case HC12_CMD_CODE_GET_VERSION:
                if( _responseArgs == HC12_ARGS_RSP_GET_VERSION )
                {
                    _moduleParam.hwInfo.major = major - '0';
                    _moduleParam.hwInfo.minor = minor - '0';
                    _commandStatus = HC12_CMD_STATUS_DONE;
                    _currentCommand = HC12_CMD_CODE_NULL;
                    retVal = NO_MORE_DATA;
                }
                break;
            default:
                _commandStatus = HC12_CMD_STATUS_UNKNOWN;
//                retVal = NO_MORE_DATA;
                retVal = TRY_MORE_DATA;
                break;
        }


        if( retVal == NO_MORE_DATA && _commandStatus >= 0 )
        {
            _currentCommand = HC12_CMD_CODE_NULL;
            retVal = NO_MORE_DATA;
        }
        else
        {
            if( _commandStatus < 0 )
            {
// look here if application hangs
                while( retVal == E_BUFSPACE || retVal > 0 )
                {
                    retVal = _connection->readBuffer( _ioBuffer,
                              IO_BUFFER_SIZE-1 );
                };

                _currentCommand = HC12_CMD_CODE_NULL;
                retVal = NO_MORE_DATA;
            }
        }
    }
    else
    {
        retVal = E_NULL_CONNECTION;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::sendRequest( void )
 *
 * send the command buffer to the attached HC-12 board
 * returns the amount if characters written or an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::sendRequest( void )
{
    int retVal;
    bool moreData;

printf("command >%s", _ioBuffer);

    if( _connection != NULL )
    {
//        _connection->flushOutput();
//        _connection->flushInput();

        retVal = _connection->ser_write( _ioBuffer,
                                          strlen(_ioBuffer) );
        if( retVal > 0 )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
            moreData = true;
            while(moreData)
            {
                retVal = getResponse();

sprintf(hc12LogBuffer, "RESPONSE: %s\n", _ioBuffer);
doLog(DEBUG_LEVEL_1);

                if(retVal == E_READ_TIMEOUT )
                {
                    moreData = false;
                }
                else
                {
                    if(retVal != E_BUFSPACE )
                    {
                        switch( retVal = parseResponse() )
                        {
                            case NO_MORE_DATA:
                                moreData = false;
                                break;
                            case TRY_MORE_DATA:
                                break;
                        }
                    }
                    else
                    {
                        moreData = false;
                    }
                }
            }

            if( (retVal = _commandStatus) == HC12_CMD_STATUS_DONE )
            {
sprintf(hc12LogBuffer, "Command complete ...\n");
doLog(DEBUG_LEVEL_1);
                retVal = E_OK;
            }
            else
            {
sprintf(hc12LogBuffer, "Command terminates with error %d\n", retVal);
doLog(DEBUG_LEVEL_1);
            }
        }
    }
    else
    {
        retVal = E_NULL_CONNECTION;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::connect( void )
 *
 * connect to an attached smart-TFT using given parameters
 * return E_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::connect( struct _hc12_serial_param *pParam )
{
    int retVal;

    if( _connection != NULL )
    {
        if( pParam != NULL )
        {
#if defined(__linux__)
            _moduleParam.serialParam.device = strdup( pParam->device );
#else // NOT defined(__linux__)
    #if defined(ARDUINO)
            _moduleParam.serialParam.pHPort = pParam->pHPort;
            _moduleParam.serialParam.pSPort = pParam->pSPort;
            _moduleParam.serialParam.isHWPort = pParam->isHWPort;
    #endif // defined(ARDUINO)
#endif // defined(__linux__)
            _moduleParam.serialParam.baud = pParam->baud;
            _moduleParam.serialParam.databit = pParam->databit;
            _moduleParam.serialParam.parity = pParam->parity;
            _moduleParam.serialParam.stopbits = pParam->stopbits;
#if defined(__linux__)
            _moduleParam.serialParam.handshake = pParam->handshake;
#endif // defined(__linux__)
        }

#if defined(__linux__)
        retVal = _connection->ser_open( _moduleParam.serialParam.device,
                                        _moduleParam.serialParam.baud,
                                        _moduleParam.serialParam.databit,
                                        _moduleParam.serialParam.parity,
                                        _moduleParam.serialParam.stopbits,
                                        _moduleParam.serialParam.handshake );
#else // NOT defined(__linux__)
    #if defined(ARDUINO)
        if( _moduleParam.serialParam.isHWPort )
        {
            retVal = _connection->ser_open( _moduleParam.serialParam.pHPort,
                                            _moduleParam.serialParam.baud,
                                            _moduleParam.serialParam.databit,
                                            _moduleParam.serialParam.parity,
                                            _moduleParam.serialParam.stopbits );
        }
        else
        {
            retVal = _connection->ser_open( _moduleParam.serialParam.pSPort,
                                            _moduleParam.serialParam.baud );
        }
    #endif // defined(ARDUINO)
#endif // defined(__linux__)
    }
    else
    {
        retVal = E_NULL_CONNECTION;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::disconnect( void )
 *
 * close connection to attached smart-TFT
 * return E_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::disconnect( void )
{
    int retVal;



    if( _connection != NULL )
    {
        retVal = _connection->ser_close( );
    }
    else
    {
        retVal = E_NULL_CONNECTION;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::enterCommandMode( void )
 *
 * If using a PC or MCU to dynamically modify the module parameters, after 
 * pulling  pin 5 (“SET”)  low wait at least 40ms before sending any AT 
 * commands to the module.
 * 
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::enterCommandMode( void )
{
    int retVal = HC12_ERR_OK;

printf("enter Command mode\n");

    if( (retVal = _status) == HC12_ERR_OK )
    {
        if( _moduleParam.setPin != HC12_NULLPIN )
        {
#if defined(RASPBERRY)
            gpioWrite(_moduleParam.setPin, HC12_SETPIN_CMD_MODE);
            usleep(60 * MILLISECONDS);
#else // NOT defined(RASPBERRY)
fprintf(stdout, "Please switch SET pin to GND and press <ENTER> when done.\n");
fprintf(stdout, "Press <ESC> tp cancel operation\n");
getchar();
#endif // defined(RASPBERRY)
        }

        _currOpMode = HC12_OP_CMD_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::leaveCommandMode( void )
 *
 * After releasing pin 5 (“SET”), wait at least 80ms for the module to 
 * return to serial port pass-through mode.
 * 
 * return HC12_ERR_OKE_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::leaveCommandMode( void )
{
    int retVal = HC12_ERR_OK;

printf("leave Command mode\n");
    if( (retVal = _status) == HC12_ERR_OK )
    {
        if( _moduleParam.setPin != HC12_NULLPIN )
        {
#if defined(RASPBERRY)
            gpioWrite(_moduleParam.setPin, HC12_SETPIN_TT_MODE);
            usleep(100 * MILLISECONDS);
#else // NOT defined(RASPBERRY)
fprintf(stdout, "Please switch SET pin back to Vcc and press <ENTER> when done.\n");
fprintf(stdout, "Press <ESC> tp cancel operation\n");
getchar();
#endif // defined(RASPBERRY)
        }

        _currOpMode = HC12_OP_TT_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * void hc12Radio::init( void )
 *
 * initialize internal structs
 *
 * return nothing
 ------------------------------------------------------------------------------
*/
void hc12Radio::init( void )
{

#if defined(RASPBERRY)
    if( _moduleParam.setPin != HC12_NULLPIN ||
        _moduleParam.powerPin != HC12_NULLPIN )
    {
        if (gpioInitialise() < 0)
        {
            _status = HC12_ERR_INIT_PIGPIO;
sprintf(hc12LogBuffer, "ERR init pigpio\n");
doLog(DEBUG_LEVEL_1);
        }
        else
        {
            _status = HC12_ERR_OK;

            if( _moduleParam.setPin != HC12_NULLPIN )
            {
                gpioSetMode(_moduleParam.setPin, PI_OUTPUT);
                gpioWrite(_moduleParam.setPin, HC12_SETPIN_TT_MODE);
            }

            if( _moduleParam.powerPin != HC12_NULLPIN )
            {
                gpioSetMode(_moduleParam.powerPin, PI_OUTPUT);
                gpioWrite(_moduleParam.powerPin, HC12_POWERPIN_ON);
            }
        }
    }
#endif // defined(RASPBERRY)

    _moduleParam.comChannel = HC12_DEFAULT_CHANNEL;
    _moduleParam.ttMode = HC12_DEFAULT_TTMODE;
    _moduleParam.power = HC12_DEFAULT_POWER;
 
#if defined(__linux__)
    _moduleParam.serialParam.device = strdup( (char*) HC12_DEFAULT_DEVICE );

    _moduleParam.serialParam.dev_fd = -1;

    memset( &_moduleParam.serialParam.oldtio, '\0', sizeof(struct termios) );
    memset( &_moduleParam.serialParam.rawtio, '\0', sizeof(struct termios) );
#else // NOT defined(__linux__)
    #if defined(ARDUINO)
    _moduleParam.serialParam.pHPort = NULL;
    _moduleParam.serialParam.pSPort = NULL;
    _moduleParam.serialParam.isHWPort = false;
    #endif // defined(ARDUINO)
#endif // defined(__linux__)

    _moduleParam.serialParam.baud = HC12_DEFAULT_BAUD;
    _moduleParam.serialParam.databit = HC12_DEFAULT_DATABIT;
    _moduleParam.serialParam.parity = HC12_DEFAULT_PARITY;
    _moduleParam.serialParam.stopbits = HC12_DEFAULT_STOPBITS;
    _moduleParam.serialParam.handshake = HC12_DEFAULT_HANDSHAKE;

    _moduleParam.hwInfo.major = HC12_DEFAULT_V_MAJOR;
    _moduleParam.hwInfo.minor = HC12_DEFAULT_V_MINOR;
    _moduleParam.hwInfo.patch  = HC12_DEFAULT_V_PATCH;

    _interfaceType = HC12_DEFAULT_INTERFACE;
    _currOpMode = HC12_DEFAULT_OPMODE;

    memset( _ioBuffer, '\0', IO_BUFFER_SIZE );

}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::test( void )
 *
 * AT
 * Test command. Send command “AT” to the module, and the module returns “OK”.
 *
 * return HC12_ERR_OKE_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::test( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_TEST );
        _currentCommand = HC12_CMD_CODE_TEST;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
            if( (retVal = strncmp( _ioBuffer, HC12_RSP_TEST, 
                               strlen(HC12_RSP_TEST))) == 0 )
            {
                retVal = HC12_ERR_OK;
            }
            else
            {
                retVal = HC12_ERR_RESPONSE;
            }
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
//sprintf(hc12LogBuffer, "ERR send request failed [%d]\n", retVal);
// doLog(DEBUG_LEVEL_1);
        }

    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * void hc12Radio::reset( void )
 *
 * reset internal vars to their defaults
 *
 * return nothing
 ------------------------------------------------------------------------------
*/
void hc12Radio::reset( void )
{
    _moduleParam.comChannel = HC12_DEFAULT_CHANNEL;
    _moduleParam.ttMode = HC12_DEFAULT_TTMODE;
    _moduleParam.power = HC12_DEFAULT_POWER;
 
    _moduleParam.serialParam.baud = HC12_DEFAULT_BAUD;
    _moduleParam.serialParam.databit = HC12_DEFAULT_DATABIT;
    _moduleParam.serialParam.parity = HC12_DEFAULT_PARITY;
    _moduleParam.serialParam.stopbits = HC12_DEFAULT_STOPBITS;
    _moduleParam.serialParam.handshake = HC12_DEFAULT_HANDSHAKE;

}

/* 
 ------------------------------------------------------------------------------
 * int setDefault( void )
 *
 * AT+DEFAULT
 * Set serial port baud rate and configuration, communication channel, power,
 * and serial port transparent transmission mode back to the factory default 
 * values.
 * e.g: Send command “AT+DEFAULT” to the module, and the module returns
 * “OK+DEFAULT”, with the factory default values restored. The factory default
 * serial port baud rate is 9600bps, 8 data bits, no parity, 1 stop bit,
 * communication channel is 001, transmitting power is 20dBm, and serial port
 * transparent transmission mode is FU3.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setDefault( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_SET_DEFAULT );
        _currentCommand = HC12_CMD_CODE_SET_DEFAULT;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
            if( (retVal = strncmp( _ioBuffer, HC12_RSP_SET_DEFAULT, 
                               strlen(HC12_RSP_SET_DEFAULT))) == 0 )
            {
                reset();
                retVal = HC12_ERR_OK;
            }
            else
            {
                retVal = HC12_ERR_RESPONSE;
            }
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::goSleepMode( void )
 *
 * AT+SLEEP
 * After receiving this command, the module will enter sleep mode upon 
 * exiting from command mode, with a working current of about 22uA. 
 * This mode doesn’t allow serial port data transmission. Upon entering 
 * command mode again the module will exit from sleep mode automatically.
 * e.g: When wireless data transmission is not needed, to save power send 
 * command “AT+SLEEP” to the module, and the module will return “OK+SLEEP”. 
 * Upon exit from command mode the working current will drop to about 22uA.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::goSleepMode( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_SLEEP );
        _currentCommand = HC12_CMD_CODE_SLEEP;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;


        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
            if( (retVal = strncmp( _ioBuffer, HC12_RSP_SLEEP, 
                               strlen(HC12_RSP_SLEEP))) == 0 )
            {
                retVal = HC12_ERR_OK;
            }
            else
            {
                retVal = HC12_ERR_RESPONSE;
            }
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::goUpdateMode( void )
 *
 * AT+UPDATE
 * Puts the module in the state of waiting for a software update. 
 * After receiving this command the module will not respond to any further 
 * AT commands until power has been cycled.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::goUpdateMode( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_UPDATE );
        _currentCommand = HC12_CMD_CODE_UPDATE;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;
        sendRequest();
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setBaud( uint32_t baud )
 *
 * AT+Bxxxx
 * Change the serial port baud rate. The baud rate can be set to 1200bps, 
 * 2400bps, 4800bps, 9600bps, 19,200bps, 38,400bps, 57,600bps, or 115,200bps. 
 * The default value is 9600bps.
 * e.g: To set the serial port baud rate of the module to 19,200bps, send 
 * command “AT+B19200” to the module, and the module will return “OK+B19200”. 
 * After exiting from command mode, the module will begin to communicate at 
 * 19,200bps.
 *
 * return HC12_ERR_OKE_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setBaud( uint32_t baud )
{
    int retVal = HC12_ERR_OK;
    uint32_t newBaud;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        if( isValidBaud( baud ) )
        {
            sprintf( _ioBuffer, HC12_CMD_SET_BAUD, baud );
            _currentCommand = HC12_CMD_CODE_SET_BAUD;
            _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;

            if( (retVal = sendRequest()) == E_OK )
            {
                _commandStatus = HC12_CMD_STATUS_ACTIVE;
                if( sscanf( _ioBuffer, HC12_RSP_SET_BAUD, 
                             &newBaud ) == HC12_ARGS_RSP_SET_BAUD )
                {
                    if( baud == newBaud )
                    {    
                        _moduleParam.serialParam.baud = baud;
                        retVal = HC12_ERR_OK;
                    }
                    else
                    {
                        retVal = HC12_ERR_FAIL;
                    }
                }
                else
                {
                    retVal = HC12_ERR_ARGS;
                }
            }
            else
            {
                _commandStatus = HC12_CMD_STATUS_FAILED;
            }
        }
        else
        {
            retVal = HC12_ERR_BAUD;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}


/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setComChannel( int chan )
 *
 * AT+Cxxx
 * Change wireless communication channel, selectable from 001 to 127 
 * (for wireless channels exceeding 100, the communication distance cannot 
 * be guaranteed). The default value for the wireless channel is 001, with 
 * a working frequency of 433.4MHz. The channel stepping is 400KHz, and 
 * the working frequency of channel100 is 473.0MHz.
 * e.g: To set the module to work on channel 21, send command “AT+C021” to the
 * module, and the module will return “OK+C021”. After exiting from command 
 * mode, the module will work on channel 21, with a working frequency of 
 * 441.4MHz.
 * Note: As the wireless receiving sensitivity of the HC-12 module is 
 * relatively high, when the serial port baud rate  greater than 9600bps 
 * five adjacent channels should be staggered for use. Even when the serial 
 * port baud rate is not greater than 9600bps, over short distances 
 * (less than 10m) also five adjacent channels should be staggered for use.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setComChannel( int chan )
{
    int retVal = HC12_ERR_OK;
    int newChan;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        if( isValidChannel( chan ) )
        {
            sprintf( _ioBuffer, HC12_CMD_SET_CHANNEL, chan );
            _currentCommand = HC12_CMD_CODE_SET_CHANNEL;
            _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;

            if( (retVal = sendRequest()) == E_OK )
            {
                _commandStatus = HC12_CMD_STATUS_ACTIVE;
                if( sscanf( _ioBuffer, HC12_RSP_SET_CHANNEL, 
                             &newChan ) == HC12_ARGS_RSP_SET_CHANNEL )
                {
                    if( chan == newChan )
                    {    
                        _moduleParam.comChannel = chan;
                        retVal = HC12_ERR_OK;
                    }
                    else
                    {
                        retVal = HC12_ERR_FAIL;
                    }
                }
                else
                {
                    retVal = HC12_ERR_ARGS;
                }
            }
            else
            {
                _commandStatus = HC12_CMD_STATUS_FAILED;
            }
        }
        else
        {
            retVal = HC12_ERR_CHANNEL;
        }
}
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}


/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setTTMode( int mode )
 *
 * AT+FUx
 * Change the serial port transparent transmission mode of the module. 
 * Four modes are available, namely FU1, FU2, FU3, and FU4. Only when the 
 * serial port speed, channel, and transparent transmission mode of two 
 * modules is set to be the same, can normal wireless communications occur. 
 * e.g: Send command “AT+FU1” to the module, and the module returns “OK+FU1”.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setTTMode( int mode )
{
    int retVal = HC12_ERR_OK;
    int newTTMode;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        if( isValidTTMode( mode ) )
        {
            sprintf( _ioBuffer, HC12_CMD_SET_TTMODE, mode );
            _currentCommand = HC12_CMD_CODE_SET_TTMODE;
            _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;


            if( (retVal = sendRequest()) == E_OK )
            {
                _commandStatus = HC12_CMD_STATUS_ACTIVE;
                if( sscanf( _ioBuffer, HC12_RSP_SET_TTMODE, 
                             &newTTMode ) == HC12_ARGS_RSP_SET_TTMODE )
                {
                    if( mode == newTTMode )
                    {    
                        _moduleParam.ttMode = mode;
                        retVal = HC12_ERR_OK;
                    }
                    else
                    {
                        retVal = HC12_ERR_FAIL;
                    }
                }
                else
                {
                    retVal = HC12_ERR_ARGS;
                }
            }
            else
            {
                _commandStatus = HC12_CMD_STATUS_FAILED;
            }
        }
        else
        {
            retVal = HC12_ERR_TTMODE;
        }
}
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setTPower( int power )
 *
 * AT+Px
 * Set the transmitting power of the module, with x selectable from 1 to 8. 
 * The corresponding transmitting power of the module is as shown below:
 *
 *  +---------------+------+-----+-----+-----+------+------+------+------+
 *  |   x value     |  1   |  2  |  3  |  4  |   5  |   6  |   7  |  8   |
 *  +---------------+------+-----+-----+-----+------+------+------+------+
 *  |Transmitting   |-1 dBm|2 dBm|5 dBm|8 dBm|11 dBm|14 dBm|17 dBm|20 dBm|
 *  |power of module| 0.8mW|1.6mW|3.2mW|6.3mW| 12mW | 25mW | 50mW | 100mW|
 *  +---------------+------+-----+-----+-----+------+------+------+------+
 *
 * The default value is 8, and the higher the transmitting power, the farther
 * the possible wireless communication distance. When the transmitting power 
 * level is set to 1, the transmitting power is at the minimum. Generally 
 * speaking, every time the transmitting power is reduced by 6dB, the 
 * communication distance will be reduced by half.
 * e.g: Send command “AT+P5” to the module, and the module returns “OK+P5”.
 * After exiting from command mode, the transmitting power of the module 
 * will be set to 11dBm.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setTPower( int power )
{
    int retVal = HC12_ERR_OK;
    int newPower;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        if( isValidPower( power ) )
        {
            sprintf( _ioBuffer, HC12_CMD_SET_POWER, power );
            _currentCommand = HC12_CMD_CODE_SET_POWER;
            _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;

            if( (retVal = sendRequest()) == E_OK )
            {
                _commandStatus = HC12_CMD_STATUS_ACTIVE;
                if( sscanf( _ioBuffer, HC12_RSP_SET_POWER, 
                             &newPower ) == HC12_ARGS_RSP_SET_POWER )
                {
                    if( power == newPower )
                    {    
                        _moduleParam.power = power;
                        retVal = HC12_ERR_OK;
                    }
                    else
                    {
                        retVal = HC12_ERR_FAIL;
                    }
                }
                else
                {
                    retVal = HC12_ERR_ARGS;
                }
            }
            else
            {
                _commandStatus = HC12_CMD_STATUS_FAILED;
            }
        }
        else
        {
            retVal = HC12_ERR_POWER;
        }
}
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setParam( struct _hc12_param* pParam )
 *
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setParam( struct _hc12_param* pParam )
{
    int retVal = HC12_ERR_OK;
// HC12_CMD_SET_PARAM
// HC12_RSP_SET_PARAM
// HC12_ARGS_RSP_SET_PARAM
// TODO

    _currentCommand = HC12_CMD_CODE_SET_PARAM;
    _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;
    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::setSerialParam( int databits, char parity, int stopbits )
 *
 * AT+Udps
 * Set data bits (d), parity (p), and stop bits (s) for serial port 
 * communication. For parity, N means none, O means odd check, and E means 
 * even check. For stop bits, 1 means one stop bit, 2 means two stop bits, 
 * and 3 means 1.5 stop bits.
 * e.g: To set the serial port format to eight data bits, odd parity, and 
 * one stop bit, send command “AT+U8O1” to the module. 
 * The module will return “OK+U8O1”.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::setSerialParam( int databits, char parity, int stopbits )
{
    int retVal = HC12_ERR_OK;
    int newDatabits; 
    char newParity;
    int newStopbits;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        if( isValidDatabits( databits ) )
        {
            if( isValidParity( parity ) )
            {
                if( isValidStopbits( stopbits ) )
                {
                    sprintf( _ioBuffer, HC12_CMD_SET_SERIAL, databits, 
                             parity, stopbits );

                    _currentCommand = HC12_CMD_CODE_SET_SERIAL;
                    _commandStatus = HC12_CMD_STATUS_REQUEST;
                    _responseArgs = 0;

                    if( (retVal = sendRequest()) == E_OK )
                    {
                        _commandStatus = HC12_CMD_STATUS_ACTIVE;

                        if( sscanf( _ioBuffer, HC12_RSP_SET_SERIAL, 
                                   &newDatabits, &newParity, &newStopbits ) == 
                                   HC12_ARGS_RSP_SET_SERIAL )
                        {
                            if( ( databits == newDatabits ) &&
                                ( parity == newParity ) &&
                                ( stopbits == newStopbits ) )
                            {    
                                _moduleParam.serialParam.databit = databits;
                                _moduleParam.serialParam.parity = parity;
                                _moduleParam.serialParam.stopbits = stopbits;
                                retVal = HC12_ERR_OK;
                            }
                            else
                            {
                                retVal = HC12_ERR_FAIL;
                            }
                        }
                        else
                        {
                            retVal = HC12_ERR_ARGS;
                        }
                    }
                    else
                    {
                        _commandStatus = HC12_CMD_STATUS_FAILED;
                    }
                }
                else
                {
                    retVal = HC12_ERR_STOPBITS;
                }
            }
            else
            {
                retVal = HC12_ERR_PARITY;
            }
        }
        else
        {
            retVal = HC12_ERR_DATABITS;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}


/* ****************************************************************************
 *
 * AT+Ry
 * Obtain a single parameter from the module, where y is any letter among B, 
 * C, F, and P, respectively representing: baud rate, communication channel, 
 * serial port transparent transmission mode, and transmitting power.
 *
 * ****************************************************************************
*/

/* 
 ------------------------------------------------------------------------------
 * uint32_t hc12Radio::getBaud( void )
 *
 * Example 1:
 * Send command “AT+RB” to the module, and if the module returns “OK+B9600” 
 * it is confirmed that the serial port baud rate of the module is 9600bps.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getBaud( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_GET_BAUD );
        _currentCommand = HC12_CMD_CODE_GET_BAUD;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getComChannel( void )
 *
 * Example 2:
 * Send command “AT+RC” to the module, and if the module returns “OK+RC001” 
 * it is confirmed that the communication channel of the module is 001.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getComChannel( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_GET_CHANNEL );
        _currentCommand = HC12_CMD_CODE_GET_CHANNEL;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getTTMode( void )
 *
 * Example 3:
 * Send command “AT+RF” to the module, and if the module returns “OK+FU3” 
 * it is confirmed that the module is working in serial port transparent 
 * transmission mode FU3.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getTTMode( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_GET_TTMODE );
        _currentCommand = HC12_CMD_CODE_GET_TTMODE;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getTPower( void )
 *
 * Example 4:
 * Send command “AT+RP” to the module, and if the module returns “OK+RP:+20dBm”
 * it is confirmed that the transmitting power of module is set to 
 * 20dBm (100mW).
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getTPower( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_GET_POWER );
        _currentCommand = HC12_CMD_CODE_GET_POWER;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getParam( struct _hc12_param* pParam )
 *
 * AT+RX
 * Obtain all parameters from the module. Returns serial port transparent
 * transmission mode, serial port baud rate, communication channel, and
 * transmitting power in that order.
 * e.g: Send command “AT+RX” to the module, and the module returns 
 * “OK+FU3\r\nOK+B9600\r\n OK+C001\r\n OK+RP:+20dBm\r\n”. 
 * (“\r\n” means return\newline)
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getParam( void )
{
    int retVal = HC12_ERR_OK;

    if( _currOpMode == HC12_OP_CMD_MODE )
    {
        sprintf( _ioBuffer, HC12_CMD_GET_PARAM );
        _currentCommand = HC12_CMD_CODE_GET_PARAM;
        _commandStatus = HC12_CMD_STATUS_REQUEST;
        _responseArgs = 0;

        if( (retVal = sendRequest()) == E_OK )
        {
            _commandStatus = HC12_CMD_STATUS_ACTIVE;
        }
        else
        {
            _commandStatus = HC12_CMD_STATUS_FAILED;
        }
    }
    else
    {
        retVal = HC12_ERR_OP_MODE;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getSerialParam( int *databits, char *parity, int *stopbits );
 *
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getSerialParam( int *databits, char *parity, int *stopbits )
{
    int retVal = HC12_ERR_OK;

    _currentCommand = HC12_CMD_CODE_GET_SERIAL;
    _commandStatus = HC12_CMD_STATUS_REQUEST;
    _responseArgs = 0;

    if( ( databits != NULL ) && ( parity != NULL ) && ( stopbits != NULL ) )
    {
        retVal = HC12_ERR_OK;

        *databits = _moduleParam.serialParam.databit;
        *parity = _moduleParam.serialParam.parity;
        *stopbits = _moduleParam.serialParam.stopbits;
        _commandStatus = HC12_CMD_STATUS_DONE;
        _currentCommand = HC12_CMD_CODE_NULL;
    }
    else
    {
        retVal = HC12_ERR_NULLP;
    }

    return( retVal );
}

/* 
 ------------------------------------------------------------------------------
 * int hc12Radio::getFWVersion( void )
 *
 * AT+V
 * Request firmware version information from the module.
 * e.g: Send command “AT+V” to the module, and the module returns “HC-12_V2.3”.
 *
 * return HC12_ERR_OK on succes, otherwise an error code
 ------------------------------------------------------------------------------
*/
int hc12Radio::getFWVersion( void )
{
    int retVal = HC12_ERR_OK;

        if( _currOpMode == HC12_OP_CMD_MODE )
        {
            sprintf( _ioBuffer, HC12_CMD_GET_VERSION );
            _currentCommand = HC12_CMD_CODE_GET_VERSION;
            _commandStatus = HC12_CMD_STATUS_REQUEST;
            _responseArgs = 0;

            if( (retVal = sendRequest()) == E_OK )
            {
                _commandStatus = HC12_CMD_STATUS_ACTIVE;
            }
            else
            {
                _commandStatus = HC12_CMD_STATUS_FAILED;
            }
        }
        else
        {
            retVal = HC12_ERR_OP_MODE;
        }

    return( retVal );
}



//
#ifdef NEVERDEF


155|0x9B|CI|CSI| Control Sequence Intro 	
               | Einleitung einer Steuersequenz. Siehe ANSI-Escapesequenz.
156|0x9C|SI|ST | String Terminator
               | Zeichen für das Ende einer Zeichenkette, die mit 
               | APC, DCS, OSC, PM oder SOS begonnen wurde.
157|0x9D|OC|OSC| Operating System Command
               | Markiert den Beginn einer „Operating System Command“-
               | Zeichenkette, die mit ST („String Terminator“) beendet wird. 
               | Die Interpretation der Zeichenkette obliegt dem jeweiligen 
               | Betriebssystem.
158|0x9E|PM|PM | Privacy Message
               | Markiert den Beginn einer „Privacy Message“, die mit 
               | ST („String Terminator“) beendet wird.
159|0x9F|AC|APC| Application Program Command
               | Markiert den Beginn einer „Application Program Command“-
               | Zeichenkette, die mit ST („String Terminator“) beendet wird. 
               | Die Interpretation der Zeichenkette obliegt dem jeweiligen 
               | Programm.
















#define HC12_ARGS_RSP_SET_DEFAULT   0
#define HC12_ARGS_RSP_SLEEP         0
#define HC12_ARGS_RSP_UPDATE        0
#define HC12_ARGS_RSP_SET_BAUD      1
#define HC12_ARGS_RSP_SET_CHANNEL   1
#define HC12_ARGS_RSP_SET_TTMODE    1
#define HC12_ARGS_RSP_SET_POWER     1
#define HC12_ARGS_RSP_SET_PARAM     0
#define HC12_ARGS_RSP_SET_SERIAL    3
#define HC12_ARGS_RSP_GET_BAUD      1
#define HC12_ARGS_RSP_GET_CHANNEL   1
#define HC12_ARGS_RSP_GET_TTMODE    1
#define HC12_ARGS_RSP_GET_POWER     1
#define HC12_ARGS_RSP_GET_PARAM     4
#define HC12_ARGS_RSP_GET_SERIAL    0
#define HC12_ARGS_RSP_GET_VERSION   2

#define HC12_CMD_STATUS_REQUEST     9
#define HC12_CMD_STATUS_ACTIVE     90
#define HC12_CMD_STATUS_DONE       99

// ///////////////////////

_currentCommand = HC12_CMD_CODE_TEST;
_currentCommand = HC12_CMD_CODE_SET_DEFAULT;
_currentCommand = HC12_CMD_CODE_SLEEP;
_currentCommand = HC12_CMD_CODE_UPDATE;
_currentCommand = HC12_CMD_CODE_SET_BAUD;
_currentCommand = HC12_CMD_CODE_SET_CHANNEL;
_currentCommand = HC12_CMD_CODE_SET_TTMODE;
_currentCommand = HC12_CMD_CODE_SET_POWER;
_currentCommand = HC12_CMD_CODE_SET_PARAM;
_currentCommand = HC12_CMD_CODE_SET_SERIAL;
_currentCommand = HC12_CMD_CODE_GET_BAUD;
_currentCommand = HC12_CMD_CODE_GET_CHANNEL;
_currentCommand = HC12_CMD_CODE_GET_TTMODE;
_currentCommand = HC12_CMD_CODE_GET_POWER;
_currentCommand = HC12_CMD_CODE_GET_PARAM;
_currentCommand = HC12_CMD_CODE_GET_SERIAL;
_currentCommand = HC12_CMD_CODE_GET_VERSION;

_currentCommand = HC12_CMD_CODE_NULL;
_commandStatus = HC12_CMD_STATUS_REQUEST;
_commandStatus = HC12_CMD_STATUS_ACTIVE;
_commandStatus = HC12_CMD_STATUS_DONE;



#endif // NEVERDEF
