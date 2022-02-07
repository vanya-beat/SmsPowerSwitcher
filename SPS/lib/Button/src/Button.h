#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Arduino.h"

class Button
{
public:
    Button(byte pin);

    bool click();

private:
    byte _pin;
    uint32_t _tmr;
    bool _flag;
};

#endif //_BUTTON_H_
