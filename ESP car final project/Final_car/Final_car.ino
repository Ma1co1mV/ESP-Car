#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoOTA.h>

// connections for drive Motors
int PWM_A = D1;
int PWM_B = D2;
int DIR_A = D3;
int DIR_B = D4;

#define BAUDRATE 115200

const int TRIGGER_PIN = 12;
const int  ECHO_PIN = 13;


long distance = 0;
long max_distance = 0;
long p_millis = 0;
int number = 5;


const int buzPin = D5;      
const int ledPin = D8;      
const int wifiLedPin = D0;  

String command;          
int SPEED = 1023;        
int speed_Coeff = 3;

ESP8266WebServer server(80);      
String lcd_line1,lcd_line2;
unsigned long previousMillis = 0;



String sta_ssid = "Malcolms WiFi";
      
String sta_password = "dapassword1"; 




void setup(){
  Serial.begin(115200);    
  Serial.println();
  Serial.println("*WiFi Robot Remote Control Mode*");
  Serial.println("--------------------------------------");
 
  pinMode(buzPin, OUTPUT);      
  pinMode(ledPin, OUTPUT);      
  pinMode(wifiLedPin, OUTPUT);  
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);
    
  
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  
  
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);

  
  String chip_id = String(ESP.getChipId(), HEX);
  int i = chip_id.length()-4;
  chip_id = chip_id.substring(i);
  chip_id = "wificar-" + chip_id;
  String hostname(chip_id);
  
  Serial.println();
  Serial.println("Hostname: "+hostname);

  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(sta_ssid);
  Serial.print("Password: ");
  Serial.println(sta_password);

  
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
    currentMillis = millis();
  }

  // if failed to connect with Wifi network set NodeMCU as AP mode
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("*WiFi-STA-Mode*");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(wifiLedPin, LOW);   
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi failed connected to " + sta_ssid);
    Serial.println("");
    Serial.println("*WiFi-AP-Mode*");
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    digitalWrite(wifiLedPin, HIGH);   // Wifi LED off when status as AP mode
    delay(3000);
  }
 

  server.on ( "/", HTTP_handleRoot );       
  server.onNotFound ( HTTP_handleRoot );    
  server.begin();                           
  
  ArduinoOTA.begin();    

 pinMode(LED_BUILTIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  Serial.begin(BAUDRATE);

   Serial.println("* range finder *");
  delay(2000);

  Serial.println("distance");

}
#define M_PERIOD 500
  



void loop() {
    ArduinoOTA.handle();          
    server.handleClient();        
    
      command = server.arg("State");          
      if (command == "F") Forward();          
      else if (command == "B") Backward();
      else if (command == "R") TurnRight();
      else if (command == "L") TurnLeft();
      else if (command == "G") ForwardLeft();
      else if (command == "H") BackwardLeft();
      else if (command == "I") ForwardRight();
      else if (command == "J") BackwardRight();
      else if (command == "S") Stop();
      else if (command == "V") BeepHorn();
      else if (command == "W") TurnLightOn();
      else if (command == "w") TurnLightOff();
      else if (command == "0") SPEED = 330;
      else if (command == "1") SPEED = 400;
      else if (command == "2") SPEED = 470;
      else if (command == "3") SPEED = 540;
      else if (command == "4") SPEED = 610;
      else if (command == "5") SPEED = 680;
      else if (command == "6") SPEED = 750;
      else if (command == "7") SPEED = 820;
      else if (command == "8") SPEED = 890;
      else if (command == "9") SPEED = 960;
      else if (command == "q") SPEED = 1023;

       if(millis() - p_millis > M_PERIOD){
    distance = find_range(ECHO_PIN, TRIGGER_PIN);

    
    Serial.println(String(distance));
    Serial.print(" cm");
    p_millis = millis();

  }
 
  
 
}

long find_range(int EchoPin, int TriggerPin) {
  long range = 0;

  do{
    digitalWrite(TriggerPin,LOW);
    delayMicroseconds(2);
    digitalWrite(TriggerPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerPin,LOW);
    range = pulseIn(EchoPin,HIGH)/58;
    
  }while(range>280);
  return range;                           
}
  


void HTTP_handleRoot(void){
  server.send ( 200, "text/html", "" );       
  
  if( server.hasArg("State") ){
     Serial.println(server.arg("State"));
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); 
}


void Forward(){ 
 
  
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);

  
  
}


void Backward(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to turn right
void TurnRight(){
  
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
  
   
  
  }



void TurnLeft(){
 
 
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);

  if(distance < 40 ){
    delay(100);
     digitalWrite(DIR_A, LOW);
     digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  }

}

// function to move forward left
void ForwardLeft(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED/speed_Coeff);
}

// function to move backward left
void BackwardLeft(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED/speed_Coeff);
}

// function to move forward right
void ForwardRight(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED/speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to move backward left
void BackwardRight(){ 
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED/speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to stop motors
void Stop(){  
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

// function to beep a buzzer
void BeepHorn(){
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}

// function to turn on LED
void TurnLightOn(){
  digitalWrite(ledPin, HIGH);
}

// function to turn off LED
void TurnLightOff(){
  digitalWrite(ledPin, LOW);
}
