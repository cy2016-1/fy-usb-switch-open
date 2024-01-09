#ifndef SYSTEM_HANDLE_H
#define SYSTEM_HANDLE_H

#include "Constant.h"
#include "EepromUtil.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#define VERSION "v0.0.1"
#define PARAM_ENABLE "enable"
#define PARAM_ENABLE_LED "enableLed"
#define PARAM_VERSION "version"
#define SWITCH_PIN 14
#define G_LED_PIN 12
#define WIFI_LED_PIN 2

struct sys_config
{
    uint8_t enable;    // 是否启用
    uint8_t enableLed; // 启用LED
};

class SystemHandle
{
private:
    sys_config config; // 系统参数配置
    long lastTime = 0; // 刷新配置
    void loadConfig(); // 读取eeprom配置

public:
    static SystemHandle *_singleton; // 单例对象
    SystemHandle()
    {
        _singleton = this;
    }

    void begin();

    void loop();

    void sysHandle();

    void setSwitchValue(int data);

    static void writeHandler(ESP8266WebServer *_server);

    static char *readHandler(ESP8266WebServer *_server);
};

#endif