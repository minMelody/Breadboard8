#include "cpu.h"

void Breadboard8::CPU::reset()
{
    PC = IR = 0x00;
    A = B = OUT = 0x00;
    HALT = OE = ZF = CF = 0;
    alu.reset();
    ram.fill(0x00);
}

void Breadboard8::CPU::tick()
{
    // set emulation flags
    if (ubitset(ucode::HLT)) HALT = true;
    if (HALT) return;
    OE = ubitset(ucode::OI);

    control_word = decode_instruction(ZF << 1 | CF, IR >> 4, step);

    alu.calc(A, B, ubitset(ucode::SU));

    uins_out();
    uins_in();

    // increment counters
    step = (step + 1) % 5;
    if (ubitset(ucode::CE)) PC = (PC + 1) & 0x0F;
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

// All 'resgister out' micro-instructions happen first as they're not tied to the clock
void Breadboard8::CPU::uins_out()
{
    if (ubitset(ucode::CO)) bus = PC;
    if (ubitset(ucode::IO)) bus = IR & 0x0F;
    if (ubitset(ucode::RO)) bus = ram[MAR];
    if (ubitset(ucode::EO)) bus = alu.sum._8;
    if (ubitset(ucode::AO)) bus = A;
}
// All 'register in' micro-instructions are grouped together for ease of access
void Breadboard8::CPU::uins_in()
{
    if (ubitset(ucode::MI)) MAR = bus & 0x0F;
    if (ubitset(ucode::RI)) ram[MAR] = bus;
    if (ubitset(ucode::II)) IR = bus;
    if (ubitset(ucode::BI)) B = bus;
    if (ubitset(ucode::OI)) OUT = bus;
    if (ubitset(ucode::FI)) alu.update_flags(ZF, CF);
    if (ubitset(ucode::AI)) A = bus;
    if (ubitset(ucode::J )) PC = bus;
}
// Test microcode bit
bool Breadboard8::CPU::ubitset(uint16_t ucode)
{
    return (control_word & ucode) > 0;
}