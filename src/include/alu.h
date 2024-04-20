#pragma once
#include <cstdint>

namespace Breadboard8
{
    class ALU
    {
    public:
        void reset();
        void calc(uint8_t A, uint8_t B, bool substract);
        bool substract = 0;

        union
        {
            uint8_t _8;
            uint16_t _16;
        } sum;
    };
}