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
void renderDisplay(String txID, String data, int rssi);

// Display setup
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// Setup
void setup() {
  Serial.begin(115200);
  initDisplay();
  initLoRa(); 
}

void loop() {
  String txID = "";
  int value_1, value_2, value_3;
  int rssi;
  int packetSize = LoRa.parsePacket();
  if(packetSize){
    while (LoRa.available()) {
      txID = LoRa.readStringUntil('|');
      value_1 = (LoRa.readStringUntil('|')).toInt();
      value_2 = (LoRa.readStringUntil('|')).toInt();
      value_3 = (LoRa.readStringUntil('|')).toInt();
    }
    rssi = LoRa.packetRssi();
    // Temporary display fix
    String data = "";
    String m1 = String(value_1);
    String m2 = String(value_2);
    String m3 = String(value_3);
    data = m1 + " " + m2 + " " + m3;
    renderDisplay(txID, data, rssi);
  }
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
  display.setCursor(0, 30);
  if(!LoRa.begin(BAND)){
    Serial.println("LoRa: Failed");
  }else{
    Serial.println("LoRa: Ok");
  }
  delay(1000);
}

// Display Render
void renderDisplay(String txID, String data, int rssi){
    display.clearDisplay();
    display.setTextSize(1);
    display.drawRect(0, 0, 128, 64, 1);
    display.drawRect(0, 0, 128, 13, 1);
    display.setCursor(0, 3);
    display.setTextWrap(0);
    display.setCursor(43, 3);
    display.println("LoRa RX");
    display.setCursor(0, 15);
    display.setTextWrap(0);
    display.setCursor(28, 15);
    display.println("Package Info");
    display.setCursor(3, 28);
    display.print("Sender:");
    display.setCursor(45, 28);
    display.print(txID);
    display.setCursor(3, 40);
    display.print("Data:");
    display.setCursor(33, 40);
    display.print(data);
    display.setCursor(3, 52);
    display.print("RSSI:");
    display.setCursor(33, 52);
    display.print(rssi);
    display.display();
  }