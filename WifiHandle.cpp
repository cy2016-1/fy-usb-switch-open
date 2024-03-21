#include "WifiHandle.h"
#include "ConfigWifiHtml.h"

WifiHandle *WifiHandle::_singleton;

void WifiHandle::loop()
{
    // 如果已经连接了wifi，则不执行下面的任务
    while (WiFi.status() != WL_CONNECTED)
    {
        _singleton->_server->handleClient();
        dnsServer.processNextRequest();
        if (millis() - lastTime >= 200)
        {
            digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN)); // LED 闪烁
            lastTime = millis();
            _loop();
        }
    }
}

void WifiHandle::saveConfig()
{
    EepromUtil::saveData(REMEMBER_WIFI, (uint8_t *)&config, sizeof(wifi_config));
}

void WifiHandle::readConfig()
{
    EepromUtil::loadData(REMEMBER_WIFI, (uint8_t *)&config, sizeof(wifi_config));
    if (config.remember == FY_ZERO || config.remember == FY_0XFF)
    {
        config.connect_wifi = FY_FALSE;
        config.remember = FY_FALSE;
    }
}

void WifiHandle::initWebServer()
{
    WiFi.mode(WIFI_AP_STA);                                             // 设置模式为wifi热点模式
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));         // 初始化AP模式
    WiFi.softAP(AP_NAME, NULL, 1, 0, 4);                                // 初始化AP模式
    _singleton->_server->on(WIFI_INDEX_URL, HTTP_GET, handleIndex);     // 设置主页回调函数
    _singleton->_server->on(WIFI_LIST_URL, HTTP_GET, handleConfigWifi); // 设置Post请求回调函数
    _singleton->_server->on(WIFI_CONFIG_URL, HTTP_GET, handleWifiList); // 设置获取wifi列表回调函数
    _singleton->_server->onNotFound(handleIndex);                       // 设置无法响应的http请求的回调函数
    _singleton->_server->begin();                                       // 启动WebServer
    dnsServer.start(53, "*", apIP);                                     // DNS服务器发现
}

void WifiHandle::setLoopCallback(void (*loopCallback)())
{
    _loop = loopCallback;
}

void WifiHandle::noWifiLoop()
{
    int t = millis();
    while (1)
    {
        if (millis() - t >= 100)
        {
            break;
        }
        if (_loop != NULL)
        {
            _loop();
        }
    }
}

void WifiHandle::begin()
{
    readConfig();                                                     // 先读取wifi配置信息
    digitalWrite(WIFI_LED_PIN, HIGH);                                 // WIFI连接指示灯不显示
    if (config.remember == FY_TRUE && config.connect_wifi == FY_TRUE) // 判断wifi是否连接
    {
        int timer = 0;
        WiFi.mode(WIFI_STA);
        WiFi.begin();
        while (WiFi.status() != WL_CONNECTED)
        {
            timer++;
            _singleton->noWifiLoop();
            if (timer >= 30) // 如果计数器大于60次,表示超过一分钟,则说明一分钟都没有连接上wifi,就不连了
            {
                timer = 0;                                  // 清零计数器
                _singleton->config.connect_wifi = FY_FALSE; // 重启设备之前将设备模式修改为热点模式
                _singleton->saveConfig();                   // 标记wifi是已经连接
                ESP.restart();                              // 重启设备
            }
        }
        Serial.println(WiFi.localIP());
        WiFi.setAutoConnect(true);       // 设置自动连接
        digitalWrite(WIFI_LED_PIN, LOW); // 将WiFi指示灯设置为常亮
    }
    else
    {
        // 进入配网模式以后，就需要立马改成wifi模式，避免下次上电的时候直接进入到配网模式
        config.connect_wifi = FY_TRUE;
        saveConfig();
        // wifi没有连接则进行配网操作
        if (configMode == 0x00)
        {
            // 暂不支持智能配网，体验实在是太差了
            initWebServer();
        }
        else
        {
            Serial.println("网页配网");
            initWebServer();
        }
    }
}

void WifiHandle::handleIndex()
{
    _singleton->_server->send(200, API_SERVER_HTML_TEXT, page_html);
}

void WifiHandle::handleWifiList()
{
    int n = WiFi.scanNetworks(); // 开始同步扫描，将返回值存放在变量n中
    if (n > 0)                   // 只有有数据的时候才处理
    {
        char *wifilist = new char[640]; // 返回给网页的数据
        Serial.println("sacn wifi .");
        for (int i = 0; i < n; i++) // 开始逐个打印扫描到的
        {
            Serial.println(WiFi.SSID(i).c_str());                             // 打印wifi列表信息
            sprintf(wifilist, "%s%s%s", wifilist, WiFi.SSID(i).c_str(), ","); // 组装信息返回给接口
        }
        _singleton->_server->send(200, API_SERVER_HTML_TEXT, wifilist); // 返回保存成功页面
        free(wifilist);                                                 // 清理内存
        return;                                                         // 结束这里的操作
    }
    Serial.println("no scan any wifi list");                         // 打印没有任何wifi日志
    _singleton->_server->send(200, API_SERVER_HTML_TEXT, ".nodata"); // 返回保存成功页面
}

void WifiHandle::handleConfigWifi()
{
    Serial.println(_singleton->_server->arg(WIFI_SSID).c_str());
    Serial.println(_singleton->_server->arg(WIFI_PWD).c_str());
    WiFi.persistent(true);                                                                               // 首次在flash中保存WiFi配置-可选
    WiFi.begin(_singleton->_server->arg(WIFI_SSID).c_str(), _singleton->_server->arg(WIFI_PWD).c_str()); // 使用配网获取的wifi信息
    int count = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        _singleton->noWifiLoop();
        count++;
        if (count > 6) // 如果20秒内没有连上，就开启Web配网 可适当调整这个时间
        {
            _singleton->_server->send(200, API_SERVER_HTML_TEXT, WIFI_CONN_FAIL_TIP); // 返回保存成功页面
            break;                                                                    // 跳出 防止无限初始化
        }
        Serial.println(".");
    }
    if (WiFi.status() == WL_CONNECTED) // 如果连接上 就输出IP信息
    {
        WiFi.setAutoConnect(true);                                                // 设置自动连接
        _singleton->config.connect_wifi = FY_TRUE;                                //
        _singleton->config.remember = FY_TRUE;                                    //
        _singleton->saveConfig();                                                 // 标记wifi是已经连接
        Serial.print("WIFI Connected:");                                          // 打印esp8266的IP地址
        Serial.println(WiFi.localIP());                                           // 打印esp8266的IP地址
        _singleton->_server->send(200, API_SERVER_HTML_TEXT, WIFI_CONN_SUCC_TIP); // 返回保存成功页面
        delay(1000);
        ESP.restart();
    }
}
