#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "LoRa.h"

#define TTGO_LORA32 1
#define ESP32_RA02 0

#if TTGO_LORA32
// Pines para una placa TTGO LoRa32 (433MHz)
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26
#elif ESP32_RA02
// Pines para una placa ESP32 + moóulo Ra-02 (433MHz)
#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5
#define RST 14
#define DIO0 2
#endif

// Frecuencia de la placa
#define BAND 433E6

// Pines del display OLED 128x64
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Declaración de funciones
void initDisplay();
void initLoRa();
void renderDisplay(String txID, int v1, int v2, int v3);

// Display setup
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// Identificador del transmisor
const String txID = "TX01";

// Variables para almacenar los valores generados pseudoaleatoriamente
int v1, v2, v3;

// Setup
void setup() {
  Serial.begin(115200);
  initDisplay();
  initLoRa();
}

void loop() {
  esp_random();
  v1 = random(1, 101);
  v2 = random(1, 101);
  v3 = random(1, 101);

  LoRa.beginPacket();
  LoRa.print(txID);
  LoRa.print(",");
  LoRa.print(v1);
  LoRa.print(",");
  LoRa.print(v2);
  LoRa.print(",");
  LoRa.print(v3);
  LoRa.print(",");
  LoRa.endPacket();

  renderDisplay(txID, v1, v2, v3);

  delay(5000);
}

// Inicialización del display
void initDisplay(){
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  Serial.println("Display: Ok");
  delay(1000);
}

// Inicialización del módulo LoRa
void initLoRa(){
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if(!LoRa.begin(BAND)){
    Serial.println("LoRa: Failed");
  }else{
    Serial.println("LoRa: Ok");
  }
  delay(1000);
}

// Display render
void renderDisplay(String txID, int v1, int v2, int v3){
    display.clearDisplay();
    display.drawRect(0, 0, 128, 64, 1);
    display.setCursor(0, 2);
    display.setTextWrap(0);
    display.setCursor(43, 2);
    display.println("LoRa TX");
    display.drawLine(0, 10, 128, 10, 1);
    display.setCursor(3, 12);
    display.print("LoRa:Ok");
    display.drawLine(0, 20, 128, 20, 1);
    display.drawLine(46, 20, 46, 10, 1);
    display.setCursor(50, 12);
    display.print(txID);
    display.drawLine(0, 30, 128, 30, 1);
    display.setCursor(0, 22);
    display.setTextWrap(0);
    display.setCursor(37, 22);
    display.println("Data sent");
    display.setCursor(3, 33);
    display.print("v1=");
    display.setCursor(21, 33);
    display.print(v1);
    display.setCursor(45, 33);
    display.print("v2=");
    display.setCursor(63, 33);
    display.println(v2);
    display.setCursor(86, 33);
    display.print("v3=");
    display.setCursor(104, 33);
    display.println(v3);
    display.display();
  }