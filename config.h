/*
  Project:      tetrisClock
  Description:  public (non-secret) configuration data for tetrisClock
*/

// Configuration Step 1: Set debug message output
// comment out to turn off; 1 = summary, 2 = verbose
#define CODEDEBUG 1

// Set a timezone using the following list
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define MYTIMEZONE "America/Los_Angeles"

// Sets whether the clock should be 12 hour format or not.
const bool twelveHourFormat = true;

// If this is set to false, the number will only change if the value behind it changes
// e.g. the digit representing the least significant minute will be replaced every minute,
// but the most significant number will only be replaced every 10 minutes.
// When true, all digits will be replaced every minute.
const 	bool forceRefresh = false;

// set client ID; used by wifi
#define DEVICE_ID "tetrisClock"

// Configuration variables that are less likely to require changes

#define CONNECT_ATTEMPT_LIMIT	3 // max connection attempts to internet services
#define CONNECT_ATTEMPT_INTERVAL 10 // seconds between internet service connect attempts

// LED matrix parameters
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 21 // Adafruit ESP32 Feather to @witnessmenow Feather adapter board
//#define P_OE 2 // Generic ESP32

// LED matrix scan rate
#define SCAN_RATE 16