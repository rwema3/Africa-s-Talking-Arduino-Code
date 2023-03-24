#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#define LED_BUILTIN 16
#define SENSOR  4
const int Relay_Enable=25;
char* ssid = "Rwema";
char* password = "rwema123!";
float litres;
float availableLitter =0;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float currentLiter;
float flowLitres;
float totalLitres;
//------------------------------------End 1 For Water flow Sensor----------------------------------------
//int R1 = 12;  // Pin 3 on Arduino
//----------^--------^--------Codes for Relays------------------------------------------------------------
WiFiClient client;
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
 
void setup()
{
  Serial.begin(9600);
//  connectingWiFiSetup();//Connecting to Wifi
 // pinMode(Relay_Enable,INPUT_PULLUP);
   pinMode(Relay_Enable,OUTPUT);
   lcd.init();                      // initialize the lcd 
   lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Starting...");
  
  delay(2000); //delay for 2 seconds to make sure the modules get the signal
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("SMART METER");
  Serial.println("Starting...");
  delay(3000); //delay for 3 seconds to make sure the modules get the signal
  Serial.println("Mobile Water");
  delay(3000);
  Serial.println("Liter:");
  delay(2000);
  Serial.println(litres);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
