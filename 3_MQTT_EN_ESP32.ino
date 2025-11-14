//CURSO DE ARDUINO N3 - MQTT EN ESP32

#include <WiFi.h>
#include <PubSubClient.h>

// ===== CONFIGURACIÓN DE RED WiFi =====
const char* ssid = "AXionics";           // Nombre de la red WiFi
const char* password = "AXionics12345";  // Contraseña de la red

// ===== CONFIGURACIÓN DEL BROKER MQTT =====
const char* mqtt_server = "broker.hivemq.com";  // Servidor público MQTT
const int mqtt_port = 1883;                     // Puerto estándar MQTT
const char* mqtt_topic = "AXionics/test";       // Tópico de comunicación

WiFiClient espClient;           // Cliente base para conexión WiFi
PubSubClient client(espClient); // Cliente MQTT que usa la conexión WiFi

unsigned long lastMsg = 0;      // Variable para controlar envío periódico
const int ledPin = 2;           // LED interno del ESP32 (GPIO 2)

// ===== CALLBACK: FUNCIÓN QUE SE EJECUTA CUANDO LLEGA UN MENSAJE MQTT =====
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("\n Mensaje recibido en el tópico: ");
  Serial.print(topic);
  Serial.print(" -> ");
  
  // Convertir los bytes recibidos en texto legible
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.println(msg);

  // Analizar el mensaje recibido y ejecutar una acción
  if (msg == "ON") {
    digitalWrite(ledPin, HIGH);          // Enciende el LED
    Serial.println("LED encendido");
  } else if (msg == "OFF") {
    digitalWrite(ledPin, LOW);           // Apaga el LED
    Serial.println("LED apagado");
  }
}

// ===== FUNCIÓN: CONECTAR EL ESP32 A LA RED WiFi =====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a la red WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);  // Inicia la conexión con la red

  // Mientras no se conecte, el LED parpadea cada medio segundo
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, !digitalRead(ledPin)); 
    delay(500);
    Serial.print(".");
  }

  // Cuando logra conectarse, deja el LED encendido
  digitalWrite(ledPin, HIGH);
  Serial.println("\n Conectado a WiFi correctamente");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

// ===== FUNCIÓN: RECONEXIÓN AUTOMÁTICA AL BROKER MQTT =====
void reconnect() {
  // Intentará reconectarse hasta que lo logre
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    
    // Intento de conexión con un ID único
    if (client.connect("ESP32Client_AXionics")) {
      Serial.println(" ¡Conectado al broker!");
      
      // Suscripción al tópico para recibir mensajes desde la app
      client.subscribe(mqtt_topic);
      Serial.print(" Suscrito al tópico: ");
      Serial.println(mqtt_topic);
    } 
    else {
      // Si falla, muestra el código de error y reintenta
      Serial.print(" Error, código: ");
      Serial.print(client.state());
      Serial.println(" -> Reintentando en 3 segundos...");
      delay(3000);
    }
  }
}

// ===== CONFIGURACIÓN INICIAL =====
void setup() {
  Serial.begin(115200);       // Inicia comunicación serial
  pinMode(ledPin, OUTPUT);    // Configura el LED como salida

  setup_wifi();               // Conexión a WiFi
  client.setServer(mqtt_server, mqtt_port);  // Configura el broker
  client.setCallback(callback);              // Asigna la función callback
}

// ===== LOOP PRINCIPAL =====
void loop() {
  // Verifica constantemente la conexión MQTT
  if (!client.connected()) {
    reconnect();  // Si se desconecta, intenta reconectarse
  }
  client.loop();  // Mantiene viva la conexión con el broker

  // Cada 5 segundos envía un mensaje al tópico
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    if (client.connected()) {
      client.publish(mqtt_topic, "Hola desde ESP32");
      Serial.println(" Mensaje publicado: Hola desde ESP32");
    }
  }
}
