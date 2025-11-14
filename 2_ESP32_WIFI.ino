//CURSO DE ARDUINO N3 - CONECTA TU ESP32 A WIFI
#include <WiFi.h>  // Librería para manejar el WiFi en ESP32

// Datos de tu red WiFi
const char* ssid = "AXionics";
const char* password = "AXionics12345";

WiFiServer server(80);  // Servidor web en el puerto HTTP (80)

void setup() {
  Serial.begin(115200);   // Inicia comunicación serie
  pinMode(2, OUTPUT);     // LED en pin 2 como salida (indicador de conexión)

  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);  // Inicia conexión con tu red

  // Espera hasta que se conecte al WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Cuando conecta, muestra IP y enciende el LED
  Serial.println("\n¡Conectado a WiFi!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.begin();         // Inicia el servidor web
  digitalWrite(2, HIGH);  // LED encendido indica éxito
}

void loop() {
  // Revisa si hay algún cliente intentando conectarse
  WiFiClient client = server.available();  
  if (client) {
    Serial.println("Nuevo cliente conectado");

    // Lee la primera línea de la petición HTTP
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();  // Limpia el buffer

    // Responde al cliente con una página HTML simple
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html><html>");
    client.println("<h1>¡Hola desde ESP32!</h1>");
    client.println("<p>Tu mensaje llego correctamente :)</p>");
    client.println("</html>");
    client.println();

    client.stop();  // Cierra conexión con el cliente
    Serial.println("Cliente desconectado");
  }
}
