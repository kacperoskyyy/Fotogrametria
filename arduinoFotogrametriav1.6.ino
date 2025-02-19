/*
================================================================
  Fotogrametria - sterowanie silnikiem krokowym, który kręci
  talerzem, w celu zeskanowania rzeczy w namiocie bezcieniowym
                          Wersja 1.6
               Autor kodu arduino: Kacper Dyląg
                        @RESET ATH 2024
================================================================
Parametry/ ustawienia: 

Przerwa - przerwa pomiedzy ruchami silniczka, w jej czasie ma wykonac sie zdjecie
Wartosci - 1. 0.5 sek     2. 1 sek     3. 1.5 sek      4. 2 sek      5. 2.5 sek      6. 3 sek

Kierunek - kierunek w ktroym bedzie krecic sie silniczek
Wartosci - 1. Lewo      2. Prawo 

Ilosc zdjec na cykl - ustawia interwał dlugosci krecenia silniczka miedzy zatrzymaniami na zdjecie
Wartosci - 12, 24, 48, 75, 96, 120, 200

Predkosc - szybkosc z jaka ma sie krecic silniczek
Wartosci - 1. Bardzo Wolno      2. Wolno      3. Normalnie      4. Szybko     5. Bardzo szybko

Dokumentacja dostępna w pliku "dokumentacjaArduino"

================================================================
*/
//biblioteki
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <string.h>
//Definiowanie
const int dirPin = 2;     //tutaj beda podawane informacje o kierunku krecenia do sterownika
const int stepPin = 3;    //tutaj podawane są informacje o krokach silnika
const int buzzerPin = 6;  //tutaj wysylane sa informacje o dźwieku
const int relayPin = 8;
int photoCounter = 0;
//int buzzerSound = 1;    //jest to zmienna potrzebna do przyszłej funkcji konfiguracji za pośrednictwem portu szeregowego
const int stepsPerRevolution = 200; //jeden pelny obrot malego kolka to 200
const int stepsPerBigSpin = 2400;   //jeden pelny obrot duzego kola to 2400
bool buttonStatus1, buttonStatus2;  //zmienne typu bool (prawda/falsz) o stanach guzika
#define buttonPin1 4          //zdefiniowanie pinu przycisku1
#define buttonPin2 5          //zdefiniowanie pinu przycisku2
int valuePot = 0; // zmienna do potencjometru
LiquidCrystal_I2C lcd(0x27, 16, 2);   //zdefiniowanie lcd

///Zmienne globalne, do ktorych bedzie kopiowana zawartosc eepromu
int timeG, rotateDirectG, stepResolutionG, rotationSpeedG;

void stepMotor();      ///Funkcja do silniczka, ktora ustawia kierunek i uruchoamia startMotor()
void startMotor();     ///Funkcja krecaca silniczkiem
void lcdStart();       ///Funkcja z poczatkowymi napisami i wejsciem do pierwszego menu (Start/ Konfiguracja)
void configPar();      ///Funkcja w której będą konfigurowane wszystkie wartosci
void startCycle();     ///Funkcja startująca cykl na zapisanych ustawieniach
void writeEEprom();    ///zapisywanie configa w eepromie, zeby nie trzeba bylo za kazdym razem ustawiac
void readEEprom();     ///czytanie configu z eepromu
void buzzer();         ///brzdek buzzerem
void photoCapture();   ///funkcja wyzwalająca migawke aparatu

void setup() {
  Serial.begin(9600);                 //rozpoczecie komunikacji serial portem z predkoscia 9600
  Serial.println("Start");            //Napis w SM 
  pinMode(buttonPin1,INPUT_PULLUP);    //ustawienie pinu guzika1 na wejsciowy
  pinMode(buttonPin2,INPUT_PULLUP);    //ustawienie pinu guzika2 na wejsciowy
  pinMode(stepPin, OUTPUT);           //ustawienie pinu kroku na wyjsciowy
  pinMode(dirPin, OUTPUT);            //ustawienie pinu kierunku na wyjsciowy
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  readEEprom();
  lcdStart();                         //funkcja do inicjalizacji i wyswietlenia pierwszych wartosci na lcd
} 
void loop() {
  buttonStatus1 = digitalRead(buttonPin1);      ////Odczytanie wartsci buttona 1
  buttonStatus2 = digitalRead(buttonPin2);      ////Odczytanie wartsci buttona 2
  if(buttonStatus1 == 0){ startCycle(); buzzer(0); }      ////Button 1 wystaruje cykl  1 w buzzzerze - dluga melodia  0 w buzzerze krotka melodia
  if(buttonStatus2 == 0){ configPar(); }        ////Button 2 odpali konfiguracje
  if(buttonStatus1 == 0 && buttonStatus2 == 0){  }  ///Jeszcze nie wiem co ale coś powstanie
}
///////////////////////////////////////////////krecenie silniczkiem
void stepMotor(bool direction,int timer){                             //true - ruch przeciwny do wskazowek zegara, false - ruch zgodny ze wskazowkami zegara
  int speedOfRotor=0;
  switch(rotationSpeedG){
  case 1: speedOfRotor = 2700; break;   //najwolniej
  case 2: speedOfRotor = 2400; break;   //wolno
  case 3: speedOfRotor = 2000; break;   //normalnie
  case 4: speedOfRotor = 1000; break;   //szybko
  case 5: speedOfRotor = 500; break;    //najszybciej
  default: speedOfRotor = 2000; break;    
  }
  int timePeriod=timeG*500;
  lcd.clear();
  lcd.print("Praca...");
  if(direction == true){                                    //ruch przeciwny
    Serial.println("Ruch przeciwny do wskazowek zegara");   //Napis w SM
    digitalWrite(dirPin, HIGH);                             //Ustawienie kierunku krecenia dirPinem
    startMotor(timer, timePeriod, speedOfRotor);
  }
  else{                                                  //ruch zgodny
  Serial.println("Ruch zgodny ze wskazowkami zegara");  //SM
  digitalWrite(dirPin, LOW);                          //kierunek
  startMotor(timer, timePeriod, speedOfRotor);
  }
  lcd.clear();
}
//////////////////////////////////////////wlaczenie silniczka zeby sie krecil
void startMotor(int timer, int speed, int speedOfRotor){
  int helpInt=0;

  int accelerationTime = 1000;
  unsigned long startTime = millis();

  while(helpInt < stepsPerBigSpin){
    for(int x = 0; x < timer; x++)
    {
    unsigned long currentTime = millis();
    int elapsedTime = currentTime - startTime;

    if (elapsedTime < accelerationTime) { speedOfRotor = map(elapsedTime, 0, accelerationTime, 2700, speedOfRotor); } 
    else { speedOfRotor = speedOfRotor; }

      digitalWrite(stepPin, HIGH);
      delayMicroseconds(speedOfRotor);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(speedOfRotor);
      helpInt++;
    }
  photoCounter++;
  delay(300);
  photoCapture();
  lcd.setCursor(0,1);
  lcd.print(photoCounter);
  delay(speed);
  }
  photoCounter = 0;
}
//////////////////////////////////////////Startowe napisy wyswietlacza i inicjalizacja go
void lcdStart(){
  lcd.init();
  lcd.backlight();
  lcd.print("Fotogametria");
  lcd.setCursor(0,1);
  lcd.print("@RESET 2024");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Kacper Dylag");
  lcd.setCursor(0,1);
  lcd.print("Marcelina Sitek");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Start");
  lcd.setCursor(0,1);
  lcd.print("2.Konfiguracja");
  delay(200);
}
/////////////////////////////////////////funkcja konfiguracji danych (w przyszlosci ma zapisywać do eepromu)
void configPar() {
  int time = timeG, rotateDirect = rotateDirectG, stepResolution = stepResolutionG, speedOfSpin = rotationSpeedG;
  int lastPotValue = 0;  // Zmienna przechowująca poprzednią wartość potencjometru
  int threshold = 20;    // Próg, po którym uznajemy, że potencjometr został poruszony

  //////////////////////////// Długość przerwy, na zrobienie zdjęcia (0,5sek - 3sek) co 0,5 sek
  lcd.clear();
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Dlugosc przerwy:");
  valuePot = time;  // Wyświetl początkową wartość z timeG
  lastPotValue = analogRead(A0);  // Odczytaj bieżącą wartość potencjometru

  do {
    buttonStatus2 = digitalRead(buttonPin2);
    int currentPotValue = analogRead(A0);

    // Sprawdzenie, czy potencjometr został poruszony
    if (abs(currentPotValue - lastPotValue) > threshold) {
      valuePot = map(currentPotValue, 1, 800, 1, 6);
      if (valuePot == 7) valuePot = 6;
    }

    lcd.setCursor(0,7);
    switch (valuePot) {
      case 1: lcd.print("0.5 s  "); break;
      case 2: lcd.print("1 s   "); break;
      case 3: lcd.print("1.5 s  "); break;
      case 4: lcd.print("2 s   "); break;
      case 5: lcd.print("2.5 s  "); break;
      case 6: lcd.print("3 s   "); break;
    }
    Serial.println(valuePot);
  } while (buttonStatus2 == 1);
  time = valuePot;

  //////////////////////////// Kierunek kręcenia silniczka
  delay(600);
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Kierunek: ");
  lcd.print("Kierunek:");
  valuePot = rotateDirect;  // Wyświetl początkową wartość z rotateDirectG
  lastPotValue = analogRead(A0);

  do {
    buttonStatus2 = digitalRead(buttonPin2);
    int currentPotValue = analogRead(A0);

    // Sprawdzenie, czy potencjometr został poruszony
    if (abs(currentPotValue - lastPotValue) > threshold) {
      if (currentPotValue <= 512) valuePot = 1;
      else valuePot = 2;
    }

    lcd.setCursor(0, 1);
    if (valuePot == 1) { lcd.print("Przeciwnie "); Serial.println("Lewo"); }
    else { lcd.print("Z zegarem "); Serial.println("Prawo"); }
  } while (buttonStatus2 == 1);
  rotateDirect = valuePot;

  //////////////////////////// Ilość zdjęć na duży obrót
  delay(600);
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Zdjec na cykl: ");
  lcd.print("Zdjec na cykl:");
  valuePot = stepResolution;  // Wyświetl początkową wartość z stepResolutionG
  lastPotValue = analogRead(A0);

  do {
    buttonStatus2 = digitalRead(buttonPin2);
    int currentPotValue = analogRead(A0);

    // Sprawdzenie, czy potencjometr został poruszony
    if (abs(currentPotValue - lastPotValue) > threshold) {
      valuePot = map(currentPotValue, 1, 900, 1, 7);
      if (valuePot == 8) valuePot = 7;
    }

    lcd.setCursor(0, 1);
    switch (valuePot) {
      case 1: lcd.print("12  "); break;
      case 2: lcd.print("24  "); break;
      case 3: lcd.print("48  "); break;
      case 4: lcd.print("75  "); break;
      case 5: lcd.print("96  "); break; 
      case 6: lcd.print("120  "); break;
      case 7: lcd.print("200  "); break;
    }
    Serial.println(valuePot);
  } while (buttonStatus2 == 1);
  stepResolution = valuePot;

  //////////////////////////// Prędkość kręcenia
  delay(600);
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Predkosc obrotu: ");
  lcd.print("Predkosc obrotu:");
  valuePot = speedOfSpin;  // Wyświetl początkową wartość z rotationSpeedG
  lastPotValue = analogRead(A0);

  do {
    buttonStatus2 = digitalRead(buttonPin2);
    int currentPotValue = analogRead(A0);

    // Sprawdzenie, czy potencjometr został poruszony
    if (abs(currentPotValue - lastPotValue) > threshold) {
      valuePot = map(currentPotValue, 1, 900, 1, 5);
      if (valuePot == 6) valuePot = 5;
    }

    lcd.setCursor(0, 1);
    switch (valuePot) {
      case 1: lcd.print("Bardzo wolno "); break;
      case 2: lcd.print("Wolno        "); break;
      case 3: lcd.print("Normalnie    "); break;
      case 4: lcd.print("Szybko       "); break;
      case 5: lcd.print("Bardzo szybko"); break;
    }
    Serial.println(valuePot);
  } while (buttonStatus2 == 1);
  speedOfSpin = valuePot;

  /////////////////////////////////////////////// Zapis do EEPROM i wyświetlanie menu głównego 
  writeEEprom(time, rotateDirect, stepResolution, speedOfSpin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ustawienia");
  lcd.setCursor(0, 1);
  lcd.print("zapisane");
  delay(500);
  lcd.clear();
  lcdStart();
}
////////////////////////////////////////////start cyklu 360 stopni
void startCycle(){
  delay(100);
  int timer = 0;
  switch(stepResolutionG){
    case 1: timer = stepsPerRevolution; break;          //1          //12 zdj
    case 2: timer = 0.5 * stepsPerRevolution; break;    //1/2       //24 zdj
    case 3: timer = 0.25 * stepsPerRevolution; break;    //1/4     //48 zdj
    case 4: timer = 0.16 * stepsPerRevolution; break;    //1/6     //75 zdj
    case 5: timer = 0.125 * stepsPerRevolution; break;    //1/8      //96 zdj
    case 6: timer = 0.1 * stepsPerRevolution; break;    //1/10      //120 zdj
    case 7: timer = 0.083 * stepsPerRevolution; break;    //1/12   //200 zdj
    default: timer = stepsPerRevolution; break;
  }
  if(rotateDirectG ==1){  stepMotor(true,timer);} //for(int i=0;i<10;i++)
  else{ stepMotor(false,timer);}
  lcdStart();
}
////////////////////////////////////////////zapisywanie konfigu w eepromie
void writeEEprom(int time, int rotateDirect, int stepResolution, int speedOfSpin){
  for(int i = 0; i<4;i++) EEPROM.write(i,0);                        ///czyszczenie eepromu
    Serial.println("===========================================");
    Serial.println("Zapisywanie do EEpromu");
    Serial.println("===========================================");
    Serial.print("Zapisywanie dlugosci przerwy: ");
    Serial.print(time);
    timeG=time;
    EEPROM.write(0, time);
    Serial.println("");
    Serial.print("Zapisywanie kierunku: ");
    if(rotateDirect == 0) Serial.print("Lewo");
    else Serial.print("Prawo");
    rotateDirectG = rotateDirect;
    EEPROM.write(1, rotateDirect);
    Serial.println("");
    Serial.print("Zapisywanie ilosci zdjec na obrot: ");
    stepResolutionG = stepResolution;
    switch (stepResolutionG){
    case 1: Serial.print("12"); break;
    case 2: Serial.print("24"); break;
    case 3: Serial.print("48"); break;
    case 4: Serial.print("75"); break;
    case 5: Serial.print("96"); break;
    case 6: Serial.print("120"); break;
    case 7: Serial.print("200"); break;
    }
    EEPROM.write(2, stepResolution);
    Serial.println("");
    Serial.print("Zapisywanie predkosci krecenia: ");
    rotationSpeedG=speedOfSpin;
    switch(rotationSpeedG){
    case 1: Serial.print("Bardzo wolno"); break;
    case 2: Serial.print("Wolno"); break;
    case 3: Serial.print("Normalnie"); break;
    case 4: Serial.print("Szybko"); break;
    case 5: Serial.print("Bardzo szybko"); break;
    }
    EEPROM.write(3, speedOfSpin);
    Serial.println("");
    Serial.println("===========================================");
    Serial.println("Zapisano!");
    Serial.println("===========================================");
    EEPROM.end();
}
////////////////////////////////////////////odczytywanie konfigu z eepromie
void readEEprom(){
  Serial.println("===========================================");
  Serial.println("Odczyt z EEpromu");
  Serial.println("===========================================");
  timeG=EEPROM.read(0);
  Serial.print("Odczytana dlugosc przerwy: ");
  Serial.print(timeG);
  rotateDirectG=EEPROM.read(1);
  Serial.println("");
  Serial.print("Odczytany kierunek: ");
  if(rotateDirectG == 0) Serial.print("Lewo");
  else Serial.print("Prawo");
  stepResolutionG=EEPROM.read(2);
  Serial.println("");
  Serial.print("Odczytana ilosc zdjec na cykl: ");
  switch (stepResolutionG){
  case 1: Serial.print("12"); break;
  case 2: Serial.print("24"); break;
  case 3: Serial.print("48"); break;
  case 4: Serial.print("75"); break;
  case 5: Serial.print("96"); break;
  case 6: Serial.print("120"); break;
  case 7: Serial.print("200"); break;
 }
  rotationSpeedG=EEPROM.read(3);
  Serial.println("");
  Serial.print("Odczytana predkosc: ");
   switch(rotationSpeedG){
    case 1: Serial.print("Bardzo wolno"); break;
    case 2: Serial.print("Wolno"); break;
    case 3: Serial.print("Normalnie"); break;
    case 4: Serial.print("Szybko"); break;
    case 5: Serial.print("Bardzo szybko"); break;
   }
  Serial.println("");
  Serial.println("===========================================");
  Serial.println("Odczyt zakonczony");
  Serial.println("===========================================");
  
}
////////////////////////////////////////////funkcja wywolujaca migawke aparatu
void photoCapture(){    
  Serial.print("takepic");    //Fraza służąca wywołaniu migawki aparatu telefonu poprzez specjalną aplikację
  digitalWrite(relayPin, HIGH);
  delay(500);
  digitalWrite(relayPin, LOW);
}
///////////////////////////////////////////glosniczek
void buzzer(int buzzerCheck){
  if(buzzerCheck == 0){
  tone(buzzerPin, 784, 300);   //G
  delay(300);
  tone(buzzerPin, 659, 300);   //E
  delay(300);
  tone(buzzerPin, 622, 300);   //D#
  delay(300);
  }
  else{
    //melodia samsunga
    tone(buzzerPin, 330, 250);   //E
    delay(250);

    tone(buzzerPin, 440, 250);   //A
    delay(250);
    tone(buzzerPin, 440, 250);   //A
    delay(250);
    tone(buzzerPin, 523, 250);   //C
    delay(250);
    tone(buzzerPin, 523, 250);   //C
    delay(250);

    tone(buzzerPin, 440, 500);   //A
    delay(500);
    tone(buzzerPin, 330, 250);   //E
    delay(250);
    tone(buzzerPin, 330, 250);   //E
    delay(250);
    
    tone(buzzerPin, 330, 375);   //E
    delay(375);
    tone(buzzerPin, 330, 125);   //E
    delay(125);
    tone(buzzerPin, 494, 125);   //H
    delay(125);
    tone(buzzerPin, 440, 125);   //A
    delay(125);
    tone(buzzerPin, 392, 125);   //G
    delay(125);
    tone(buzzerPin, 349, 125);   //F
    delay(125);

    tone(buzzerPin, 330, 750);   //E
    delay(750);
    tone(buzzerPin, 330, 250);   //E
    delay(250);

    tone(buzzerPin, 440, 250);   //A
    delay(250);
    tone(buzzerPin, 440, 250);   //A
    delay(250);
    tone(buzzerPin, 523, 250);   //C
    delay(250);
    tone(buzzerPin, 523, 250);   //C
    delay(250);

    tone(buzzerPin, 440, 500);   //A
    delay(500);
    tone(buzzerPin, 330, 250);   //E
    delay(250);
    tone(buzzerPin, 440, 250);   //A  
    delay(250);
    
    tone(buzzerPin, 392, 250);   //G
    delay(250);
    tone(buzzerPin, 349, 125);   //F
    delay(125);
    tone(buzzerPin, 392, 125);   //G
    delay(125);
    tone(buzzerPin, 440, 250);   //A
    delay(250);
    tone(buzzerPin, 311, 250);   //D#
    delay(250);

    tone(buzzerPin, 330, 750);   //E
    delay(750);
    noTone(buzzerPin);
  }
}