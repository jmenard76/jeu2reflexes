/*   Jeu de Réflexes
     Jérôme Menard
     Version 1.0 - Mai 2019
*/

#include <RedMP3.h>
#include <LiquidCrystal.h>

// Tests
const bool DEBUG = false;

/*
   Gestion du son
*/
#define MP3_RX 19
#define MP3_TX 18
MP3 lecteurAudio(MP3_RX, MP3_TX);
int8_t volume = 0x15;     // 0~0x1e (niveau ajustable)
#define sonPointGagne   0x01    // 0x01 (laser lorsqu'un point est gagné)
#define sonDepart       0x02    // 0x02 (sifflet au départ)
#define sonFin          0x03    // 0x03 (gong lorsque le temps est écoulé)
#define sonParametre    0x04    // 0x04 (attente)
#define sonAccueil      0x05    // 0x05 (accueil au démarrage)

/*
   Gestion de l'afficheur
*/
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
      else if ((ledState == LOW) && (currentMillis - previousMillis >= onTime))
      {
        ledState = HIGH;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
      }
    }
    else if (onTime == 0)
    {
      digitalWrite(ledPin, LOW);
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

/*
 * Gestion
 * Paramètre onTime (0 : éteinte, -1 : allumée, >0 : clignote)
 */
class BoutonLumineux
{
  byte ledPin;
  byte switchPin;
  long onDelay;
  unsigned long previousMillis;
  unsigned long currentMillis;
  long onTimeLed;
  long offTimeLed;
  int ledState;
  int lastState;
  unsigned long previousMillisLed; 
public:
  bool active = false;
  byte pointGagne;
  BoutonLumineux(int pinLed, int pinSwitch, long on)
  {
    ledPin = pinLed;
    switchPin = pinSwitch;
    pinMode(ledPin, OUTPUT);
    pinMode(switchPin, INPUT);
    onDelay = on;
    onTimeLed = -1;
    offTimeLed = 100;
    ledState = LOW;
    previousMillisLed = 0;
    lastState = LOW;
  }
  void Update()
  {
    // Gestion de la Led
    if (onTimeLed > 0)
    {
      unsigned long currentMillisLed = millis();
      if ((ledState == HIGH) && (currentMillisLed - previousMillisLed >= onTimeLed))
      {
        ledState = LOW;
        previousMillisLed = currentMillisLed;
        digitalWrite(ledPin, ledState);
      }
      else if ((ledState == LOW) && (currentMillisLed - previousMillisLed >= offTimeLed))
      {
        ledState = HIGH;
        previousMillisLed = currentMillisLed;
        digitalWrite(ledPin, ledState);
      }
    }
    else if (onTimeLed == 0)
    {
      ledState == LOW;
      digitalWrite(ledPin, LOW);
    }
    else if (onTimeLed == -1)
      {
        ledState = HIGH;
        digitalWrite(ledPin, HIGH);
      }
    
    // Gestion du bouton
    if ((digitalRead(switchPin) == HIGH) && (lastState == LOW))
    {
      if (DEBUG)
      {
        afficheur.setCursor(14,1);
        afficheur.print(switchPin);
      }
      lastState = HIGH;
      if (active)
      {
        onTimeLed = 0;
        offTimeLed = 0;
        active = false;
        if (currentMillis - previousMillis <= onDelay)
        {
          pointGagne++;
          lecteurAudio.playWithFileName(0x01, sonPointGagne);
        }
      }
     } 
     if (digitalRead(switchPin) == LOW)
     {
      lastState = LOW;
     }
  }
  void ChangeParameters(long on)
  {
    onDelay = on;
  }
  void Activate()
  {
    active = true;
    previousMillis = millis();
  }
  void Disable()
  {
    active = false;
    onTimeLed = -1;
    offTimeLed = 0;
    pointGagne = 0;
    Show();
  }
  void Show()
  {
    if (onDelay > 0 && active)
    {
      currentMillis = millis();
      if (currentMillis - previousMillis <= (onDelay * 0.8))
      {
        onTimeLed = -1;
        offTimeLed = 0;
      }
      else if (currentMillis - previousMillis > (onDelay * 0.8) && currentMillis - previousMillis <= onDelay)
      {
        onTimeLed = 100;
        offTimeLed = 100;
      }
      else if (currentMillis - previousMillis > onDelay)
      {
        onTimeLed = 0;
        offTimeLed = 0;
        active = false;
      }
    }
    else
    {
      onTimeLed = 0;
      offTimeLed = 0;
    }
  Update();
  }
};

Flasher boutonStart(23, 300, 300);
BoutonLumineux bouton1(25, 24, 3000);
BoutonLumineux bouton2(27, 26, 3000);
BoutonLumineux bouton3(29, 28, 3000);
BoutonLumineux bouton4(31, 30, 3000);
BoutonLumineux bouton5(33, 32, 3000);
BoutonLumineux bouton6(35, 34, 3000);
BoutonLumineux bouton7(37, 36, 3000);
BoutonLumineux bouton8(39, 38, 3000);
BoutonLumineux bouton9(41, 40, 3000);
long voyant;
int points;
int niveau = 3;
int niveauEqu = trunc(185.71*sq(niveau)-2054.3*niveau+6370);
bool menu = true;
bool gameStarted;
int lastState[50];
unsigned long dureePartie = 60;
unsigned long startTickCount;
int temps;

void AffichageMenu()
{
  afficheur.clear();
  afficheur.print("JEU DE REFLEXES");
  afficheur.setCursor(0,1);
  afficheur.print("Temps=");
  afficheur.setCursor(6,1);
  afficheur.print(dureePartie);
  afficheur.setCursor(10,1);
  afficheur.print("Niv=");
  afficheur.setCursor(14,1);
  afficheur.print(niveau);
  menu = true;
}

void setup() {
  afficheur.begin(16, 2);
  AffichageMenu();
  pinMode(22, INPUT);
  pinMode(30, INPUT);
  pinMode(34, INPUT);
  pinMode(26, INPUT);
  pinMode(38, INPUT);  
  randomSeed(analogRead(0));
  delay(500);
  lecteurAudio.setVolume(volume);
  lecteurAudio.playWithFileName(0x01, sonAccueil);
}

void loop() {
  
  if (gameStarted)
  {
    if (not(bouton1.active || bouton2.active || bouton3.active || bouton4.active || bouton5.active || bouton6.active || bouton7.active || bouton8.active || bouton9.active))
    {
      voyant = random(1,10);
      if (DEBUG)
      {
        afficheur.setCursor(15,0);
        afficheur.print(voyant);
      }
      switch (voyant)
      {
        case 1:
          bouton1.Activate();
          break;
        case 2:
          bouton2.Activate();
          break;
        case 3:
          bouton3.Activate();
          break;
        case 4:
          bouton4.Activate();
          break;
        case 5:
          bouton5.Activate();
          break;
        case 6:
          bouton6.Activate();
          break;
        case 7:
          bouton7.Activate();
          break;
        case 8:
          bouton8.Activate();
          break;
        case 9:
          bouton9.Activate();
          break;
      }
    }
  
    bouton1.Show();
    bouton2.Show();
    bouton3.Show();
    bouton4.Show();
    bouton5.Show();
    bouton6.Show();
    bouton7.Show();
    bouton8.Show();
    bouton9.Show();
  
    points = bouton1.pointGagne + bouton2.pointGagne + bouton3.pointGagne + bouton4.pointGagne + bouton5.pointGagne + bouton6.pointGagne + bouton7.pointGagne + bouton8.pointGagne + bouton9.pointGagne;
    temps = dureePartie -((millis() - startTickCount)/1000);
    if (temps <= 0)
    {
      gameStarted = false;
      boutonStart.ChangeParameters(300, 300);
      bouton1.Disable();
      bouton2.Disable();
      bouton3.Disable();
      bouton4.Disable();
      bouton5.Disable();
      bouton6.Disable();
      bouton7.Disable();
      bouton8.Disable();
      bouton9.Disable();
      lecteurAudio.playWithFileName(0x01, sonFin);
    }
    afficheur.setCursor(8,1);
    afficheur.print(points);
    afficheur.setCursor(8,0);
    afficheur.print(temps);
    if (temps == 9) {afficheur.print(" ");}
  }
  else
  {
    if ((digitalRead(22) == HIGH) && (lastState[22] == LOW))
    {
      lastState[22] = HIGH;
      boutonStart.ChangeParameters(0, 0);
      niveauEqu = trunc(185.71*sq(niveau)-2054.3*niveau+6370);
      bouton1.ChangeParameters(niveauEqu);
      bouton2.ChangeParameters(niveauEqu);
      bouton3.ChangeParameters(niveauEqu);
      bouton4.ChangeParameters(niveauEqu);
      bouton5.ChangeParameters(niveauEqu);
      bouton6.ChangeParameters(niveauEqu);
      bouton7.ChangeParameters(niveauEqu);
      bouton8.ChangeParameters(niveauEqu);
      bouton9.ChangeParameters(niveauEqu);
      afficheur.clear();
      afficheur.setCursor(0,0);
      afficheur.print("Temps =");
      afficheur.setCursor(0,1);
      afficheur.print("Score =");
      startTickCount = millis();
      points = 0;
      gameStarted = true;
      menu = false;
      lecteurAudio.playWithFileName(0x01, sonDepart);
     } 
    if (digitalRead(22) == LOW)
    {
      lastState[22] = LOW;
    }  
    if ((digitalRead(30) == HIGH) && (lastState[30] == LOW))
    {
      lastState[30] = HIGH;
      if (not(menu)) {AffichageMenu();}
      if (dureePartie >= 60 ) {dureePartie = dureePartie - 30;}
      afficheur.setCursor(6,1);
      afficheur.print(dureePartie);
      afficheur.print(" ");
      lecteurAudio.playWithFileName(0x01, sonParametre);
    } 
    if (digitalRead(30) == LOW)
    {
      lastState[30] = LOW;
    }
    if ((digitalRead(34) == HIGH) && (lastState[34] == LOW))
    {
      lastState[34] = HIGH;
      if (not(menu)) {AffichageMenu();}
      if (dureePartie <= 90 ) {dureePartie = dureePartie + 30;}
      afficheur.setCursor(6,1);
      afficheur.print(dureePartie);
      lecteurAudio.playWithFileName(0x01, sonParametre);
    } 
    if (digitalRead(34) == LOW)
    {
      lastState[34] = LOW;
    }
    if ((digitalRead(26) == HIGH) && (lastState[26] == LOW))
    {
      lastState[26] = HIGH;
      if (not(menu)) {AffichageMenu();}
      if (niveau < 5 )
      {
        niveau++;
        afficheur.setCursor(14,1);
        afficheur.print(niveau);
        lecteurAudio.playWithFileName(0x01, sonParametre);
      }
    } 
    if (digitalRead(26) == LOW)
    {
      lastState[26] = LOW;
    }
    if ((digitalRead(38) == HIGH) && (lastState[38] == LOW))
    {
      lastState[38] = HIGH;
      if (not(menu)) {AffichageMenu();}
      if (niveau > 1 )
      {
        niveau--;
        afficheur.setCursor(14,1);
        afficheur.print(niveau);
        lecteurAudio.playWithFileName(0x01, sonParametre);
      }
    } 
    if (digitalRead(38) == LOW)
    {
      lastState[38] = LOW;
    }          
  }
  
  boutonStart.Update();
  delay(20);
}
