# Follow up on my [Breadboard8mini](https://github.com/minMelody/Breadboard8mini) 8bit cpu emulator

This is an in-depth emulator of Ben Eater's [8 bit breadboard computer](https://youtube.com/playlist?list=PLowKtXNTBypGqImE405J2565dvjafglHU) down to the micro instructions.

Build the project by running `./make.sh`, this creates a 'bb8.exe' executable using g++, as well as the ROM image.

Run the emulator using `./bb8 <program file> <clock speed> <(optional)rom file>`. It loads 'rom.out' by default.

## Editing the microcode

The makerom python script is based on Ben Eater's arduino [EEPROM programmer](https://github.com/beneater/eeprom-programmer).
Edit the template to add instructions.<br>
When changing the micro-instruction definitions make sure to edit both the python script and 'src/include/microcode.h'.

Running `py makerom.py` outputs the rom image in 'rom.out', unless a different file is specified.

## Test program
When the microcode contains no jump if carry, `shiftleft.out` outputs all zeros once a value greater than 128 is reached:

![example of conditional jump not implemented](screenshot-no-jc.png)

Meanwhile a working jump if carry will execute in a loop:

![example of working jump carry](screenshot-working-jc.png)
