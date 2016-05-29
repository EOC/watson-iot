#include <SPI.h>
#include <Ethernet2.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <SoftwareSerial.h>

//Bluetooth Settings
#define RxD  7
#define TxD  6
SoftwareSerial BlueToothSerial(RxD,TxD);
char flag=1;

#define MQTT_MAX_PACKET_SIZE 100
#define SIZE 100
#define MQTT_PORT 1883

#define PUBLISH_TOPIC "iot-2/evt/status/fmt/json"
#define SUBSCRIBE_TOPIC "iot-2/cmd/+/fmt/json"
#define AUTHMETHOD "use-token-auth"

#define CLIENT_ID "d:ywsuqc:ArduinoBoard:90A2DA103E03"
#define MS_PROXY "ywsuqc.messaging.internetofthings.ibmcloud.com"
#define AUTHTOKEN "Arduino001"
// Update these with values suitable for your network.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0x3E, 0x03 };
EthernetClient c; // replace by a YunClient if running on a Yun
IPStack ipstack(c);

MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

void setup() {
  Serial.begin(9600);
  Serial.println("Setting up Ethernet...");
  if (!Ethernet.begin(mac)) {
    Serial.println("Failed to configure Ethernet");
    return;
  }
  Serial.println("Ethernet ready!");
  delay(1000);
  BlueToothSerial.begin(38400);
  delay(500);
  BlueToothSerial.println("****** Bluetooth Controller Example ******");  
  delay(1000);
}

void loop() {
  int rc = -1;
  BlueToothSerial.println("Please enter some text: ");
  while (BlueToothSerial.available()==0) {  
  }
  String serialString = BlueToothSerial.readString();
  if (!client.isConnected()) {
    Serial.print("Connecting using Registered mode with clientid : ");
    Serial.print(CLIENT_ID);
    Serial.print("\tto MQTT Broker : ");
    Serial.print(MS_PROXY);
    Serial.print("\ton topic : ");
    Serial.println(PUBLISH_TOPIC);
    while (rc != 0) {
      rc = ipstack.connect(MS_PROXY, MQTT_PORT);
    }
    Serial.println("Can connect to hub!");
    MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
    options.MQTTVersion = 3;
    options.clientID.cstring = CLIENT_ID;
    options.username.cstring = AUTHMETHOD;
    options.password.cstring = AUTHTOKEN;
    options.keepAliveInterval = 10;
    rc = -1;
    while ((rc = client.connect(options)) != 0)
      ;
    Serial.println("Can connect using mqtt!");
    Serial.println("Subscription tried......");
    Serial.println("Connected successfully\n");
    Serial.println("Device Event (JSON)");
    Serial.println("____________________________________________________________________________");
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0; 
  message.retained = false;
  char json[56] = "{\"d\":{\"message\":\"NodeRedTest\"}}";
  json[55] = '\0';
  Serial.println(json);
  message.payload = json; 
  message.payloadlen = strlen(json);
  rc = client.publish(PUBLISH_TOPIC, message);
  if (rc != 0) {
    Serial.print("Message publish failed with return code : ");
    Serial.println(rc);
  }
  Serial.println("Result:" + rc);
  client.yield(1000);
}

void sendBlueToothCommand(char *Command) {
    BlueToothSerial.print(Command);
    Serial.print(Command);
    delay(100);
    while(BlueToothSerial.available()) {
      Serial.print(char(BlueToothSerial.read()));
    }
}
