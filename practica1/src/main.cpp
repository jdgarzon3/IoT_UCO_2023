#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// En este campo se ingresa nombre de red local y contraseña
const char *ssid = "RedmiNote8";
const char *password = "12345678";

// se crea un servidor con el puerto 80
ESP8266WebServer server(80);

// variable para HTML
String WebPage = "";

// variables auxiliares para el estado de la salida
String output1State = "off";
String output2State = "off";

// metodo para manejar las respuestas de los clientes en HTML
void handleRoot(){

  WebPage += "<!DOCTYPE html><html>";
  WebPage += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  WebPage += "<link rel=\"icon\" href=\"data:,\">";
  WebPage += "<style>html { font-family: Arial; display: inline-block; margin: 10px auto; text-align: center}";
  WebPage += "</style></head>";

  WebPage += "<body><h1> <FONT SIZE='5' COLOR='red'> PRACTICA 1 IOT </h1>";
  WebPage += "<p> <FONT SIZE='5' COLOR=#3498DB> Led 1 - Estado " + output1State + " </p>";

  WebPage += "<p> <a href=\"/1/on\"><button style='width:100px; height:50px; background-color: #4CAF50; border: none; color: white; padding: 16px 35px;font-size: 20px; margin: 2px; cursor: pointer' >ON</button></a></p>";
  
  WebPage += "<p> <a href=\"/1/off\"><button style='width:100px; height:50px; background-color: #E74C3C; border: none; color: white; padding: 16px 20px;font-size: 20px; margin: 2px; cursor: pointer' >OFF</button></a></p>";
  
  WebPage += "<p> <FONT SIZE='5' COLOR=#3498DB> Led 2 - State " + output2State + " </p>";

  WebPage += "<p> <a href=\"/2/on\"><button style='width:100px; height:50px; background-color: #4CAF50; border: none; color: white; padding: 16px 35px;font-size: 20px; margin: 2px; cursor: pointer' >ON</button></a></p>";
  WebPage += "<p> <a href=\"/2/off\"><button style='width:100px; height:50px; background-color: #E74C3C; border: none; color: white; padding: 16px 20px;font-size: 20px; margin: 2px; cursor: pointer' >OFF</button></a></p>";
 
 WebPage += "</body></html>";

}
//metodo para encender led1
void encenderled1(){
  output1State = "on";
  Serial.println("Out 1 on");    
  digitalWrite(D1, HIGH);
  server.send(200,"text/plain","LED1ON");
}

//metodo para apagar led1
void apagarled1(){
  output1State = "off";
  Serial.println("Out 1 off");    
  digitalWrite(D1, LOW);
  server.send(200,"text/plain","LED1OFF");
}

//metodo para encender led2
void encenderled2(){
  output1State = "on";
  Serial.println("Out 2 on");    
  digitalWrite(D2, HIGH);
  server.send(200,"text/plain","LED2ON");
}

//metodo para apagar led2
void apagarled2(){
  output1State = "off";
  Serial.println("Out 2 off");    
  digitalWrite(D2, LOW);
  server.send(200,"text/plain","LED2OFF");
}

void setup(){
  handleRoot();
  Serial.begin(115200);
  // asigno pines a cada led
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  //inicializo los leds en bajo
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  // se lee la instrucción del cliente del sitio web
  server.on("/1/on", encenderled1);
  server.on("/1/off", apagarled1);
  server.on("/2/on", encenderled2);
  server.on("/2/off", apagarled2);

  // comienza la comunicación serial y wifi
  WiFi.begin(ssid, password);
  Serial.println("");
  // mientras el wifi se carga se muestra cada 2 seg conectando
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Conectando...");
    delay(2000);
  }

  // obtenemos la dirección IP y se muestra
  Serial.print("Conectado");
  Serial.print("Dirección IP del modulo: ");
  Serial.println(WiFi.localIP());

  // Web Page handler on root "/" using .send() method from ESP8266WebServer class
  server.on("/", []()
            { server.send(200, "text/html", WebPage); });

  // iniciamos el servidor web
  server.begin();
  Serial.print("HTTP Server Started");
}

void loop()
{
  // hacemos una variable de cliente
  server.handleClient();

}
