#ifndef _SENDMSGAWS_H
#define _SENDMSGAWS_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#include <time.h>

#define AWS_IOT_PUB_TOPIC "esp32/pub"

BearSSL::WiFiServerSecure serverSSL(443);

WiFiClientSecure net = WiFiClientSecure();
PubSubClient clientMqtt(net);

void setClock()
{
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

void connectAws(){
    setClock();

    BearSSL::X509List *serverCertList = new BearSSL::X509List(AWS_CERT_CRT);
    BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(AWS_CERT_PRIVATE);
    serverSSL.setRSACert(serverCertList, serverPrivKey);

    // Require a certificate validated by the trusted CA
    BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(AWS_CERT_CA);
    serverSSL.setClientTrustAnchor(serverTrustedCA);

    clientMqtt.setServer(AWS_IOT_ENDPOINT, 8883);

    clientMqtt.setCallback(messageHandler);
    Serial.println("Conectando a AWS...");
    while(!clientMqtt.connect(THING_NAME)){
        Serial.print(".");
        delay(100);
    }
    if(!clientMqtt.connected()){
        Serial.println("Error al conectar con AWS");
        return;
    }

    Serial.println("Conectado a AWS");
}
void publishMsg(int temp, int hum){
    StaticJsonDocument<200> doc;
    doc["temperatura"] = temp;
    doc["humedad"] = hum;
    char jsonBuffer[500];
    serializeJson(doc, jsonBuffer);
    clientMqtt.publish(AWS_IOT_PUB_TOPIC, jsonBuffer);

}

#endif