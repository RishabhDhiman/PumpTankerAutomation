#include <SPI.h>
#include <RH_NRF24.h>

#define RELAY_PIN 6
#define PUMP_ON_LED 5
#define PUMP_OFF_LED 3
RH_NRF24 nrf24;
struct Data
{
    int TANK_HEIGHT_2 = 0;
    float waterLevel;
    uint8_t command;
};

void setup()
{
    Serial.begin(9600);
    initializeTransciver();
}
void loop()
{
    checkForData();
}

void checkForData()
{
    if (nrf24.available())
    {
        uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (nrf24.recv((uint8_t *)&buf, &len))
        {
            if (((struct Data *)buf)->command == 0)
            {
                Serial.print("Turn Off Signal Received ");
                analogWrite(RELAY_PIN, 0);
            }
            else if (((struct Data *)buf)->command == 1)
            {
                Serial.print("Turn On Signal Received ");
                analogWrite(RELAY_PIN, 255);
            }
        }
        else
        {
            Serial.println("recv failed");
        }
    }
}

void initializeTransciver()
{
    if (!nrf24.init())
        Serial.println("Init Failed");
    else
    {
        Serial.println("Init Success");
    }
    // Defaults after init are 2.402 GHz(channel 2), 2Mbps, 0dBm
    if (!nrf24.setChannel(1))
        Serial.println("SetChannel Failed");
    else
    {
        Serial.println("SetChannel Succcess");
    }

    if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPowerm20dBm))
        Serial.println("SetRF Failed");
    else
    {
        Serial.println("Set RF Success");
    }
}