#include <debug.h>
#include <MicroGear.h>
#include <AuthClient.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <PubSubClient.h>

const char* ssid = "bank";
const char* password = "10001000";
#define APPID   "EmbedBanktts"
#define KEY     "pl3KdqdJuxOOZFe"
#define SECRET  "BqCeaePBAPFfU1Ix7jPlFQYPu"
#define ALIAS   "ESP32"
#define TARGET   "Banktts"
#define LED     2
const int LED_PIN = 21;
const int PHOTO_CELL = 34;
const int WAITTIME = 50;
const int STEP = 5;

int light_value;
int incomingByte;
const int light_max=2600;
int buffers;
WiFiClient client;
int timer = 0;
MicroGear microgear(client);
void onMsghandler(char* topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();
  String stateStr = String(strState).substring(0, msglen);
  if (stateStr == "CH1ON")
  {
    digitalWrite(LED, HIGH);
    Serial.println("ON LED");
  }
  else if (stateStr == "CH1OFF")
  {
    digitalWrite(LED, LOW);
    Serial.println("OFF LED");
  }
}
void onFoundgear(char* attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
void onLostgear(char* attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
/*When a microgear is connected, dothis*/
void onConnected(char* attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgearALIAS */
  microgear.setAlias(ALIAS);
}



void setup()
{
  ledcSetup(0, 5000, 13);
  ledcAttachPin( LED_PIN, 0 );
 
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(LED, OUTPUT);
  digitalWrite(LED,LOW);

  if (WiFi.begin(ssid, password))
  {
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFiconnected");
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);

}



void loop()
{
  int i;
  light_value = analogRead(PHOTO_CELL);
  incomingByte= Serial.read();
  Serial.println(light_value);
  buffers=Serial.read();
   ledcWrite( 0, buffers );
  char tmp[10];
  sprintf(tmp,"%d",light_value);
  if (microgear.connected()) {
    microgear.loop();
    microgear.chat("Banktth",tmp);
    bool led = digitalRead(LED);
    String status_LED;
    if (led == HIGH) {
      status_LED = "CH1ON,";
    }
    else {
      status_LED = "CH1OFF,";
    }
    String data2freeboard = status_LED ;
    microgear.chat(TARGET, data2freeboard);
    Serial.print("Send message to NetPie: ");
    Serial.println(data2freeboard);
  }
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(1000);
}
