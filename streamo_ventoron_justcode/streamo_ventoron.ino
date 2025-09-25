/* By Ventoron 
Youtube: Ventoron
MakerWorld: Ventoron
  */
#include <BleKeyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include "apps.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

BleKeyboard bleKeyboard("Streamo");
Preferences prefs;

const int clkPin = 32;
const int dtPin  = 33;
const int swPin  = 25;


int currentIndex = 0;
int lastCLKState = HIGH;

bool buttonPressed = false;
unsigned long pressStartTime = 0;
const unsigned long longPressTime = 1500;

// Store preference: 0 = use appCommand, 1 = use siteCommand
int appPreferences[20];  // enough for all apps

void displayApp(const char* name, bool showType=false, int pref=0) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  String text(name);
  int spaceIndex = text.indexOf(' ');

  if(spaceIndex > 0) {
    // Two words → split into 2 lines
    String first = text.substring(0, spaceIndex);
    String second = text.substring(spaceIndex + 1);

    display.setTextSize(2);
    int16_t x1, y1;
    uint16_t w, h;

    // First line
    display.getTextBounds(first.c_str(), 0, 0, &x1, &y1, &w, &h);
    int16_t x = (SCREEN_WIDTH - w) / 2;
    display.setCursor(x, 10);
    display.println(first);

    // Second line
    display.getTextBounds(second.c_str(), 0, 0, &x1, &y1, &w, &h);
    x = (SCREEN_WIDTH - w) / 2;
    display.setCursor(x, 35);
    display.println(second);
  } else {
    // Single word → center vertically
    display.setTextSize(2);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(name, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (SCREEN_WIDTH - w) / 2;
    int16_t y = (SCREEN_HEIGHT - 2*h) / 2;
    display.setCursor(x, y);
    display.println(name);
  }

  if(showType) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(pref == 0 ? "App" : "Site");
  }

  display.display();
}

void launchApp(int idx) {
  displayApp("Launching...");
  if (!bleKeyboard.isConnected()) return;

  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press('r');
  bleKeyboard.releaseAll();
  delay(400);

  if(appPreferences[idx] == 0 && apps[idx].appCommand[0] != 0)
    bleKeyboard.print(apps[idx].appCommand);
  else
    bleKeyboard.print(apps[idx].siteCommand);

  bleKeyboard.write(KEY_RETURN);
  displayApp(apps[idx].name);
}

void setup() {
  Serial.begin(115200);

  bleKeyboard.end();
  delay(100);
  bleKeyboard.begin();
  delay(2000);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);

  // Load preferences
  prefs.begin("appPrefs", true);
  for(int i = 0; i < totalApps; i++){
    appPreferences[i] = prefs.getInt(String(i).c_str(), 0);
  }
  prefs.end();

  displayApp(apps[currentIndex].name);
}

void loop() {
  if (!bleKeyboard.isConnected()) return;

  // Rotary encoder rotation
  int clkState = digitalRead(clkPin);
  if (clkState != lastCLKState) {
    if (digitalRead(dtPin) != clkState) currentIndex++;
    else currentIndex--;

    if (currentIndex >= totalApps) currentIndex = 0;
    if (currentIndex < 0) currentIndex = totalApps - 1;

    displayApp(apps[currentIndex].name);
  }
  lastCLKState = clkState;

  // Button handling
  bool swState = digitalRead(swPin) == LOW;

  if(swState && !buttonPressed){
    buttonPressed = true;
    pressStartTime = millis();
  }

  if(!swState && buttonPressed){
    unsigned long pressDuration = millis() - pressStartTime;
    buttonPressed = false;

    if(pressDuration >= longPressTime){
      // Long press → toggle App/Site
      appPreferences[currentIndex] = !appPreferences[currentIndex];

      prefs.begin("appPrefs", false);
      prefs.putInt(String(currentIndex).c_str(), appPreferences[currentIndex]);
      prefs.end();

      displayApp(apps[currentIndex].name, true, appPreferences[currentIndex]);
      delay(800);
      displayApp(apps[currentIndex].name);
    } else {
      // Short press → launch
      launchApp(currentIndex);
    }
  }
}
