#include "alu.h"

void Breadboard8::ALU::reset()
{
    sum._16 = 0x00;
}

void Breadboard8::ALU::calc(uint8_t A, uint8_t B, bool substract)
{
    if (substract) B = ~B;
    sum._16 = A + B + substract;
}

void Breadboard8::ALU::update_flags(bool &ZF, bool &CF)
{
    ZF = sum._8 == 0x00;
    CF = (sum._16 & 0x100) > 0;
}
