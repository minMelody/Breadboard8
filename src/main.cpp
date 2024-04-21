#include <iostream>
#include <thread>
#include <fstream>

#include "clock.h"
#include "cpu.h"

using namespace Breadboard8;

template<std::size_t _size>
void read_binary(std::array<uint8_t, _size>& dest, std::string path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
	if (f.is_open())
	{
		std::size_t fileSize = f.tellg();
		std::size_t romSize = (fileSize < _size) ? fileSize : _size;
		char buffer[romSize];
        
        f.seekg(0, std::ios_base::beg);
        f.read(&buffer[0], romSize);
		f.close();

        memcpy(dest.begin(), buffer, romSize);

		return;
	}

	std::cout << "Unable to read ROM image.";
	exit(1);
}

int main(int argc, const char* argv[])
{
    std::string _PROGRAM = argv[1];
    std::string _ROMFILE = (argc > 2) ? argv[2] : "rom.out";

    Clock clk {50};
    CPU cpu{};
    read_binary(cpu.rom, _ROMFILE);
    cpu.reset();
    read_binary(cpu.ram, _PROGRAM);

    while (!cpu.HALT)
    {
        std::this_thread::sleep_for(clk.tick());
        
        cpu.tick();

        if (cpu.OE) printf("%d  %c%c\n", cpu.OUT, cpu.CF ? 'C' : '.', cpu.ZF ? 'Z' : '.');
    }

    return 0;
}