all: bb8.exe eas.exe rom.out

bb8.exe : core core/include
	g++ core/*.cpp -I core/include -std=c++11 -o ./bb8.exe
eas.exe : assembler
	g++ assembler/*.cpp -std=c++20 -o ./eas.exe
rom.out : makerom.py
	python3 makerom.py
