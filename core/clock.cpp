#include "clock.h"


std::chrono::nanoseconds Breadboard8::Clock::tick()
{
    return std::chrono::nanoseconds(1000000000 / frequency);
}

Breadboard8::Clock::Clock(unsigned int hertz)
{
    frequency = hertz;
}
