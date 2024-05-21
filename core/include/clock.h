#pragma once
#include <chrono>

namespace Breadboard8
{
    class Clock
    {
    public:
        unsigned int frequency {0};
        bool single_step {false};
        Clock(unsigned int hertz);
        // return the time delay between two clock cycles 
        std::chrono::nanoseconds tick();
    };
}
