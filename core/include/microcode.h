#pragma once

// Define the micro-instuctions
struct ucode {
    static constexpr uint16_t
        HLT= 0b1000000000000000, // Halt clock
        MI = 0b0100000000000000, // Memory address register in
        RI = 0b0010000000000000, // RAM data in
        RO = 0b0001000000000000, // RAM data out
        IO = 0b0000100000000000, // Instruction register out
        II = 0b0000010000000000, // Instruction register in
        AI = 0b0000001000000000, // A register in
        AO = 0b0000000100000000, // A register out
        EO = 0b0000000010000000, // ALU out
        SU = 0b0000000001000000, // ALU subtract
        BI = 0b0000000000100000, // B register in
        OI = 0b0000000000010000, // Output register in
        CE = 0b0000000000001000, // Program counter enable
        CO = 0b0000000000000100, // Program counter out
        J  = 0b0000000000000010, // Jump (program counter in)
        FI = 0b0000000000000001; // Flags in

    // used for UI only
    static inline std::string str(uint16_t word)
    {
        std::string s {};
        s.append((word & HLT)? "HLT" : "...");
        s.append((word & MI) ? " MI" : " ..");
        s.append((word & RI) ? " RI" : " ..");
        s.append((word & RO) ? " RO" : " ..");
        s.append((word & IO) ? " IO" : " ..");
        s.append((word & II) ? " II" : " ..");
        s.append((word & AI) ? " AI" : " ..");
        s.append((word & AO) ? " AO" : " ..");
        s.append((word & EO) ? " EO" : " ..");
        s.append((word & SU) ? " SU" : " ..");
        s.append((word & BI) ? " BI" : " ..");
        s.append((word & OI) ? " OI" : " ..");
        s.append((word & CE) ? " CE" : " ..");
        s.append((word & CO) ? " CO" : " ..");
        s.append((word & J)  ?  " J" :  " .");
        s.append((word & FI) ? " FI" : " ..");
        return s;
    }
};
