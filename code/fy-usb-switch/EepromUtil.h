#ifndef EEPROM_UTIL_H
#define EEPROM_UTIL_H

#include <EEPROM.h>

#define EEPROM_BEGIN 1536

enum EEPROM_ADDRESS_ENUM
{
    REMEMBER_WIFI = EEPROM_BEGIN + 1,    // 是否记住wifi密码
    SYSTEM_CONFIG = EEPROM_BEGIN + 5,    // 系统配置
    SYSTEM_AUTH = EEPROM_BEGIN + 15,     // 点灯的auth
    TIME_JOB_CONFIG = EEPROM_BEGIN + 50, // 定时开关设置
    KEY_CONFIG = EEPROM_BEGIN + 75,      // 按键功能设置
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
     * @brief 保存数据
     */
    static void saveStrData(int offset, const char *str, int length);
    /**
     * @brief 清空字符串
     */
    static void clearStrData(int offset, int length);
    /**
     * @brief 加载数据
     */
    static void loadStrData(int offset, char *str, int length);
    /**
     * @brief 清空所有数据
     *
     */
    static void clearAll();
};
#endif