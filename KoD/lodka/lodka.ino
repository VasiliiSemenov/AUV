#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h> // Для GY-68
#include <Adafruit_MS5803.h>   // Для GY-MS5803
#include <MPU6050.h>           // Для GY-521

// Настройка радиомодуля NRF24L01
RF24 radio(9, 10); // CE, CSN пины
const byte address[6] = "00001"; // Адрес для связи

// Объекты для датчиков
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); // GY-68
Adafruit_MS5803 pressureSensor = Adafruit_MS5803();
MPU6050 mpu;

// Структура данных для приёма с пульта
struct ControllerPacket {
  int stickX;             // Стик X
  int stickY;             // Стик Y
  bool buttons[4];        // Состояние кнопок
} receivedControlData;

// Пины для драйверов линейных приводов
const int DRIVER1_STEP = 18;
const int DRIVER1_DIR = 19;
const int DRIVER2_STEP = 16;
const int DRIVER2_DIR = 17;
const int DRIVER3_STEP = 14;
const int DRIVER3_DIR = 15;

// Пины концевых выключателей
const int LIMIT1 = 7;
const int LIMIT2 = 8;
const int LIMIT3 = 9;

// Переменные для отслеживания положения линейных приводов
int positionDriver1 = 0; // Положение линейного привода 1
int positionDriver2 = 0; // Положение линейного привода 2
int positionDriver3 = 0; // Положение линейного привода 3
const int maxPosition = 100; // Максимальное положение линейного привода

// Флаг для режима парковки
bool parkingMode = false;

void setup() {
  Wire.begin(); // Инициализация I2C
  Serial.begin(9600); // Инициализация сериал монитора для отладки

  // Инициализация радиомодуля
  if (!radio.begin()) {
    Serial.println("Ошибка инициализации NRF24L01");
    while (1);
  }
  radio.openReadingPipe(1, address); // Установка адреса для связи
  radio.setPALevel(RF24_PA_HIGH);    // Уровень мощности сигнала
  radio.startListening();            // Режим приёма

  // Настройка пинов для драйверов и концевиков
  pinMode(DRIVER1_STEP, OUTPUT);
  pinMode(DRIVER1_DIR, OUTPUT);
  pinMode(DRIVER2_STEP, OUTPUT);
  pinMode(DRIVER2_DIR, OUTPUT);
  pinMode(DRIVER3_STEP, OUTPUT);
  pinMode(DRIVER3_DIR, OUTPUT);
  pinMode(LIMIT1, INPUT_PULLUP);
  pinMode(LIMIT2, INPUT_PULLUP);
  pinMode(LIMIT3, INPUT_PULLUP);

  Serial.println("Система лодки готова к работе");
}

void loop() {
  // Проверка данных от пульта
  if (radio.available()) {
    radio.read(&receivedControlData, sizeof(receivedControlData));

    // Проверка кнопки для активации парковки
    if (receivedControlData.buttons[0]) { // Предположим, кнопка 1 запускает парковку
      Serial.println("Режим парковки активирован");
      parkingMode = true;
      performParking(); // Запускаем парковку
    } else {
      parkingMode = false;
    }
  }

  // Если парковка не активирована, выполнять другие задачи
  if (!parkingMode) {
    // Ваши остальные задачи (например, управление двигателями и чтение датчиков)
  }
}

void performParking() {
  // Парковка линейного привода 1
  while (digitalRead(LIMIT1) != LOW) {
    digitalWrite(DRIVER1_DIR, LOW); // Двигаться к концевику
    digitalWrite(DRIVER1_STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(DRIVER1_STEP, LOW);
    delayMicroseconds(500);
    positionDriver1--; // Уменьшение положения
  }
  positionDriver1 = 0; // Установка позиции в 0
  Serial.println("Привод 1 на месте");

  // Автоматическое движение к максимуму
  moveToMaximum(DRIVER1_STEP, DRIVER1_DIR, positionDriver1);

  // Парковка линейного привода 2
  while (digitalRead(LIMIT2) != LOW) {
    digitalWrite(DRIVER2_DIR, LOW); // Двигаться к концевику
    digitalWrite(DRIVER2_STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(DRIVER2_STEP, LOW);
    delayMicroseconds(500);
    positionDriver2--; // Уменьшение положения
  }
  positionDriver2 = 0; // Установка позиции в 0
  Serial.println("Привод 2 на месте");

  // Автоматическое движение к максимуму
  moveToMaximum(DRIVER2_STEP, DRIVER2_DIR, positionDriver2);

  // Парковка линейного привода 3
  while (digitalRead(LIMIT3) != LOW) {
    digitalWrite(DRIVER3_DIR, LOW); // Двигаться к концевику
    digitalWrite(DRIVER3_STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(DRIVER3_STEP, LOW);
    delayMicroseconds(500);
    positionDriver3--; // Уменьшение положения
  }
  positionDriver3 = 0; // Установка позиции в 0
  Serial.println("Привод 3 на месте");

  // Автоматическое движение к максимуму
  moveToMaximum(DRIVER3_STEP, DRIVER3_DIR, positionDriver3);

  // Остановка парковочного режима
  parkingMode = false;
  Serial.println("Парковка завершена");
}

void moveToMaximum(int stepPin, int dirPin, int &position) {
  digitalWrite(dirPin, HIGH); // Двигаться к максимальному положению
  while (position < maxPosition) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    position++; // Увеличение положения
  }
  Serial.println("Привод достиг максимума");
}
