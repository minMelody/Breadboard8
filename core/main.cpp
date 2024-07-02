#include <iostream>
#include <sstream>
#include <thread>
#include <fstream>
#include <cstring>
#include <algorithm>

//#include "cpu.h"
//#include "clock.h"
#include "ui.h"

template<std::size_t _size>
void read_binary(std::array<uint8_t, _size>& dest, std::string path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (f.is_open())
    {
        std::size_t filesize = f.tellg();
        std::size_t romsize = (filesize < _size) ? filesize : _size;
        char buffer[romsize];

        f.seekg(0, std::ios_base::beg);
        f.read(&buffer[0], romsize);
        f.close();

        memcpy(dest.begin(), buffer, romsize);
        return;
    }

    std::cout << "Unable to read binary file." << std::endl;
    exit(EXIT_FAILURE);
}

bool Breadboard8::pause {};
bool Breadboard8::running {};
bool Breadboard8::reset {};

using namespace Breadboard8;
void input_loop(Breadboard8::Clock *clk)
{
    char c;
    while (running)
    {
        if (UI::editing_ram) continue; // main thread will handle ram editing
        switch (c = getchar())
        {
        case 'e':
        case 'E':
            UI::editing_ram = true;
            break;
        case 's':
        case 'S':
            clk->single_step = !clk->single_step;
            pause = clk->single_step;
            break;
        case 'i':
        case 'I':
            clk->frequency = (++clk->frequency > 999) ? 999 : clk->frequency;
            break;
        case 'd':
        case 'D':
            clk->frequency = (--clk->frequency < 1) ? 1 : clk->frequency;
            break;
        case 'k':
        case 'K':
            running = false;
            /* fall through */
        case 'r':
        case 'R':
            reset = true;
            /* fall through */
        default:
            pause = false;
            break;
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        std::cout << "usage: ./bb8 <program> <speed> <(opt) rom>\n"
                  << "<program>   - compiled binary file\n"
                  << "<speed>     - clock speed in hertz\n"
                  << "<(opt) rom> - ROM image containing microprogram, loads 'rom.out' by default"
                  << std::endl;
        return EXIT_FAILURE;
    }

#define OUT_BUFFER_LENGTH 16
    // allocate a bit more space than needed just to be safe
    char out_buffer[OUT_BUFFER_LENGTH];
    // write to this stream instead of stdout, or it will mess with ncurses screen
    std::stringstream out_stream {};

    std::string  _PROGRAM = argv[1];
    int _CLKSPD  = std::stoi(argv[2]);
    std::string  _ROMFILE = (argc == 4) ? argv[3] : "rom.out";

    Clock clk {_CLKSPD};
    CPU cpu;
    read_binary(cpu.rom, _ROMFILE);
    read_binary(cpu.ram, _PROGRAM);

    running = true;
    UI::init(&cpu, _PROGRAM);
    std::thread input_tread (input_loop, &clk);

    if (clk.frequency < 1) clk.frequency = pause = clk.single_step = 1;

cpu_reset:
    reset = false;
    cpu.reset();

    while (running)
    {
        if (reset) goto cpu_reset;

        if (UI::editing_ram) UI::edit_ram(&cpu);

        if (pause)
        {
            UI::set_footer("--Press any key to continue--");
            UI::update(&cpu, &clk);
            continue;
        }

        cpu.tick();
        if (cpu.OE)
        {
            memset(out_buffer, '\0', sizeof(char) * OUT_BUFFER_LENGTH); // make sure buffer is empty
            sprintf(out_buffer, "\n%i  %c%c", cpu.OUT, cpu.CF ? 'C' : '.', cpu.ZF ? 'Z' : '.');
            out_stream << out_buffer;
        }

        UI::set_footer("");
        UI::update(&cpu, &clk);

        if (clk.single_step || cpu.HALT) pause = true;
        else std::this_thread::sleep_for(clk.tick());
    }

    UI::close();
    input_tread.join();
    // ncurses is closed
    // we can now write the stream to cout without creating a mess
    std::cout << out_stream.str() << std::endl;
    return EXIT_SUCCESS;
}
