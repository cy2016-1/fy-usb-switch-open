#include "EepromUtil.h"
#include <Arduino.h>

EepromUtil::EepromUtil()
{
}

void EepromUtil::eeprom_begin()
{
    EEPROM.begin(4096);
}

void EepromUtil::loadData(int offset, uint8_t *data, int length)
{
    EEPROM.begin(4096);
    for (int i = 0; i < length; i++)
    {
        data[i] = EEPROM.read(offset + i);
    }
}

void EepromUtil::saveData(int offset, uint8_t *data, int length)
{
    EEPROM.begin(4096);
    for (int i = 0; i < length; i++)
    {
        EEPROM.write(offset + i, data[i] & 0xff);
    }
    EEPROM.commit();
}

void EepromUtil::clearAll()
{
    EEPROM.begin(4096);
    for (int i = 0; i < 4096; i++)
    {
        EEPROM.write(EEPROM_BEGIN + i, 0x0);
    }
    EEPROM.end();
}

void EepromUtil::clearStrData(int offset, int length)
{
    EEPROM.begin(4096);
    for (int i = 0; i < length; i++)
    {
        EEPROM.write(offset + i, 0x0);
    }
    EEPROM.commit();
}

void EepromUtil::saveStrData(int offset, const char *str, int length)
{
    EEPROM.begin(4096);
    for (int i = 0; i < length; i++)
    {
        uint8_t data = ((uint8_t)str[i]) & 0xff;
        EEPROM.write(offset + i, data);
    }
    EEPROM.commit();
}

void EepromUtil::loadStrData(int offset, char *str, int length)
{
    EEPROM.begin(4096);
    for (int i = 0; i < length; i++)
    {
        char data = (char)EEPROM.read(offset + i);
        str[i] = data;
    }
}