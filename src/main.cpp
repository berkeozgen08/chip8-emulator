#include <thread>
#include <chrono>
#include "cpu.hpp"

void decrementTimers(CPU &cpu) {
	while (cpu.isRunning()) {
		cpu.decrementTimers();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
}

void emulate(CPU &cpu) {
	while (cpu.isRunning()) {
		cpu.emulate();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 500));
	}
}

void displayCycle(CPU &cpu) {
	cpu.displayCycle();
}

int main(int argc, char **argv) {
	CPU cpu;

	if (argc > 1) {
		cpu.load(argv[1]);
	}

	std::thread timerThread(decrementTimers, std::ref(cpu));
	std::thread cpuThread(emulate, std::ref(cpu));
	std::thread displayThread(displayCycle, std::ref(cpu));

	timerThread.join();
	cpuThread.join();
	displayThread.join();

	return 0;
}