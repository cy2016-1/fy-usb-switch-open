#ifndef EEPROM_UTIL_H
#define EEPROM_UTIL_H

#include <EEPROM.h>

enum EEPROM_ADDRESS_ENUM
{
    REMEMBER_WIFI = 1,    // 是否记住wifi密码
    SYSTEM_CONFIG = 5,    // 系统配置
    TIME_JOB_CONFIG = 10, // 定时开关设置
    KEY_CONFIG = 25,      // 按键功能设置
};

class EepromUtil
{
private:
public:
    EepromUtil();
    /**
     * @brief 初始化
     *
     */
    static void eeprom_begin();
    /**
     * @brief 加载数据
     */
    static void loadData(int offset, uint8_t *data, int length);
    /**
     * @brief 保存数据
     */
    static void saveData(int offset, uint8_t *data, int length);
    /**
     * @brief 清空所有数据
     *
     */
    static void clearAll();
};
#endif