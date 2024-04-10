#ifndef TIME_HANDLE_H
#define TIME_HANDLE_H

#include <NTPClient.h>
#include <WiFiUdp.h>

#define NTP_SERVER "time.apple.com"   // ntp服务器地址
#define TIME_OFFSET 3600 * 8          // 北京时间+8
#define UPDATE_INTERVAL 5 * 60 * 1000 // 每5分钟同步一次服务器时间

class TimeHandle
{
private:
    long lastUpdateTime = 0; //
    WiFiUDP wifiUdp;         // wifiUdp
public:
    NTPClient _ntpClient = NTPClient(wifiUdp, NTP_SERVER, TIME_OFFSET, UPDATE_INTERVAL);

    void begin();

    void loop();
};

#endif