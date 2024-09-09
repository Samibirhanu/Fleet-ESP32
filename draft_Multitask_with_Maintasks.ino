// macro for selecting the core of stensaw processor of esp32  which is two core processors
#if CONFIG_FREERTOS_UNICORE
// The number one core processor
#define ARDUINO_RUNNING_CORE 0
#else
// The number two core processor
#define ARDUINO_RINNING_CORE 1
#endif

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
byte Naolid[] ={1,2};
byte Samuelid[] ={3,4};
byte Aronid[] = {5,6};
byte Fikremariamid[] ={12,21};

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

// functions

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

bool verifyFingerprint() {
  int id = getFingerprintID();
  if (id == -1) {
    return false;
  }

  if (id == Naolid[0 || 1]) {
    Serial.print("Fingerprint recognized! ID: ");
    Serial.println(id);
    return true;
  }
  else if (id == Fikremariamid[0 || 1]) {
    Serial.print("Fingerprint recognized! ID: ");
    Serial.println(id);
    return true;
  }
  else if (id == Samuelid[0 || 1]) {
    Serial.print("Fingerprint recognized! ID: ");
    Serial.println(id);
    return true;
  }
  else if (id == Aronid[0 || 1]) {
    Serial.print("Fingerprint recognized! ID: ");
    Serial.println(id);
    return true;
  }
  else (Serial.println("Someone tries to start the car with unknown fingerprint"));
}

void startEngine(bool byFingerprint) {
  Serial.println("Starting engine...");
  digitalWrite(START_ENGINE_PIN, ON);  // Turn on relay to start the engine
  delay(5000);                           // Keep the relay on for 5 seconds
  digitalWrite(START_ENGINE_PIN, OFF);
  Serial.println("ENGINE STARTED!");   // Turn off the relay
  engineStarted = true;

  // Logging message based on the method used
  String currentTime = timeClient.getFormattedTime();
  String currentDate = getCurrentDate();
  int id = getFingerprintID();
  if ((byFingerprint) && id == Fikremariamid[0 || 1]) {
    Serial.println("Fikremariam starts the Engine on " + currentDate + " at " + currentTime + " using Biometric");
  }
  
  else if ((byFingerprint) && id == 1||2) {
    Serial.println("Naol starts the Engine on " + currentDate + " at " + currentTime + " using Biometric");
  }

  else if ((byFingerprint) && id == Samuelid[1]) {
    Serial.println("Samuel starts the Engine on " + currentDate + " at " + currentTime + " using Biometric");
  }

  else if((byFingerprint) && id == Aronid[0 || 1]) {
    Serial.println("Aron starts the Engine on " + currentDate + " at " + currentTime + " using Biometric");
  }
}

void stopEngine(bool byFingerprint) {
  Serial.println("Stopping engine...");
  digitalWrite(ENGINE_CUT_PIN, ON);    // Turn on relay to cut the engine
  delay(4000);                           // Keep the relay on for 4 seconds
  digitalWrite(ENGINE_CUT_PIN, OFF);     // Turn off the relay
  Serial.println("Engine STOPPED!");
  engineStarted = false;

  // Logging message based on the method used
  String currentTime = timeClient.getFormattedTime();
  String currentDate = getCurrentDate();
  if (byFingerprint) {
    Serial.println("Fikremariam stops the engine on " + currentDate + " at " + currentTime + " using fingerprint");
  } else {
    Serial.println("Fikremariam stops the engine on " + currentDate + " at " + currentTime + " using RFID");
  }
}

bool verifyRFID() {
  if (memcmp(nfc.uid.uidByte,NaolRfid, nfc.uid.size) == 0) {
    Serial.println("RFID tag is found!");
    return true;
  }
  else if (memcmp(nfc.uid.uidByte,FikremariamRfid, nfc.uid.size) == 0) {
    Serial.println("Fikremariam Starts the car by RFID");
    return true;
  }
  else if (memcmp(nfc.uid.uidByte,SamuelRfid, nfc.uid.size) == 0) {
    Serial.println("Samuel Starts the engine by RFID");
    return true;
  }
  Serial.println("Someone tries to start the car with unknown RFID");
  return false;
}

int getFingerprintID() {
  int p = finger.getImage();
  
  // Only proceed if a finger is detected
  if (p == FINGERPRINT_NOFINGER) {
    return -1;  // No finger detected
  }
  
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

String getCurrentDate() {
  // Calculate the current date based on the time received from the NTP server
  time_t rawtime = timeClient.getEpochTime();
  struct tm * timeinfo = localtime(&rawtime);

  char buffer[11];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  return String(buffer);
}


// Create task functions

void taskFunction1(void * pvParameters){

  (void) pvParameters;

  for(;;){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    timeClient.update();

    // Check fingerprint sensor and start/stop engine based on fingerprint verification
    if (verifyFingerprint()) {
      if (!engineStarted) {
        startEngine(true);  // True indicates the fingerprint sensor was used
      } else {
        stopEngine(true);   // True indicates the fingerprint sensor was used
      }
    }

    vTaskDelay(1000/ portTICK_PERIOD_MS);  // delay for 1 second

  // the time is calculated in terms of ticks per milliseconds
  
  }
}

void taskFunction2(void * pvParameters){

  (void) pvParameters;
  
  for(;;){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    if (nfc.PICC_IsNewCardPresent() && nfc.PICC_ReadCardSerial()) {
    if (verifyRFID()) {
      if (!engineStarted) {
        startEngine(false);  // False indicates the RFID tag was used
      } else {
        stopEngine(false);   // False indicates the RFID tag was used
      }
    }
    nfc.PICC_HaltA(); // Halt the RFID card to prevent multiple reads
  }
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay for 1 second

  // the time is calculated in terms of ticks per milliseconds
  
  }
}


void setup(){
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
  
  // create the task
  // taskName, descriptive name, stack depth, Pv parameter, priority(low number indicates low priority ), pointer to a task_T(interacting tasks from other tasks), arduino_running_core

  xTaskCreatePinnedToCore(taskFunction1, "Task1", 1024, NULL, 1, NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(taskFunction2, "Task2", 1024, NULL, 2, NULL,ARDUINO_RUNNING_CORE);


  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

}

void loop(){
// Nothing goes here, all the work is done in the tasks
}