#pragma once
#include <utility>
#include <vector>
#include <cstdint>
#include <ncurses.h>

#include "cpu.h"
#include "clock.h"

namespace Breadboard8::UI
{
    // copy the value of the memory address register to clear pointer character later
    extern uint8_t lastMAR;
    // user input
    extern bool editing_ram;
    void edit_ram(Breadboard8::CPU *cpu);
    // cli
    extern std::vector<std::pair<WINDOW*, const char*>> board;
    void init(Breadboard8::CPU *cpu, std::string prg_path);
    void update(Breadboard8::CPU *cpu, Breadboard8::Clock *clk);
    void set_footer(const char* str);
    void close();
}
