#include <Wire.h> // Добавляем необходимые библиотеки
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN A0 // к какому пину будет подключен сигнальный выход датчика
#define DHTTYPE DHT11 // DHT 11
#define analogHumidityPin A1 // порт для подключения аналогового датчика влажности
#define  MIN        1019                    // Определяем минимальное показание датчика (в воздухе),
#define  MAX        4400 

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
  lcd.init(); // Инициализация lcd
  lcd.backlight(); // Включаем подсветку
  lcd.createChar(1, degree); // Создаем символ под номером 1
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Добавляем паузы в несколько секунд между измерениями
  delay(5000);
  
  // Чтение влажности и температуры с DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Чтение аналогового значения с датчика влажности
  // int percentage=0;
  // uint16_t sensor;
  float sensor_signal = 0;
  float sensor_procent = 0;

  //  sensor;
  // float sensor_read;
  // int analogHumidityValue = analogRead(analogHumidityPin);
  sensor_signal = analogRead(analogHumidityPin); 
  // Serial.println(sensor);  
  

  

  // percentage = map(analogHumidityValue, 4400, 1020, 100, 0);
  
  // Преобразование аналогового значения в процент (предположим, что это соответствует диапазону)
  // float analogHumidityPercentage = (analogHumidityValue / 100); // Преобразуем в проценты (0-100%)

  // int percentage = mapPercentage(analogHumidityValue);
  sensor_procent = map(sensor_signal, 1019, 244, 0, 100);
  Serial.println(sensor_signal);

  // Выводим показания влажности и температуры
  lcd.setCursor(0, 0); // Устанавливаем курсор в начало 1 строки
  lcd.print("Hum ="); // Выводим текст
  lcd.setCursor(6, 0); // Устанавливаем курсор на 6 символ
  lcd.print(h, 1); // Выводим на экран значение влажности DHT
  lcd.print("%");

  // lcd.setCursor(11, 0); // Устанавливаем курсор на 12 символ
  // lcd.print(analogHumidityPercentage, 1); // Выводим значение влажности с аналогового датчика
  lcd.print(sensor_procent, 1); // Выводим значение влажности с аналогового датчика
  // lcd.setCursor(15, 0);
  // Serial.println(analogHumidityValue);
  // Serial.println(percentage);

  // lcd.setCursor(0, 1); // Устанавливаем курсор в начало 2 строки
  // lcd.print("Analog Hum = "); // Выводим текст
  

  lcd.setCursor(0, 1); // Устанавливаем курсор в начало третьей строки (если есть)
  // lcd.print("Temp = \1C "); // Выводим текст, 1 - значок градуса
  lcd.print("Temp="); // Выводим текст, 1 - значок градуса

  lcd.setCursor(6, 1); // Устанавливаем курсор на 7 символ
  lcd.print(t,1); // Выводим значение температуры
  lcd.print("\1"); 
  lcd.setCursor(11, 1); // Устанавливаем курсор на 7 символ

  lcd.print(sensor_signal, 1); // Выводим значение влажности с аналогового датчика


}
