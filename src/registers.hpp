#pragma once

#include "chip8.hpp"

class Registers {
public:
	/** 
	 * General purpose registers.
	 */
	u8 V[16];

	/**
	 * For memory addresses.
	 */
	u16 I;

	/**
	 * Delay timer.
	 */
	u8 DT;
	/**
	 * Sound timer
	 */
	u8 ST;

	/**
	 * Program counter.
	 */
	u16 PC;
	/**
	 * Stack pointer.
	 */
	u8 SP;

	/**
	 * Call Registers::initialize().
	 */
	Registers();

	/**
	 * Set all registers to 0.
	 */
	void initialize();
};
