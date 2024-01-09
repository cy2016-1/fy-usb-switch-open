#include "KeyHandle.h"

KeyHandle *KeyHandle::_singleton;

void KeyHandle::loadConfig()
{
    EepromUtil::loadData(KEY_CONFIG, (uint8_t *)&config, sizeof(key_config));
    // 如果系统预设配置为空，则使用初始值
    if (config.click_mode == FY_0XFF)
    {
        config.double_click_mode = 0;
        config.click_mode = 0;
        config.click_data = 0;
        config.double_click_data = 0;
    }
}

void KeyHandle::doubleClickCallback()
{
    Serial.println("double click");
    _singleton->keyHandle(_singleton->config.double_click_mode, _singleton->config.double_click_data);
}

void KeyHandle::singleClickCallback()
{
    Serial.println("click");
    _singleton->keyHandle(_singleton->config.click_mode, _singleton->config.click_data);
}

void KeyHandle::longClickCallback()
{
    Serial.println("long click");
    EepromUtil::clearAll();
    ESP.reset(); // 重启设备
}

void KeyHandle::begin()
{
    loadConfig(); // 先加载配置
    btn1.attachDoubleClick(KeyHandle::doubleClickCallback);
    btn1.attachClick(_singleton->singleClickCallback);
}

void KeyHandle::loop()
{
    if (millis() - lastTime >= 20)
    {
        btn1.tick();
        if (btn1.isLongPressed())
        {
            if (millis() - keyTime >= LONG_CLICK_TIME)
            {
                longClickCallback();
            }
        }
        else
        {
            keyTime = millis();
        }
    }
}

void KeyHandle::keyHandle(uint8_t mode, uint32_t data)
{
    switch (mode)
    {
    case 0:
        _systemHandle->setSwitchValue(99);
        return;
    case 1:
        _timeJobHandle->setDownTime(data);
        return;
    case 2:
        Serial.println("not do any thing");
        return;
    default:
        return;
    }
}

// 写入数据回调处理
void KeyHandle::writeHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    if (mode == 0)
    {
        _singleton->config.click_mode = _server->arg(P_CLICK).toInt();
        _singleton->config.click_data = _server->arg(P_CLICK_DATA).toInt();
        _singleton->config.double_click_mode = _server->arg(P_DOUBLE_CLICK).toInt();
        _singleton->config.double_click_data = _server->arg(P_DOUBLE_CLICK_DATA).toInt();
    }
    EepromUtil::saveData(KEY_CONFIG, (uint8_t *)&_singleton->config, sizeof(key_config));
}

// 读取数据回调处理
char *KeyHandle::readHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    char *str = new char[JOSN_SIZE_128];
    if (mode == 0)
    {
        StaticJsonDocument<JOSN_SIZE_128> jsonBuffer;
        jsonBuffer[P_CLICK] = _singleton->config.click_mode;
        jsonBuffer[P_CLICK_DATA] = _singleton->config.click_data;
        jsonBuffer[P_DOUBLE_CLICK] = _singleton->config.double_click_mode;
        jsonBuffer[P_DOUBLE_CLICK_DATA] = _singleton->config.double_click_data;
        serializeJson(jsonBuffer, str, JOSN_SIZE_128);
        jsonBuffer.clear();
    }
    return str;
}