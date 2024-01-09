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

void setup()
{
    Serial.begin(115200);       // 初始化串口
    EepromUtil::eeprom_begin(); // 初始化eeprom
    keyHandle.begin();          // 按键初始化
    wifiHandle.begin();         // wifi连接
    wifiHandle.loop();          // 连接WiFi
    timehandle.begin();         // 初始化时间
    systemHandle.begin();       // 初始化系统设置
    apiServer.begin();          // 接口服务初始化
    timeJobHandle.begin();      // 定时开关初始化
    capacityHandle.begin();     // 功率信息初始化
    
}

void loop()
{
    timehandle.loop();
    systemHandle.loop();
    timeJobHandle.loop();
    keyHandle.loop();
    apiServer.loop();
}