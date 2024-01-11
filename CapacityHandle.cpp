#include "CapacityHandle.h"

CapacityHandle *CapacityHandle::_singleton;

void CapacityHandle::begin()
{
    Wire.begin();
    ina.setMaxCurrentShunt(8, 0.01);
}

void CapacityHandle::loop()
{
    if (millis() - lastTime >= 1000)
    {
        lastTime = millis();
        v = ina.getBusVoltage();             // VBUS电压V
        float mV = ina.getShuntVoltage_mV(); // 采样电阻分压
        a = mV / 10;                         // 采样电阻分压/采样电阻值 = 电流
        p = ina.getPower();                  // 分压电压
    }
}

char *CapacityHandle::readHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    char *stateStr = new char[JOSN_SIZE_2048];
    if (mode == 0)
    {
        StaticJsonDocument<JOSN_SIZE_64> jsonBuffer;
        jsonBuffer["v"] = _singleton->v;
        jsonBuffer["a"] = _singleton->a;
        jsonBuffer["p"] = _singleton->p;
        serializeJson(jsonBuffer, stateStr, JOSN_SIZE_64);
        jsonBuffer.clear();
    }
    return stateStr;
}