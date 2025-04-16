#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Настройка радиомодуля NRF24L01
RF24 radio(9, 10); // CE, CSN пины
const byte address[6] = "00001"; // Адрес для связи

// Настройка дисплея
#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     7
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Пины стиков и кнопок
const int STICK_X = A0; // Горизонтальное движение стика
const int STICK_Y = A1; // Вертикальное движение стика
const int BUTTON1 = 2;  // Кнопка 1
const int BUTTON2 = 3;  // Кнопка 2
const int BUTTON3 = 4;  // Кнопка 3
const int BUTTON4 = 5;  // Кнопка 4

// Структура данных для приёма с лодки
struct DataPacket {
  float temperature;      // Температура
  float pressure;         // Давление
  float angles[3];        // Углы X, Y, Z с гироскопа
};
DataPacket receivedData;

// Структура данных для отправки на лодку
struct ControllerPacket {
  int stickX;             // Стик X
  int stickY;             // Стик Y
  bool buttons[4];        // Состояние кнопок
} sentData;

unsigned long previousMillis = 0; // Переменная для управления временем
const unsigned long interval = 50; // Интервал отправки данных (50 мс)

void setup() {
  Serial.begin(9600);

  // Настройка кнопок и стиков
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  // Настройка дисплея
  tft.init(240, 240); // Инициализация дисплея ST7789
  tft.fillScreen(ST77XX_BLACK); // Очистка экрана
  tft.setTextColor(ST77XX_WHITE); // Установка цвета текста
  tft.setTextSize(2); // Установка размера текста
  tft.setCursor(0, 0);
  tft.println("Пульт включен!");

  // Инициализация радиомодуля
  if (!radio.begin()) {
    Serial.println("Ошибка инициализации NRF24L01");
    while (1); // Остановка программы при ошибке
  }

  radio.openReadingPipe(1, address); // Установка адреса для приёма
  radio.setPALevel(RF24_PA_HIGH); // Уровень мощности сигнала
  radio.startListening(); // Включение режима приёма
}

void loop() {
  unsigned long currentMillis = millis();

  // Приём данных с лодки
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Температура: "); Serial.println(receivedData.temperature);
    Serial.print("Давление: "); Serial.println(receivedData.pressure);
    Serial.print("Углы: X="); Serial.print(receivedData.angles[0]);
    Serial.print(", Y="); Serial.print(receivedData.angles[1]);
    Serial.print(", Z="); Serial.println(receivedData.angles[2]);

    // Отображение данных на дисплее
    tft.fillScreen(ST77XX_BLACK); // Очистка экрана
    tft.setCursor(0, 0);
    tft.print("Темп: "); tft.println(receivedData.temperature);
    tft.print("Давл: "); tft.println(receivedData.pressure);
    tft.print("Углы X: "); tft.println(receivedData.angles[0]);
    tft.print("Углы Y: "); tft.println(receivedData.angles[1]);
    tft.print("Углы Z: "); tft.println(receivedData.angles[2]);
  }

  // Считывание данных управления (каждые 50 мс)
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sentData.stickX = analogRead(STICK_X);
    sentData.stickY = analogRead(STICK_Y);
    sentData.buttons[0] = !digitalRead(BUTTON1);
    sentData.buttons[1] = !digitalRead(BUTTON2);
    sentData.buttons[2] = !digitalRead(BUTTON