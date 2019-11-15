
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo myservo; 
WiFiClient espClient;
PubSubClient client(espClient);

// Update these with values suitable for your network.
const char* ssid         = "UserName_2.4G";
const char* password     = "12123435";
// Config MQTT Server
#define mqtt_server "soldier.cloudmqtt.com"
#define mqtt_port 15978

#define mqtt_user "user2"
#define mqtt_password "12345"



void setup() {
  
  myservo.attach(5); // set port esp8266
  Serial.begin(9600);
  set_up_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); 

    myservo.write(0); // สั่งให้ Servo หมุนไปองศาที่ 0
    delay(1000); // หน่วงเวลา 1000ms

}
void set_up_wifi(){
   delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void loop() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("servo", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/ESP/LED");
     client.subscribe("/ESP/Clear");
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i=0;
  while (i<length) msg += (char)payload[i++];

  
  Serial.println(msg);

  
   if(msg=="1"||msg=="0"){
    myservo.write(0); // สั่งให้ Servo หมุนไปองศาที่ 0
    delay(1000); // หน่วงเวลา 1000ms

  }else
  if(msg!=""){
    myservo.write(110); // สั่งให้ Servo หมุนไปองศาที่ 0
    delay(8000); // หน่วงเวลา 1000ms
  
  }

}
