#pragma once
#include <chrono>

namespace Breadboard8
{
    class Clock
    {
    public:
        unsigned int frequency;
        Clock(unsigned int hertz);
        std::chrono::milliseconds tick();
    };
}
