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
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
 
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  //digitalWrite(Relay_Enable,LOW);
}
 
void loop()
{
  connectingWiFiSetup();//Connecting to Wifi
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) 
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;
 
    // Because this loop may not complete in exactly 1 second intervals we calculate
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
 
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
 
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    availableLitter = currentLiter/1000;
    
    // Print the flow rate for this second in litres / minute
  
        // Print the cumulative total of litres flowed since starting
    //----------------------------Codes for Relay-------------------------------------------------------
    if(currentLiter<=0.0)
    {
       lcd.setCursor(3,0); 
       pinMode(Relay_Enable,OUTPUT);
      digitalWrite(Relay_Enable,HIGH);
        pinMode(Relay_Enable,INPUT_PULLUP);
      lcd.println("OFF");
      currentLiter=0.0;
      Serial.println("Your Smart water meter is off");
    }

//------------------------------End of codes of Relay --------------------------------------------
    lcd.setCursor(3,1);
    lcd.print("Litre:");
    lcd.print(currentLiter);
    updatingToDb();
     // Print tab space
 

     
  }
  lcd.clear();
}
void connectingWiFiSetup()
{
    
  delay(1000);   
  
  // Open serial communications and wait for port to open:
Serial.begin(9600);
while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  //Connect to your WiFi router#######################
  WiFi.begin(ssid, password); 
//  lcd.setCursor(3,0);    
//  lcd.println("");
//  lcd.print("Connecting to ");
//  lcd.print(ssid);
  // Wait for connection##############################
  while (WiFi.status() != WL_CONNECTED) {
//  lcd.setCursor(3,1);
//    lcd.print(".");
    Serial.print(".");
    delay(250);
    //#################################################
  }

  //when connected!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  lcd.setCursor(3,0);
//  lcd.println("");
//  lcd.print("Connected to Network/SSID ");
//  lcd.println(ssid);
//  lcd.print("IP address: ");
//  lcd.println(WiFi.localIP());  //IP address assigned to your ESP
  Serial.print("Connected to Network/SSID ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  requestingLiter();
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
void requestingLiter()
{
  // Serve the data as plain text, for example
   HTTPClient http;    //Declare object of class HTTPClient
String id = "1";
 
String postData = "id=" + id; 
   

  //http.begin(client,"http://192.168.137.1/mowater/liter.php");   //Offline Specify request destination
  http.begin(client,"http://api.rwema.com/liter.php");   //Online Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header


  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  
   litres= payload.toFloat();
           // Print the cumulative total of litres flowed since starting
    //----------------------------Codes for Relay-------------------------------------------------------
    if(litres<=0.0)
    {
//      pinMode(Relay_Enable,OUTPUT);
//      digitalWrite(Relay_Enable,HIGH);
//        pinMode(Relay_Enable,INPUT_PULLUP);
      digitalWrite(Relay_Enable,LOW);
      litres=0.0;
      lcd.setCursor(4,1);
      lcd.println("Your Smart water meter is off");
    }


//------------------------------End of codes of Relay --------------------------------------------
   lcd.clear();
//   lcd.setCursor(8, 2);
//   lcd.print("Litre:");
//   lcd.println(litres);
   Serial.print("http code:");
   Serial.println(httpCode);
   Serial.println(litres);
   lcd.clear();
  http.end();  //Close connection
}
void updatingToDb()
{
  // Serve the data as plain text, for example
   HTTPClient http;    //Declare object of class HTTPClient
  String id = "1";
  
float liter = Serial.parseFloat();
String postData = "id=" + id + "&liter=" + currentLiter; 
   //http.begin(client,"http://192.168.137.1/mowater/update.php");   //Offline Specify request destination
  http.begin(client,"http://api.rwema.com/update.php");   //Online Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(postData);   //Send the request
  http.end();  //Close connection
}
