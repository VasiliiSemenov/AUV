const int motorPinPWM = 9; // Единственный пин для управления драйвером двигателя

void setup() {
  pinMode(motorPinPWM, OUTPUT); // Установка пина в режим вывода
  Serial.begin(9600); // Инициализация последовательного соединения
  Serial.println("Калибровка диапазона...");
  
  // Калибровка диапазона
  Serial.println("Подача минимального значения...");
  analogWrite(motorPinPWM, 0); // Минимум
  delay(2000);

  Serial.println("Подача максимального значения...");
  analogWrite(motorPinPWM, 255); // Максимум
  delay(2000);

  Serial.println("Возврат к нейтральному значению...");
  analogWrite(motorPinPWM, 127); // Нейтральное значение
  delay(1000);

  Serial.println("Калибровка завершена.");
  Serial.println("Введите скорость двигателя в процентах (0-100):");
}

void loop() {
  if (Serial.available() > 0) {
    int percentage = Serial.parseInt(); // Чтение значения от пользователя
    if (percentage >= 0 && percentage <= 100) {
      int pwmValue = map(percentage, 0, 100, 0, 255); // Преобразование процентов в значение ШИМ
      analogWrite(motorPinPWM, pwmValue);            // Управление драйвером
      Serial.print("Скорость двигателя установлена на ");
      Serial.print(percentage);
      Serial.println("%.");
    } else {
      Serial.println("Ошибка: введите значение от 0 до 100.");
    }
  }
}
