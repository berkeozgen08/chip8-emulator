#include "chip8.hpp"
#include "registers.hpp"

Registers::Registers() {
	initialize();
}

void Registers::initialize() {
	for (auto &i : V) {
		i = 0;
	}
	I = 0;
	DT = 0;
	ST = 0;
	PC = PROGRAM_START;
	SP = 0;
}