#include <iostream>
#include <thread>
#include <fstream>

#include "clock.h"
#include "cpu.h"

using namespace Breadboard8;

void program_rom(CPU* cpu, std::string path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
	if (f.is_open())
	{
		unsigned long fileSize = f.tellg();
		unsigned long address = 0x00;
		unsigned char byte = 0x00;
		while (address < cpu->rom.size() && address < fileSize)
		{
			f.seekg(address, std::ios_base::beg);
			f.read((char*)&byte, 1);
			cpu->rom[address] = byte;
			address++;
		}

		f.close();
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
    program_rom(&cpu, ROM_IMAGE);
    cpu.reset();

    cpu.ram[0x0] = 0x58; // LDI 8
    cpu.ram[0x1] = 0x3f; // SUB 15
    cpu.ram[0x2] = 0x61; // JMP 1
    cpu.ram[0xf] = 0x02;

    // substract 2 from A
    std::array<int, 5> microcode {
        cpu.MI | cpu.CO,
        cpu.RO | cpu.II | cpu.CE,
        cpu.IO | cpu.MI,
        cpu.RO | cpu.BI,
        cpu.EO | cpu.AI | cpu.SU | cpu.FI
    };
    uint8_t instruction = 0b0011; // SUB

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