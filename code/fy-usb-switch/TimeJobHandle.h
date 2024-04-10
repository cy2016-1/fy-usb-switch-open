#ifndef TIME_JOB_HANDLE_H
#define TIME_JOB_HANDLE_H

#include "Constant.h"
#include "EepromUtil.h"
#include "SystemHandle.h"
#include "TimeHandle.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

struct job_config
{
    uint8_t enable_open;  // 是否启用定时开启功能
    uint8_t enable_close; // 是否启用定时关闭功能
    uint8_t open_hour;    // 定时开启时钟
    uint8_t open_minute;  // 定时开启分钟
    uint8_t close_hour;   // 定时关闭时钟
    uint8_t close_minute; // 定时关闭分钟
    uint32_t downTime;    // 倒计时
};

#define P_OPEN_ENABLE "openEnable"
#define P_CLOSE_ENABLE "closeEnable"
#define P_OPEN_H "openHour"
#define P_OPEN_M "openMinute"
#define P_CLOSE_H "closeHour"
#define P_CLOSE_M "closeMinute"
#define P_CLOSE_S "closeSecond"

class TimeJobHandle
{
private:
    job_config config;           // 系统参数配置
    void loadConfig();           // 读取eeprom配置
    long lastTime;               // 最后更新时间
    TimeHandle *_timeHandle;     // 时间对象
    SystemHandle *_systemHandle; // 开关设置

public:
    static TimeJobHandle *_singleton; // 单例对象
    TimeJobHandle(TimeHandle &timeHandle, SystemHandle &systemHandle)
    {
        _timeHandle = &timeHandle;
        _systemHandle = &systemHandle;
        _singleton = this;
    }

    void begin();

    void loop();

    static void writeHandler(ESP8266WebServer *_server); // 写入数据回调处理

    static char *readHandler(ESP8266WebServer *_server); // 读取数据回调处理

    void setDownTime(uint32_t data); // 设置倒计时
};

#endif