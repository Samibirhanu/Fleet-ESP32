#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi credentials (replace with your network credentials)
const char* ssid = "SQUARED_TECH_SOLUTIONS-2";
const char* password = "STS@2024";

// Time setup for Ethiopian time (UTC+3)
const long utcOffsetInSeconds = 3 * 3600;  // UTC+3 hours
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000);  // Update every 60 seconds

// Pin Definitions for ESP32
#define SDA_PIN 21
#define RST_PIN 22
#define START_ENGINE_PIN 2
#define ENGINE_CUT_PIN 4
byte OFF = 1;
byte ON = 0;

// FINGER PRINT ID
uint16_t Naolid[] ={1,2};
uint16_t Samuelid[] ={3,4};
uint16_t Aronid[] = {5,6};
uint16_t Fikremariamid[] ={12,21};

// RFID Setup
MFRC522 nfc(SDA_PIN, RST_PIN);

// Fingerprint Sensor using UART2 (GPIO16=RX2, GPIO17=TX2)
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger(&mySerial);

// RFID Tag (example tag: 1D 8F 9F 82)
byte FikremariamRfid[] = {0x1D, 0x8F, 0x9F, 0x82};
byte SamuelRfid[] = {0x04, 0x16, 0xDA, 0xEA, 0x58, 0x13, 0x90};
byte NaolRfid[] = {0x03, 0x1F, 0x67, 0x3B};

bool engineStarted = false;
int after =0, emerg=0, net=1, i=1, j=1, m=1;


bool messageReceived = false;
String incomingMessage = "";
String message;
String compareValue;
String convertedValue;
int on=1;
int cutted=0;

byte status;
byte data[32];
byte serial[5];

bool rfidVerified = false;
bool fingerprintVerified = false;

int emergecyabort =0; 

void setup() {
  // Initialize pins
  pinMode(START_ENGINE_PIN, OUTPUT);
  pinMode(ENGINE_CUT_PIN, OUTPUT);

  // Set initial states
  digitalWrite(START_ENGINE_PIN, OFF);
  digitalWrite(ENGINE_CUT_PIN, OFF);


  // Initialize Serial Communication
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, 16, 17);  // UART2 for fingerprint sensor

  // Initialize Wi-Fi
  connectToWiFi();

  // Initialize NTP Client
  timeClient.begin();
  timeClient.update();

  // Initialize RFID
  SPI.begin();  
  nfc.PCD_Init();


  // Initialize Fingerprint Sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected successfully!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);  // Halt
  }

  Serial.println("System ready.");
}

void loop() {
  
  checkRFID();
  if(on)
    checkFingerprint();
  //}

  if (rfidVerified && fingerprintVerified) {
    Serial.println("Access Granted.");
    rfidVerified = false;
    fingerprintVerified = false;
  }
  
  delay(500); // Adjust the delay as necessary

}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  const unsigned long timeout = 10000; // 10 seconds timeout

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    if (millis() - startTime >= timeout) {
      Serial.println("\nFailed to connect to WiFi. Retrying...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      startTime = millis();  // Reset the timer
    }
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void checkRFID() {
  /* byte mainStatus;
  byte mainData[16];
  byte mainSerial[5];
  mainStatus = nfc.requestTag(MF1_REQIDL, mainData);
  if (mainStatus == MI_OK) {
    mainStatus = nfc.antiCollision(mainData);
    memcpy(mainSerial, mainData, 5);
    nfc.selectTag(mainSerial);
    nfc.haltTag();
    for(int i=0; i<5; i++)
      serial[i] = mainSerial[i];
    */
  
  byte serial[16];
   //used nfc.uid.size to store the rfid tag being readed to serial array
    for (byte i = 0; i < nfc.uid.size; i++) {
      serial[i] = nfc.uid.uidByte[i];
      }
    
  
      
    if(rfidUserCheck(serial,FikremariamRfid,"Fikremariam"));
    else if(rfidUserCheck(serial,SamuelRfid,"Samuel"));
    else if(rfidUserCheck(serial,NaolRfid,"Naol"));
    else {
      Serial.println("Unknown RFID.");
      rfidVerified = false;
      digitalWrite(3, HIGH);
      delay(100);
      digitalWrite(3, LOW);
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        return;
      }
      m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      delay(100);
  

      message= "1, Unknown RFID. "+String(serial[0],HEX)+" "+ String(serial[1],HEX)+" "+String(serial[2],HEX)+" "+String(serial[3],HEX)+" Tries to start the car. \n\r"; // Message content
  
      net = 0;
      Serial.println("It has finished the sensor");
      delay(1000); 
    }
  //}
}

void checkFingerprint() {
  int id = getFingerprintIDez();
  if (id != -1) {
    fingerprintVerified = true;
  }
  else {
    fingerprintVerified = false;
  }
}

// Returns -1 if failed, otherwise returns ID #

int getFingerprintIDez() {
  uint8_t p;

  p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.fingerFastSearch();

  if (p != FINGERPRINT_OK) {
    Serial.println("Fingerprint do not match!");
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    
    while(after || emerg){
      if(m){
        Serial.println("It may be in Emergency or Get from the Server");
        m = 0;
      }
      yield();
      return -1;
    }
    m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    delay(100);
    message ="1, Unknown person with fingerprint Tries to start the car.\n \r";
    net = 0;
    Serial.println("It has finished the sensor");
    delay(1000); 
    return -1;
  } 
  else {

    if(fingerprintUserCheck(finger.fingerID, Naolid, "Naol", "03 1F 67 3B",serial,NaolRfid));
    else if(fingerprintUserCheck(finger.fingerID, Samuelid, "Samuel", "04 16 DA EA 58 13 90",serial,SamuelRfid));
    else if(fingerprintUserCheck(finger.fingerID, Aronid, "Aron", "03 1F 67 3B",serial,NaolRfid));
    else if(fingerprintUserCheck(finger.fingerID, Fikremariamid, "Fikremariam", "03 1F 67 3B",serial,NaolRfid));

    delay(1000); 
    Serial.print("Found ID #");
    Serial.print(finger.fingerID); 
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID; 
  }
}


// RFID Checking Mechanisim -------------------------------------------------------------------------------
int rfidUserCheck(byte serial[], byte mohamedTag[], String name){
  if(memcmp(serial, mohamedTag, 4) == 0 ){
    Serial.println("Tag verified as "+name+"'s RFID.");
    rfidVerified = true;
    convertedValue="";
    for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
      if (i > 0) {
        convertedValue += " ";
      }
      convertedValue += String(mohamedTag[i], HEX);
    }
    // Compare the converted value with the compareValue
    if (compareValue.indexOf(convertedValue) != -1) {
      Serial.println("Match found");
      Serial.println("This Driver is banned!");
      message ="1, "+name+" with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
    }
    else {
      Serial.println("No Banned Driver match found \n "+convertedValue+"\n");
      if(on){
        message ="1, "+name+" with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
        emergecyabort =1;
        digitalWrite(START_ENGINE_PIN, LOW);
        delay(800);
        digitalWrite(START_ENGINE_PIN, HIGH);
        on = 0;
        delay(100);
        //fingerlight=0;
      }
      else{
        if(!cutted){
          emergecyabort =1;
          digitalWrite(ENGINE_CUT_PIN, LOW);
          delay(1000);
          digitalWrite(ENGINE_CUT_PIN, HIGH);
          on = 1;
          message ="1, "+name+" with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
          delay(100);
        }
      }

    }
    while(after || emerg){
      status = nfc.requestTag(MF1_REQIDL, data);
      if (status == MI_OK) {
        status = nfc.antiCollision(data);
        memcpy(serial, data, 5);
        nfc.selectTag(serial);
        nfc.haltTag();
        Serial.println("It may be in Emergency or Get from the Server");
        //
        if(memcmp(serial, mohamedTag, 4) == 0) {
          Serial.println("Tag verified as "+name+"'s RFID.");
          rfidVerified = true;
          convertedValue="";
          for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
            if (i > 0) {
              convertedValue += " ";
            }
            convertedValue += String(mohamedTag[i], HEX);
          }
          // Compare the converted value with the compareValue
          if (compareValue.indexOf(convertedValue) != -1) {
            Serial.println("Match found");
            Serial.println("This Driver is banned!");
            //message ="1, Naol with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
          }
          else {
            Serial.println("No match found \n "+convertedValue+"\n");
            if(on){
              //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
              emergecyabort =1;
              digitalWrite(STARTENGINE, LOW);
              delay(800);
              digitalWrite(STARTENGINE, HIGH);
              on = 0;
              delay(100);
              //fingerlight=0;
            }
            else{
              if(!cutted){
                emergecyabort =1;
                digitalWrite(ENGINECUT, LOW);
                delay(1000);
                digitalWrite(ENGINECUT, HIGH);
                on = 1;
                //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
                delay(100);
              }
            }
          }
        }
      }        
      yield();
      //return;
    }
    //m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    delay(100);
    net = 0;
    Serial.println("It has finished the sensor");
    delay(1000); 
    return 1;
  }
  else{
    return 0;
  }
  
}

// Finger Print Checking Mechanisim------------------------------------------------------------------------
int fingerprintUserCheck(uint16_t fingerId, uint16_t id[], String name, String rfid, byte serial[], byte mohamedTag[]){
  if(fingerId == id[0] || fingerId == id[1] ){
    if((compareValue.indexOf(rfid) != -1)){
      message ="1, "+name+" with fingerprint Tries to start the car. \n\r"; // Message content
    }
    else{
      message = "1, "+name+" with fingerprint started the car. \n\r"; 
      emergecyabort =1;
      digitalWrite(START_ENGINE_PIN, LOW);
      delay(800);
      digitalWrite(START_ENGINE_PIN, HIGH);
      //fingerlight = 0;
      on = 0;
      delay(100);
    }
    while(after || emerg){
      status = nfc.requestTag(MF1_REQIDL, data);
      if (status == MI_OK) {
        status = nfc.antiCollision(data);
        memcpy(serial, data, 5);
        nfc.selectTag(serial);
        nfc.haltTag();
        Serial.println("It may be in Emergency or Get from the Server");
        //
        if(memcmp(serial, mohamedTag, 4) == 0) {
          Serial.println("Tag verified as "+name+"'s RFID.");
          rfidVerified = true;
          convertedValue="";
          for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
            if (i > 0) {
              convertedValue += " ";
            }
            convertedValue += String(mohamedTag[i], HEX);
          }
          // Compare the converted value with the compareValue
          if (compareValue.indexOf(convertedValue) != -1) {
            Serial.println("Match found");
            Serial.println("This Driver is banned!");
            //message ="1, Naol with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
          }
          else {
            Serial.println("No match found \n "+convertedValue+"\n");
            if(on){
              //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
              emergecyabort =1;
              digitalWrite(START_ENGINE_PIN, LOW);
              delay(800);
              digitalWrite(START_ENGINE_PIN, HIGH);
              on = 0;
              delay(100);
              //fingerlight=0;
            }
            else{
              if(!cutted){
                emergecyabort =1;
                digitalWrite(ENGINE_CUT_PIN, LOW);
                delay(1000);
                digitalWrite(ENGINE_CUT_PIN, HIGH);
                on = 1;
                //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
                delay(100);
              }
            }
          }
        }
      }        
      yield();
      //return;
    }
    //m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    net = 0;
    Serial.println("It has finished the sensor");
    return 1;
  }

  else{
    return 0;
  }
}