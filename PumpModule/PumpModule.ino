#include <SPI.h>
#include <RH_NRF24.h>
void setup()
{
    while (1)
    {
        Serial.begin(9600);
        Serial.println("1. Switch On\n2. Switch Off:-\n");
        getNumber();
    }
}
void loop(){
    
}
void getNumber()
{
    while (1)
    {
        if (Serial.available())
        {
            int l;
            l = Serial.read();
            if (l != 10)
            {
                if (l == '1')
                {
                    Serial.println("Switch Turned On");
                    analogWrite(6, 255);
                    break;
                }
                else if(l == '2')
                {
                    Serial.println("Switch Turned Off");
                    analogWrite(6, 0);
                    break;
                }
                else{
                    Serial.println("Wrong Input");
                    break;
                }
            }
        }
    }
}