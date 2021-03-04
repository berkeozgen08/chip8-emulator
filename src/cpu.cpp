#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <ctime>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "chip8.hpp"
#include "cpu.hpp"

// 0x0NNN
#define ADDR (opcode & 0x0FFF)
// 0x000N
#define N (opcode & 0x000F)
// 0x0X00
#define X ((opcode & 0x0F00) >> 8)
// 0x00Y0
#define Y ((opcode & 0x00F0) >> 4)
// 0x00KK
#define KK (opcode & 0x00FF)
#define V(x) registers.V[x]

CPU::CPU() {
	initialize();

	srand(time(NULL));
}

CPU::~CPU() {
	delete[] memory;
	delete[] display;
	delete[] keyboard;
}

void CPU::initialize() {
	memory = new u8[MEMORY_SIZE];
	display = new u8[DISPLAY_SIZE];
	keyboard = new u8[KEY_COUNT];	

	// Initialize arrays
	for (u16 i = 0; i < MEMORY_SIZE; i++) {
		memory[i] = 0;
	}
	for (u16 i = 0; i < DISPLAY_SIZE; i++) {
		display[i] = 0;
	}
	for (u8 i = 0; i < KEY_COUNT; i++) {
		keyboard[i] = 0;
	}

	opcode = 0;
	playSound = false;
	running = true;
	ready = false;

	// Store fontsent in memory
	for(u16 i = FONTSET_START; i < FONTSET_END; i++) {
		memory[i] = FONTSET[i - FONTSET_START];
	}
}

void CPU::incrementPC() {
	registers.PC += 2;
}

void CPU::error(std::string errorMsg = "") {
	throw std::runtime_error(errorMsg);
}

bool CPU::load(std::string fileDir) {
	std::ifstream file(fileDir, std::ios::in | std::ios::binary);
	if (file.fail()) {
		// error("File could not be opened.");
		return false;
	}

	std::vector<u8> vec {
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};

	u16 i = PROGRAM_START;
	for (auto j = vec.begin(); j != vec.end(); j++) {
		memory[i++] = *j;
	}

	ready = true;
	return true;
}

void CPU::emulate() {
	if (!ready) return;

	// opcodes are 16 bits and the memory is an 8 bit array. So read 8 bits,
	// shift left 8 times, bitwise AND with the rest of the bits to concatenate.
	opcode = (memory[registers.PC] << 8) | memory[registers.PC + 1];

	// Compare the first 4 bits
	switch (opcode & 0xF000) {
		// 0~~~
		case 0x0000:
			// 00E0
			if (KK == 0xE0) {
				for (u16 i = 0; i < DISPLAY_SIZE; i++) {
					display[i] = 0;
				}
				incrementPC();
			}
			// 00EE
			else if (KK == 0xEE) {
				if (registers.SP > 0) {
					stack.pop(registers.PC, registers.SP);
				} else {
					error("Tried to pop item from stack while SP <= 0.");
				}
			}
			// 0000
			else if (KK == 0x00) {
				incrementPC();
			} else {
				error("Unknown opcode.");
			}
			break;
		// 1NNN
		case 0x1000:
			registers.PC = ADDR;
			break;
		// 2NNN
		case 0x2000:
			if (registers.SP == STACK_SIZE - 1) {
				error("Stack overflow.");
			} else {
				stack.push(registers.PC, registers.SP);
				registers.PC = ADDR;
			}
			break;
		// 3XKK
		case 0x3000:
			if (V(X) == KK) {
				incrementPC();
			}
			incrementPC();
			break;
		// 4XKK
		case 0x4000:
			if (V(X) != KK) {
				incrementPC();
			}
			incrementPC();
			break;
		// 5XY0
		case 0x5000:
			if (V(X) == V(Y)) {
				incrementPC();
			}
			incrementPC();
			break;
		// 6XKK
		case 0x6000:
			V(X) = KK;
			incrementPC();
			break;
		// 7XKK
		case 0x7000:
			V(X) += KK;
			incrementPC();
			break;
		// 8XY~
		case 0x8000:
			switch (N) {
				// 8XY0
				case 0x0:
					V(X) = V(Y);
					break;
				// 8XY1
				case 0x1:
					V(X) = V(X) | V(Y);
					break;
				// 8XY2
				case 0x2:
					V(X) = V(X) & V(Y);
					break;
				// 8XY3
				case 0x3:
					V(X) = V(X) ^ V(Y);
					break;
				// 8XY4
				case 0x4:
					if (V(X) + V(Y) > 0xFF) {
						V(0xF) = 1;
					} else {
						V(0xF) = 0;
					}
					V(X) = (V(X) + V(Y)) & 0xFF;
					break;
				// 8XY5
				case 0x5:
					if (V(X) >= V(Y)) {
						V(0xF) = 1;
					} else {
						V(0xF) = 0;
					}
					V(X) -= V(Y);
					break;
				// 8XY6
				case 0x6:
					V(0xF) = V(X) & 0x1;
					V(X) >>= 1;
					break;
				// 8XY7
				case 0x7:
					if (V(Y) >= V(X)) {
						V(0xF) = 1;
					} else {
						V(0xF) = 0;
					}
					V(X) = V(Y) - V(X);
					break;
				// 8XYE
				case 0xE:
					V(0xF) = (V(X) & 0x80) >> 7;
					V(X) <<= 1;
					break;
			}
			incrementPC();
			break;
		// 9XY0
		case 0x9000:
			if (V(X) != V(Y)) {
				incrementPC();
			}
			incrementPC();
			break;
		// ANNN
		case 0xA000:
			registers.I = ADDR;
			incrementPC();
			break;
		// BNNN
		case 0xB000:
			registers.PC = ADDR + V(0);
			break;
		// CXKK
		case 0xC000:
			V(X) = (rand() % 0xFF) & KK;
			incrementPC();
			break;
		// DXYN
		case 0xD000:
			// y-axis
			for (u8 i = 0; i < N; i++) {
				// x-axis
				for (u8 j = 0; j < 8; j++) {
					if ((memory[registers.I + i] & (0x80 >> j)) != 0) {
						u16 pos = (V(X) + j + DISPLAY_WIDTH * (V(Y) + i)) % DISPLAY_SIZE;
						if (pos < 0) pos += DISPLAY_SIZE;
						V(0xF) = display[pos];
						display[pos] ^= 1;
					}
				}
			}
			incrementPC();
			break;
		// EX~~
		case 0xE000: {
			u8 index;
			for (index = 0; index < KEY_COUNT; index++) {
				if (KEYS[index] == V(X)) {
					break;
				}
			}
			// EX9E
			if (KK == 0x9E) {
				if (keyboard[index] == 1) {
					incrementPC();
				}
				incrementPC();
			}
			// EXA1
			else if(KK == 0xA1) {
				if (keyboard[index] != 1) {
					incrementPC();
				}
				incrementPC();
			} else {
				error("Unknown opcode.");
			}}
			break;
		// FX~~
		case 0xF000:
			switch(KK) {
				// FX07
				case 0x07:
					V(X) = registers.DT;
					break;
				// FX0A
				case 0x0A: {
					u8 input = 0;
					for (u8 i = 0; i < KEY_COUNT; i++) {
						if (keyboard[i] == 1) {
							input = KEYS[i];
							break;
						}
					}
					if (input != 0) {
						V(X) = input;
					} else {
						return;
					}}
					break;
				// FX15
				case 0x15:
					registers.DT = V(X);
					break;
				// FX18
				case 0x18:
					registers.ST = V(X);
					break;
				// FX1E
				case 0x1E:
					registers.I += V(X);
					break;
				// FX29
				case 0x29:
					registers.I = V(X) * 5 + FONTSET_START;
					break;
				// FX33
				case 0x33:
					memory[registers.I] = V(X) / 100;
					memory[registers.I + 1] = (V(X) / 10) % 10;
					memory[registers.I + 2] = V(X) % 10;
					break;
				// FX55
				case 0x55:
					for (u8 i = 0; i <= X; i++) {
						memory[registers.I + i] = V(i);
					}
					break;
				// FX65
				case 0x65:
					for (u8 i = 0; i <= X; i++) {
						V(i) = memory[registers.I + i];
					}
					break;
				default:
					error("Unknown opcode.");
					break;
			}
			incrementPC();
			break;
		default:
			error("Unknown opcode.");
			break;
	}
}

void CPU::decrementTimers() {
	if (!ready) return;

	if (registers.DT > 0) {
		registers.DT--;
	}
	if (registers.ST > 0) {
		if (registers.ST == 1) {
			playSound = true;
		}
		registers.ST--;
	}
}

bool CPU::isRunning() {
	return running;
}

#undef X

void CPU::displayCycle() {
	int scale = 20;
	sf::RenderWindow window(sf::VideoMode(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale), "CHIP-8");
	window.setFramerateLimit(60);
	
	sf::Uint8 *pixels = new sf::Uint8[DISPLAY_WIDTH * DISPLAY_HEIGHT * 4];
	sf::Texture texture;
	texture.create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	sf::Sprite sprite(texture);
	sprite.setScale(scale, scale);

	sf::SoundBuffer soundBuffer;
	sf::Sound *sound;
	if (!soundBuffer.loadFromFile("res/sound.wav")) {
		sound = nullptr;
	} else {
		sound = new sf::Sound;
		sound->setBuffer(soundBuffer);
	}

	bool menu = true;
	sf::String msg("ENTER to run the program\nESCAPE to return to this screen\n\nDirectory to the program: ");
	sf::String input;
	sf::Text inputText;
	inputText.setString(msg + "_");

	sf::Font font;
	if (!font.loadFromFile("res/arial.ttf")) exit(1);
	inputText.setFont(font);

	sf::Event event;
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				running = false;
			} else if (menu && event.type == sf::Event::TextEntered && event.text.unicode < 128) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
					if (load(static_cast<std::string>(input))) {
						menu = false;
						ready = true;
					} else {
						inputText.setString(inputText.getString() + "\n\nFile could not be opened.");
					}
				} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace) && input.getSize() > 0) {
					input.erase(input.getSize() - 1, 1);
					inputText.setString(msg + input + "_");
				} else if (event.text.unicode >= 32) {
					input += event.text.unicode;
					inputText.setString(msg + input + "_");
				}
			} 
		}

		for (auto i = 0; i < KEY_COUNT; i++) {
			keyboard[i] = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
			keyboard[0] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
			keyboard[1] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
			keyboard[2] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
			keyboard[3] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
			keyboard[4] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			keyboard[5] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
			keyboard[6] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			keyboard[7] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			keyboard[8] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			keyboard[9] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			keyboard[10] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
			keyboard[11] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
			keyboard[12] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
			keyboard[13] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
			keyboard[14] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
			keyboard[15] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			this->~CPU();
			initialize();
			registers.initialize();
			stack.initialize();

			input.clear();
			inputText.setString(msg);
			menu = true;
		}

		if (sound != nullptr && playSound) {
			sound->play();
			playSound = false;
		}

		window.clear(sf::Color::Black);

		if (menu) {
			window.draw(inputText);
		} else {
			for (int i = 0; i < DISPLAY_SIZE * 4; i += 4) {
				for (int j = 0; j < 4; j++) {
					pixels[i + j] = display[i / 4] == 1 ? 255 : 0;
				}
			}
			texture.update(pixels);
			window.draw(sprite);
		}

		window.display();
	}
}
