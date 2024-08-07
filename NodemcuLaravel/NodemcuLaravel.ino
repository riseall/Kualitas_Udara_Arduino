#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

// Ganti dengan SSID dan password WiFi Anda
#define WIFI_SSID "Riseall"
#define WIFI_PASSWORD "rizal123"

// URL API Laravel
const char* serverNameLaravel = "http://192.168.47.74/air_monitoring/public/api/sensor-data";
// // URL API Flask
// const char* serverNameFlask = "http://192.168.47.74:5000/notify"; // Pastikan IP dan port sesuai

// Pin untuk komunikasi serial dengan Arduino
#define RX_PIN D2
#define TX_PIN D3

SoftwareSerial espSerial(RX_PIN, TX_PIN); // RX, TX pins

unsigned long dataMillis = 0;

void setup() {
  Serial.begin(9600);

  // Koneksi ke jaringan WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Inisialisasi komunikasi serial dengan Arduino
  espSerial.begin(9600);
}

void loop() {
  // Periksa koneksi WiFi dan hubungkan kembali jika terputus
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi");
  }

  if (millis() - dataMillis > 5000) {
    dataMillis = millis();

    // Check if data is available on the serial port
    if (espSerial.available()) {
      String dataString = espSerial.readStringUntil('\n'); // Read data string from Arduino
      Serial.println("Data received: " + dataString);

      // Variables to hold parsed values
      float co2, no2, co, benzene, toluene, pm25, humidity, temperature;

      // Parse the data string
      int numParsed = sscanf(dataString.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f", &co2, &no2, &co, &benzene, &toluene, &pm25, &humidity, &temperature);
      if (numParsed == 8) {
        // Data successfully parsed, prepare JSON object
        String jsonData = "{\"co2\":" + String(co2) + ",\"no2\":" + String(no2) + ",\"co\":" + String(co) + ",\"benzene\":" + String(benzene) + ",\"toluene\":" + String(toluene) + ",\"pm25\":" + String(pm25) + ",\"humidity\":" + String(humidity) + ",\"temperature\":" + String(temperature) + "}";

        if (WiFi.status() == WL_CONNECTED) {
          WiFiClient wifiClient;
          HTTPClient http;

          // Mengirim data ke Laravel
          http.begin(wifiClient, serverNameLaravel);
          http.addHeader("Content-Type", "application/json");

          int httpResponseCode = http.POST(jsonData);

          if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
          } else {
            Serial.print("Error on sending POST to Laravel: ");
            Serial.println(httpResponseCode);
          }

          http.end();

          // // Mengirim notifikasi ke Flask
          // http.begin(wifiClient, serverNameFlask);
          // http.addHeader("Content-Type", "application/json");

          // int notifyResponseCode = http.POST(jsonData);

          // if (notifyResponseCode > 0) {
          //   String notifyResponse = http.getString();
          //   Serial.println(notifyResponseCode);
          //   Serial.println(notifyResponse);
          // } else {
          //   Serial.print("Error on sending POST to Flask: ");
          //   Serial.println(notifyResponseCode);
          // }

          // http.end();
        } else {
          Serial.println("Error in WiFi connection");
        }
      } else {
        Serial.println("Error parsing data:");
        Serial.println(dataString);
      }

      // Clear the serial buffer
      while (espSerial.available()) {
        espSerial.read();
      }
    }
  }
}
