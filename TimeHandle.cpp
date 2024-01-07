#include "TimeHandle.h"

void TimeHandle::begin()
{
    _ntpClient.begin();
}

void TimeHandle::loop()
{
    // 同步时间
    _ntpClient.update();
    if (millis() - lastUpdateTime >= 60 * 1000) // 每分钟打印一次时间
    {
        printf("time=%s\n", _ntpClient.getFormattedTime().c_str());
        lastUpdateTime = millis();
    }
}