/*
Cofre
Projeto realizador por: Jorge Pereira 40148, Gonçalo Moreira 40114,Diogo Ferreira 40101, David Pinto 40348
*/
#include <Servo.h>
int ledRed = A1;   //led rgb
int ledGreen = A3; //led rgb
int ledBlue = A2;  //led rgb
int servoPin = A4; //servo
int btnPin = 6;    // pin do botao verificar codigo
int btn2Pin = 2;   // pin do botao new code
int tiltPin = 5;   // pin do tilt sensor
int PinPotenciometro = A0;
int piezoPin = 4;
int pins[7] = {7, 8, 9, 10, 11, 12, 13};
int array[10][7] = {{1, 1, 1, 1, 1, 1, 0},  //0
                    {0, 1, 1, 0, 0, 0, 0},  //1
                    {1, 1, 0, 1, 1, 0, 1},  //2
                    {1, 1, 1, 1, 0, 0, 1},  //3
                    {0, 1, 1, 0, 0, 1, 1},  //4
                    {1, 0, 1, 1, 0, 1, 1},  //5
                    {1, 0, 1, 1, 1, 1, 1},  //6
                    {1, 1, 1, 0, 0, 0, 0},  //7
                    {1, 1, 1, 1, 1, 1, 1},  //8
                    {1, 1, 1, 0, 0, 1, 1}}; //9

Servo myservo;
int code[4] = {1, 1, 1, 1}; //Codigo correto
int codeInserted[4];        //Codigo inserido
int codeLength = 0;         //Posicao no array 'codeInserted'
int codeLengthAdmin = 0;
int val = 0;
bool correct = false;
int i;
int j;
bool admin = false;
bool assault = false;
/////////////////////////////////////////////////////////////////////////////////////////
static const int buttonPin = 2; // pin do butao
int buttonStatePrevious = LOW;
unsigned long minButtonLongPressDuration = 200; // delay wait
unsigned long buttonLongPressMillis;            // Tempo de qd o botao foi pressionado
bool buttonStateLongPress = false;              // é verdade se for pressionado por mais de dois segundos

unsigned long buttonPressDuration; // Tempo q o butao é pressionado

//// GENERAL ////

unsigned long currentMillis; // variavel q conta o tempo

void setup()
{
  myservo.attach(3);
  pinMode(A0 * 9, INPUT);    //Potenciometro
  pinMode(ledBlue, OUTPUT);  //led rgb
  pinMode(ledRed, OUTPUT);   //led rgb
  pinMode(ledGreen, OUTPUT); //led rgb
  pinMode(btnPin, INPUT);    // button1
  pinMode(btn2Pin, INPUT);   // button2
  pinMode(tiltPin, INPUT);

  for (int i = 0; i < 7; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
}

void readButtonState()
{
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && buttonStatePrevious == HIGH && !buttonStateLongPress)
  {
    buttonLongPressMillis = currentMillis;
    buttonStatePrevious = LOW;
    //Serial.println("Button pressed");
  }
  buttonPressDuration = currentMillis - buttonLongPressMillis;

  if (buttonState == LOW && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration)
  {
    buttonStateLongPress = true;
    //Serial.println("Button long pressed");
    if (correct)
    {
      admin = true;
      digitalWrite(ledBlue, HIGH);
    }
  }

  if (buttonState == HIGH && buttonStatePrevious == LOW)
  {
    buttonStatePrevious = HIGH;
    buttonStateLongPress = false;
    //Serial.println("Button released");

    if (buttonPressDuration < minButtonLongPressDuration)
    {
      //Serial.println("Button pressed shortly");
    }
  }
}

void loop()
{
  currentMillis = millis(); //começamos o tempo
  readButtonState();        // lemos o estado do butao 2

  if (digitalRead(tiltPin) == 0)
  {
    digitalWrite(ledRed, HIGH);
    assault = true;
  }
  else
  {
    digitalWrite(ledRed, LOW);
    assault = false;
  }

  int estadobotao = digitalRead(btnPin); // ver se botao esta premido ou nao

  val = map(analogRead(PinPotenciometro), 0, 1023, 0, 9);
  for (i = 0; i < 7; i++)
  {
    digitalWrite(pins[i], array[val][i]);
  }

  if (estadobotao == LOW)
  { // funcao caso botao seja premido
    if (!assault)
    {
      if (!admin)
      {
        tone(piezoPin, 700, 20);
        codeInserted[codeLength] = val;

        delay(100);
        codeLength++;
        if (codeLength == 4)
        {
          verificarCodigo();
        }
        noTone(4);
      }
      else
      { //modo admin
        digitalWrite(ledBlue, HIGH);
        tone(piezoPin, 700, 20);
        code[codeLengthAdmin] = val;
        delay(100);
        codeLengthAdmin++;
        if (codeLengthAdmin == 4)
        {
          newPin();
        }
        noTone(4);
      }
    }
  }
}

void verificarCodigo()
{
  correct = true;
  for (j = 0; j < 4; j++)
  {
    if (code[j] != codeInserted[j])
    {
      digitalWrite(ledRed, HIGH);
      tone(piezoPin, 500, 500);
      delay(1000);
      digitalWrite(ledRed, LOW);
      reset();
      return;
    }
  }
  digitalWrite(ledGreen, HIGH);
  tone(piezoPin, 1700, 500);
  myservo.write(180);
  delay(5000);
  reset();
}

void reset()
{
  digitalWrite(ledGreen, LOW);
  if (correct)
  {
    myservo.write(90);
  }
  noTone(4);
  memset(codeInserted, 0, sizeof(codeInserted));
  codeLength = 0;
  codeLengthAdmin = 0;
  Serial.println("apaguei");
}

void newPin()
{
  delay(1000);
  digitalWrite(ledBlue, LOW);
  Serial.println("new code");
  Serial.println(code[0]);
  Serial.println(code[1]);
  Serial.println(code[2]);
  Serial.println(code[3]);
  admin = false;
  reset();
}