//CURSO DE ARDUINO N3 - PROYECTO FINAL

#include <WiFi.h>           // Librería para conectar el ESP32 a una red WiFi
#include <PubSubClient.h>   // Librería para la comunicación MQTT
#include "DHT.h"            // Librería para el sensor DHT11


// ===== CONFIGURACIÓN DE RED WiFi =====
const char* ssid = "AXionics";           // Nombre de tu red
const char* password = "AXionics12345";  // Contraseña de la red


// ===== CONFIGURACIÓN DEL BROKER MQTT =====
// En este caso usamos un broker público gratuito.
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;


// ===== CONFIGURACIÓN DEL SENSOR DHT11 =====
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); // Inicializa el objeto del sensor


// ===== CONFIGURACIÓN DE LEDs =====
const int led1 = 5;   // LED 1 conectado al pin GPIO 5
const int led2 = 18;  // LED 2 conectado al pin GPIO 18


// ===== CLIENTES DE RED Y MQTT =====
// Estos objetos gestionan la conexión del ESP32 a Internet y al broker.
WiFiClient espClient;
PubSubClient client(espClient);


// ===== VARIABLES GLOBALES =====
unsigned long lastMsg = 0;


// ===== FUNCIÓN CALLBACK: Se ejecuta cuando llega un mensaje =====
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("\n Mensaje recibido en ");
  Serial.print(topic);
  Serial.print(": ");
  
  // Convertir el mensaje recibido (bytes) a texto legible
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.println(msg);

  // Control de LEDs según el tópico
  if (String(topic) == "AXionics/led1") {
    digitalWrite(led1, msg == "ON" ? HIGH : LOW);
  } 
  else if (String(topic) == "AXionics/led2") {
    digitalWrite(led2, msg == "ON" ? HIGH : LOW);
  }
}

// ===== CONEXIÓN A WiFi =====
void setup_wifi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  // Esperar hasta que el ESP32 se conecte a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Una vez conectado, mostramos la IP asignada
  Serial.println("\n Conectado a WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===== RECONEXIÓN AL BROKER MQTT =====
// Si la conexión se pierde, el ESP32 intentará reconectarse automáticamente.
void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker...");
    if (client.connect("ESP32_AXionics_Final")) {
      Serial.println("¡Conectado!");
      // Suscripciones
      client.subscribe("AXionics/led1");
      client.subscribe("AXionics/led2");
    } else {
      Serial.print(" Error: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  dht.begin();

  setup_wifi();

  // Configuración del cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ===== LOOP PRINCIPAL =====
void loop() {
  // Reintenta conexión si se pierde la comunicación MQTT
  if (!client.connected()) reconnect();
  client.loop();

  // Enviar lecturas del DHT cada 5 segundos
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();
    
    // Validar que las lecturas sean correctas
    if (!isnan(temp) && !isnan(hum)) {
      // Crear mensaje en formato de texto
      char msg[50];
      snprintf(msg, 50, "Temp: %.1f °C | Hum: %.1f%%", temp, hum);

      // Publicar los datos en el tópico correspondiente
      client.publish("AXionics/dht", msg);
      Serial.println("Datos enviados: " + String(msg));
    }
  }
}
