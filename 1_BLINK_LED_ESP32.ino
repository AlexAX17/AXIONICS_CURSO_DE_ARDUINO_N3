//CURSO DE ARDUINO N3 - BLINK DE UN LED CON ESP32

int ledPin = 2; // Pin donde conectaste el Led
//Nota: Algunos ESP32 ya tienen un LED integrado en este pin

void setup() {
  pinMode(ledPin, OUTPUT); // Configura el pin como salida
}

void loop() {
  digitalWrite(ledPin, HIGH); // Enciende LED
  delay(1000);                // Espera 1 segundo
  digitalWrite(ledPin, LOW);  // Apaga LED
  delay(1000);                // Espera 1 segundo
}
