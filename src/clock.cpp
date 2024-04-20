#include "clock.h"


std::chrono::milliseconds Breadboard8::Clock::tick()
{
    return std::chrono::milliseconds(1000 / frequency);
}

Breadboard8::Clock::Clock(unsigned int hertz)
{
    frequency = hertz;
}
