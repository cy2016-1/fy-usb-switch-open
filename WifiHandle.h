#ifndef WIFI_HANDLE_H
#define WIFI_HANDLE_H

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <IPAddress.h>
#include <LittleFS.h>

#include "Constant.h"
#include "EepromUtil.h"

#ifndef APSSID
#define APSSID "FY-USB-SWITCH"
#endif
#define WIFI_LED_PIN 2

#define WIFI_CONN_FAIL_TIP "fail"
#define WIFI_CONN_SUCC_TIP "success"
#define WIFI_LIST_URL "/configwifi"
#define WIFI_CONFIG_URL "/wifilist"
#define WIFI_INDEX_URL "/"

struct wifi_config
{
    uint8_t remember;
    uint8_t connect_wifi;
};

static DNSServer dnsServer;            // dnsServer
static IPAddress apIP(192, 168, 4, 1); // esp8266-AP-IP地址

class WifiHandle
{
private:
    uint8_t configMode;             // 配网模式（0：智能配网，1：网页配网）
    ESP8266WebServer *_server;      // web服务
    const char *AP_NAME = APSSID;   // wifi名字
    static WifiHandle *_singleton;  // 单例对象
    wifi_config config;             // wifi相关配置
    long lastTime = 0;              // 最后更新时间
    void initWebServer();           // 初始化web服务
    static void handleIndex();      // 处理主页请求
    static void handleWifiList();   // wifi列表扫描
    static void handleConfigWifi(); // 配网成功操作
    void saveConfig();              // 记录wifi配网信息
    void readConfig();              // 判断是否已经记住过wifi密码
    void noWifiLoop();              // 没有WiFi的时候的循环
    void (*_loop)();                // 配置网络时的循环
public:
    WifiHandle()
    {
        configMode = 0x01;
        _singleton = this;
        pinMode(WIFI_LED_PIN, OUTPUT);
    }

    WifiHandle(ESP8266WebServer &server)
    {
        configMode = 0x01;
        _singleton = this;
        _server = &server;
        pinMode(WIFI_LED_PIN, OUTPUT);
    }

    void begin();

    void setLoopCallback(void (*loopCallback)());

    void loop();
};

#endif