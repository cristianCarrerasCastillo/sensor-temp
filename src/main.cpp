#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "EEPROM.h"
#include "funcionesEeprom.h"

#define DATA_DHT22 D5 // Pin para el sensor DHT22
#define DHTTYPE DHT22
float hum;
float temp;

#define SSIDCONFIG "TEMP-WIFI"    // Nombre de la red WiFi
#define PASSWORDCONFIG "12345678" // Contraseña de la red WiFi
char ssid[20];
char password[20];

ESP8266WebServer server(80);
IPAddress ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress internalIp;
WiFiClient client;

DHT dht(DATA_DHT22, DHTTYPE);

void setup_wifi() {
  // Conexión WIFI 
  int contconexion=0;
  WiFi.config(ip,gateway,subnet);
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(ssid,password);
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

String mensaje = "";

String pagina = "<!DOCTYPE html>"
"<html>"
"<head>"
"<title>Temp Config</title>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<meta charset='UTF-8'>"
"<style>"
  "tr:nth-child(even){"
  "          background-color: #ddd;"
  "      }"
  ".btn{"
    "background-color: rgb(179, 179, 179);"
    "color: white;"
    "padding: 14px 20px;"
    "margin: 8px 0;"
    "border: none;"
    "cursor: pointer;"
  "}"
  ".center{"
  "position: absolute;"
  "top: 50%;"
  "left: 50%;"
  "transform: translate(-50%, -50%);"
  "width: 300px;"
  "background: white;"
  "border-radius: 10px;"
  "box-shadow: 10px 10px 15px rgba(0,0,0,0.05);"
  "}"
  "input{"
  "margin: 8px 0;"
  "border-radius: 4px;"
  "}"
  ".center h1{"
  "text-align: center;"
  "padding: 20px 0;"
  "border-bottom: 1px solid silver;"
  "}"
"</style>"
"</head>"
"<body style='background-color:#eee;'>"
"<div class='center'>"
"<center><p>Ingrese Wifi</p></center>"
"<form action='guardar_conf' method='get'>"
"<center>SSID <input placeholder='Nombre Wifi' name='ssid' type='text'></center>"
"<br>"
"<center>PASS <input placeholder='Password' name='pass' type='password'></center><br>"
"<center><input class='btn' type='submit' value='GUARDAR'/></center><br>"
"</form>"
"<center><a href='escanear'><button class='btn'>ESCANEAR</button></a><br><br>";

String paginafin = "</div>"
"</body>"
"</html>";

void paginaConfig(){
  server.send(200, "text/html", pagina + mensaje + paginafin);
}

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
    mensaje = "<table><tr><th>Name</th><th>Rssi</th></tr>";
    for (int i = 0; i < n; ++i)
    {
      // agrega al STRING "mensaje" la información de las redes encontradas 
      mensaje = (mensaje)+"<tr><td>" + WiFi.SSID(i) + "</td> <td>" + WiFi.RSSI(i) + "</td></tr>\r\n";
      delay(10);
    }
    mensaje = mensaje +"</table>";
    Serial.println(mensaje);
    paginaConfig();
  }
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
  server.on("/escanear", escanear); //Escanean las redes wifi disponibles
  server.begin();
  while (true) {
    server.handleClient();
  }
  setup_wifi();
}

void deep_sleep(int stime_sleep) {
  //Requiere conectar el pin D0 al pin de reset del ESP32

  Serial.println("entrando en modo de sleep");
  ESP.deepSleep(stime_sleep * 1000000); //segundos * 1000000 = segundos
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
  Serial.println(WiFi.localIP());
  delay(10);

}

void loop() {
  // put your main code here, to run repeatedly:
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Error de lectura");
  }
  else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" *C ");
    Serial.print("Humedad: ");
    Serial.print(hum);
    Serial.println(" %");
    deep_sleep(10);
  }
  delay(1000);
}