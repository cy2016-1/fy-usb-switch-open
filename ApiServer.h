#ifndef API_SERVER_H
#define API_SERVER_H

#include "CapacityHandle.h"
#include "KeyHandle.h"
#include "SystemHandle.h"
#include "TimeJobHandle.h"
#include <ESP8266LLMNR.h>

#define API_INDEX "/"
#define API_WRITE "/usb/write"
#define API_READ "/usb/read"

class ApiServer
{
private:
    static ApiServer *_singleton; // 单例对象
    ESP8266WebServer *_server;    // web服务
    long lastTime = 0;            //
    static void serverIndex();    // 主页
    static void writeHandler();   // 写数据操作主方法
    static void readHandler();    // 读数据操作主方法
    static void serverNotfound(); // 404

public:
    ApiServer()
    {
        _singleton = this;
    }

    ApiServer(ESP8266WebServer &server)
    {
        _singleton = this;
        _server = &server;
    }

    void begin();

    void loop();
};

#endif