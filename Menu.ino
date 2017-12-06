#include <LiquidCrystal_I2C.h>


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
// двойной массив, обозначающий кнопку

//States for the menu.
int currentMenuItem = 0;
int lastState = 0;
int state = 1;
bool isMain = true;

int speed = 10;
int start = 10;
int end = 160;

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
  Serial.begin(9600); // открываем Serial порт
}

void loop() {
  //Call the main menu.
  mainMenu();
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
    if (currentMenuItem > 4) {
    currentMenuItem = 4;
  }
  
Serial.println(currentMenuItem);
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

    if (!isMain && currentMenuItem ==1 &&state == 2) {
      speed +=2;
      if(speed > 40) speed = 40;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem ==1 &&state == 8) {
      speed -=2;
      if(speed < 0) speed = 0;
      selectMenu(currentMenuItem);
    }
    
    if (!isMain && currentMenuItem ==2 &&state == 2) {
      start +=5;
      if(start < 0) start = 90;
       if(start > end) start = end;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem ==2 &&state == 8) {
      start -=5;
      if(start < 0) start = 0;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem ==3 &&state == 2) {
      end +=5;
      if(end > 180) end = 180;
      selectMenu(currentMenuItem);
    }
    if (!isMain && currentMenuItem ==3 &&state == 8) {
      end -=5;
      if(end < start) end = start;
      if(end<0) end = 0;
      selectMenu(currentMenuItem);
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
      lcd.print ("-> Start!");
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
  
  switch (x) {
    case 1:
    isMain=false;
      clearPrintTitle();
      lcd.print ("Speed: ");
          lcd.setCursor(10, 1);
      lcd.print(speed);
      //Call the function that belongs to Option 1
      break;
    case 2:
    isMain=false;
      clearPrintTitle();
      lcd.print ("Start: ");
         lcd.setCursor(10, 1);
      lcd.print(start);
  
      //Call the function that belongs to Option 2
      break;
    case 3:
    isMain=false;
      clearPrintTitle();
      lcd.print ("End: ");
      lcd.setCursor(10, 1);
      lcd.print(end);
      //Call the function that belongs to Option 3
      break;
 
  }
}
