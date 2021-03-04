#include "chip8.hpp"

class Stack {
private:
	u16 *data;

public:
	/**
	 * Call Stack::initialize().
	 */
	Stack();

	/**
	 * Initialize data.
	 */
	void initialize();

	/**
	 * Insert the program counter to the top of the stack
	 * and increment the stack pointer.
	 * @param PC Program counter.
	 * @param SP Stack pointer.
	 */
	void push(u16 &PC, u8 &SP);

	/**
	 * Set the program counter to the address at the top
	 * of the stack and decrement the stack pointer.
	 * @param PC Program counter.
	 * @param SP Stack pointer.
	 */
	void pop(u16 &PC, u8 &SP);
};
