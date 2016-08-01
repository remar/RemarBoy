package se.remar.remarboy;

public class Emulator {
	private Memory mem;
	private CPU cpu;

	public Emulator() {
		mem = new Memory();
		cpu = new CPU(mem);
	}

	public void step() {
		cpu.step();
	}

	public void insertCart(String path) {
		mem.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");
	}

	public Memory getMemory() {
		return mem;
	}

	public String toString() {
		return cpu.toString();
	}
}
