#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);


const char* ssid         = "UserName_2.4G"; //ชื่อ Wifi ที่ต้องการ connect 
const char* password     = "12123435"; // password wifi ที่ต้องการ connect
 
const char* mqttServer   = "soldier.cloudmqtt.com"; // mqtt server name 
const char* mqttUser     = "test"; // user
const char* mqttPassword = "12345"; //password 
const int   mqttPort     =  15978; //port 

const int inPin = 15; // nodemcu 
const int pingPin = 13;

const int inPin2 = 12;
const int pingPin2 = 14;

int cm1,cm2;

String msg = "";
//#define PUB_GPIO2_STATUS "/status"  //topic /status 
#define PUB_GPIO2_DATA "/ESP/LED"      //topic /data 


 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {

    Serial.begin(9600);
    initWifiStation();
    initMQTTClient();

    //subce..

}
void loop() {
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback); 
    if (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      if (client.connect("test", mqttUser, mqttPassword)) {
        Serial.println("connected");
       client.subscribe("/ESP/Clear");
      
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(1000);
        return;
      }
    }
    client.loop();
    mqttClient.loop();
    sonar_1();
    sonar_2();
    Serial.print("msg : ");
    Serial.println(msg);
    
    if(msg == ""){ 
      process_check_fullcar();
    }else{
      delay(10000);
      Serial.print("msg : ");
      Serial.println(msg);
      msg="";
    } 
   
    delay(1000);

   
}
int rounds_full = 0;
int rounds_free = 0;

void process_check_fullcar(){ 
  
  // check find sonar 1 & 2 push Full
  /*  if(cm1 < 10 && cm2 < 10){
       mqttClient.publish(PUB_GPIO2_STATUS, "Full");
       mqttClient.subscribe(SUB_GPIO2_ACTION);
    
    }else {
        mqttClient.publish(PUB_GPIO2_STATUS, "Free");
        mqttClient.subscribe(SUB_GPIO2_ACTION);
    }
  */
  
  // before submit Full check Full 1 times/s 5 time after submit Full
    
    if(cm1 < 10 && cm2 < 10){
      rounds_full = rounds_full +1;
     rounds_free=0;
      if(rounds_full>=5){
        mqttClient.publish(PUB_GPIO2_DATA, "1");
           Serial.print("  send Full  : ");
    Serial.println(rounds_full);
        rounds_full = 0;
        
      }    
    
    }else if(cm1 < 10 && cm2 > 10){
       rounds_free = rounds_free +1;
      rounds_full=0;
      if(rounds_free>=5){
        mqttClient.publish(PUB_GPIO2_DATA, "1");
            Serial.print("  send Error  : ");
    Serial.println(rounds_free);
        rounds_free = 0;
      }     
    
    }else {
      rounds_free = rounds_free +1;
      rounds_full=0;
      if(rounds_free>=5){
        mqttClient.publish(PUB_GPIO2_DATA, "0");
            Serial.print("  send Free  : ");
    Serial.println(rounds_free);
        rounds_free = 0;
      }  
    }

    
    Serial.print("  rounds_full  : ");
    Serial.print(rounds_full);
    Serial.print("  rounds_free  : ");
    Serial.println(rounds_free);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
 
  int i=0;
  while (i<length) msg += (char)payload[i++];

}

void sonar_1()
{
  int position;
  long duration;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  
  int lenght = microsecondsToCentimeters(duration);
  Serial.print("sonar 1 : ");
  Serial.print(lenght);
  Serial.print(" || ");
  cm1 = lenght;
}


void sonar_2()
{
  int position;
  long duration;
  pinMode(pingPin2, OUTPUT);
  digitalWrite(pingPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin2, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin2, LOW);
  pinMode(inPin2, INPUT);
  duration = pulseIn(inPin2, HIGH);
  
  int lenght = microsecondsToCentimeters(duration);
  Serial.print("sonar 2 : ");
  Serial.print(lenght);
  cm2 = lenght;
}

long microsecondsToCentimeters(long microseconds)
{
  
  return microseconds / 29 / 2;
}


void initWifiStation() {

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);    
    Serial.print("\nConnecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
       delay(2000);        
       Serial.print(".");
    }
    Serial.println(String("\nConnected to the WiFi network (") + ssid + ")" );
}
 
void initMQTTClient() {

    // Connecting to MQTT server
    mqttClient.setServer(mqttServer, mqttPort);
    //mqttClient.setCallback(PubSubCallback);
    while (!mqttClient.connected()) {
        Serial.println(String("Connecting to MQTT (") + mqttServer + ")...");
        if (mqttClient.connect("sonar", mqttUser, mqttPassword)) {
            Serial.println("MQTT client connected");  
        } else {
            Serial.print("\nFailed with state ");
            Serial.println(mqttClient.state());
            
            if (WiFi.status() != WL_CONNECTED) {
               initWifiStation();
            }
            delay(2000);
        }
    }
    
    // Declare Pub/Sub topics
    mqttClient.publish(PUB_GPIO2_DATA, "START");
 
}
