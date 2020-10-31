/*
 Name:		Transformerstation_Node.ino
 Created:	30.10.2020 13:16:53
 Author:	w-tim
*/



/*
Payload Format:
Byte 0  node_id
Byte 1  water_conductivity
Byte 2  distance_hi
Byte 3  distance_lo
Byte 4  pump_state


*/
#include "loramodem.h"
#include "credentials.h" //appeui and appkey are stored here

#define PUMP_PIN 12
LoRaWANModem modem;

uint8_t node_id = 0x01;

long nextSend = 0;
int sendinterval = 60000;
long nextRead = 0;
int readInterval = 1000;

int nextlevelread = 0;
int levelreadinterval = 500;

bool pump_on = false;


uint8_t conductivity = 0;


#define HC_TRIG_PIN 10
#define HC_ECHO_PIN 9
long duration;
int dist;
uint8_t disthb = 0;
uint8_t distlb = 0;
uint8_t presshb = 0;
uint8_t presslb = 0;

void getDistance() {
    digitalWrite(HC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(HC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HC_TRIG_PIN, LOW);
    duration = pulseIn(HC_ECHO_PIN, HIGH);
    dist = duration * 0.343 / 2;
    if (dist > 3000) {
        dist = 3000;
    }
    disthb = dist >> 8;
    distlb = dist;
}

void getConductivity() {
    conductivity = map(analogRead(A0), 0, 1023, 255, 0);
}
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);
    // while (!Serial);
    pinMode(PUMP_PIN, OUTPUT);

    modem.begin();
    modem.info();
    modem.join(appeui, appkey);
}

// the loop function runs over and over again until power down or reset
void loop() {
    if (millis() > nextRead) {
        
        getDistance();
        getConductivity();
        Serial.println("Distance: " + String(dist) + " mm");
        Serial.println("Conductivity: " + String(conductivity) + " [0 .. 255]");
        Serial.print("payload: ");
        Serial.print(String(node_id, HEX) + " ");
        Serial.print(String(conductivity, HEX) + " ");
        Serial.print(String(disthb, HEX) + " ");
        Serial.print(String(distlb, HEX) + " ");
        Serial.print(String(presshb, HEX) + " ");
        Serial.println(String(presslb, HEX));



        nextRead = millis() + readInterval;
    }
    
    if (millis() > nextSend) {
        Serial.println("sending");
        uint8_t payload[5] = { node_id,conductivity,disthb,distlb,pump_on };
        modem.send(payload, 5);
        nextSend = millis() + sendinterval;
    }

    if (millis() > nextlevelread) {
        getDistance();
        if (!pump_on) {
            if (dist < 100) {
                pump_on = true;
            }
        }
        else {
            if (dist > 150) {
                pump_on = false;
            }
        }
        
        nextlevelread = millis() + levelreadinterval;
    }
    
    if (pump_on) {
        digitalWrite(PUMP_PIN, HIGH);
        
    }
    else {
        digitalWrite(PUMP_PIN, LOW);
    }
    
}
