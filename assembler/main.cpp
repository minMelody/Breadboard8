#include <iostream>
#include <fstream>

#include "eas.h"

using namespace Breadboard8;
int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cout << "usage: ./eas <program> [outfile]\n"
                  << "<program> - assembly file\n"
                  << "[outfile] - output file path, 'a.out' by default"
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::string _PROGRAM = argv[1];
    const char* _OUTFILE = argc == 3 ? argv[2] : "a.out";

    std::array<uint8_t, 16> prg {};
    Assembler eas {&prg};
    eas.assemble_program(_PROGRAM);

    // write to outfile
    std::ofstream of(_OUTFILE, std::ios::binary);
    try
    {
        std::cout << '\n';
        for (int i = 0; i < prg.size(); i++)
        {
            printf("%02x ", prg[i]);
            of << prg[i];
        }
        of.close();

        std::cout << "\n> " << _OUTFILE << std::endl;
        return EXIT_SUCCESS;
    }
    catch(...)
    {
        of.close();
        std::cout << "Unable to create binary file." << std::endl;
        return EXIT_FAILURE;
    }
}
