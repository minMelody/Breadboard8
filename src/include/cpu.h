#pragma once
#include <cstdint>
#include <array>

#include "alu.h"

namespace Breadboard8
{
    class CPU // is really just the breadboard's bus
    {
    public:
        void reset();
        void tick();

        ALU alu;
        uint8_t bus; // The cpu's internal data bus
        std::array<uint8_t, 16> ram;
        std::array<uint8_t, 2048> rom;

        // Registers
        uint8_t A, B, OUT;
        uint8_t MAR; // memory address register
        uint8_t IR;  // instruction register
        int PC;      // program counter

        // Flags
        bool ZF, CF, HALT;
        
        // Control logic
        uint16_t decode_instruction(uint8_t opcode);
        uint8_t step;
        uint16_t control_word;
        #include "microcode.h"

    protected:
        void uins_out(), uins_in();
        // Test microcode bit
        bool ubitset(uint16_t code);
    };
}