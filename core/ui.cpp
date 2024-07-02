#include "ui.h"

// pattern for formatting a byte in binary
#define BYTE_TO_BINARY_FORMAT "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')
// format a 4 bits integer in binary
#define NIBBLE_TO_BINARY_FORMAT "%c%c%c%c"
#define NIBBLE_TO_BINARY(nib) \
  ((nib) & 0x08 ? '1' : '0'), \
  ((nib) & 0x04 ? '1' : '0'), \
  ((nib) & 0x02 ? '1' : '0'), \
  ((nib) & 0x01 ? '1' : '0')

// define mnemonics for each panel
#define UI_RAM  0
#define UI_CLK  1
#define UI_CNTR 2
#define UI_AREG 3
#define UI_ALU  4
#define UI_BREG 5
#define UI_OUT  6
#define UI_IR   7
#define UI_WORD 8
#define UI_CTRL 9

// some helper variables for composition
#define LEFT_PANEL   0
#define RIGHT_PANEL 20
#define ALIGN_FOOTER "" // set to "-" to align left, "" aligns to the right

std::vector<std::pair<WINDOW*, const char*>> Breadboard8::UI::board;
uint8_t Breadboard8::UI::lastMAR {};
bool Breadboard8::UI::editing_ram = false;

void Breadboard8::UI::init(Breadboard8::CPU *cpu, std::string prg_path)
{
    initscr();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "Program: %s", prg_path.c_str());
    move(18, 0); // set cursor position to bottom left corner

    board = {
        {subwin(stdscr, 18, 19,  1, LEFT_PANEL), "RAM"},
        {subwin(stdscr,  3, 15,  5, RIGHT_PANEL +  0), "CLOCK"},
        {subwin(stdscr,  4, 30,  8, RIGHT_PANEL +  0), "COUNTERS"},
        {subwin(stdscr,  3, 18,  5, RIGHT_PANEL + 31), "A"},
        {subwin(stdscr,  4, 18,  8, RIGHT_PANEL + 31), "ALU"},
        {subwin(stdscr,  3, 18, 12, RIGHT_PANEL + 31), "B"},
        {subwin(stdscr,  3, 15,  5, RIGHT_PANEL + 15), "OUT"},
        {subwin(stdscr,  3, 30, 12, RIGHT_PANEL +  0), "INSTRUCTION-REGISTER"},
        {subwin(stdscr,  3, 49, 15, RIGHT_PANEL +  0), "CONTROL-WORD"},
        {subwin(stdscr,  4, 49,  1, RIGHT_PANEL +  0), "CONTROLS"}
    };

    mvwprintw(board[UI_ALU].first, 1,  4, "CF=");
    mvwprintw(board[UI_ALU].first, 1, 10, "ZF=");

    mvwprintw(board[UI_CTRL].first, 2, 4, "(S)tepping Mode   (E)dit RAM     (K)ill");
    mvwprintw(board[UI_CTRL].first, 1, 4, "(I)nc. speed      (D)ec. speed   (R)eset");
 
    for (int i = 0; i < board.size(); i++)
    {
        box(board[i].first, ACS_VLINE, ACS_HLINE);
        mvwprintw(board[i].first, 0, 2, board[i].second);
    }
}

void Breadboard8::UI::update(Breadboard8::CPU *cpu, Breadboard8::Clock *clk)
{
    if (clk != nullptr)
    {
        if (clk->single_step) mvwprintw(board[UI_CLK].first, 1, 2, "SINGLE STEP");
        else mvwprintw(board[UI_CLK].first, 1, 2, " %-3i Hertz ", clk->frequency);
    }

    // display a pointer for the memory address register
    mvwaddch(board[UI_RAM].first,  lastMAR + 1, 1, ' ');
    mvwaddch(board[UI_RAM].first, cpu->MAR + 1, 1, '*');
    for (int j = 0; j < cpu->ram.size(); j++)
    {
        mvwprintw(board[UI_RAM].first, j + 1, 3,  BYTE_TO_BINARY_FORMAT "  $%02X", BYTE_TO_BINARY(cpu->ram[j]), cpu->ram[j]);
    }

    // ram contents and instruction register are displayed as binary + hex, out is only decimal, other registers are binary + decimal
    mvwprintw(board[UI_CNTR].first, 1,  3, "PROGRAM COUNTER: " NIBBLE_TO_BINARY_FORMAT " %2i", NIBBLE_TO_BINARY(cpu->PC), cpu->PC);
    mvwprintw(board[UI_CNTR].first, 2,  3, "   STEP COUNTER: " NIBBLE_TO_BINARY_FORMAT " %2i", NIBBLE_TO_BINARY(cpu->step), cpu->step);
    mvwprintw(board[UI_OUT ].first, 1,  6, "%03i", cpu->OUT);
    mvwaddch( board[UI_ALU ].first, 1,  7, (cpu->CF) ? '1' : '0');
    mvwaddch( board[UI_ALU ].first, 1, 13, (cpu->ZF) ? '1' : '0');
    mvwprintw(board[UI_ALU ].first, 2,  3, BYTE_TO_BINARY_FORMAT " %03i" , BYTE_TO_BINARY(cpu->alu.sum._8), cpu->alu.sum._8);
    mvwprintw(board[UI_AREG].first, 1,  3, BYTE_TO_BINARY_FORMAT " %03i" , BYTE_TO_BINARY(cpu->A), cpu->A);
    mvwprintw(board[UI_BREG].first, 1,  3, BYTE_TO_BINARY_FORMAT " %03i" , BYTE_TO_BINARY(cpu->B), cpu->B);
    mvwprintw(board[UI_IR  ].first, 1,  9, BYTE_TO_BINARY_FORMAT " $%02X", BYTE_TO_BINARY(cpu->IR), cpu->IR);
    mvwprintw(board[UI_WORD].first, 1,  1, Breadboard8::CPU::ucode::str(cpu->control_word).c_str());

    for (int i = 0; i < 9; i++) // controls window's contents never change and never needs refreshing
    {
        wrefresh(board[i].first);
    }

    refresh();
    lastMAR = cpu->MAR;
}

void Breadboard8::UI::set_footer(const char* str)
{
    mvprintw(18, RIGHT_PANEL, "%" ALIGN_FOOTER "49s", str);
}

void Breadboard8::UI::edit_ram(CPU *cpu)
{
    unsigned int begy, begx;
    getbegyx(UI::board[UI_RAM].first, begy, begx);
    bool nibble = false;
    int in;
    uint8_t adr = cpu->MAR;

    set_footer("--Press enter to quit editing mode--");
    curs_set(1); // show cursor when editing ram

    while (editing_ram)
    {
        move(begy + 1 + adr, begx + 15 - nibble);
        switch (in = getch())
        {
        case KEY_UP:
            adr = --adr & 0x0F; 
            break;
        case KEY_DOWN:
            adr = ++adr & 0x0F;
            break;
        case KEY_RIGHT:
            nibble = 0;
            break;
        case KEY_LEFT:
            nibble = 1;
            break;
        case '\n':
            editing_ram = false;
            break;
        default:
            if (!isxdigit(in)) break;
            std::string hexit {static_cast<char>(in)};

            cpu->ram[adr] &= 0xF0 >> (4 * nibble);
            cpu->ram[adr] |= stoi(hexit, nullptr, 16) << (4 * nibble);
            nibble = !nibble; // move cursor to the next hexit, wrap around
            break;
        }

        update(cpu, nullptr);
    }

    curs_set(0);
    move(18, 0);
}

void Breadboard8::UI::close()
{
    for (std::pair<WINDOW*, const char*> subwindow : board)
    {
        if (subwindow.first != nullptr) delwin(subwindow.first);
    }
    endwin();
}
