#include "EepromUtil.h"

EepromUtil::EepromUtil()
{
}

void EepromUtil::eeprom_begin()
{
    EEPROM.begin(1024);
}

void EepromUtil::loadData(int offset, uint8_t *data, int length)
{
    for (int i = 0; i < length; i++)
    {
        data[i] = EEPROM.read(offset + i);
    }
}

void EepromUtil::saveData(int offset, uint8_t *data, int length)
{
    for (int i = 0; i < length; i++)
    {
        EEPROM.write(offset + i, data[i] & 0xff);
    }
    EEPROM.commit();
}

void EepromUtil::clearAll()
{
    EEPROM.begin(1024);
    for (int i = 0; i < 4096; i++)
    {
        EEPROM.write(i, 0x0);
    }
    EEPROM.end();
}