/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-post-ifttt-thingspeak-arduino/ 

  Modified to use HTTPS by [Your Name]

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>  // Include the WiFiClientSecure library for HTTPS

const char* ssid = "SQUARED_TECH_SOLUTIONS-2";
const char* password = "STS@2024";

// Update the server URL to use HTTPS
const char* serverName = "https://api.thingspeak.com/update";

// Service API Key
String apiKey = "P2U83G2SDAZR07SV";

// Root CA certificate for api.thingspeak.com
// You can obtain the root CA certificate from the certificate authority.
// For Let's Encrypt, the ISRG Root X1 is commonly used.
// Below is the certificate in PEM format. Ensure it's up-to-date.

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
  "MIIE9DCCA9ygAwIBAgIQCF+UwC2Fe+jMFP9T7aI+KjANBgkqhkiG9w0BAQsFADBh\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
  "MjAeFw0yMDA5MjQwMDAwMDBaFw0zMDA5MjMyMzU5NTlaMFkxCzAJBgNVBAYTAlVT\n" \
  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh\n" \
  "bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD\n" \
  "ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV\n" \
  "cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy\n" \
  "FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc\n" \
  "3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8\n" \
  "osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT\n" \
  "zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAa4wggGqMB0GA1Ud\n" \
  "DgQWBBR0hYDAZsffN97PvSk3qgMdvu3NFzAfBgNVHSMEGDAWgBROIlQgGJXm427m\n" \
  "D/r6uRLtBhePOTAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEG\n" \
  "CCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwdgYIKwYBBQUHAQEEajBoMCQG\n" \
  "CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG\n" \
  "NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH\n" \
  "Mi5jcnQwewYDVR0fBHQwcjA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t\n" \
  "L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA3oDWgM4YxaHR0cDovL2NybDQuZGln\n" \
  "aWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDAwBgNVHSAEKTAnMAcG\n" \
  "BWeBDAEBMAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEB\n" \
  "CwUAA4IBAQB1i8A8W+//cFxrivUh76wx5kM9gK/XVakew44YbHnT96xC34+IxZ20\n" \
  "dfPJCP2K/lHz8p0gGgQ1zvi2QXmv/8yWXpTTmh1wLqIxi/ulzH9W3xc3l7/BjUOG\n" \
  "q4xmfrnti/EPjLXUVa9ciZ7gpyptsqNjMhg7y961n4OzEQGsIA2QlxK3KZw1tdeR\n" \
  "Du9Ab21cO72h2fviyy52QNI6uyy/FgvqvQNbTpg6Ku0FUAcVkzxzOZGUWkgOxtNK\n" \
  "Aa9mObm9QjQc2wgD80D8EuiuPKuK1ftyeWSm4w5VsTuVP61gM2eKrLanXPDtWlIb\n" \
  "1GHhJRLmB7WqlLLwKPZhJl5VHPgB63dx\n" \
  "-----END CERTIFICATE-----\n";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(33));
}

void loop() {
  // Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status() == WL_CONNECTED){
      // Use WiFiClientSecure for HTTPS
      WiFiClientSecure client;
      
      // Option 1: Verify the server's certificate using the root CA
      client.setCACert(root_ca);
      
      // Option 2: (Not recommended) Disable certificate verification
     // client.setInsecure();
      
      HTTPClient https;

      // Your Domain name with URL path or IP address with path
      if (https.begin(client, serverName)) {  // HTTPS
        // Specify content-type header
        https.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        String httpRequestData = "api_key=" + apiKey + "&field1=" + String(random(40));           
        // Send HTTP POST request
        int httpResponseCode = https.POST(httpRequestData);
        
        /*
        // If you need an HTTP request with a content type: application/json, use the following:
        https.addHeader("Content-Type", "application/json");
        // JSON data to send with HTTP POST
        String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
        // Send HTTP POST request
        int httpResponseCode = https.POST(httpRequestData;*/
       
        if(httpResponseCode > 0){
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
        }
        else{
          Serial.print("Error on sending POST: ");
          Serial.println(https.errorToString(httpResponseCode).c_str());
        }
          
        // Free resources
        https.end();
      }
      else {
        Serial.println("Unable to connect to server");
      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
