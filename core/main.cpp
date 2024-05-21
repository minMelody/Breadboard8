#include <iostream>
#include <thread>
#include <fstream>
#include <cstring>

#include "cpu.h"
#include "clock.h"

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

using namespace Breadboard8;
int main(int argc, const char* argv[])
{
    if (argc < 3 || std::stoi(argv[2]) == 0)
    {
        std::cout << "usage: ./bb8 <program> <speed> [rom]\n"
                  << "<program> - compiled binary file\n"
                  << "<speed>   - clock speed in hertz\n"
                  << "[rom]     - ROM image containing microprogram, loads 'rom.out' by default"
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::string  _PROGRAM = argv[1];
    unsigned int _CLKSPD  = std::stoi(argv[2]);
    std::string  _ROMFILE = (argc == 4) ? argv[3] : "rom.out";

    Clock clk {_CLKSPD};
    CPU cpu;
    read_binary(cpu.rom, _ROMFILE);
    read_binary(cpu.ram, _PROGRAM);

    cpu.reset();

    while (!cpu.HALT)
    {
        cpu.tick();
        if (cpu.OE) printf("\n%i %c%c", cpu.OUT, cpu.CF ? 'C' : '.', cpu.ZF ? 'Z' : '.');

        std::this_thread::sleep_for(clk.tick());
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

