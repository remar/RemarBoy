package se.remar.remarboy;

import java.util.Set;

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
		mem.insertCart(path);
	}

	public Memory getMemory() {
		return mem;
	}

	public Set<Integer> getVisited() {
		return mem.getVisited();
	}

	public String toString() {
		return cpu.toString();
	}
}
