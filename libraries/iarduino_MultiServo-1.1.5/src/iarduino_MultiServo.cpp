#include "iarduino_MultiServo.h"
																											//
/**		ОСНОВНЫЕ ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ: **/																//
																											//
//		ИНИЦИАЛИЗАЦИЯ ЧИПА:																					//
void	iarduino_MultiServo::PCA_begin(uint8_t i, uint16_t j){												//	i - адрес чипа, j - частота ШИМ в Гц.
		//	Сохраняем полученные адрес чипа и частоту ШИМ:													//	
			PCA_uint_ID=i; PCA_uint_PWM=j;																	//	сохраняем адрес чипа и частоту ШИМ
		//	Инициируем работу с шиной I2C:																	//
			selI2C->begin();																				//	Инициируем работу с выбранной шиной I2C в качестве мастера.
		//	Инициируем работу с модулем:																	//
			PCA_func_SET_MODE(); digitalWrite(SERVO_ALL,LOW);												//	Устанавливаем регистры режимов работы, устанавливаем все выходы в логические «0».
}																											//
//		УСТАНОВКА ШИМ НА ВЫХОДЕ:																			//
void	iarduino_MultiServo::analogWrite(uint8_t i, uint16_t j, uint16_t k){								//	i - № выхода от 0 до 15 или SERVO_ALL, j - коэффициент заполнения ШИМ от 0 до 4095, k - фазовый сдвиг от 0 до 4095.
		//	Проверяем корректность параметров:																//
			if(i>=16&&i!=SERVO_ALL){return;}																//	Игнорируем команду.
		//	Устанавливаем логические уровни для ШИМ = 0 или 100%:											//
			if(j==0)  {digitalWrite(i,LOW ); return;}														//	Устанавливаем логический 0.
			if(j>4095){digitalWrite(i,HIGH); return;}														//	Устанавливаем логическую 1.
		//	Выполняем фазовую коррекцию:																	//
			if(k>4095){k=4095;}	j+=k; if(j>4095){j-=4096;}													//	Отодвигаем спад импульса в зависимости от фазового сдвига.
		//	Записываем данные в регистры модуля:															//
			uint8_t data[4]={k,k>>8,j,j>>8};																//	Определяем массив data из 4 байт данных для записи.
			selI2C->writeBytes(PCA_uint_ID, (i==SERVO_ALL?0xFA:(i*4+6)), data, 4);							//	Записываем 4 байта массива data в модуль PCA_uint_ID начиная с регистра (i==SERVO_ALL?0xFA:(i*4+6)).
}																											//
//		УСТАНОВКА ЛОГИЧЕСКОГО УРОВНЯ НА ВЫХОДЕ:																//
void	iarduino_MultiServo::digitalWrite(uint8_t i, bool j){												//	i - № выхода от 0 до 15 или SERVO_ALL, j - LOW/HIGH.
		//	Проверяем корректность параметров:																//
			if(i>=16&&i!=SERVO_ALL){return;}																//	Игнорируем команду.
		//	Записываем данные в регистры модуля:															//
			uint8_t data[4]={0,j?16:0,0,j?0:16};															//	Определяем массив data из 4 байт данных для записи.
			selI2C->writeBytes(PCA_uint_ID, (i==SERVO_ALL?0xFA:(i*4+6)), data, 4);							//	Записываем 4 байта массива data в модуль PCA_uint_ID начиная с регистра (i==SERVO_ALL?0xFA:(i*4+6)).
}																											//
//		ЧТЕНИЕ УСТАНОВЛЕННОГО КОЭФФИЦИЕНТА ЗАПОЛНЕНИЯ НА ВЫХОДЕ:											//
uint16_t iarduino_MultiServo::analogRead(uint8_t i){														//	i - № выхода от 0 до 15.
		//	Проверяем корректность параметров:																//
			if(i>=16){return 0;} uint16_t j=0, k=0;															//	Игнорируем команду.
		//	Читаем данные из регистров модуля:																//
			uint8_t data[4];																				//	Объявляем массив data из 4 элементов.
			selI2C->readBytes(PCA_uint_ID, (i*4+6), data, 4);												//	Заполняем массив data читая 4 байта из модуля PCA_uint_ID начиная с регистра (i*4+6).
		//	Возвращаем результат:																			//
			if(data[3]&0x10){return 0;}																		//	На читаемом выходе установлен 0.
			if(data[1]&0x10){return 4096;}																	//	На читаемом выходе установлена 1.
			k=(data[1]<<8)+data[0];																			//	Количество тактов до фронта импульса.
			j=(data[3]<<8)+data[2];																			//	Количество тактов до спада импульса.
			if(j<k){j+=4096;} j-=k; return j;																//	Выводим коэффициент заполнения с учётом фазового сдвига.
}																											//
//		УСТАНОВКА УГЛА ПОВОРОТА СЕРВОПРИВОДА:																//
void	iarduino_MultiServo::servoWrite(uint8_t i, uint16_t j){												//	i - № выхода от 0 до 15 или SERVO_ALL, j - угол поворота от 0° до 360°.
		//	Создаём диапазон от k до i для SERVO_ALL:														//
			uint8_t k=i; if(i==SERVO_ALL){i=15; k=0;}														//
		//	Проверяем корректность параметров:																//
			if(i>=16||j>360){return;} 																		//	Игнорируем команду.
		//	Записываем данные в регистры модуля:															//
			for(int n=k; n<=i; n++){analogWrite(n,map((j<=PCA_uint_ANGLE[n]?j:PCA_uint_ANGLE[n]),0,PCA_uint_ANGLE[n],PCA_uint_ANGLE_MIN[n],PCA_uint_ANGLE_MAX[n]));}
}																											//
//		УСТАНОВКА ПАРАМЕТРОВ ДЛЯ СЕРВОПРИВОДА:																//
void	iarduino_MultiServo::servoSet(uint8_t i, uint16_t j, uint16_t a, uint16_t b){						//	i - № выхода от 0 до 15 или SERVO_ALL, j - максимальный угол поворота от 1° до 360°, a - коэффициент заполнения от 0 до 4095 для угла 0°, b - коэффициент заполнения от 0 до 4095 для максимального угла поворота.
		//	Создаём диапазон от k до i для SERVO_ALL:														//
			uint8_t k=i; if(i==SERVO_ALL){i=15; k=0;}														//
		//	Проверяем корректность параметров:																//
			if(i>=16||j==0||j>360||a>4095||b>4095||b==0){return;}											//	Игнорируем команду.
		//	Применяем параметры:																			//
			for(int n=k; n<=i; n++){PCA_uint_ANGLE[n]=j; PCA_uint_ANGLE_MIN[n]=a; PCA_uint_ANGLE_MAX[n]=b;}	//	применяем значения максимального угла поворота и коэффициентов заполнения для крайних положений сервопривода
}																											//
//		УСТАНОВКА ПАРАМЕТРОВ ДЛЯ СЕРВОПРИВОДА:																//
void	iarduino_MultiServo::servoSet(uint8_t i, uint8_t j){												//	i - № выхода от 0 до 15 или SERVO_ALL, j - название сервопривода: SERVO_SG90/SERVO_MG90/SERVO_MG996R/SERVO_FutabaS3003.
		//	Применяем параметры сервопривода согласно его названию:											//
			switch(j){																						//
				case SERVO_SG90:		servoSet(i, 180, 127, 540); break;									//	servoSet( вывод i, максимальный угол поворота 180°, коэффициент заполнения для 0° = 127/4095=3.1% , коэффициент заполнения для максимального угла = 540/4095=13.2%.
				case SERVO_MG90:		servoSet(i, 180, 150, 555); break;									//	servoSet( вывод i, максимальный угол поворота 180°, коэффициент заполнения для 0° = 150/4095=3.7% , коэффициент заполнения для максимального угла = 555/4095=13.6%.
				case SERVO_MG996R:		servoSet(i, 180, 130, 515); break;									//	servoSet( вывод i, максимальный угол поворота 180°, коэффициент заполнения для 0° = 130/4095=3.2% , коэффициент заполнения для максимального угла = 515/4095=12.6%.
				case SERVO_FutabaS3003:	servoSet(i, 180, 140, 490); break;									//	servoSet( вывод i, максимальный угол поворота 180°, коэффициент заполнения для 0° = 140/4095=3.4% , коэффициент заполнения для максимального угла = 490/4095=12.0%.
			}																								//
}																											//
																											//
/**		ДОПОЛНИТЕЛЬНЫЕ ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ: **/														//
																											//
//		инверсия сигналов на всех выходах																	//
void	iarduino_MultiServo::invert(bool i){PCA_flag_INVRT=i; PCA_func_SET_MODE();}							//	установка или сброс флага INVRT
//		выбор интегрированной схемы подключения выходов в чипе												//
void	iarduino_MultiServo::outdrv(bool i){PCA_flag_OUTDRV=i; PCA_func_SET_MODE();}						//	установка или сброс флага OUTDRV
//		состояние на выходах при подаче на вход OE уровня логической «1»									//
void	iarduino_MultiServo::outState(uint8_t i){if(i>2){return;} PCA_uint_OUTNE=i; PCA_func_SET_MODE();}	//	установка двух битов OUTNE
//		установка частоты внешнего источника тактирования													//
void	iarduino_MultiServo::extClock(uint16_t i){if(i){PCA_flag_EXTCLK=true; PCA_uint_OSC=i; PCA_uint_OSC*=1000; PCA_func_SET_MODE();}else{PCA_flag_EXTCLK=false; restart(); PCA_uint_OSC=25000000; PCA_func_SET_MODE(); digitalWrite(SERVO_ALL,LOW);}}
//		перезагрузка всех чипов на шине																		//
void	iarduino_MultiServo::restart(void){uint8_t i; selI2C->writeBytes(0x0, 0x6, &i, 0);}					//	отправка команды «SWRST Call». Отправляем в модуль с адресом 0, номер регистра 6, не передавая данные в этот регистр.
//		чтение/запись одного байта данных из/в регистр														//
uint8_t	iarduino_MultiServo::reg(uint8_t i           ){return selI2C->readByte (PCA_uint_ID, i   );}		//
uint8_t	iarduino_MultiServo::reg(uint8_t i, uint8_t j){return selI2C->writeByte(PCA_uint_ID, i, j);}		//
																											//
/**		ВНУТРЕННИЕ ФУНКЦИИ: **/																				//
																											//
//		установка регистров режимов работы MODE1, MODE2 и PRE_SCALE:										//
void	iarduino_MultiServo::PCA_func_SET_MODE(){															//
		uint16_t j=(PCA_uint_OSC/(4096*uint32_t(PCA_uint_PWM)))-1; if(j<3){j=3;} if(j>255){j=255;}			//	определяем значение предделителя
		reg(0,0x30);																						//	отправляем байт данных в регистр MODE1     (устанавливаем флаги AL и SLEEP, остальные флаги сброшены)
		reg(0xFE,j);																						//	отправляем байт данных в регистр PRE_SCALE (устанавливаем предделитель для частоты ШИМ)
		reg(1,PCA_flag_INVRT<<4|PCA_flag_OUTDRV<<2|PCA_uint_OUTNE);											//	отправляем байт данных в регистр MODE2     (записываем флаги INVRT, OUTDRV, OUTNE, флаг OCH сброшен)
		reg(0,PCA_flag_EXTCLK<<6|0x30);																		//	отправляем байт данных в регистр MODE1     (записываем флаг EXTCLK, флаги AL и SLEEP установлены, остальные флаги сброшены)
		reg(0,0x20);																						//	отправляем байт данных в регистр MODE1     (сбрасываем флаг SLEEP, остальные флаги без изменений) / флаг EXTCLK не сбрасывается записью нуля
		delayMicroseconds(500);																				//	ждём выполнение действий по сбросу флага SLEEP
		reg(0,0xA0);																						//	отправляем байт данных в регистр MODE1     (сбрасываем флаг RESTART, остальные флаги без изменений) / флаг RESTART сбрасывается записью единицы
}