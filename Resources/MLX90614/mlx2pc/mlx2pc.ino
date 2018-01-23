/*****************************************************************************************
 * A sketch for Arduino_uno_rev.3 to communicate with Melexis mlx_90614 temperature sensor
 * by means of Adafruit_MLX90614 library on I2C interface.
 ****************************************************************************************/
#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define READ_MEASUREMENTS_PERIOD 250

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
unsigned long timeCounter = 0;
unsigned int timeLast = 0;

void setup() {
  Serial.begin(9600);
  mlx.begin();  
}

void loop() {
  
  timeLast = millis() - timeCounter; 
  
  if(timeLast >= READ_MEASUREMENTS_PERIOD) {
    Serial.print("\nA:");
    Serial.print(mlx.readAmbientTempC());
    Serial.print("\nO:");
    Serial.print(mlx.readObjectTempC());
    timeCounter = millis();
  } else if(timeLast < 0) {
    timeCounter = millis();  
  }
  
}
