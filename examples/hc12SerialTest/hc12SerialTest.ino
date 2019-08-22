#include <hc12Radio.h>
#include <SoftwareSerial.h>

#define SOFT_RX_PIN                     5
#define SOFT_TX_PIN                     4
#define HC12_SET_PIN                    6

#define COMPORT            "/dev/ttyUSB0"

SoftwareSerial hc12Serial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX
hc12Radio *pHC12;

void setup() 
{
    struct _hc12_serial_param ctl_param;

    pinMode(HC12_SET_PIN,      OUTPUT);
    digitalWrite(HC12_SET_PIN, HIGH);
    delay(5);

//    ctl_param.device = (char*) COMPORT;
    ctl_param.baud = 9600;
    ctl_param.databit = 8;
    ctl_param.parity = 'N';
    ctl_param.stopbits = 1;
//    ctl_param.handshake = 'N';

    if( (pHC12 = new hc12Radio(HC12_SET_PIN, &hc12Serial)) != NULL )
    {
        if( pHC12->connect(&ctl_param) != E_OK )
        {
            delete pHC12;
            pHC12 = NULL;
        }
    }
}

void loop() 
{
    if( pHC12 != NULL )
    {
        delay(1000);
    }
}
