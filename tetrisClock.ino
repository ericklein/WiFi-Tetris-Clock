/*
  Project:      tetrisClock
  Description:  An internet connected clock displaying time using falling tetris blocks
*/

// hardware and software configuration parameters
#include "config.h"
// private credentials
#include "secrets.h"

// WiFi used for NTP access
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

// LED matrix control, https://github.com/2dom/PxMatrix
#include <PxMatrix.h>

// Draws out characters using tetris block animation, https://github.com/toblum/TetrisAnimation
#include <TetrisMatrixDraw.h>

// Access NTP and adjust for DST, https://github.com/ropg/ezTime
#include <ezTime.h>
Timezone myTZ;

// instantiate LED matrix objects
PxMATRIX display(64, 32, ledMatrixLAT, ledMatrixOE, ledMatrixA, ledMatrixB, ledMatrixC, ledMatrixD, ledMatrixE);

TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

// Global variables
// timers
uint32_t timeLastWiFiConnectMS = 0;

uint8_t rssi = 0; // 0 value used to indicate no WiFi connection

bool setMatrixTime() {
  static String lastDisplayedTime;
  static String lastDisplayedAmPm;

  bool timeChanged = false;

  String timeString;
  String AmPmString;
  if (time12HourFormat) {
    // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
    timeString = myTZ.dateTime("g:i");

    //If the length is only 4, pad it with
    // a space at the beginning
    if (timeString.length() == 4) {
      timeString = " " + timeString;
    }

    //Get if its "AM" or "PM"
    AmPmString = myTZ.dateTime("A");
    if (lastDisplayedAmPm != AmPmString) {
      debugMessage(String("Switching AM/PM to: ") + AmPmString,2);
      lastDisplayedAmPm = AmPmString;
      // Second character is always "M"
      // so need to parse it out
      tetris2.setText("M", ledMatrixForceRefresh);

      // Parse out first letter of String
      tetris3.setText(AmPmString.substring(0, 1), ledMatrixForceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    debugMessage(String("Switching minutes to: ") + timeString,2);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, ledMatrixForceRefresh);
    timeChanged = true;
  }

  return timeChanged;
}

// set function pointer to the active animation
auto activeAnimation = time12HourFormat ? animateTwelveHour : animateTwentyFourHour;

void setup() {
  #ifdef CODEDEBUG
    Serial.begin(115200);
    // wait for serial port connection
    while (!Serial)
    // Display key configuration parameters
    debugMessage(String("tetrisClock started with device ID: ") + networkDeviceID, 1);
    debugMessage(String("WiFi connection check interval is ") + (timeWiFiKeepAliveIntervalMS/1000) + " seconds",1);
  #endif

  // Intialize display library
  display.begin(ledMatrixScanRate);
  tetris.scale = 2;

  // connect to Wifi
  drawConnecting(5,10);
  if (networkConnect())
  {
    // set the time
    waitForSync();
    myTZ.setLocation(timeMyTimeZone);
    debugMessage(String("Time in local timezone: ") + myTZ.dateTime(),1);
  }
  else
  {
    // alert user to the WiFi connectivity problem
    debugMessage(String("Connection to ") + WIFI_SSID + " failed", 1);
    drawNoNetwork(5,10);
    powerDisable(hardwareRebootInterval);
  }
}

void loop()
{
  static bool animationDone = false;
  static bool colonVisible = true;

  unsigned long now = millis();

  // re-establish WiFi connection if needed
  if ((WiFi.status() != WL_CONNECTED) && (millis() - timeLastWiFiConnectMS > timeWiFiKeepAliveIntervalMS)) {
    timeLastWiFiConnectMS = millis();
    if (!networkConnect()) {
      // alert user to the WiFi connectivity problem
      rssi = 0;
      debugMessage(String("Connection to ") + WIFI_SSID + " failed", 1);
      // IMPROVEMENT: How do we want to handle this? the rest of the code will barf...
      // ESP.restart();
    }
    waitForSync(); // wait until time is synchonized from NTP
  }

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

static bool showColon()
{
  static uint8_t colonCounter = 0;
  colonCounter++;
  uint8_t colonFraction = colonCounter / 5;
  return !!(colonFraction % 2);
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

void drawConnecting(uint8_t initialX, uint8_t initialY)
{
  debugMessage("Starting drawConnecting",2);
  tetris.drawChar("C", initialX, initialY, tetris.tetrisCYAN);
  tetris.drawChar("o", initialX + 5, initialY, tetris.tetrisMAGENTA);
  tetris.drawChar("n", initialX + 11, initialY, tetris.tetrisYELLOW);
  tetris.drawChar("n", initialX + 17, initialY, tetris.tetrisGREEN);
  tetris.drawChar("e", initialX + 22, initialY, tetris.tetrisBLUE);
  tetris.drawChar("c", initialX + 27, initialY, tetris.tetrisRED);
  tetris.drawChar("t", initialX + 32, initialY, tetris.tetrisWHITE);
  tetris.drawChar("i", initialX + 37, initialY, tetris.tetrisMAGENTA);
  tetris.drawChar("n", initialX + 42, initialY, tetris.tetrisYELLOW);
  tetris.drawChar("g", initialX + 47, initialY, tetris.tetrisGREEN);
  display.display(70);
  debugMessage("exiting drawConnecting",2);
}

void drawNoNetwork(uint8_t initialX, uint8_t initialY)
{
  tetris.drawChar("N", initialX, initialY, tetris.tetrisCYAN);
  tetris.drawChar("o", initialX + 5, initialY, tetris.tetrisMAGENTA);
  tetris.drawChar(" ", initialX + 11, initialY, tetris.tetrisYELLOW);
  tetris.drawChar("n", initialX + 17, initialY, tetris.tetrisGREEN);
  tetris.drawChar("e", initialX + 22, initialY, tetris.tetrisBLUE);
  tetris.drawChar("t", initialX + 27, initialY, tetris.tetrisRED);
  tetris.drawChar("w", initialX + 32, initialY, tetris.tetrisWHITE);
  tetris.drawChar("o", initialX + 37, initialY, tetris.tetrisMAGENTA);
  tetris.drawChar("r", initialX + 42, initialY, tetris.tetrisYELLOW);
  tetris.drawChar("k", initialX + 47, initialY, tetris.tetrisGREEN);
  display.display(70);
}

void debugMessage(String messageText, uint8_t messageLevel)
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

bool networkConnect() 
// Connect to WiFi network specified in secrets.h
{
  // reconnect to WiFi only if needed
  if (WiFi.status() == WL_CONNECTED) 
  {
    debugMessage("Already connected to WiFi",2);
    return true;
  }

  WiFi.mode(WIFI_STA); // IMPROVEMENT: test to see if this improves connection time
  // set hostname has to come before WiFi.begin
  WiFi.hostname(networkDeviceID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t timeWiFiConnectStart = millis();
  while ((WiFi.status() != WL_CONNECTED) && ((millis() - timeWiFiConnectStart) < timeNetworkConnectTimeoutMS)) {
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) 
    {
      rssi = abs(WiFi.RSSI());
      debugMessage(String("WiFi IP address lease from ") + WIFI_SSID + " is " + WiFi.localIP().toString(), 1);
      debugMessage(String("WiFi RSSI is: ") + rssi + " dBm", 2);
      return true;
    }
  else {
    return false;
  }
}

const char* networkWiFiMessage(wl_status_t status)
// Converts WiFi.status() to string
{
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

void networkDisconnect()
// Disconnect from WiFi network
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  debugMessage("power off: WiFi",1);
}

void powerDisable(uint8_t deepSleepTime)
// Powers down hardware in preparation for board deep sleep
{
  debugMessage("powerDisable started",1);

  networkDisconnect();

  esp_sleep_enable_timer_wakeup(deepSleepTime*1000000); // ESP microsecond modifier
  debugMessage(String("powerDisable complete: ESP32 deep sleep for ") + deepSleepTime + " seconds",1);
  esp_deep_sleep_start();
}