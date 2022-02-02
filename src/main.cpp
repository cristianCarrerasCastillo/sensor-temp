#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "EEPROM.h"
#include "funcionesEeprom.h"
#include "pages.h"
#include "sendMsgAws.h"

#define DATA_DHT22 D4 // Pin para el sensor DHT22
#define DHTTYPE DHT22
float hum;
float temp;

#define SSIDCONFIG "TEMP-WIFI"    // Nombre de la red WiFi
#define PASSWORDCONFIG "12345678" // Contraseña de la red WiFi
char ssid[20];
char password[20];

ESP8266WebServer server(80);
IPAddress ip(192, 168, 1, 130);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8); // Google DNS
IPAddress internalIp;
WiFiClient client;

DHT dht(DATA_DHT22, DHTTYPE);




void setup_wifi() {
  // Conexión WIFI 
  int contconexion=0;
  WiFi.config(ip,gateway,subnet);
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(ssid,password);
  WiFi.config(ip, gateway, subnet, dns);
  Serial.println(ssid);
  Serial.println(password);  
  while (WiFi.status() != WL_CONNECTED and contconexion <30){
    delay(500);
    Serial.print(".");
    contconexion++;
  }
    if(WiFi.status() != WL_CONNECTED){
    Serial.println("");
    Serial.println("Error de conexion");  
    return;  
    }
  else{
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println(ssid);
    Serial.println(WiFi.localIP());
  }
  Serial.println("");
}

String mensaje = ""; // mensaje que se quiera añadir a la pagina web

void escanear() {
  int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
  Serial.println("escaneo terminado");
  if (n == 0) { //si no encuentra ninguna red
    Serial.println("no se encontraron redes");
    mensaje = "no se encontraron redes";
  }
  else
  {
    Serial.print(n);
    Serial.println(" redes encontradas");
    mensaje = "";
    for (int i = 0; i < n; ++i)
    {
      // agrega al STRING "mensaje" la información de las redes encontradas 
      mensaje = (mensaje)+"<option value='" + WiFi.SSID(i) +"'>" + WiFi.SSID(i) + "</option>\r\n";
      delay(10);
    }
    Serial.println(mensaje);
  }
}

void paginaConfig(){
  escanear();
  server.send(200, "text/html", header_html + body_page_wifi_scan(mensaje) + footer_html);
}

void wifi_config() {
  int len_ssid = 0;
  int len_pass = 0;

  len_ssid = sizeof(ssid);
  len_pass = sizeof(password);

  String wifi_id = "";
  String wifi_pass = "";

  Serial.println("SSid:");
  wifi_id = leerEeprom(0,len_ssid);
  Serial.println("psw:");
  wifi_pass = leerEeprom(len_ssid,(len_ssid +len_pass));
  wifi_id.toCharArray(ssid,sizeof(ssid));
  wifi_pass.toCharArray(password,sizeof(password));
  Serial.println(wifi_id);
  Serial.println(wifi_pass);

}

void guardar_conf() {
  Serial.println(server.arg("ssid"));//Recibimos los valores que envia por GET el formulario web
  grabarEeprom(0, server.arg("ssid"));
  Serial.println(server.arg("pass"));
  grabarEeprom(sizeof(ssid), server.arg("pass"));
  mensaje = "Configuracion Guardada...";
  wifi_config();
  setup_wifi();
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Clave Invalida");
    paginaConfig();
  }
  else{
    ESP.restart();
  }  
}

void modoconf() {
  WiFi.softAP(SSIDCONFIG, PASSWORDCONFIG);
  IPAddress myIP = WiFi.softAPIP();//ip por defecto para conectarse es la 192.168.4.1 <--------------
  Serial.print("IP del acces point: ");
  Serial.println(myIP);
  Serial.println("WebServer iniciado...");

  borrarEeprom(0,100);//borra los datos ssid y pass que estaban guardados anteriormente al no encontrarlos en la memoria
  server.on("/", paginaConfig); //esta es la pagina de configuracion
  server.on("/guardar_conf", guardar_conf); //Graba en la eeprom la configuracion
  server.begin();
  while (true) {
    server.handleClient();
  }
}

void deep_sleep(int64_t stime_sleep) {

  //Requiere conectar el pin D0 al pin de reset del ESP32
  Serial.println("entrando en modo de sleep");
  ESP.deepSleep(stime_sleep * 1000000); //stime_sleep * 1000000 = X segundos
}

void handle_OnConnect() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  
  server.send(200, "text/html",header_html + body_page_tempHum(temp,hum) + footer_html);
}

void setup() {
  
  Serial.begin(9600);
  EEPROM.begin(512);
  dht.begin();
  wifi_config();
  setup_wifi();
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("modo softAp iniciado");
    modoconf();
  }
  else{
    server.on("/", handle_OnConnect);
    server.begin();
    Serial.println("WebServer iniciado...");
    connectAws();
    Serial.println("AWS conectado");
  }
  Serial.println(WiFi.localIP());
  delay(10);

}

void loop() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  server.handleClient();
  publishMsg(temp,hum);
  deep_sleep(10000);
}

