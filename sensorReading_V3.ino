#include <SPI.h>
#include <LoRa.h>
#include <MD_TCS230.h>
#include <FreqCount.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <cactus_io_BME280_I2C.h>
//change1
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
//  int red = senseColor('R') * 2;    //outputs time of low signal read in microseconds
//  int blue = senseColor('B') * 2;
//  int green = senseColor('G') * 2;
//  int white = senseColor('W') * 2;

  //sets irradiance readings
  unsigned long Red =  ((senseColor('R') * 2) * 474); //returns value of irradiance in uW/cm^2 (check to see if these will be correct and correct for % error between min and max values)
  unsigned long Blue = ((senseColor('B') * 2) * 331);   //multiplied by 2 to get full frequency and multiplied by 474, 331, or 386 since those are the respective frequency/intensity values via the datasheet and inverted so units are uW/cm^2 not cm^2/uW
  unsigned long Green = ((senseColor('G') * 2) * 386);

  double red = 1 / (Red * 0.000001);
  double blue = 1 / (Blue * 0.000001);
  double green = 1 / (Green * 0.000001);
  
  int white = senseColor('W') * 2;    //no intensity value since the other values are dependent on clear photodiode reading

  //LoRa comms
//  LoRa.beginPacket();
//  LoRa.print("TempBME: ");
//  LoRa.print(bme.getTemperature_C());
//  LoRa.print("\t Pressure (kP): ");
//  LoRa.print(bme.getPressure_HP() / 10);
//  LoRa.print("\t Humidity: ");
//  LoRa.print(bme.getHumidity());
//  LoRa.print("\t Soil Moisture: ");
//  LoRa.print(analogRead(soilMoisture));
//  LoRa.print("\t TempDS18B20: ");
//  LoRa.print(tempSensor.getTempCByIndex(0));  //ByIndex refers to which IC is on the bus. Since there is only one the index is 0 since it is the first one
//  LoRa.print("\t Red (uW/cm^2): ");   //returning intensities
//  LoRa.print(red);
//  LoRa.print("\t Blue (uW/cm^2): ");
//  LoRa.print(blue);
//  LoRa.print("\t Green (uW/cm^2): ");
//  LoRa.print(green);
//  LoRa.print("\t White (us): ");
//  LoRa.print(white);
//  LoRa.endPacket();

  LoRa.beginPacket();
  LoRa.print(" C ");    //identifiers for data type on photon side
  LoRa.print(bme.getTemperature_C());
  LoRa.print(" P ");
  LoRa.print(bme.getPressure_HP() / 10);
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
  Serial.print(bme.getPressure_HP() / 10);
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
  Serial.print("\t Red (uW/cm^2): ");   //returning intensities
  Serial.print(red);
  Serial.print("\t Blue (uW/cm^2): ");
  Serial.print(blue);
  Serial.print("\t Green (uW/cm^2): ");
  Serial.print(green);
  Serial.print("\t White (us): ");
  Serial.println(white);

  delay(2000);
}
