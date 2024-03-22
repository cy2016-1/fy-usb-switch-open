#include "ApiServer.h"
#include "CapacityHandle.h"
#include "KeyHandle.h"
#include "SystemHandle.h"
#include "TimeHandle.h"
#include "TimeJobHandle.h"
#include "WifiHandle.h"

ESP8266WebServer server(80);                                           // 创建dnsServer实例
TimeHandle timehandle = TimeHandle();                                  // 时间处理对象
SystemHandle systemHandle = SystemHandle();                            // 系统设置对象
CapacityHandle capacityHandle = CapacityHandle();                      // 功率信息对象
WifiHandle wifiHandle = WifiHandle(server);                            // wifi 处理对象
TimeJobHandle timeJobHandle = TimeJobHandle(timehandle, systemHandle); // 定时开启关闭对象
KeyHandle keyHandle = KeyHandle(systemHandle, timeJobHandle);          // 按键功能自定义对象
ApiServer apiServer = ApiServer(server);                               // 提供接口服务

#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT
#include <Blinker.h>

/**************** 点灯功能 *******************/
BlinkerButton powerBtn("btn-enable"); // 开关按键
BlinkerButton ledBtn("btn-led");      // 开关按键
BlinkerNumber V("v");                 // 电压
BlinkerNumber A("a");                 // 电流
BlinkerNumber P("p");                 // 功率

/**
 * 数据心跳包
 */
void heartbeat()
{
    V.print(capacityHandle.v); // 给blinkerapp回传数据
    A.print(capacityHandle.a); // 给blinkerapp回传数据
    P.print(capacityHandle.p); // 给blinkerapp回传数据
}

/**
 * 云存储温湿度数据函数
 */
void dataStorage()
{
    Blinker.dataStorage("chat-v", capacityHandle.v); // 存储温度
    Blinker.dataStorage("chat-a", capacityHandle.a); // 存储气压
    Blinker.dataStorage("chat-p", capacityHandle.p); // 存储相对高度
}

/**
   小爱同学接入方法 - 开关功能回调
*/
void powerState(const String &state)
{
    Serial.print("小爱语音设置ENABLE状态为:");
    Serial.println(state);
    uint8_t power = 0;
    if (state == BLINKER_CMD_ON)
    {
        power = 1;
        BlinkerMIOT.powerState("on");
        powerBtn.print("on");
        BlinkerMIOT.print();
    }
    else if (state == BLINKER_CMD_OFF)
    {
        power = 0;
        BlinkerMIOT.powerState("off");
        powerBtn.print("off");
        BlinkerMIOT.print();
    }
    systemHandle.setSwitchValue(power == 1 ? FY_TRUE : FY_FALSE);
}

/**
   小爱同学接入方法 - 开关功能回调
*/
void ledState(const String &state)
{
    Serial.print("设置LED状态为:");
    Serial.println(state);
    uint8_t power = 0;
    if (state == BLINKER_CMD_ON)
    {
        power = 1;
        ledBtn.print("on");
    }
    else if (state == BLINKER_CMD_OFF)
    {
        power = 0;
        ledBtn.print("off");
    }
    systemHandle.setLedValue(power == 1 ? FY_TRUE : FY_FALSE);
}

/**
 * 小爱同学接入方法 - 查询状态
 */
void miotQuery(int32_t queryCode)
{
    Serial.print("小爱语音查询状态代码:");
    Serial.println(queryCode);
}

/**
 * 初始化点灯功能
 */
void initBlinker()
{
    Blinker.begin(systemHandle.auth, WiFi.SSID().c_str(), WiFi.psk().c_str()); // 初始化Blinker(这里只有在连接wifi成功的情况下才能用)
    Blinker.attachHeartbeat(heartbeat);
    Blinker.attachDataStorage(dataStorage);   // 调用云函数
    powerBtn.attach(powerState);              // 注册开关按键
    ledBtn.attach(ledState);                  // 注册开关按键
    BlinkerMIOT.attachPowerState(powerState); // 注册屏幕开关的回调
    BlinkerMIOT.attachQuery(miotQuery);       // 注册小爱同学语音状态查询
}

void powerLoop()
{
    systemHandle.loop();
    keyHandle.loop();
}

void setup()
{
    Serial.begin(115200);                  // 初始化串口
    keyHandle.begin();                     // 按键初始化
    systemHandle.begin();                  // 初始化系统设置
    wifiHandle.setLoopCallback(powerLoop); // 没有WiFi的时候执行的方法付
    wifiHandle.begin();                    // wifi连接
    wifiHandle.loop();                     // 连接WiFi
    timehandle.begin();                    // 初始化时间
    apiServer.begin();                     // 接口服务初始化
    timeJobHandle.begin();                 // 定时开关初始化
    capacityHandle.begin();                // 功率信息初始化
    if (strlen(systemHandle.auth) > 0)
    {
        initBlinker(); // 初始化点灯代码
    }
}

void loop()
{
    timehandle.loop();
    systemHandle.loop();
    timeJobHandle.loop();
    keyHandle.loop();
    apiServer.loop();
    capacityHandle.loop();
    if (strlen(systemHandle.auth) > 0)
    {
        Blinker.run();
    }
}