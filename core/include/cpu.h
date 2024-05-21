#pragma once
#include <array>
#include <string>
#include "alu.h"

namespace Breadboard8
{
    class CPU
    {
    public:
        void reset();
        // Step one clock cycle
        void tick();

        ALU alu;
        uint8_t bus; // The cpu's internal data bus
        std::array<uint8_t, 16> ram;
        std::array<uint8_t, 2048> rom;

        // Registers
        uint8_t A, B, OUT;
        uint8_t MAR;        // memory address register
        uint8_t IR;         // instruction register
        uint8_t PC;    // program counter

        // CPU flags
        bool ZF, CF;
        // Emulation flags
        bool HALT, OE;
        
        // Control logic
        #include "microcode.h"
        uint16_t control_word;
        uint8_t step;
        // Takes in the 2 bits flags register, 4 bits opcode and 3 bits step counter.
        // Returns the control word stored at the corresponding address.
        uint16_t decode_instruction(uint8_t flags, uint8_t opcode, uint8_t step);

    protected:
        void uins_out(), uins_in();
        // Test microcode bit
        bool ubittest(uint16_t code);
    };
}
