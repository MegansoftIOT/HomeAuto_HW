#include "WiFi.h" 
#include <HTTPClient.h>

#define DEBUG_CODE 1 

const char *ssid = "MEGSOFT.NETWORK";
const char *password = "248-230-9625";

WiFiServer wifiServer(12345);

int sensorValue;
int device_id = 134;
bool status_light =true ;
int relayPin = 19;
unsigned long startMillis;  //some global variables available anywhere in the program
const unsigned long period = 150000; 




// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.0.4/lightdb.php";

void processReceivedValue(char command)
{
  if(command == '1')
  { 
    #ifdef DEBUG_CODE
    Serial.println("ON"); 
    digitalWrite(relayPin, HIGH); 
  }
  else if(command == '0')
  { 
    #ifdef DEBUG_CODE
    Serial.println("OFF");
    digitalWrite(relayPin, LOW);
  }
  return;
}

void setup() 
{
  Serial.begin(115200);
  
  pinMode(relayPin ,OUTPUT);
  
  WiFi.begin(ssid, password);
  
  #ifdef DEBUG_CODE
  Serial.println("Connecting");
  #endif
  
  while(WiFi.status() != WL_CONNECTED) 
    { 
      delay(500);
  
      #ifdef DEBUG_CODE
      Serial.print(".");
      #endif
    
    }
    
  #ifdef DEBUG_CODE
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  #endif
  
  wifiServer.begin();
}

void loop() 
{
  startMillis = millis();
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED)
    {
        HTTPClient http;
              
        sensorValue = analogRead(A0); // read analog input pin 0
    
        #ifdef DEBUG_CODE
        Serial.print(sensorValue, DEC); // prints the value read
        Serial.print(" \n"); // prints a space between the numbers
        #endif
        
        status_light=digitalRead(relayPin);

        
        // Your Domain name with URL path or IP address with path
        http.begin(serverName);
        
        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
      
        // Prepare your HTTP POST request data
        String httpRequestData = "device_id=" +String(device_id)+ "&sensor=" +String(sensorValue)+"&status=" +String(status_light);
        
        #ifdef DEBUG_CODE
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);
        #endif
       
        int httpResponseCode = http.POST(httpRequestData);
        if (httpResponseCode>0)
        {
        
          #ifdef DEBUG_CODE
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          #endif
        
        }
        else 
        {
        
          #ifdef DEBUG_CODE
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          #endif
        
        }
      
        http.end();
             
    }
      
    
 else 
  {
    
    #ifdef DEBUG_CODE
    Serial.println("WiFi Disconnected");
    #endif
  
  }

 while (millis() < startMillis + period)  //test whether the period has elapsed
 {
    WiFiClient client = wifiServer.available();
    if (client) 
      {
        while (client.connected()) 
          {
            while (client.available()>0) 
              {
                char c = client.read();
                processReceivedValue(c);
             
                #ifdef DEBUG_CODE
                Serial.write(c);
                #endif
              
              }
            delay(10);
          }
          client.stop();
          
          #ifdef DEBUG_CODE
          Serial.println("Client disconnected");
          #endif
     
     }
 }
  
}

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
