#include "cpu.h"

void Breadboard8::CPU::reset()
{
    PC = IR = 0x00;
    A = B = OUT = 0x00;
    HALT = OE = ZF = CF = 0;
    control_word = 0;
    step = 4; // initialize step counter to overflow to 0 the first time tick() is called
    alu.reset();
}

void Breadboard8::CPU::tick()
{
    step = ++step % 5;
    control_word = decode_instruction(ZF << 1 | CF, IR >> 4, step);

    if (HALT = ubittest(ucode::HLT)) return;
    OE = ubittest(ucode::OI);

    alu.calc(A, B, ubittest(ucode::SU));

    uins_out();
    uins_in();

    if (ubittest(ucode::CE)) PC = ++PC & 0x0F;
}

// Takes in the 2 bits flags register, 4 bits opcode and 3 bits step counter.
// Returns the control word stored at the corresponding address.
uint16_t Breadboard8::CPU::decode_instruction(uint8_t flags, uint8_t opcode, uint8_t step)
{
    uint16_t address = (flags << 8) | (opcode << 3) | (step & 0b0111);

    uint8_t hi = rom[address];
    uint8_t lo = rom[address | 0x80];
    return (hi << 8) | lo;
}

// All register out micro-instructions happen first as they're not tied to the clock
void Breadboard8::CPU::uins_out()
{
    if (ubittest(ucode::CO)) bus = PC;
    if (ubittest(ucode::IO)) bus = IR & 0x0F;
    if (ubittest(ucode::RO)) bus = ram[MAR];
    if (ubittest(ucode::EO)) bus = alu.sum._8;
    if (ubittest(ucode::AO)) bus = A;
}
// All register in micro-instructions are grouped together to make them easier to edit
void Breadboard8::CPU::uins_in()
{
    if (ubittest(ucode::MI)) MAR = bus & 0x0F;
    if (ubittest(ucode::RI)) ram[MAR] = bus;
    if (ubittest(ucode::II)) IR = bus;
    if (ubittest(ucode::BI)) B = bus;
    if (ubittest(ucode::OI)) OUT = bus;
    if (ubittest(ucode::FI)) alu.update_flags(ZF, CF);
    if (ubittest(ucode::AI)) A = bus;
    if (ubittest(ucode::J )) PC = bus;
}
// Test microcode bit
bool Breadboard8::CPU::ubittest(uint16_t ucode)
{
    return (control_word & ucode) > 0;
}
