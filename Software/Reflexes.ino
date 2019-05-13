/*   Jeu de Réflexes (Batak)
     Jérôme Menard
     Version 1.1 - Mai 2019
*/

#include <RedMP3.h>
#include <LiquidCrystal.h>

// Tests
const bool DEBUG = false;

/*
 *   Gestion du son
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
 *   Gestion de l'afficheur
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
 * Bouton
 */
class Switch
{
  byte switchPin;
  int lastState;
public:
  int state;
  
  Switch(int pin)
  {
    switchPin = pin;
    pinMode(switchPin, INPUT);
    state = LOW;
    lastState = LOW;
  }
  void Update()
  {
    if (digitalRead(switchPin) == HIGH && state == LOW && lastState == LOW)
    {
      state = HIGH;
      lastState = HIGH;
    } 
    else if (digitalRead(switchPin) == HIGH && state == HIGH)
    {
      state = LOW;
    }
    else if (digitalRead(switchPin) == LOW)
    {
      state = LOW;
      lastState = LOW;
    }
  }
};

/*
 * Gestion d'un bouton lumineux
 * Paramètres pinLed, pinSwitch et on (durée laissée avant extinction) 
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
  
  void Initialisation(int pinSwitch, int pinLed, long on)
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
    pointGagne = 0;
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

private:
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
};

/*
 * Déclarations des variables du programme
 */
Flasher ledBoutonStart(23, 300, 300);
BoutonLumineux* bouton = new BoutonLumineux[9];
Switch boutonStart(22);
Switch boutonTempsMoins(30);
Switch boutonTempsPlus(34);
Switch boutonNiveauMoins(38);
Switch boutonNiveauPlus(26);
Switch boutonMode(32);
byte selectionJeu = 0;
byte nombreAleatoire;
int score = 0;
int niveau = 3;
long niveauEqu;
bool menu = true;
bool gameStarted;
unsigned long dureePartie = 60;
unsigned long startTickCount;
int temps;

/*
 * Affichage du menu général
 */
void AffichageMenu(byte jeu)
{
  afficheur.clear();
  if (jeu == 0)
  {
    afficheur.print("BATAK");
    afficheur.setCursor(0,1);
    afficheur.print("Temps=");
    afficheur.setCursor(6,1);
    afficheur.print(dureePartie);
    afficheur.setCursor(10,1);
    afficheur.print("Niv=");
    afficheur.setCursor(14,1);
    afficheur.print(niveau);
  }
  else if (jeu == 1)
  {
    afficheur.print("SIMON");
    afficheur.setCursor(0,1);
    afficheur.print("Niv=");
    afficheur.setCursor(4,1);
    afficheur.print(niveau);
  }
  menu = true;
}

/*
 * Calcul du score
 */
int CalculScore()
{
  return bouton[1].pointGagne + bouton[2].pointGagne + bouton[3].pointGagne + bouton[4].pointGagne + bouton[5].pointGagne + bouton[6].pointGagne + bouton[7].pointGagne + bouton[8].pointGagne + bouton[9].pointGagne;
}

/*
 * Calcul du niveau équivalent pour les objets boutonLumineux
 */
long CalculNiveau(int niveau)
{
  return trunc(185.71*sq(niveau)-2054.3*niveau+6370);
}

/*
 * Setup du programme ARDUINO
 */
void setup() {
  niveauEqu = CalculNiveau(niveau);
  // Inititialisation des 9 boutons lumineux
  bouton[1].Initialisation(24, 25, niveauEqu);
  bouton[2].Initialisation(26, 27, niveauEqu);
  bouton[3].Initialisation(28, 29, niveauEqu);
  bouton[4].Initialisation(30, 31, niveauEqu);
  bouton[5].Initialisation(32, 33, niveauEqu);
  bouton[6].Initialisation(34, 35, niveauEqu);
  bouton[7].Initialisation(36, 37, niveauEqu);
  bouton[8].Initialisation(38, 39, niveauEqu);
  bouton[9].Initialisation(40, 41, niveauEqu);
  // Initialisation de l'afficheur
  afficheur.begin(16, 2);
  AffichageMenu(selectionJeu);
  // Initialisation du générateur de nombres aléatoires
  randomSeed(analogRead(0));
  // Initialisation du son et lecture du son d'accueil
  lecteurAudio.setVolume(volume);
  lecteurAudio.playWithFileName(0x01, sonAccueil);
  delay(1000);
}

/*
 * Programme principal ARDUINO
 */
void loop() {
  
  if (gameStarted)
  // Partie en cours
  {
    if (selectionJeu == 0)
    // ++++++ Jeu de Batak ++++++
    {
      if (not(bouton[1].active || bouton[2].active || bouton[3].active || bouton[4].active || bouton[5].active || bouton[6].active || bouton[7].active || bouton[8].active || bouton[9].active))
      {
        nombreAleatoire = random(1,10);
        if (DEBUG)
        {
          afficheur.setCursor(15,0);
          afficheur.print(nombreAleatoire);
        }
        bouton[nombreAleatoire].Activate();
      }
      for (int i=1; i<=9; i++)
      {
        bouton[i].Show();
      }
      score = CalculScore();
      temps = dureePartie -((millis() - startTickCount)/1000);
      if (temps <= 0)
      {
        gameStarted = false;
        ledBoutonStart.ChangeParameters(300, 300);
        for (int i=1; i<=9; i++)
        {
          bouton[i].Disable();
        }
        lecteurAudio.playWithFileName(0x01, sonFin);
        afficheur.setCursor(6,0);
        afficheur.print(temps);
        delay(3000);
      }
      afficheur.setCursor(6,1);
      afficheur.print(score);
      afficheur.setCursor(6,0);
      afficheur.print(temps);
      if (temps == 9 || temps == 99) {afficheur.print("  ");}
    }
    if (selectionJeu == 1)
    // ++++++ Jeu de Simon ++++++
    {
      // Insérer le code du jeu de Simon
    }
  }
  else
  {
    // Gestion des paramètres avant démarrage d'une partie
    
    // Appui sur le bouton MODE JEU
    if (boutonMode.state == HIGH)
    {
      if (selectionJeu == 0) {selectionJeu =1;} else {selectionJeu = 0;}
      AffichageMenu(selectionJeu);
      lecteurAudio.playWithFileName(0x01, sonDepart);
    }
    // Appui sur le bouton START
    if (boutonStart.state == HIGH)
    {
      ledBoutonStart.ChangeParameters(0, 0);
      niveauEqu = CalculNiveau(niveau);
      for (int i=1; i<=9; i++)
      {
        bouton[i].ChangeParameters(niveauEqu);
      }
      afficheur.clear();
      afficheur.setCursor(0,0);
      afficheur.print("Temps=");
      afficheur.setCursor(0,1);
      afficheur.print("Score=");
      startTickCount = millis();
      score = CalculScore();
      gameStarted = true;
      menu = false;
      lecteurAudio.playWithFileName(0x01, sonDepart);
    } 
    // Appui sur le bouton TEMPS -
    if (boutonTempsMoins.state == HIGH) 
    {
      if (not(menu)) {AffichageMenu(selectionJeu);}
      if (dureePartie >= 60 ) {dureePartie = dureePartie - 30;}
      afficheur.setCursor(6,1);
      afficheur.print(dureePartie);
      afficheur.print(" ");
      lecteurAudio.playWithFileName(0x01, sonParametre);
    } 
    // Appui sur le bouton TEMPS +
    if (boutonTempsPlus.state == HIGH)
    {
      if (not(menu)) {AffichageMenu(selectionJeu);}
      if (dureePartie <= 90 ) {dureePartie = dureePartie + 30;}
      afficheur.setCursor(6,1);
      afficheur.print(dureePartie);
      lecteurAudio.playWithFileName(0x01, sonParametre);
    } 
    // Appui sur le bouton NIVEAU +
    if (boutonNiveauPlus.state == HIGH)
    {
      if (not(menu)) {AffichageMenu(selectionJeu);}
      if (niveau < 5 )
      {
        niveau++;
        afficheur.setCursor(14,1);
        afficheur.print(niveau);
        lecteurAudio.playWithFileName(0x01, sonParametre);
      }
    } 
    // Appui sur le bouton NIVEAU -
    if (boutonNiveauMoins.state == HIGH)
    {
      if (not(menu)) {AffichageMenu(selectionJeu);}
      if (niveau > 1 )
      {
        niveau--;
        afficheur.setCursor(14,1);
        afficheur.print(niveau);
        lecteurAudio.playWithFileName(0x01, sonParametre);
      }
    }
    // Mise à jour des boutons que lorsqu'une partie n'est pas démarrée
    boutonStart.Update();
    boutonTempsMoins.Update();
    boutonTempsPlus.Update();
    boutonNiveauMoins.Update();
    boutonNiveauPlus.Update();          
  }

  // Mise à jour des LED
  ledBoutonStart.Update();
  delay(20);
}
