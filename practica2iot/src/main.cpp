#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//WIfi SSID & PASS
const char *ssid = "xxxxxxxxxxxx"; 
const char *password = "xxxxxxxxxxx";
const char *mqttserver = "xxxxxxxxxxx"; // Dirección IP MQTT broker
const int mqttport = 1883;               // Puerto de MQTT broker
const char *input = "/Timezone";         // variable del topico de entrada 
const char *output = "/output";          // variable del topico de salida 

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// metodo para conectar ESP8266 con Broker 
void connection_mqtt()
{ 
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client"))
    {
      Serial.println("Connected");
      client.subscribe(input);
    }
    else
    {
      Serial.println("Failed with state ");
      Serial.print(client.state());
      delay(200);
    }
  }
}

// Metodo para consumir API con hora y fecha mundial
void API_world_time(byte *payload, unsigned int length)
{
  String url = "http://worldtimeapi.org/api/timezone/";
  HTTPClient http;
  // Make a GET request to the World Time API
  char mensaje[5] = {0x00};
  for (int i = 0; i < length; i++)
    mensaje[i] = (char)payload[i];
  mensaje[length] = 0x00;

  url += String(mensaje);

  if (http.begin(espClient, url))
  { // conexión HTTP con el ESP8266 y la url

    int httpCode = http.GET(); 
    if (httpCode > 0 || httpCode == HTTP_CODE_OK)
    {
      connection_mqtt();
      String payload = http.getString();
      String day = "";
      String num_month = "";
      String month = "";
      String year = "";
      String timeS = "";
      String month_obtained = "";
      String day_obtained = "";

      int datetime = payload.indexOf("datetime");
      int dayofweek = payload.indexOf("day_of_week");
      int dayofyear = payload.indexOf("day_of_year");
      // Serial.println(payload);
      day = payload.substring(dayofweek + 13, dayofyear - 2);
      num_month = payload.substring(datetime + 19, dayofweek - 25);
      month = payload.substring(datetime + 16, dayofweek - 28);
      year = payload.substring(datetime + 11, dayofweek - 31);
      timeS = payload.substring(datetime + 22, dayofweek - 19);

      if (day == "1")
      {
        day_obtained = "Lunes";
      }
      else if (day == "2")
      {
        day_obtained = "Martes";
      }
      else if (day == "3")
      {
        day_obtained = "Miercoles";
      }
      else if (day == "4")
      {
        day_obtained = "Jueves";
      }
      else if (day == "5")
      {
        day_obtained = "Viernes";
      }
      else if (day == "6")
      {
        day_obtained = "Sabado";
      }
      else if (day == "0")
      {
        day_obtained = "Domingo";
      }

      if (month == "01")
      {
        month_obtained = "Enero";
      }
      else if (month == "02")
      {
        month_obtained = "Febrero";
      }
      else if (month == "03")
      {
        month_obtained = "Marzo";
      }
      else if (month == "04")
      {
        month_obtained = "Abril";
      }
      String fecha_hora = day_obtained + ", " + num_month + " de " + month_obtained + " de " + year + " -- " + timeS;
      //Topico de salida con la fecha y hora solicitada
      client.publish(output, fecha_hora.c_str());
      return; 
    }
    else
    {
      const String error = http.errorToString(httpCode);
      Serial.println("Error making API call");
      Serial.printf("[HTTP] GET... failed, error: %s", error.c_str());
      Serial.println("");
    }
  }
 http.end();
 delay(1000);
}
// Metodo de notificación del mensaje llegado
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  API_world_time(payload, length);
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttserver, mqttport);
  client.setCallback(callback);

  //Conecta a mqtt broker 
  connection_mqtt();
  client.subscribe(input);
}

void loop()
{
  connection_mqtt();
  client.loop();
}