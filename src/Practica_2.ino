#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>

#include <stdio.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 27
#define DHTTYPE DHT11
#define MI_ABS(x) ((x) < 0 ? -(x) : (x))

TFT_eSPI tft = TFT_eSPI();
DHT dht(DHTPIN, DHTTYPE);

/******
 * Define Constants
 ******/
const char *UBIDOTS_TOKEN = "BBFF-rva1GqRKd6wMpqEcSWFtE4mDQ2lH0z";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "Practica2";   // Put here your Device label to which data will be published
const char *VARIABLE_LABEL1= "sw1";
const char *VARIABLE_LABEL2= "sw2";
const char *TEMPERATURA_VARIABLE_LABEL = "temp"; // Temperatura
const char *HUMEDAD_VARIABLE_LABEL = "hum"; // humedad

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);

int tamano;
int posicion;
char boton = '0';
char val = '0';

bool sw1State = false; // Estado inicial del sw1 (apagado)
bool sw2State = false; // Estado inicial del sw2 (apagado)
uint16_t greenColor = TFT_GREEN; // Color verde
uint16_t blueColor = TFT_BLUE;   // Color azul
uint16_t grayColor = TFT_DARKGREY; // Color gris

const int LED_PIN = 26;

/******
 * Auxiliar Functions
 ******/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
            
  for (int i = 0; i < length; i++)
  {Serial.print((char)payload[i]);
  }
  Serial.println();

  tamano =strlen (topic);
  posicion =tamano-4;
  printf("switch: %c\n",topic[posicion]);
  boton = topic[posicion];
  val = payload[0];
    if (boton == '1')
    
    if ((char)payload[0] == '1') {
    sw1State = true; // Cambiar estado de sw1 a encendido
    tft.fillCircle(190,25, 10, TFT_PURPLE); // Dibujar círculo verde
  } else {
    sw1State = false; // Cambiar estado de sw1 a apagado
    tft.fillCircle(190,25, 10, TFT_DARKGREY); // Dibujar círculo gris
  }
    if (boton == '2')
  if ((char)payload[0] == '1') {
    sw2State = true; // Cambiar estado de sw2 a encendido
    tft.fillCircle(220, 25, 10, TFT_NAVY); // Dibujar círculo azul
  } else {
    sw2State = false; // Cambiar estado de sw2 a apagado
    tft.fillCircle(220, 25, 10,TFT_DARKGREY); // Dibujar círculo gris
  }
  }

/******
 * Main Functions
 ******/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  float hum = dht.readHumidity();
  float temp=dht.readTemperature();
  tft.init();
  tft.fillScreen(TFT_WHITE);
  tft.setRotation(3);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Realizado por:", 10, 5, 2);
  tft.setTextColor(TFT_MAGENTA, TFT_WHITE);
  tft.drawString("Silvia Rueda", 10, 23, 4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Humedad", 10, 70, 2);
  tft.drawString("Temperatura", 140, 70, 2);
  tft.drawFastHLine(10, 50, 170, TFT_MAGENTA);
  tft.fillRect(110, 65, 3, 80, TFT_MAGENTA);

  tft.fillCircle(190,25, 10, grayColor); // Círculo verde
  tft.fillCircle(220, 25, 10,grayColor); // Círculo azul
  pinMode(LED_PIN, OUTPUT); // Configurar el pin del LED como salida
  digitalWrite(LED_PIN, LOW); // Apagar el LED al inicio


  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  timer = millis();

  
}

void loop()
{
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2); 
  }
  
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  
  if ((MI_ABS(millis() - timer)) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" | Humedad: ");
    Serial.println(hum);
    
     tft.drawString(String(temp), 150, 100);
    tft.drawString(String(hum), 20, 100);
    ubidots.add(TEMPERATURA_VARIABLE_LABEL, temp);  
    ubidots.add(HUMEDAD_VARIABLE_LABEL, hum);   
        ubidots.publish(DEVICE_LABEL);     
    timer = millis();
  }
  
  ubidots.loop();
  // Cambiar color de los círculos en base a los estados de sw1 y sw2
  if (sw1State) {
    tft.fillCircle(190,25, 10, TFT_PURPLE);
    digitalWrite(LED_PIN, HIGH); // Encender el LED
  } else {
    tft.fillCircle(190,25, 10, TFT_DARKGREY);
    digitalWrite(LED_PIN, LOW); // Apagar el LED
  }


  if (sw2State) {
    tft.fillCircle(220, 25, 10, TFT_NAVY);
  } else {
    tft.fillCircle(220, 25, 10,TFT_DARKGREY);
  }
  
}
