#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h> // Подключение библиотеки
#include <LiquidCrystal_PCF8574.h> // Подключение альтернативной библиотеки

#define redLed 2
#define grenLed 3

String boxUID[5];
String boxName[5];

// 23119012096
// 128190195128
// 351663585128
// 1621973228
// LiquidCrystal_I2C lcd(0x27,16,2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой
// LiquidCrystal_PCF8574 lcd(0x27); // Вариант для библиотеки PCF8574
LiquidCrystal_PCF8574 lcd(0x3f);

// Константы подключения контактов RST и SS
#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
void ShowReaderDetails();
void readUID(MFRC522::Uid *uid);

void setup()
{

  boxUID[0] = "23119012096";
  boxUID[1] = "128190195128";
  boxUID[2] = "351663585128";
  boxUID[3] = "1621973228";
  boxUID[4] = "";

  boxName[0] = "box1";
  boxName[1] = "box2";
  boxName[2] = "micron";
  boxName[3] = "box36746";
  boxName[4] = "";

  pinMode(redLed, OUTPUT);
  pinMode(grenLed, OUTPUT);

  Serial.begin(9600); // Инициализация монитора последовательного порта
  while (!Serial)
    ;                  // Ожидание включения монитора последовательного порта
  SPI.begin();         // Инициализация SPI шины
  mfrc522.PCD_Init();  // Инициализация RC522
  ShowReaderDetails(); // Вывод данных о модуле RC522
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));

  lcd.begin(16, 2);
  lcd.setBacklight(255);
}

void loop()
{
  // Поиск новой метки
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Выбор метки
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Вывод данных с карты
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  //  char* rUid  = (char*) mfrc522.uid.uidByte;
  // int* irUID = byteToInt((byte*)&(mfrc522.uid.uidByte));

  readUID(&(mfrc522.uid));

  // Serial.println("UID: \t"+String(&sriUID)+"\t"+String(mfrc522.uid.size));
}

void readUID(MFRC522::Uid *uid)
{
  String lsdStr = "";
  lcd.clear();
  lcd.print("Name x:");
  for (byte i = 0; i < uid->size; i++)
  {
    if (uid->uidByte[i] < 0x10)
    {
      // Serial.print(F(" 0"));
      // lsdStr+="0";
    }
    else
      // Serial.print(F(" "));

      // Serial.print(uid->uidByte[i]);
      lsdStr += uid->uidByte[i];
  }

  Serial.print("UID:" + lsdStr);

  lcd.setCursor(0, 1);
  // lcd.print(lsdStr);

  for (size_t i = 0; i < 5; i++)
  {
    if (boxUID[i] == lsdStr)
    {
      // Serial.print("\t is " + boxName[i]);
      Serial.print("<modules><UID>" + boxUID[i] + "</UID><name>" + boxName[i] + "</name></modules>");

      lcd.print("is " + boxName[i]);
      if (i == 2)
      {
        digitalWrite(grenLed, 1);
        digitalWrite(redLed, 0);
      }
      else
      {
        digitalWrite(grenLed, 0);
        digitalWrite(redLed, 1);
      }
    }
    else {
      Serial.print("<modules><UID>" + boxUID[i] + "</UID></modules>");

    }
  }

  Serial.println();
}

void ShowReaderDetails()
{
  // Получение номера программной версии модуля RC522
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // Когда получено 0x00 или 0xFF, передача данных нарушена
  if ((v == 0x00) || (v == 0xFF))
  {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}