/*
 * Programme de test des entrées/sorties pour le jeu de réflexes
 */

#include <RedMP3.h>
#include <LiquidCrystal.h>

#define LCD_RS 13
#define LCD_EN 12
#define LCD_D4 11
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8
LiquidCrystal afficheur(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/*
 * Clignotement d'une LED
 * Paramètre onTime (0 : éteinte, -1 : allumée, >0 : clignote)
 */
class Flasher
{
  byte ledPin;
  long onTime;
  long offTime;
  int ledState;
  unsigned long previousMillis;
public:
  Flasher(int pin, long on, long off)
  {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);
    onTime = on;
    offTime = off;
    ledState = LOW;
    previousMillis = 0;
  }
  void Update()
  {
    if (onTime > 0)
    {
      unsigned long currentMillis = millis();
      if ((ledState == HIGH) && (currentMillis - previousMillis >= onTime))
      {
        ledState = LOW;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
      }
      else if ((ledState == LOW) && (currentMillis - previousMillis >= offTime))
      {
        ledState = HIGH;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
      }
    }
    else if (onTime == -1)
    {
      digitalWrite(ledPin, HIGH);
    }
  }
  void ChangeParameters(long on, long off)
  {
    onTime = on;
    offTime = off;
  }
};

Flasher ledStart(23, 300, 100);
Flasher led1(25, 400, 200);
Flasher led2(27, 400, 200);
Flasher led3(29, 400, 200);
Flasher led4(31, 400, 200);
Flasher led5(33, 400, 200);
Flasher led6(35, 400, 200);
Flasher led7(37, 400, 200);
Flasher led8(39, 400, 200);
Flasher led9(41, 400, 200);
int lastState[50];

void setup() {
  afficheur.begin(16, 2);
  afficheur.print("JEU DE REFLEXES");
  afficheur.setCursor(0,1);
  afficheur.print("Temps=60s");
    
  pinMode(22, INPUT);
  pinMode(24, INPUT);
  pinMode(26, INPUT);
  pinMode(28, INPUT);
  pinMode(30, INPUT);
  pinMode(32, INPUT);
  pinMode(34, INPUT);
  pinMode(36, INPUT);
  pinMode(38, INPUT);
  pinMode(40, INPUT);
  
  delay(500);
}

void loop() {
  //afficheur.display();
  for (int i=22; i<=40; i=i+2){
    if ((digitalRead(i) == HIGH) && (lastState[i] == LOW))
    {
      afficheur.setCursor(15,1);
      afficheur.print((i-22)/2);
      lastState[i] = HIGH;
     } 
     if (digitalRead(i) == LOW)
     {
      lastState[i] = LOW;
     }
  }
  ledStart.Update();
  led1.Update();
  led2.Update();
  led3.Update();
  led4.Update();
  led5.Update();
  led6.Update();
  led7.Update();
  led8.Update();
  led9.Update();
  delay(20);
}
