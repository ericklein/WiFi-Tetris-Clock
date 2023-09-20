/*
  Project:      tetrisClock
  Description:  Tetris clock that fetches its time Using the EzTimeLibrary

  forked from code developed by @wene on a fork from @witnessmenow
  See README.md for target information
*/

// hardware and internet configuration parameters
#include "config.h"
// private credentials for network
#include "secrets.h"

#include <WiFi.h>

// LED matrix control
#include <PxMatrix.h>

// This library draws out characters using a tetris block animation
// https://github.com/toblum/TetrisAnimation
#include <TetrisMatrixDraw.h>

// Library used for getting the time and adjusting for DST
// https://github.com/ropg/ezTime
#include <ezTime.h>

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

// global variables

int rssi;

bool networkAvailable = false;

Timezone myTZ;

bool setMatrixTime() {
  static String lastDisplayedTime;
  static String lastDisplayedAmPm;

  bool timeChanged = false;

  String timeString;
  String AmPmString;
  if (twelveHourFormat) {
    // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
    // Check the EZTime Github page for info on
    // time formatting
    timeString = myTZ.dateTime("g:i");

    //If the length is only 4, pad it with
    // a space at the beginning
    if (timeString.length() == 4) {
      timeString = " " + timeString;
    }

    //Get if its "AM" or "PM"
    AmPmString = myTZ.dateTime("A");
    if (lastDisplayedAmPm != AmPmString) {
      debugMessage(AmPmString,2);
      lastDisplayedAmPm = AmPmString;
      // Second character is always "M"
      // so need to parse it out
      tetris2.setText("M", forceRefresh);

      // Parse out first letter of String
      tetris3.setText(AmPmString.substring(0, 1), forceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    debugMessage(timeString,1);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, forceRefresh);
    timeChanged = true;
  }

  return timeChanged;
}

// set function pointer to the active animation
auto activeAnimation = animateIntro;

bool animateIntro(bool showColon)
{
  static unsigned long first_call = millis();  // remember the time of the first call

  tetris.drawText(1, 21);

  if(millis() > first_call + 1000)   // switch to clock animation 20 seconds after first call
  {
    activeAnimation = twelveHourFormat ? animateTwelveHour : animateTwentyFourHour;
    tetris.scale = 2;
  }

  return false;
}

bool animateTwelveHour(bool showColon)
{
  setMatrixTime();

  // Place holders for checking are any of the tetris objects
  // currently still animating.
  bool tetris1Done = false;
  bool tetris2Done = false;
  bool tetris3Done = false;

  tetris1Done = tetris.drawNumbers(-6, 26, showColon);
  tetris2Done = tetris2.drawText(56, 25);

  // Only draw the top letter once the bottom letter is finished.
  if (tetris2Done) {
    tetris3Done = tetris3.drawText(56, 15);
  }

  return tetris1Done && tetris2Done && tetris3Done;
}

bool animateTwentyFourHour(bool showColon)
{
  setMatrixTime();

  return tetris.drawNumbers(2, 26, showColon);
}

void drawConnecting(int x = 0, int y = 0)
{
  tetris.drawChar("C", x, y, tetris.tetrisCYAN);
  tetris.drawChar("o", x + 5, y, tetris.tetrisMAGENTA);
  tetris.drawChar("n", x + 11, y, tetris.tetrisYELLOW);
  tetris.drawChar("n", x + 17, y, tetris.tetrisGREEN);
  tetris.drawChar("e", x + 22, y, tetris.tetrisBLUE);
  tetris.drawChar("c", x + 27, y, tetris.tetrisRED);
  tetris.drawChar("t", x + 32, y, tetris.tetrisWHITE);
  tetris.drawChar("i", x + 37, y, tetris.tetrisMAGENTA);
  tetris.drawChar("n", x + 42, y, tetris.tetrisYELLOW);
  tetris.drawChar("g", x + 47, y, tetris.tetrisGREEN);
}

void setup() {
  #ifdef CODEDEBUG
    Serial.begin(115200);
    // wait for serial port connection
    while (!Serial)

    // Display key configuration parameters
    debugMessage("tetrisClock started", 1);
    debugMessage("Device ID: " + String(DEVICE_ID),1);
    debugMessage(String("Internet service reconnect delay is ") + CONNECT_ATTEMPT_INTERVAL + " seconds", 2);
  #endif

  // Intialize display library
  display.begin(SCAN_RATE);
  display.clearDisplay();
  display.display(70);

  WiFi.hostname(DEVICE_ID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  drawConnecting(5,10);
  for (int tries = 1; tries <= CONNECT_ATTEMPT_LIMIT; tries++)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      rssi = abs(WiFi.RSSI());
      debugMessage(String("WiFi IP address lease from ") + WIFI_SSID + " is " + WiFi.localIP().toString(),1);
      debugMessage(String("WiFi RSSI is: ") + rssi + " dBm",2);
      networkAvailable = true;
      //setDebug(INFO);
      waitForSync();
      myTZ.setLocation(F(MYTIMEZONE));
      debugMessage(String("Time in local timezone: ") + myTZ.dateTime(),1);
      break;
    }
    else
    {
      unsigned long now = millis();
      unsigned long start_time = now;
      while(now < start_time + (CONNECT_ATTEMPT_LIMIT*1000))
      {
        now = millis();
        if(0 == now % (unsigned long)2)
        {
          display.display(70);
        }
      }
    }
  }
  if (!networkAvailable)
    debugMessage("BAM",1);
}

static bool showColon()
{
  static unsigned int colonCounter = 0;
  colonCounter++;
  unsigned int colonFraction = colonCounter / 5;
  return !!(colonFraction % 2);
}

void loop()
{
  static bool animationDone = false;
  static bool colonVisible = true;

  unsigned long now = millis();

  if(0 == now % (unsigned long)100)
  {
    bool colonChanged = false;
    if(colonVisible != showColon())
    {
      colonVisible = !colonVisible;
      colonChanged = true;
    }

    if(!animationDone || colonChanged)
    {
      display.clearDisplay();
      animationDone = activeAnimation(colonVisible);
    }
  }

  if(0 == now % (unsigned long)2)
  {
    display.display(70);
  }
}

void debugMessage(String messageText, int messageLevel)
// wraps Serial.println as #define conditional
{
  #ifdef CODEDEBUG
    if (messageLevel <= DEBUG)
    {
      Serial.println(messageText);
      //Serial.flush();  // Make sure the message gets output (before any sleeping...)
    }
  #endif
}