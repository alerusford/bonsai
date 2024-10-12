#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN A2 // Пин подключения сигнального выхода датчика температуры
#define DHTTYPE DHT11 // DHT-11
#define analogHumidityPin A1 // Порт для подключения аналогового датчика влажности почвы
#define analogHumidityPower 8 // Определяем пин подачи питания на датчик YL-69

// Инициализация датчика DHT-11
DHT dht(DHTPIN, DHTTYPE);
byte degree[8] = // Кодируем символ градуса
{
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
};

LiquidCrystal_I2C lcd(0x27, 16, 2); // Задаем адрес и размерность дисплея

void setup()
{
  lcd.begin(); // Инициализация lcd
  lcd.backlight(); // Включаем подсветку
  lcd.createChar(1, degree); // Создаем символ под номером 1
  pinMode(analogHumidityPower, OUTPUT);  // Определяем пин питания датчика влажности почвы, как выход
  Serial.begin(9600); // Инициализируем серийный порт как устройство вывода
  dht.begin(); // Инициализация DHT-11
}

void loop() {
  delay(30000); // Добавляем паузы между измерениями

  // Чтение влажности и температуры с DHT-11
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  // Serial.println(h);
  // Serial.println(t);

  // Чтение аналогового значения с датчика влажности почвы / версия 2
  int very_moist_value = 46; // Показания датчика (analogHumiditySignal) при очень мокрой почве (для калибровки процентного вывода)
  digitalWrite(analogHumidityPower, HIGH); // Включаем питание на пине, к которому подключен плюс датчика
  delay(300); // Время, необходимое датчику, чтобы включиться и замерить показания
  int analogHumiditySignal = analogRead(analogHumidityPin); // Считываем показания датчика YL-69 (от 0 до 1023, где 1023 - сухо)
  digitalWrite(analogHumidityPower, LOW);  // Отключаем питание на пине, к которому подключен плюс датчика
  int analogHumiditySignalPercent = map(analogHumiditySignal, very_moist_value, 1023, 100, 0);  // Переводим показания датчика YL-69 в проценты
  Serial.print(String(analogHumiditySignal) + " = "); // Выводим показания датчика в строковом формате на устройство вывода
  Serial.println(String(analogHumiditySignalPercent) + "%"); // Выводим показания о влажности в процентах в строковом формате на устройство вывода

  // Выводим показания влажности и температуры
  lcd.setCursor(0, 0); // Устанавливаем курсор в начало 1 строки
  lcd.print("Temp ="); // Выводим текст
  lcd.setCursor(7, 0); // Устанавливаем курсор
  lcd.print(h, 1); // Выводим на экран значение влажности DHT
  lcd.print("%");

  lcd.setCursor(11, 0); // Устанавливаем курсор
  lcd.print(t,1); // Выводим значение температуры
  lcd.print("\1"); // Знак градуса

  lcd.setCursor(0, 1); // Устанавливаем курсор в начало второй строки
  lcd.print("Hum ="); // Выводим текст
  lcd.setCursor(6, 1); // Устанавливаем курсор
  lcd.print(analogHumiditySignalPercent, 1); // Выводим процент влажности с аналогового датчика
  lcd.print("% ");

  lcd.setCursor(11, 1); // Устанавливаем курсор
  lcd.print(analogHumiditySignal, 1); // Выводим значение влажности с аналогового датчика
}
