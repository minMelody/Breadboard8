#include <iostream>
#include <thread>
#include <fstream>

#include "clock.h"
#include "cpu.h"

using namespace Breadboard8;

template<std::size_t SIZE>
void read_binary(std::array<uint8_t, SIZE>& dest, std::string path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
	if (f.is_open())
	{
		std::size_t fileSize = f.tellg();
		std::size_t romSize = (fileSize < dest.size()) ? fileSize : SIZE;
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
    std::string ROM_IMAGE = "rom.out";
    if (argc > 1)
    {
        ROM_IMAGE = argv[2];
    }

    Clock clk {50};
    CPU cpu{};
    read_binary(cpu.rom, ROM_IMAGE);
    cpu.reset();

    cpu.ram[0x0] = 0x58; // LDI 8
    cpu.ram[0x1] = 0x3f; // SUB 15
    cpu.ram[0x2] = 0x61; // JMP 1
    cpu.ram[0xf] = 0x02;

    while (!cpu.HALT)
    {
        std::this_thread::sleep_for(clk.tick());
        
        cpu.tick();

        if (cpu.step > 0) continue;
        printf("%d  %c%c\n", cpu.A, cpu.CF ? 'C' : '.', cpu.ZF ? 'Z' : '.');
        if (cpu.PC == 0) std::cout << "\n";
    }

    return 0;
}