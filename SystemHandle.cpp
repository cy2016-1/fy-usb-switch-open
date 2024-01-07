#include "SystemHandle.h"

SystemHandle *SystemHandle::_singleton;

void SystemHandle::begin()
{
    pinMode(SWITCH_PIN, OUTPUT);   // 设置引脚为输出模式
    pinMode(WIFI_LED_PIN, OUTPUT); // 设置引脚为输出模式
    pinMode(G_LED_PIN, OUTPUT);    // 设置引脚为输出模式
    loadConfig();                  // 先读取系统预设配置
    sysHandle();                   // 处理系统设置
}

void SystemHandle::loop()
{
    if (millis() - lastTime >= 20)
    {
        sysHandle(); // 处理系统设置
        lastTime = millis();
    }
}

void SystemHandle::sysHandle()
{
    digitalWrite(SWITCH_PIN, (config.enable == FY_FALSE) ? LOW : HIGH);
    if (config.enableLed == FY_FALSE)
    {
        digitalWrite(G_LED_PIN, HIGH);
        digitalWrite(WIFI_LED_PIN, HIGH);
        return;
    }
    else
    {
        if (config.enable == FY_FALSE)
        {
            // 不工作的时候关灯
            digitalWrite(G_LED_PIN, HIGH);
        }
        else
        {
            // 工作的时候开灯
            digitalWrite(G_LED_PIN, LOW);
        }
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        // 只有WiFi已经连接了才能开关wifi指示灯
        digitalWrite(WIFI_LED_PIN, config.enableLed == FY_FALSE ? HIGH : LOW);
    }
}

void SystemHandle::setSwitchValue(int data)
{
    config.enable = (data == 99) ? config.enable == FY_TRUE ? FY_FALSE : FY_TRUE : data;
    EepromUtil::saveData(SYSTEM_CONFIG, (uint8_t *)&config, sizeof(sys_config));
}

void SystemHandle::writeHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    switch (mode)
    {
    case 0:
        _singleton->config.enable = _server->arg(PARAM_ENABLE).toInt();
        _singleton->config.enableLed = _server->arg(PARAM_ENABLE_LED).toInt();
        break;
    case 98:
        // 这里执行系统重启
        ESP.restart();
        break;
    case 99:
        // 这里执行恢复出厂设置
        EepromUtil::clearAll();
        ESP.restart();
        break;
    default:
        break;
    }
    EepromUtil::saveData(SYSTEM_CONFIG, (uint8_t *)&_singleton->config, sizeof(sys_config));
}

char *SystemHandle::readHandler(ESP8266WebServer *_server)
{
    int mode = _server->arg(PARAM_M).toInt();
    char *str = new char[128];
    if (mode == 0)
    {
        StaticJsonDocument<128> jsonBuffer;
        jsonBuffer[PARAM_ENABLE] = _singleton->config.enable;
        jsonBuffer[PARAM_ENABLE_LED] = _singleton->config.enableLed;
        jsonBuffer[PARAM_VERSION] = VERSION;
        serializeJson(jsonBuffer, str, 128);
        jsonBuffer.clear();
    }
    return str;
}

void SystemHandle::loadConfig()
{
    EepromUtil::loadData(SYSTEM_CONFIG, (uint8_t *)&config, sizeof(sys_config));
    // 如果系统预设配置为空，则使用初始值
    if (config.enable == FY_ZERO || config.enable == FY_0XFF)
    {
        config.enable = FY_TRUE;
        config.enableLed = FY_TRUE;
    }
}
