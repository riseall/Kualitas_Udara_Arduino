#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define pinAOutMQ3 A0   // Analog input pin for MQ3
#define pinAOutMQ7 A1   // Analog input pin for MQ7
#define pinAOutMQ135 A2 // Analog input pin for MQ135
#define measurePin A3   // Analog input pin for SharpGp2y1010a0uf
#define RL_VALUE 10.0   // Resistance in kohms of the load resistor

SoftwareSerial espSerial(2, 3); // RX, TX pins (replace if different)
#define DHTPIN 6          // Digital pin connected to the DHT sensor
#define ledPower 7        // Digital pin connected to SharpGp2y1010a0uf
#define DHTTYPE DHT22     // DHT 

float voMeasured = 0;
float calcVoltage = 0;
float pm25 = 0;

// Declare Sensors
MQUnifiedsensor MQ135("Arduino", 5.0, RL_VALUE, pinAOutMQ135, "MQ-135");
MQUnifiedsensor MQ3("Arduino", 5.0, RL_VALUE, pinAOutMQ3, "MQ-3");
MQUnifiedsensor MQ7("Arduino", 5.0, RL_VALUE, pinAOutMQ7, "MQ-7");
DHT dht(DHTPIN, DHTTYPE);
//DHT dht11(DHTPIN11, DHTTYPE11);

void sendDataToNodeMCU(float CO2, float NO2, float CO, float benzene, float toluene, float pm25, float humidity, float temperature) {
  String dataToSend = String(CO2) + "," +
                     String(NO2) + "," +
                     String(CO) + "," +
                     String(benzene) + "," +
                     String(toluene) + "," +
                     String(pm25) + "," +
                     String(humidity) + "," +
                     String(temperature);
  espSerial.println(dataToSend);
}

void setup() {
  Serial.begin(9600); // Initialize serial communication
  espSerial.begin(9600);

  // initialize the lcd 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Air Quality");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring");
  delay(4000);

  lcd.clear();

  // Set math model to calculate the PPM concentration and the value of constants
  MQ135.setRegressionMethod(1); // _PPM = a * ratio^b
  MQ3.setRegressionMethod(1);   // _PPM = a * ratio^b
  MQ7.setRegressionMethod(1);   // _PPM = a * ratio^b

  // Initialize sensors
  MQ135.init();
  MQ3.init();
  MQ7.init();
  dht.begin();

  //output sharpgp2y1010a0uf
  pinMode(ledPower, OUTPUT);

  // Calibration
  calibrateSensor(MQ135, 3.6, "MQ-135");
  calibrateSensor(MQ3, 60, "MQ-3");
  calibrateSensor(MQ7, 27.5, "MQ-7");
}

void loop() {
  MQ135.update();
  MQ3.update();
  MQ7.update();

  for(int i=0;i<100;i++){
    // PM2.5 measurement
    digitalWrite(ledPower, LOW); //Menghidupkan LED
    delayMicroseconds(280);

    voMeasured = analogRead(measurePin);

    delayMicroseconds(40);
    digitalWrite(ledPower, HIGH); //Mematikan LED
    delayMicroseconds(9680);

    calcVoltage = voMeasured * (5.0 / 1024.0);
    pm25 = (0.17 * calcVoltage - 0.1) * 1000.0;
  }

  if (pm25 < 0) {
    pm25 = 0.00;
  }

  float CO2 = calculateGasConcentration(MQ135, 110.47, -2.862) + 400; // Adding 400 PPM offset for CO2
  float NO2 = calculateGasConcentration(MQ135, 605.18, -3.937);
  float CO = calculateGasConcentration(MQ7, 99.042, -1.518);
  float benzene = calculateGasConcentration(MQ3, 4.8387, -2.68);
  float toluene = calculateGasConcentration(MQ135, 44.947, -3.445); 

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  printSensorReadings(CO2, NO2, CO, benzene, toluene, pm25, humidity, temperature);
}

void calibrateSensor(MQUnifiedsensor& sensor, float cleanAirRatio, const char* sensorName) {
  Serial.print("Calibrating ");
  Serial.print(sensorName);
  Serial.println(", please wait...");
  
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    sensor.update();
    calcR0 += sensor.calibrate(cleanAirRatio);
    Serial.print(".");
  }
  sensor.setR0(calcR0 / 10);
  Serial.println(" done!");

  if (isinf(calcR0)) {
    Serial.println("Warning: Connection issue, R0 is infinite (Open circuit detected). Please check your wiring and supply.");
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Connection issue found, R0 is zero (Analog pin shorts to ground). Please check your wiring and supply.");
    while (1);
  }
}

float calculateGasConcentration(MQUnifiedsensor& sensor, float a, float b) {
  sensor.setA(a);
  sensor.setB(b);
  return sensor.readSensor();
}

void printSensorReadings(float CO2, float NO2, float CO, float benzene, float toluene, float pm25, float humidity, float temperature) {
  Serial.print("CO2 : "); Serial.print(CO2); Serial.println(" ppm");
  Serial.print("NO2 : "); Serial.print(NO2); Serial.println(" ppm");
  Serial.print("CO  : "); Serial.print(CO); Serial.println(" ppm");
  Serial.print("Benzene  : "); Serial.print(benzene); Serial.println(" ppm");
  Serial.print("Toluene : "); Serial.print(toluene); Serial.println(" ppm");
  Serial.print("PM2.5 : "); Serial.print(pm25); Serial.println(" ug/m3");
  Serial.print("Humidity : "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Temperature : "); Serial.print(temperature); Serial.println(" °C");
  Serial.println("<====================================>");
  
  sendDataToNodeMCU(CO2, NO2, CO, benzene, toluene, pm25, humidity, temperature);

  lcd.clear();
  lcd.setCursor(6, 0); lcd.print("CO2"); lcd.setCursor(3, 1); lcd.print(CO2); lcd.print(" ppm"); delay(1500);
  lcd.clear();
  lcd.setCursor(6, 0); lcd.print("NO2"); lcd.setCursor(3, 1); lcd.print(NO2); lcd.print(" ppm"); delay(1500);
  lcd.clear();
  lcd.setCursor(6, 0); lcd.print("CO"); lcd.setCursor(3, 1); lcd.print(CO); lcd.print(" ppm"); delay(1500);
  lcd.clear();
  lcd.setCursor(4, 0); lcd.print("Benzene"); lcd.setCursor(4, 1); lcd.print(benzene); lcd.print(" ppm"); delay(1500);
  lcd.clear();
  lcd.setCursor(4, 0); lcd.print("Toluene"); lcd.setCursor(4, 1); lcd.print(toluene); lcd.print(" ppm"); delay(1500);
  lcd.clear();
  lcd.setCursor(5, 0); lcd.print("PM2.5"); lcd.setCursor(2, 1); lcd.print(pm25); lcd.print(" ug/m3 "); delay(1500); 
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("Humidity"); lcd.setCursor(4, 1); lcd.print(humidity); lcd.print(" %"); delay(1500);
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("Temperature"); lcd.setCursor(3, 1); lcd.print(temperature); lcd.print(" °C"); delay(1500);
}