#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "eas.h"

std::vector<std::string> str_split(std::string str, char delimiter, bool keep_delimiter)
{
    std::vector<std::string> vec;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        std::string subst = str.substr(0, pos - keep_delimiter);
        if (subst.length() > 0) vec.push_back(subst);
        str.erase(0, subst.length() + 1);
    }
    if (str.length() > 0) vec.push_back(str);
    return vec;
}

bool Breadboard8::Assembler::parse_number(std::string str, uint8_t* val)
{
    bool is_number = false;
    if (str[1] == '#') // arbitrary base
    {
        std::string base = "";
        base += str[0];
        *val = stoi(str.substr(2), nullptr, stoi(base));
        is_number = true;
    }
    else if (isdigit(str[0])) { *val = stoi(str); is_number = true; } // decimal
    else if (str[0] == '$') { *val = stoi(str.substr(1), nullptr, 16); is_number = true; } // hexadecimal
    else if (str[0] == '%') { *val = stoi(str.substr(1), nullptr, 2); is_number = true; }  // binary
    else if (str[0] == '@') { *val = stoi(str.substr(1), nullptr, 8); is_number = true; }  // octal

    return is_number;
}

Breadboard8::Assembler::Assembler(std::array<uint8_t, 16>* prg)
{
    this->counter = 0;
    this->prg = prg;
}

void Breadboard8::Assembler::assemble_program(const std::string filePath)
{
    this->counter = 0;
    this->symbol_table.clear();
    std::ifstream f(filePath);
    if (f.is_open()) {
        // read one line at a time
        for (std::string buffer; getline(f, buffer);) {
            if (counter >= prg->size()) break;
            parse_line(buffer);
        }

        f.close();
        link_symbols();

        return;
    }

    std::cout << "Unable to open file.";
    exit(1);
}

void Breadboard8::Assembler::parse_line(std::string line)
{
    if (line == std::string("")) return;
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);

    // split comments
    std::vector<std::string> tokens = str_split(line, ';', true);
    if (tokens[0].starts_with(';')) return;

    // split spaces
    tokens = str_split(tokens[0], ' ', false);
    parse_tokens(tokens);
}

void Breadboard8::Assembler::parse_tokens(std::vector<std::string> tokens)
{
    if (tokens.size() < 1) return;

    std::string first = tokens[0];
    std::vector<std::string> vals = std::vector<std::string>(tokens.begin() + 1, tokens.end());

    if (opcodes.contains(first))
    {
        if (write_instruction(opcodes.at(first), vals) == EXIT_FAILURE)
        {
            printf("address %i:", counter);
            for (std::string token : tokens)
            {
                std::cout << ' ' << token.c_str();
            }
            std::cout << std::endl;
            printf("Found %i operands, %i expected.\n", vals.size(), opcodes.at(first).second);
            exit(EXIT_FAILURE);
        }
    }
    else if (first.ends_with(':'))  // parse labels
    {
        first.pop_back();
        if (symbol_table.contains(first)) symbol_table.at(first).address = counter;
        else
        {
            Symbol symbol{};
            symbol.address = counter;
            symbol_table.insert_or_assign(first, symbol);
        }

        if (vals.size() > 0) parse_tokens(vals);
    }
    else if (tokens[0].starts_with('.'))  // parse directives
    {
        if (first == ".org")
        {
            std::string adr = vals[0];
            if (adr[0] == '#') adr = adr.substr(1);
            parse_number(adr, &counter);
        }
        else if (first == ".byte")
        {
            write_values(counter, vals);
        }
    }
    else if (std::isdigit(first[0])) write_values(counter, tokens); // Same as a .byte directive except it only works with decimal
    else
    {
        printf("address %i:", counter);
        for (std::string token : tokens)
        {
            std::cout << ' ' << token.c_str();
        }
        std::cout << std::endl;
        printf("Unexpected token(s).\n");
        exit(EXIT_FAILURE);
    }
}

void Breadboard8::Assembler::link_symbols()
{
    printf("%i symbol(s) found.\n", symbol_table.size());
    for (std::pair<std::string, Symbol> symbol : symbol_table)
    {
        printf("  $%x: %s\n", symbol.second.address, symbol.first.c_str());
        for (uint8_t adr : symbol.second.references)
        {
            (*prg)[adr] &= 0xF0;
            (*prg)[adr] |= symbol.second.address & 0x0F;
        }
    }
}

int Breadboard8::Assembler::write_instruction(std::pair<uint8_t, int> ins, std::vector<std::string> operands)
{
    if (operands.size() != ins.second) return EXIT_FAILURE;

    uint8_t val;
    // In case a label is referenced here, the memory address will be written with an invalid lower nibble (4 bits) until the linker function is called.
    if (operands.size() > 0 && parse_number(operands[0], &val) == false)
    {
        // add this address to the symbol's references list
        if (symbol_table.contains(operands[0])) symbol_table.at(operands[0]).references.push_back(counter);
        else
        {
            Symbol symbol{};
            symbol.references.push_back(counter);
            symbol_table.insert_or_assign(operands[0], symbol);
        }
    }

    (*prg)[counter] = (ins.first << 4) + (val & 0x0f);
    counter++;

    return EXIT_SUCCESS;
}

void Breadboard8::Assembler::write_values(uint8_t start_adr, std::vector<std::string> values)
{
    for (int i = 0; i < values.size(); i++) {
        if (start_adr + i >= prg->size()) break;
        uint8_t value;
        parse_number(values[i], &value);
        (*prg)[start_adr + i] = value;
        counter += (start_adr + i == counter);
    }
}
