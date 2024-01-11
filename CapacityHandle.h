#ifndef CAPACITY_HANDLE_H
#define CAPACITY_HANDLE_H

#include "Constant.h"
#include "TimeHandle.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <INA226.h>
#include <Wire.h>

#define INA226_I2C_ADDR 0x40

struct capacity_config
{
    uint8_t a;
};

class CapacityHandle
{
private:
    capacity_config config;               // 配置
    INA226 ina = INA226(INA226_I2C_ADDR); // INA对象
    long lastTime = 0;

public:
    static CapacityHandle *_singleton; // 单例对象
    CapacityHandle()
    {
        _singleton = this;
    }

    float v;
    float a;
    float p;

    void begin();

    void loop();

    static char *readHandler(ESP8266WebServer *_server);
};

#endif