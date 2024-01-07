#ifndef _KEY_HANDLE_H_
#define _KEY_HANDLE_H_

#include "Constant.h"
#include "EepromUtil.h"
#include "SystemHandle.h"
#include "TimeJobHandle.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <OneButton.h>

#define LONG_CLICK_TIME 10000 // 长按按键时间

#define SWITCH_BTN 16
#define P_CLICK "click"
#define P_DOUBLE_CLICK "doubleClick"
#define P_CLICK_DATA "clickData"
#define P_DOUBLE_CLICK_DATA "doubleClickData"

struct key_config
{
    uint8_t click_mode;         // 单击按键模式：0：开关，1: 延时开关，2：啥也不做，3：
    uint8_t double_click_mode;  // 双击按键模式：0：开关，1: 延时开关，2：啥也不做，3：
    uint32_t click_data;        // 单击按键需要用到的数据
    uint32_t double_click_data; // 单击按键需要用到的数据
};

class KeyHandle
{
private:
    static KeyHandle *_singleton;                        // 单例对象
    key_config config;                                   // 系统参数配置
    void loadConfig();                                   // 读取eeprom配置
    OneButton btn1 = OneButton(SWITCH_BTN, true, false); // 获取扭对象
    SystemHandle *_systemHandle;                         // 操作开关的对象
    TimeJobHandle *_timeJobHandle;                       // 定时关闭对象
    long lastTime;                                       // 最后更新时间
    long keyTime;                                        // 按键按下时间
    static void singleClickCallback();                   // 单击回调事件
    static void doubleClickCallback();                   // 按键双击事件
    static void longClickCallback();                     // 长按按键事件

public:
    KeyHandle(SystemHandle &systemHandle, TimeJobHandle &timeJobHandle)
    {
        _singleton = this;
        _systemHandle = &systemHandle;
        _timeJobHandle = &timeJobHandle;
    }

    void begin();

    void loop();

    // 写入数据回调处理
    static void writeHandler(ESP8266WebServer *_server);
    // 读取数据回调处理
    static char *readHandler(ESP8266WebServer *_server);
    // 按键事件处理
    void keyHandle(uint8_t mode, uint32_t data);
};

#endif