#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
Servo goservo;


int PinOut[4] {5, 4, 3, 2}; // пины выходы

int PinIn[4] {9, 8, 7, 6}; // пины входа

LiquidCrystal_I2C lcd(0x27, 16, 2);


const char value[4][4]
{ {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'#', '0', '*', 'D'}
};
// двойной массив, обозначающий кнопку


char b = 0; // переменная, куда кладется число из массива(номер кнопки)


int led = 13; // пин, к которому подключен светодиод

int brightness = 0; // яркость светодиода

int fadeAmount = 5; // на сколько увеличить яркость светодиода
int pos = 0;
int grad = 0; /// задаем угол
int end_pos = 120; //конечный угол


void setup()
{
  pinMode (2, OUTPUT); // инициализируем порты на выход (подают нули на столбцы)
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);

  pinMode (6, INPUT); // инициализируем порты на вход с подтяжкой к плюсу (принимают нули на строках)
  digitalWrite(6, HIGH);
  pinMode (7, INPUT);
  digitalWrite(7, HIGH);
  pinMode (8, INPUT);
  digitalWrite(8, HIGH);
  pinMode (9, INPUT);
  digitalWrite(9, HIGH);
  goservo.attach(13);

  // initialize the LCD
  lcd.begin();
  lcd.cursor();


  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello");
  lcd.setCursor(0, 1);
  Serial.begin(9600); // открываем Serial порт
}

void matrix () // создаем функцию для чтения кнопок
{
  for (int i = 1; i <= 4; i++) // цикл, передающий 0 по всем столбцам
  {
    digitalWrite(PinOut[i - 1], LOW); // если i меньше 4 , то отправляем 0 на ножку
    for (int j = 1; j <= 4; j++) // цикл, принимающих 0 по строкам
    {
      if (digitalRead(PinIn[j - 1]) == LOW) // если один из указанных портов входа равен 0, то..
      {
        b = value[i - 1][j - 1];
        if (b == '*') {
          pos = 0;
          grad = 0;
          lcd.clear();
          lcd.print("Stop");
        }
        else if (b == '#') {
          pos = 0;
          grad = 10;
          lcd.clear();
          lcd.print("Default");
        }
        else {
          grad = (int)b - 48;
          lcd.clear();
          lcd.print(b);
        }
        Serial.println(grad); // то b равно значению из двойного массива
      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}

void rotate() {
 matrix();
  goservo.write(pos);
  delay(40); // ждём 15ms пока серва займёт новое положение
  pos = pos + grad;
  // меняет значение затухания на аналогичное с противоположным знаком при граничных значениях:
 
  if (pos <= 0) {
    grad = -grad ;
    pos=0;
  }
  if(pos >= end_pos) {
    grad = -grad ;
    pos = end_pos;
  }
   Serial.println(pos);
  

}
//
//void Led() // создаем показательную функцию мигания светодиодом, чтобы потом ее прервать прерыванием от клавиатуры
//{
//
//  analogWrite(led, brightness);
//  // изменение яркости на следующей итерации с помощью цикла:
//  brightness = brightness + fadeAmount;
//  // меняет значение затухания на аналогичное с противоположным знаком при граничных значениях:
//
//  if (brightness == 0 || brightness == 255) {
//    fadeAmount = -fadeAmount ;
//
//  }
//
//  matrix();
//  delay(40);
//}
void loop()
{
  rotate(); // включаем функцию, для проверки нашего прерывания
}
