#include "chip8.hpp"
#include "stack.hpp"

Stack::Stack() {
	initialize();
}

void Stack::initialize() {
	data = new u16[STACK_SIZE];
	for (u8 i = 0; i < STACK_SIZE; i++) {
		data[i] = 0;
	}
}

void Stack::push(u16 &PC, u8 &SP) {
	data[SP++] = PC;
}

void Stack::pop(u16 &PC, u8 &SP) {
	PC = data[--SP] + 2;
}