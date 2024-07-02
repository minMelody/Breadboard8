all: bb8 eas rom.out

bb8 : core core/include
	g++ core/*.cpp -Icore/include -lncurses -std=c++11 -o ./bb8
eas : assembler
	g++ assembler/*.cpp -std=c++20 -o ./eas
rom.out : makerom.py
	python3 makerom.py
