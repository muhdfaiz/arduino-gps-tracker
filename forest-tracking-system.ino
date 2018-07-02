#include <GPRS_Shield_Arduino.h> 
#include <SoftwareSerial.h> 
#include <Wire.h> 
#include <Suli.h> 
#include <TinyGPS.h> 
#include <AltSoftSerial.h>


// Specify GPS Shield TX and RX Pin
# define GPRS_TX_PIN 2
# define GPRS_RX_PIN 3

TinyGPS gps;

AltSoftSerial ss;

char urlPath[19] = "GET / HTTP/1.0\r\n\r\n";

char fullURL[80];

float latitude;
float longitude;
      
// Specify GPRS Pin and Baudrate. Use 9600 as Baudrate Value.
GPRS gprs(GPRS_TX_PIN, GPRS_RX_PIN, 9600);

void setup() {
  // Start serial communication for GPRS Shield with baudrate 9600
  Serial.begin(115200);
  
  // Initialize GPRS Shield
  gprs.init();
  
  // Power Up GPRS Shield.
  gprs.powerUpDown(4);
  
  // Connection to GPRS Provider.
  while (false == gprs.join(F("Maxis"), F("maxis"), F("wap"))) {
    Serial.println("gprs join network error");
    delay(2000);
  }

  // Print IP Address after successfully connected.
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());
   
  // Begin Serial Communication For GPS Shield with baudrate 115200
  ss.begin(9600);

}

void loop() {
  while (ss.available()) {
    char c = ss.read();
    // uncomment this line if you want to see the GPS data flowing
    // Serial.write(c); 
    
    // If GPS data change, retrieve new GPS Latitude and longitude.
    if (gps.encode(c)) {
  
      char latitudeInStr[8];
  
      char longitudeInStr[10];
      
      unsigned long age;
  
      gps.f_get_position( & latitude, & longitude, & age);
      
      
      boolean notConnectToServer = true;
      
      while (notConnectToServer) {
        if (gprs.connect(TCP, "muhdfaiz.com", 80)) {
          notConnectToServer = false;
          
          Serial.println("connect to server success");
          
          Serial.print("Latitude: ");
          Serial.println(floatToString(latitude, 8));
         
          Serial.print("Longitude: ");
          Serial.println(floatToString(longitude, 8));
        
          char fullURI[60];
          char baseURLPath[] = "GET /location/"; 

          String url = String();
          url += "GET /location/1/1/";
          url += floatToString(latitude, 8);
          url += "/";
          url += floatToString(longitude, 8);
          url += " HTTP/1.0\r\n\r\n";
          url.toCharArray(fullURI, 60);
          Serial.println(fullURI);
          
          int dataSend = gprs.send(fullURI, sizeof(fullURI) - 1);
          gprs.close();
        }
      }
    }
  }
   
};

String floatToString(double number, uint8_t digits) {
    String resultString = "";
    // Handle negative numbers
    if (number < 0.0) {
        resultString += "-";
        number = -number;
    }
    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for (uint8_t i = 0; i < digits; ++i)
    rounding /= 10.0;
    number += rounding;
    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)number;
    double remainder = number - (double)int_part;
    resultString += int_part;
    // Print the decimal point, but only if there are digits beyond
    if (digits > 0)
    resultString += ".";
    // Extract digits from the remainder one at a time
    while (digits-- > 0) {
        remainder *= 10.0;
        int toPrint = int(remainder);
        resultString += toPrint;
        remainder -= toPrint;
    }
    return resultString;
}
