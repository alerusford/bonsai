#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN A2 // порт куда подключен сигнальный выход датчика температуры
#define DHTTYPE DHT11 // DHT 11
#define analogHumidityPin A1 // порт для подключения аналогового датчика влажности почвы YL-69
#define analogHumidityPower 8 // Определяем пин подачи питания на датчик YL-69

// инициализация датчика
DHT dht(DHTPIN, DHTTYPE);
byte degree[8] = // кодируем символ градуса
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
  dht.begin();
}

void loop() {
  delay(10000); // Добавляем паузы в несколько секунд между измерениями
  
  // Чтение влажности и температуры с DHT
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  // Serial.println(h);  
  // Serial.println(t);  
  
  // // Чтение аналогового значения с датчика влажности почвы
  // // uint16_t sensor;
  // int sensor_signal = 0;
  // int sensor_procent = 0;
  // sensor_signal = analogRead(analogHumidityPin); 
  // // Serial.println(sensor);  
  // // Преобразование аналогового значения в процент (предположим, что это соответствует диапазону)
  // // float analogHumidityPercentage = (analogHumidityValue / 100); // Преобразуем в проценты (0-100%)
  // // sensor_procent = map(sensor_signal, 1019, 244, 0, 100); // РАБОЧИЙ ВАРИАНТ
  // sensor_procent = map(sensor_signal, 100, 1023, 100, 0); // ВАРИАНТ 2
  // // Serial.println(sensor_procent);

  // Чтение аналогового значения с датчика влажности почвы / версия 2
  digitalWrite(analogHumidityPower, HIGH); // Включаем питание на пине, к которому подключен плюс датчика
  delay(100); // Время, необходимое датчику, чтобы включиться и замерить показания
  int analogHumiditySignal = analogRead(analogHumidityPin); // Считываем показания датчика YL-69 (от 0 до 1023, где 1023 - сухо)
  digitalWrite(analogHumidityPower, LOW);  // Отключаем питание на пине, к которому подключен плюс датчика
  int very_moist_value = 100; // Показания датчика (analogHumiditySignal) при очень мокрой почве (для калибровки процентного вывода)
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
  lcd.print("\1"); // знак градуса

  lcd.setCursor(0, 1); // Устанавливаем курсор в начало второй строки
  lcd.print("Hum ="); // Выводим текст
  lcd.setCursor(6, 1); // Устанавливаем курсор

  // lcd.print(sensor_procent, 1); // Выводим процент влажности с аналогового датчика
  lcd.print(analogHumiditySignalPercent, 1); // Выводим процент влажности с аналогового датчика

  lcd.print("% ");
  lcd.setCursor(11, 1); // Устанавливаем курсор
  // lcd.print(sensor_signal, 1); // Выводим значение влажности с аналогового датчика
  lcd.print(analogHumiditySignal, 1); // Выводим значение влажности с аналогового датчика

}
