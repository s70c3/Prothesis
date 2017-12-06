#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <MsTimer2.h>//Библиотека прерываний по таймеру2
#include <EEPROM.h>

Servo goservo;


// Initialize the library with the numbers of the interface pins
int PinOut[4] {5, 4, 3, 2}; // пины выходы

int PinIn[4] {9, 8, 7, 6}; // пины входа

LiquidCrystal_I2C lcd(0x27, 16, 2);


const char value[4][4]
{ {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'#', '0', '*', 'D'}
};

const int presets[4][3] {{10, 10, 170}, {20, 10, 180}, {10, 10, 90}, {10, 20, 160}};
// двойной массив, обозначающий кнопку

//States for the menu.
int currentMenuItem = 0;
int lastState = 0;
int state = 1;
bool isMain = true;

bool isStarted;

char b;
int grad = 1;
int speed = 10;
int start = 10;
int end = 160;
int time = 10;
int pos;

int pre_pos = 0;

long totalSek = 0;

byte preset_count;
void setup() {
  //Set the characters and column numbers.
  lcd.begin();
  //Print default title.
  clearPrintTitle();

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
  Serial.begin(9600); // открываем Serial порт

preset_count=EEPROM.read(0);
  MsTimer2::set(1000, to_Timer); // Здесь задаем период 1 секунда
  MsTimer2::start();//Стартуем, теперь таймер запущен

}

void loop() {
  //Call the main menu.
  if (isStarted) {
    rotate();
  }
  else {
    mainMenu();
  }
}

void to_Timer()//Функция Таймер, он считает и пишет на экран время которое прошло
{
  totalSek++;//Сюда попадаем каждую секунду и приплюсовываем одну секунду
  //Далее рассчитываем часы минуты секунды и записываем на экран

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
        state = value[i - 1][j - 1] - 48;
      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}
void matrix2 () // создаем функцию для чтения кнопок
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
          lcd.clear();
          lcd.print("Stopped.");
          delay(1000);
          isStarted = false;
        }

      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}


void rotate() {
  matrix2();
  Serial.println(pos);
  goservo.write(pos);
  delay(41 - speed); // ждём 15ms пока серва займёт новое положение
  pos = pos + grad;
  // меняет значение затухания на аналогичное с противоположным знаком при граничных значениях:

  if (pos <= start) {
    grad = -grad ;
    pos = start;
  }
  if (pos >= end) {
    grad = -grad ;
    pos = end;
  }

  lcd.setCursor(11, 0);
  lcd.print((totalSek % 3600) / 60); //Далее минуты
  lcd.print(":");
  lcd.print((totalSek % 3600) % 60); //Далее секунды

  if ((totalSek % 3600) / 60 > time) {
    lcd.clear();
    lcd.print("Execrcise ended");
  }


}


void mainMenu() {


  //State = 0 every loop cycle.
  state = 0;
  matrix();
  //Refresh the button pressed.
  //Set the Row 0, Col 0 position.
  lcd.setCursor(0, 0);

  //If we are out of bounds on th menu then reset it.
  if (currentMenuItem <= 1) {
    currentMenuItem = 1;
  }
  if (currentMenuItem > 5) {
    currentMenuItem = 5;
  }


  //If we have changed Index, saves re-draws.
  if (state != lastState) {
    if (state == 2 && isMain) {
      //If Up
      currentMenuItem = currentMenuItem - 1;
      displayMenu(currentMenuItem);
    } else if (state == 8 && isMain) {
      //If Down
      currentMenuItem = currentMenuItem + 1;
      displayMenu(currentMenuItem);
    } else if (state == 6 ) {
      //If Selected
      selectMenu(currentMenuItem);
    }
    else if (state == 4) {
      isMain = true;
      //If Selected
      displayMenu(currentMenuItem);
    }

    if (!isMain && currentMenuItem == 1 && state == 2) {
      speed += 2;
      if (speed > 40) speed = 40;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 1 && state == 8) {
      speed -= 2;
      if (speed < 0) speed = 0;
      selectMenu(currentMenuItem);
    }

    if (!isMain && currentMenuItem == 2 && state == 2) {
      start += 5;
      if (start < 0) start = 90;
      if (start > end) start = end;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 2 && state == 8) {
      start -= 5;
      if (start < 0) start = 0;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 3 && state == 2) {
      end += 5;
      if (end > 180) end = 180;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 3 && state == 8) {
      end -= 5;
      if (end < start) end = start;
      if (end < 0) end = 0;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 4 && state == 2) {
      time += 1;
      if (time > 20) time = 20;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 4 && state == 8) {
      time -= 1;
      if (time < 1) time = 1;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 5 && state == 2) {
      pre_pos -= 1;
      if (pre_pos < 0) pre_pos = 0;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 5 && state == 8) {
      pre_pos += 1;
      if (pre_pos > present_count) pre_pos = preset_count;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem == 7 && state == 5) {
      speed  = EERPOM.read(pre_pos*4+1);
      start  = EERPOM.read(pre_pos*4+2);
      end  = EERPOM.read(pre_pos*4+3);
      time  = EERPOM.read(pre_pos*4+4);
    }
    if (!isMain && currentMenuItem == 7 && state == 5) {
      EEPROM.write(speed);
      EEPROM.write(start);
      EEPROM.write(end);
       EEPROM.write(time);
    }
    //Save the last State to compare.
    lastState = state;
  }

  //Small delay
  delay(5);
}


//Display Menu Option based on Index.
void displayMenu(int x) {

  switch (x) {
    case 1:
      clearPrintTitle();
      lcd.print ("-> Select speed");
      break;
    case 2:
      clearPrintTitle();
      lcd.print ("->Select start");
      break;
    case 3:
      clearPrintTitle();
      lcd.print ("-> Select end");
      break;
    case 4:
      clearPrintTitle();
      lcd.print ("-> Select time");
      break;
    case 5:
      clearPrintTitle();
      lcd.print("-> Presets");
      break;
    case 6:
      clearPrintTitle();
      lcd.print ("-> Start");
      break;
    case 7:
      clearPrintTitle();
      lcd.print ("-> Save");
      break;
  }
}

//Print a basic header on Row 1.
void clearPrintTitle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reability");
  lcd.setCursor(0, 1);
}

//Show the selection on Screen.
void selectMenu(int x) {
  Serial.print(x);
  switch (x) {
    case 1:
      isMain = false;
      clearPrintTitle();
      lcd.print ("Speed: ");
      lcd.setCursor(10, 1);
      lcd.print(speed);
      //Call the function that belongs to Option 1
      break;
    case 2:
      isMain = false;
      clearPrintTitle();
      lcd.print ("Start: ");
      lcd.setCursor(10, 1);
      lcd.print(start);

      //Call the function that belongs to Option 2
      break;
    case 3:
      isMain = false;
      clearPrintTitle();
      lcd.print ("End: ");
      lcd.setCursor(10, 1);
      lcd.print(end);
      //Call the function that belongs to Option 3
      break;
    case 4:
      isMain = false;
      clearPrintTitle();
      lcd.print ("Time(min): ");
      lcd.setCursor(10, 1);
      lcd.print(time);
      //Call the function that belongs to Option 3
      break;
    case 5:
      isMain = false;
      lcd.clear();
      lcd.print("SpeedStartEndTime");
      lcd.setCursor(0, 1);
      lcd.print(EERPOM.read(pre_pos*4+1);;
      lcd.setCursor(4, 1);
      lcd.print(EERPOM.read(pre_pos*4+2));
      lcd.setCursor(8, 1);
      lcd.print(EERPOM.read(pre_pos*4+3));
      lcd.setCursor(12, 1);
      lcd.print(EERPOM.read(pre_pos*4+4));
      break;
    case 6:
      isStarted = true;
      pos = start;
      totalSek = 0;
      lcd.clear();
      lcd.print("Progress");
      lcd.setCursor(0, 1);
      lcd.print("For stop press *");
    case 7:
      isMain = false;
      lcd.clear();
      lcd.print("SpeedStartEndTime");
      lcd.setCursor(0, 1);
      lcd.print(speed);
      lcd.setCursor(4, 1);
      lcd.print(start);
      lcd.setCursor(8, 1);
      lcd.print(end);
       lcd.setCursor(12, 1);
      lcd.print(time);
      break;

  }
}

