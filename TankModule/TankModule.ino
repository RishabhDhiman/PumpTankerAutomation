#include "Ultrasonic.h"
#include <SPI.h>
#include <RH_NRF24.h>
#include <LowPower.h>
#include <EEPROM.h>

/**
 * MIN_DISTANCE_ADDRESS_1 ,MIN_DISTANCE_ADDRESS_1 referce to the distance of the water from the arduino module. Water cannot rise above this level 
 * becuase it can touch the Arduino or start flowing from pipe at the top of the tank. This variable is used to store the maximum height of water 
 * the tank can store.
 */
#define MIN_DISTANCE_ADDRESS_1 0
#define MIN_DISTANCE_ADDRESS_2 1

/**
 * MAX_DISTANCE_ADDRESS_1 ,MAX_DISTANCE_ADDRESS_2 This variable is used to store the address of the minimum level of water after which the pump must start.
 */
#define MAX_DISTANCE_ADDRESS_1 2
#define MAX_DISTANCE_ADDRESS_2 3
#define TANK_HEIGHT_ADDRESS_1 4
#define TANK_HEIGHT_ADDRESS_2 5
int MIN_DISTANCE_1 = 0;
int MIN_DISTANCE_2 = 0;
int MAX_DISTANCE_1 = 0;
int MAX_DISTANCE_2 = 0;
int TANK_HEIGHT_1 = 0;
int TANK_HEIGHT_2 = 0;
int TANK_HEIGHT = 0;
int currentWaterLevel = 0;
/**
 * MIN_WATER_LEVEL This variable is used to store the minimum level of water after which the pump must start. 
 */
int MIN_WATER_LEVEL = 0;
/**
 * This variable is used to store the maximum height of water the tank can store.
 */
int MAX_WATER_LEVEL = 0;
RH_NRF24 nrf24;
struct Data
{
    int TANK_HEIGHT_2 = 0;
    float waterLevel;
    uint8_t command;
};
Ultrasonic ultrasonic(2, 4);
void setup()
{
    Serial.begin(9600);
    initializeTransciver();
    initializeMinMaxDistance();
}
void loop()
{
    currentWaterLevel = ultrasonic.Ranging(CM);
    Serial.println(currentWaterLevel);
    if (currentWaterLevel >= MIN_WATER_LEVEL && currentWaterLevel>MAX_WATER_LEVEL)
    {
        Data d;
        d.command = 1;
        d.waterLevel = currentWaterLevel / TANK_HEIGHT;
        sendData(d);
        Serial.println("Turning Pump On");
    }
    else if (currentWaterLevel <= MAX_WATER_LEVEL)
    {
        Data d;
        d.command = 0;
        d.waterLevel = currentWaterLevel / TANK_HEIGHT;
        sendData(d);
        Serial.println("Turning Pump Off");
    }
    delay(300);
    for (int i = 0; i < 0; i++)
    {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}

void initializeMinMaxDistance()
{
    MIN_DISTANCE_1 = EEPROM.read(MIN_DISTANCE_ADDRESS_1);
    MIN_DISTANCE_2 = EEPROM.read(MIN_DISTANCE_ADDRESS_2);
    MAX_DISTANCE_1 = EEPROM.read(MAX_DISTANCE_ADDRESS_1);
    MAX_DISTANCE_2 = EEPROM.read(MAX_DISTANCE_ADDRESS_2);
    TANK_HEIGHT_1 = EEPROM.read(TANK_HEIGHT_ADDRESS_1);
    TANK_HEIGHT_2 = EEPROM.read(TANK_HEIGHT_ADDRESS_2);
    /**
     * Checking if the EEPROM contains any default value. If not then a default value is written to EEPROM
     * Which is 15 cm for the minimum distance from tank's top which is 5 cm Device height + 10 cm distance
     * and 44 cm for the maximum distance from tank's top which is 5 cm Device height + 39 cm distance
     * */
    Serial.print("MIN_DISTANCE_1 ");Serial.println(MIN_DISTANCE_1);
    Serial.print("MIN_DISTANCE_2 ");Serial.println(MIN_DISTANCE_2);
    Serial.print("MAX_DISTANCE_1 ");Serial.println(MAX_DISTANCE_1);
    Serial.print("MAX_DISTANCE_2 ");Serial.println(MAX_DISTANCE_2);
    Serial.print("Tank Height_1 ");Serial.println(TANK_HEIGHT_1);
    Serial.print("Tank Height_2 ");Serial.println(TANK_HEIGHT_2);
    if (MIN_DISTANCE_1 == 255 && MIN_DISTANCE_2 == 255)
    {
        EEPROM.write(MIN_DISTANCE_ADDRESS_1, 10);
        EEPROM.write(MIN_DISTANCE_ADDRESS_2, 0);
        MIN_DISTANCE_1 = 10;
        MIN_DISTANCE_2 = 0;
    }
    if (MAX_DISTANCE_1 == 255 && MAX_DISTANCE_2 == 255)
    {
        EEPROM.write(MAX_DISTANCE_ADDRESS_1, 39);
        EEPROM.write(MAX_DISTANCE_ADDRESS_2, 0);
        MAX_DISTANCE_1 = 39;
        MAX_DISTANCE_2 = 0;
    }
    if (TANK_HEIGHT_1 == 255 && TANK_HEIGHT_2 == 255)
    {
        EEPROM.write(TANK_HEIGHT_ADDRESS_1, 78);
        EEPROM.write(TANK_HEIGHT_ADDRESS_2, 0);
        TANK_HEIGHT_1 = 78;
        TANK_HEIGHT_2 = 0;
    }
    MAX_WATER_LEVEL = MIN_DISTANCE_1 + MIN_DISTANCE_2;
    MIN_WATER_LEVEL = MAX_DISTANCE_1 + MAX_DISTANCE_2;
    TANK_HEIGHT = TANK_HEIGHT_1 + TANK_HEIGHT_2;

    Serial.print("Min Water Level ");
    Serial.println(MIN_WATER_LEVEL);

    Serial.print("Max Water Level ");
    Serial.println(MAX_WATER_LEVEL);

    Serial.print("Tank Height ");
    Serial.println(TANK_HEIGHT);
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

void sendData(Data ds)
{
    boolean b = nrf24.send((uint8_t *)&ds, sizeof(struct Data));
    if (b)
    {
        Serial.println("Send Success");
    }
    else
    {
        Serial.println("Send Failed");
    }
    nrf24.waitPacketSent();
}
