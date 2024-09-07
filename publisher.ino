// publisher code 

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

char wifiSSID[] = "oplus_co_apdmkd";
char wifiPassword[] = "tohm3279";

const int triggerPin = 9;
const int echoPin = 10;
const int ledPin = 6;

float duration, distance;
float speedOfSound = 0.034;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

String lastMessage = "";

const char mqttBroker[] = "broker.emqx.io";
int mqttPort = 1883;
const char mqttTopic[] = "SIT210/waves";

const long interval = 1000;
long prevMills = 0;

void setup() {
  Serial.begin(9600);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectToWiFi();
  connectToMQTTBroker();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTTBroker();
  }
  mqttClient.poll();

  long currMills = millis();

  if (currMills - prevMills >= interval) {
    prevMills = currMills;

    distance = MeasureDistance();

    Serial.print("Distance: ");
    Serial.println(distance);

    sendmessage();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifiSSID);

  while (WiFi.begin(wifiSSID, wifiPassword) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("WiFi Connected");
}

void connectToMQTTBroker() {
  Serial.print("Connecting to MQTT Broker: ");
  Serial.println(mqttBroker);

  while (!mqttClient.connect(mqttBroker, mqttPort)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("MQTT Broker Connected");
}

float MeasureDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(3);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH, 25000);

  return (duration * speedOfSound) / 2;
}

void sendMessage() {
  if (distance > 0 && distance < 15) {
    mqttClient.beginMessage(mqttTopic);
    if (distance < 10) {
      mqttClient.print("Wave");
      Serial.println("Wave Message Sent!");
    } else {
      mqttClient.print("Pat");
      Serial.println("Pat Message Sent!");
    }

    mqttClient.endMessage();
    delay(3000);  
  }
}
