#include <SPI.h>
#include <LoRa.h>
#include <MD_TCS230.h>
#include <FreqCount.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <cactus_io_BME280_I2C.h>

//LoR32u4 433MHz V1.2 (white board)
#define SCK     15
#define MISO    14
#define MOSI    16
#define SS      8
#define RST     4
#define DI0     7
#define BAND    433E6 
#define PABOOST true 

//sets onewire comm pin
#define oneWire_Bus A0

//sets moisure sensor pin
#define soilMoisture A1

//sets color sensor pins
#define S2 13
#define S3 12
#define OE 11

BME280_I2C bme(0x76);   //configures the address for the bme280
OneWire onewire(oneWire_Bus);   //configures the OneWire Bus
DallasTemperature tempSensor(&onewire); //configures temp sensor to onewire bus

long senseColor(char color) //change to double or multiply by 1000
{
  if(color == 'W')
  {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, LOW);
    return pulseIn(OE, LOW);
  }
  else if(color == 'R')
  {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    return pulseIn(OE, LOW);
  }
  else if(color == 'G')
  {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    return pulseIn(OE, LOW);
  }
  else if(color == 'B')
  {
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    return pulseIn(OE, LOW);
  }
}

void setup() 
{
  Serial.begin(9600);
  LoRa.setPins(SS,RST,DI0);
  LoRa.begin(BAND,PABOOST);
  
  if(!bme.begin())  //exit if the address is incorrect
  {
    Serial.println("Error");
    while(1);
  }

  tempSensor.begin();   //Start the one-wire communication

  //setup Color Sensor
  pinMode(S2, OUTPUT);    //TRY TO COMMENT ME WHEN WIRE IS FIXED!!!
  pinMode(S3, OUTPUT);
  pinMode(OE, INPUT);
}

void loop()
{
  bme.readSensor();   //required to get separate readings
  tempSensor.requestTemperatures();   //sends bit-stream to request temp readings

  //get Color data
  int red = senseColor('R');    //outputs time of low signal read in microseconds
  int blue = senseColor('B');
  int green = senseColor('G');
  int white = senseColor('W');

  //LoRa comms
  LoRa.beginPacket();
  LoRa.print(" C ");    //identifiers for data type on photon side
  LoRa.print(bme.getTemperature_C());
  LoRa.print(" P ");
  LoRa.print(bme.getPressure_HP());
  LoRa.print(" H ");
  LoRa.print(bme.getHumidity());
  LoRa.print(" S ");
  LoRa.print(analogRead(soilMoisture));
  LoRa.print(" T ");
  LoRa.print(tempSensor.getTempCByIndex(0));
  LoRa.print(" R ");
  LoRa.print(red);
  LoRa.print(" B ");
  LoRa.print(blue);
  LoRa.print(" G ");
  LoRa.print(green);
  LoRa.print(" W ");
  LoRa.print(white);
  LoRa.endPacket();

  
  Serial.print("TempBME: ");
  Serial.print(bme.getTemperature_C());
  Serial.print("\t Pressure (kP): ");
  Serial.print(bme.getPressure_HP());
  Serial.print("\t Humidity: ");
  Serial.print(bme.getHumidity());
  Serial.print("\t Soil Moisture: ");
  Serial.print(analogRead(soilMoisture));
  Serial.print("\t TempDS18B20: ");
  Serial.print(tempSensor.getTempCByIndex(0));  //ByIndex refers to which IC is on the bus. Since there is only one the index is 0 since it is the first one
//  Serial.print("\t Red (us): ");    //returning times
//  Serial.print(red);
//  Serial.print("\t Blue (us): ");
//  Serial.print(blue);
//  Serial.print("\t Green (us): ");
//  Serial.print(green);
//  Serial.print("\t White (us): ");
//  Serial.println(white);
  Serial.print("\t Red (us): ");   //returning intensities
  Serial.print(red);
  Serial.print("\t Blue (us): ");
  Serial.print(blue);
  Serial.print("\t Green (us): ");
  Serial.print(green);
  Serial.print("\t White (us): ");
  Serial.println(white);

  delay(2000);
}
