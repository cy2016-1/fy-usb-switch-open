#include "TimeJobHandle.h"

TimeJobHandle *TimeJobHandle::_singleton;

void TimeJobHandle::loadConfig()
{
    EepromUtil::loadData(TIME_JOB_CONFIG, (uint8_t *)&config, sizeof(job_config));
    // 如果系统预设配置为空，则使用初始值
    if (config.enable_open == FY_ZERO || config.enable_open == FY_0XFF)
    {
        config.enable_open = FY_FALSE;
        config.enable_close = FY_FALSE;
        config.open_hour = 0;
        config.open_minute = 0;
        config.close_hour = 0;
        config.close_minute = 0;
        config.downTime = 0;
    }
}

void TimeJobHandle::begin()
{
    loadConfig(); // 加载配置
}

void TimeJobHandle::loop()
{
    if (millis() - lastTime >= 1 * 1000) // 一秒钟判断一次
    {
        int hour = _timeHandle->_ntpClient.getHours();
        int minute = _timeHandle->_ntpClient.getMinutes();
        int second = _timeHandle->_ntpClient.getSeconds();
        // 定时开启处理
        if (config.enable_open == FY_TRUE)
        {
            if (config.open_hour == hour && config.open_minute == minute && second <= 1)
            {
                Serial.println("定时开启");
                _systemHandle->setSwitchValue(FY_TRUE);
            }
        }
        // 定时关闭处理
        if (config.enable_close == FY_TRUE)
        {
            if (config.close_hour == hour && config.close_minute == minute && second <= 1)
            {
                Serial.println("定时关闭");
                _systemHandle->setSwitchValue(FY_FALSE);
            }
        }
        // 倒计时处理
        if (config.downTime != 0)
        {
            --config.downTime;
            if (config.downTime == 0)
            {
                _systemHandle->setSwitchValue(99);
            }
        }
        lastTime = millis();
    }
    // 判断功能是否启用，功能不启用这不进入循环
}

void TimeJobHandle::writeHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    if (mode == 0)
    {
        _singleton->config.enable_open = _server->arg(P_OPEN_ENABLE).toInt();
        _singleton->config.enable_close = _server->arg(P_CLOSE_ENABLE).toInt();
        _singleton->config.open_hour = _server->arg(P_OPEN_H).toInt();
        _singleton->config.open_minute = _server->arg(P_OPEN_M).toInt();
        _singleton->config.close_hour = _server->arg(P_CLOSE_H).toInt();
        _singleton->config.close_minute = _server->arg(P_CLOSE_M).toInt();
    }
    else if (mode == 1)
    {
        _singleton->config.downTime = _server->arg(P_CLOSE_H).toInt();
    }
    EepromUtil::saveData(TIME_JOB_CONFIG, (uint8_t *)&_singleton->config, sizeof(job_config));
}

char *TimeJobHandle::readHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    char *str = new char[128];
    if (mode == 0)
    {
        StaticJsonDocument<128> jsonBuffer;
        jsonBuffer[P_OPEN_ENABLE] = _singleton->config.enable_open;
        jsonBuffer[P_CLOSE_ENABLE] = _singleton->config.enable_close;
        jsonBuffer[P_OPEN_H] = _singleton->config.open_hour;
        jsonBuffer[P_OPEN_M] = _singleton->config.open_minute;
        jsonBuffer[P_CLOSE_H] = _singleton->config.close_hour;
        jsonBuffer[P_CLOSE_M] = _singleton->config.close_minute;
        serializeJson(jsonBuffer, str, 128);
        jsonBuffer.clear();
    }
    else if (mode == 1)
    {
        StaticJsonDocument<128> jsonBuffer;
        jsonBuffer[P_CLOSE_H] = _singleton->config.downTime;
        serializeJson(jsonBuffer, str, 128);
        jsonBuffer.clear();
    }
    return str;
}

void TimeJobHandle::setDownTime(uint32_t data)
{
    config.downTime = data;
    EepromUtil::saveData(TIME_JOB_CONFIG, (uint8_t *)&config, sizeof(job_config));
}