//-DESP8266
//-DESP32

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include <ArduinoOTA.h>
 
#if 0 
// avoid WiFi.persistent() setting to true
#define SSID "my-ssid"
#define PASS "my-password"
#endif

_i8p
give_wifi_status(_i32 stat)
{
    _i8p ret;

    switch (stat) {
        case WL_NO_SHIELD: ret = "WL_NO_SHIELD"; break;
        case WL_IDLE_STATUS: ret = "WL_IDLE_STATUS"; break;
        case WL_NO_SSID_AVAIL: ret = "WL_NO_SSID_AVAIL"; break;
        case WL_SCAN_COMPLETED: ret = "WL_SCAN_COMPLETED"; break;
        case WL_CONNECTED: ret = "WL_CONNECTED"; break;
        case WL_CONNECT_FAILED: ret = "WL_CONNECT_FAILED"; break;
        case WL_CONNECTION_LOST: ret = "WL_CONNECTION_LOST"; break;
#ifdef ESP8266
        case WL_WRONG_PASSWORD: ret = "WL_WRONG_PASSWORD"; break;
#endif
        case WL_DISCONNECTED: ret = "WL_DISCONNECTED"; break;
        default: ret = "unknown"; break;
    }
    return ret;
}

#define WIFI_DIAG(a) \
{ \
    Serial.printf("\n---vvv--- WIFI diag ---vvv---\n"); \
    Serial.printf("WiFi status: %s\n", give_wifi_status(WiFi.status())); \
    if (a) Serial.printf("station (re)connected on IP: %s\n", WiFi.localIP().toString().c_str()); \
    WiFi.printDiag(Serial); \
    Serial.printf("BSSID: %s\n", WiFi.BSSIDstr().c_str()); \
    Serial.printf("RSSI: %d\n", WiFi.RSSI()); \
    Serial.printf("MAC: "); \
    Serial.println(WiFi.macAddress()); /* HOW TO CONVERT THIS??? XXX */ \
    Serial.printf("---^^^--- WIFI diag ---^^^---\n"); \
}

#ifdef ESP8266
// Callback functions will be called as long as these handler objects exist.
// => so make them global
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;
#else

/*
0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
2  SYSTEM_EVENT_STA_START                < ESP32 station start
3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
25 SYSTEM_EVENT_MAX
*/

#include <WiFi.h>
 
void WiFiEvent(WiFiEvent_t event) {
//  Serial.printf("[WiFi-event] event: %d\n", event);
  Serial.printf("\nEV: ");

  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: 
      Serial.println("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
#ifdef SSID
//      WiFi.begin(SSID, PASS);    //?!?! NOT NEED
#endif
WIFI_DIAG(0);
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Obtained IP address");
WIFI_DIAG(1);
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_BSS_RSSI_LOW:
      Serial.println("WiFi SYSTEM_EVENT_STA_BSS_RSSI_LOW");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
      Serial.println("WiFi SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      Serial.println("WiFi access point  stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_ACTION_TX_STATUS:
      Serial.println("WiFi SYSTEM_EVENT_ACTION_TX_STATUS");
      break;
    case SYSTEM_EVENT_ROC_DONE:
      Serial.println("WiFi SYSTEM_EVENT_ROC_DONE");
      break;
    case SYSTEM_EVENT_STA_BEACON_TIMEOUT:
      Serial.println("WiFi SYSTEM_EVENT_STA_BEACON_TIMEOUT");
      break;
    case SYSTEM_EVENT_FTM_REPORT:
      Serial.println("WiFi SYSTEM_EVENT_FTM_REPORT");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
    case SYSTEM_EVENT_ETH_LOST_IP:
      Serial.println("WiFi SYSTEM_EVENT_ETH_LOST_IP");
      break;
    default:
      Serial.printf("============= NO CASE [ %d ] ==============\n", event);
      break;
  }
}

#endif

#include <time.h>

#define TIME_ZONE           3600        // offset in secs
#define DAY_LIGHT_SAVING    3600        // offset in secs
#define NTP_SERVER          "pool.ntp.org"

#ifdef ESP8266
_u32
getLocalTime(struct tm *tip)
{
    time_t now = time(nullptr);

    *tip = *localtime(&now);
    return 1;
}
#endif

void
mysetup_intro(_u8p prg, _u8 ntp) 
{
    Serial.flush();
    Serial.print("+");
Serial.printf("\n<TP01: %d>\n", millis());                   // <=== TP01 gives mostly 36 on ESP32
                                                         // <=== TP01 gives mostly 71 on ESP8266
#ifdef ESP8266
    gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event) {
        WIFI_DIAG(1);
    });
    disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
        WIFI_DIAG(0);
    });
#else
//    WiFi.disconnect(1);
    WiFi.onEvent(WiFiEvent);
#endif
#ifdef SSID
    Serial.printf("SSID: EXPLICIT, SAVING TO NVRAM\n");
    WiFi.persistent(true);  // save params in NVRAM when new connect with new SSID params is given
#else
    Serial.printf("SSID: NOT EXPLICIT, REUSING NVRAM\n");
    WiFi.persistent(false); // avoid wear through saving data to mem // Don't save WiFi configuration in flash
#endif
    WiFi.mode(WIFI_STA);
#ifdef SSID
    WiFi.begin(SSID, PASS); 
#else
    WiFi.begin(); 
#endif
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("#");
        delay(500);
    }
    ArduinoOTA.onStart([]() {
        _i8p type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }
        Serial.printf("start updating %s\n", type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.printf("\nend\n");
    });
    ArduinoOTA.onProgress([](_i32 progress, _i32 total) {
#ifdef MYLED
        digitalWrite(LED_BUILTIN, millis() >> 8 & 1); 
#endif
        Serial.printf("progress: %u%%\r", progress / (total / 100));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.printf("Auth Failed\n");
        else if (error == OTA_BEGIN_ERROR) Serial.printf("Begin Failed\n");
        else if (error == OTA_CONNECT_ERROR) Serial.printf("Connect Failed\n");
        else if (error == OTA_RECEIVE_ERROR) Serial.printf("Receive Failed\n");
        else if (error == OTA_END_ERROR) Serial.printf("End Failed\n");
    });
    ArduinoOTA.begin();

    if (ntp) {
        struct tm timeinfo;
        _u32 retries = 5;

        configTime(DAY_LIGHT_SAVING, TIME_ZONE, NTP_SERVER);
        while (retries) {
            // first getLocalTime() may need a little time...
            if (getLocalTime(&timeinfo)) {
                Serial.printf("[ %s ] ready to operate at [ ", prg);
#ifdef ESP8266
                Serial.printf("%02d-%02d-%02d %02d:%02d:%02d ]\n",
                                    timeinfo.tm_year - 100,
                                    timeinfo.tm_mon + 1,
                                    timeinfo.tm_mday,
                                    timeinfo.tm_hour,
                                    timeinfo.tm_min,
                                    timeinfo.tm_sec);
#else
                Serial.println(&timeinfo, "%y-%m-%d %H:%M:%S ]");
#endif
                break;
            } else {
                if (--retries) {
                    Serial.printf("getLocalTime() failed, retrying...\n");
                } else {
                    Serial.printf("getLocalTime() failed, giving up\n");
                    break;
                }
                delay(200);
            }
        }
    } else {
        Serial.printf("[ %s ] ready to operate\n", prg);
    }
Serial.printf("<TP02: %d>\n", millis());            // <=== TP02 gives mostly 1096 on ESP32
                                                  // <=== TP02 gives mostly 578  on ESP8266
}

void
myloop_intro()
{
    ArduinoOTA.handle();
}

