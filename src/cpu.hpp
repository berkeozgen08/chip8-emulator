#pragma once

#include <string>
#include "chip8.hpp"
#include "registers.hpp"
#include "stack.hpp"

class CPU {
private:
	Registers registers;
	Stack stack;
	u8 *memory;
	u8 *display;
	u8 *keyboard;
	u16 opcode;
	bool playSound;
	bool running;
	bool ready;
	
public:
	/**
	 * Initialize the CPU.
	 */
	CPU();

	/**
	 * Free the fields.
	 */
	~CPU();

	/**
	 * Initialize the fields.
	 */
	void initialize();

	/**
	 * Skip to next opcode.
	 */
	void incrementPC();

	/**
	 * Throw a runtime error with a custom message.
	 * @param errorMsg Custom error message.
	 */
	void error(std::string errorMsg);
	
	/**
	 * Load a binary file to memory.
	 * @param file Directory to the file.
	 */
	bool load(std::string file);

	/**
	 * Emulate one cycle of the CPU.
	 */
	void emulate();

	/**
	 * Decrement the delay and sound timers by 1.
	 */
	void decrementTimers();

	/**
	 * Get the state of the CPU.
	 * @return true if CPU is running.
	 */
	bool isRunning();

	/**
	 * Start the display cycle.
	 */
	void displayCycle();
};
