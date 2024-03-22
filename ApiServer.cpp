#include "ApiServer.h"
#include "WebIndexHtml.h"

ApiServer *ApiServer::_singleton;

void ApiServer::begin()
{
    _singleton->_server->on(API_INDEX, HTTP_GET, serverIndex);  // 设置主页
    _singleton->_server->on(API_WRITE, HTTP_GET, writeHandler); // 写数据接口
    _singleton->_server->on(API_READ, HTTP_GET, readHandler);   // 读数据接口
    _singleton->_server->on(API_FIND, HTTP_GET, findHandler);   // 用于发现设备
    _singleton->_server->onNotFound(serverNotfound);            // 设置404
    _singleton->_server->begin();                               // 服务启动
}

void ApiServer::loop()
{
    _singleton->_server->handleClient();
}

void ApiServer::serverIndex()
{
    _singleton->_server->send(API_OK, API_SERVER_HTML_TEXT, web_index_html);
}

void ApiServer::serverNotfound()
{
    _singleton->_server->send(API_OK, API_SERVER_HTML_TEXT, "404");
}

void ApiServer::findHandler()
{
    _singleton->_server->sendHeader("Access-Control-Allow-Origin", "*", true);
    _singleton->_server->send(API_OK, API_SERVER_HTML_TEXT, "ok");
}

void ApiServer::writeHandler()
{
    int power = _singleton->_server->arg(PARAM_P).toInt();
    switch (power)
    {
    case 0:
        SystemHandle::_singleton->writeHandler(_singleton->_server);
        break;
    case 1:
        // 这里暂时没有任何写操作，不做任何处理先
        break;
    case 2:
        TimeJobHandle::_singleton->writeHandler(_singleton->_server);
        break;
    case 3:
        KeyHandle::_singleton->writeHandler(_singleton->_server);
        break;
    default:
        break;
    }
    _singleton->_server->send(API_OK, API_SERVER_HTML_TEXT, HTTP_OK);
}

void ApiServer::readHandler()
{
    int power = _singleton->_server->arg(PARAM_P).toInt();
    char *data;
    switch (power)
    {
    case 0:
        data = SystemHandle::_singleton->readHandler(_singleton->_server);
        break;
    case 1:
        data = CapacityHandle::_singleton->readHandler(_singleton->_server);
        break;
    case 2:
        data = TimeJobHandle::_singleton->readHandler(_singleton->_server);
        break;
    case 3:
        data = KeyHandle::_singleton->readHandler(_singleton->_server);
        break;
    }
    _singleton->_server->send(API_OK, API_SERVER_HTML_TEXT, data);
    free(data);
}