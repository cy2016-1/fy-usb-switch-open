#include "CapacityHandle.h"

CapacityHandle *CapacityHandle::_singleton;

void CapacityHandle::begin()
{
    Wire.begin();
    ina.setMaxCurrentShunt(8, 0.01);
}

void CapacityHandle::loop()
{
    _singleton = this;
}

char *CapacityHandle::readHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    char *stateStr = new char[JOSN_SIZE_2048];
    if (mode == 0)
    {
        float V = _singleton->ina.getBusVoltage();       // VBUS电压V
        float mV = _singleton->ina.getShuntVoltage_mV(); // 采样电阻分压
        float power = _singleton->ina.getPower();        // 分压电压
        float A = mV / 10;                               // 采样电阻分压/采样电阻值 = 电流
        StaticJsonDocument<JOSN_SIZE_64> jsonBuffer;
        jsonBuffer["v"] = V;
        jsonBuffer["a"] = A;
        jsonBuffer["p"] = power;
        serializeJson(jsonBuffer, stateStr, JOSN_SIZE_64);
        jsonBuffer.clear();
    }
    return stateStr;
}