#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <cstdint>

namespace Breadboard8
{
	struct Assembler
	{
    public:
        // A list of all valid mnemonics, in lowercase, with the corresponding opcodes.
        // and the number of operands expected by the instruction
		// Opcodes 0x9 through 0xD can easily be added here.
		const std::unordered_map<std::string, std::pair<uint8_t, int>> opcodes {
			{"nop", {0x0, 0}},
			{"lda", {0x1, 1}},
			{"add", {0x2, 1}},
			{"sub", {0x3, 1}},
			{"sta", {0x4, 1}},
			{"ldi", {0x5, 1}},
			{"jmp", {0x6, 1}},
			{"jc" , {0x7, 1}},
			{"jz" , {0x8, 1}},
			{"", {0x9, 0}},
			{"", {0xA, 0}},
			{"", {0xB, 0}},
			{"", {0xC, 0}},
			{"", {0xD, 0}},
			{"out", {0xE, 0}},
			{"hlt", {0xF, 0}},
		};
	public:
        Assembler(std::array<uint8_t, 16>* prg);
		// Assembles a source file then links the symbol table, results can be loaded immediately or saved as a .bin file
		void assemble_program(std::string filePath);
		// Assemble one line at a time.
        // Separates the line in tokens and calls parse_tokens
		void parse_line(std::string line);
        // Assembles a list of tokens.
        void parse_tokens(std::vector<std::string> tokens);
		// Linker.
		// Writes labels addresses in place of their mnemonics.
		void link_symbols();

		struct Symbol
		{
			std::uint8_t address; // the symbol's address
			std::vector<std::uint8_t> references{}; // stores where the symbol is used
		};
		std::unordered_map<std::string, Symbol> symbol_table{};
	protected:
		int write_instruction(std::pair<uint8_t, int> ins, std::vector<std::string> operands);
		void write_values(uint8_t start_adr, std::vector<std::string> values);
		bool parse_number(std::string str, uint8_t* val);
		uint8_t counter = 0;
        std::array<uint8_t, 16>* prg; // Points to the program.
	};

}
