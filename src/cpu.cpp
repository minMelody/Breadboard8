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
    OE = false;
    if (ubitset(ucode::HLT))
    {
        HALT = true;
        return;
    }

    control_word = decode_instruction(IR >> 4);

    alu.calc(A, B, ubitset(ucode::SU));

    uins_out();
    uins_in();

    // increment counters
    step = (step + 1) % 5;
    if (ubitset(ucode::CE)) PC = (PC + 1) % 0x10;
}

uint16_t Breadboard8::CPU::decode_instruction(uint8_t opcode)
{
    uint8_t index = (opcode << 3) | step;
    uint8_t cw_h = rom[index];
    uint8_t cw_l = rom[index + 0x80];
    return (cw_h << 8) | cw_l;
}

// all output micro instructions happen first
void Breadboard8::CPU::uins_out()
{
    if (ubitset(ucode::CO)) bus = PC;
    if (ubitset(ucode::IO)) bus = IR & 0x0F;
    if (ubitset(ucode::RO)) bus = ram[MAR];
    if (ubitset(ucode::EO)) bus = alu.sum._8;
    if (ubitset(ucode::AO)) bus = A;
}

void Breadboard8::CPU::uins_in()
{
    if (ubitset(ucode::MI)) MAR = bus & 0x0F;
    if (ubitset(ucode::RI)) ram[MAR] = bus;
    if (ubitset(ucode::II)) IR = bus;
    if (ubitset(ucode::BI)) B = bus;
    if (ubitset(ucode::OI))
    {
        OUT = bus;
        OE = true;
    }
    if (ubitset(ucode::FI))
    {
        CF = (alu.sum._16 & 0x100) > 0;
        ZF = alu.sum._8 == 0x00;
    }
    if (ubitset(ucode::AI)) A = bus;
    if (ubitset(ucode::J )) PC = bus;
}

// Test microcode bit
bool Breadboard8::CPU::ubitset(uint16_t ucode)
{
    return (control_word & ucode) > 0;
}