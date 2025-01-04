# Fotogrametria / Photogrammetry: Shadowless Tent Table

## Opis projektu / Project Description

Ten projekt dotyczy automatycznego stołu obrotowego do fotogrametrii, zaprojektowanego przy użyciu platformy Arduino. System umożliwia precyzyjne fotografowanie obiektów w kontrolowanych warunkach oświetleniowych, idealne do tworzenia modeli 3D. / This project involves an automated turntable for photogrammetry, designed using the Arduino platform. The system enables precise object photography under controlled lighting conditions, ideal for creating 3D models.

## Funkcje systemu / System Features

1. **Automatyczne robienie zdjęć / Automated Photo Capture:**
   - Regulowane przerwy między zdjęciami. / Adjustable intervals between photos.
   - Konfigurowalna liczba zdjęć na cykl (pełny obrót stołu). / Configurable number of photos per cycle (full table rotation).
   - Regulowana prędkość obrotu stołu. / Adjustable table rotation speed.

2. **Wyzwalanie migawki aparatu / Camera Shutter Control:**
   - Obsługa aparatów Canon poprzez złącze N3. / Support for Canon cameras via N3 connector.
   - Możliwość użycia smartfona jako aparatu. / Compatibility with smartphones as cameras.

3. **Personalizacja pracy stołu / Customizable Table Operation:**
   - Ustawienie kierunku obrotu. / Adjustable rotation direction.
   - Wybór liczby zdjęć na cykl: od 12 do 200 zdjęć. / Configurable photos per cycle: from 12 to 200.
   - Regulowana prędkość obrotu silnika krokowego. / Adjustable stepper motor rotation speed.

4. **Przyjazny interfejs / User-Friendly Interface:**
   - Wyświetlacz LCD 2x16 pokazujący bieżące ustawienia. / LCD 2x16 display showing current settings.
   - Sterowanie za pomocą dwóch przycisków i potencjometru. / Controlled via two buttons and a potentiometer.

## Wymagania sprzętowe / Hardware Requirements

- Arduino (dowolny model obsługujący używane biblioteki). / Arduino (any model supporting the required libraries).
- Silnik krokowy. / Stepper motor.
- Przekaźnik. / Relay.
- Złącze Canon N3 lub adapter do smartfona. / Canon N3 connector or smartphone adapter.
- Wyświetlacz LCD 2x16. / LCD 2x16 display.
- Potencjometr i przyciski sterujące. / Potentiometer and control buttons.
- Talerz obrotowy na bazie talerza cukierniczego. / Rotating table based on a cake turntable.

## Instrukcja konfiguracji / Configuration Instructions

1. **Czas przerwy / Interval Duration:**
   - Opcje: 0,5s, 1s, 1,5s, 2s, 2,5s, 3s. / Options: 0.5s, 1s, 1.5s, 2s, 2.5s, 3s.
   - Ustaw za pomocą przycisku 2. / Set using button 2.

2. **Kierunek obrotu / Rotation Direction:**
   - Wybór: zgodnie z ruchem wskazówek zegara lub przeciwnie. / Options: clockwise or counterclockwise.

3. **Liczba zdjęć na cykl / Photos per Cycle:**
   - Możliwości: 12, 24, 48, 75, 96, 120, 200 zdjęć. / Options: 12, 24, 48, 75, 96, 120, 200 photos.

4. **Prędkość obrotu / Rotation Speed:**
   - Opcje: bardzo wolno, wolno, normalnie, szybko, bardzo szybko. / Options: very slow, slow, normal, fast, very fast.
   - Wartości w mikrosekundach między krokami silnika: 2700 (najwolniej) do 500 (najszybciej). / Values in microseconds between motor steps: 2700 (slowest) to 500 (fastest).

## Dokumentacja i pliki / Documentation and Files

### Wersje oprogramowania / Software Versions

- **v1.0:** Pierwsza wersja z podstawową konfiguracją. / Initial version with basic configuration.
- **v1.1:** Dodanie buzzera i podstawowych funkcji dźwiękowych. / Added buzzer and basic sound functions.
- **v1.2:** Obsługa przekaźnika do wyzwalania migawki. / Relay support for shutter triggering.
- **v1.3:** Licznik zdjęć na ekranie. / Photo counter on the display.
- **v1.4:** Dopasowanie parametrów silnika do przekładni 1:12. / Adjusted motor parameters for 1:12 gearing.
- **v1.5:** Liniowe przyspieszanie i zwalnianie silnika. / Linear motor acceleration and deceleration.
- **v1.6:** Dłuższe wyświetlanie napisu początkowego oraz autorów. / Extended display time for startup message and authors.
- **v1.7:** Wyświetlanie docelowej liczby zdjęć podczas pracy. / Displaying target photo count during operation.

### Pliki do wycięcia laserem / Laser Cutting Files

- **SVG:**
  - `cuttingLaserTable.svg` - engine plate project. 

- **Pin Connection:**
  - `connection` .png/.fzz