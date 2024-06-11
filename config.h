/*
  Project:      tetrisClock
  Description:  public (non-secret) configuration data for tetrisClock
*/

// Configuration Step 1: Set debug message output
// comment out to turn off; 1 = summary, 2 = verbose
// #define CODEDEBUG 2

// Configuration variables that change rarely

// Time
// Set a timezone using https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const String timeMyTimeZone = "America/Los_Angeles";

// Set the clock for either 12 or 24 hour time display
const bool time12HourFormat = true;

// Network
// client ID used by WiFi
const String networkDeviceID = "tetrisClock";
// max connection attempts to network services
const uint8_t networkConnectAttemptLimit = 3;
// seconds between network service connect attempts
const uint8_t networkConnectAttemptInterval = 10;

// LED Matrix
const uint8_t ledMatrixLAT = 22;
const uint8_t ledMatrixA = 19;
const uint8_t ledMatrixB = 23;
const uint8_t ledMatrixC = 18;
const uint8_t ledMatrixD = 5;
const uint8_t ledMatrixE = 15;
const uint8_t ledMatrixOE = 21; // Adafruit ESP32 Feather to @witnessmenow Feather adapter board
//const uint8_t ledMatrixOE = 2; // Generic ESP32

// LED matrix scan rate
const uint8_t ledMatrixScanRate = 16;

// When false, displayed numbers only change if the value changes
// When true, all digits will be refreshed every minute
const bool ledMatrixForceRefresh = false;

// Hardware
// Sleep time in seconds if hardware error occurs
const uint8_t hardwareRebootInterval = 10;