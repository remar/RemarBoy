package se.remar.remarboy;

import java.util.Set;

public class Emulator {
	private Memory mem;
	private CPU cpu;
	private LCD lcd;

	public Emulator() {
		mem = new Memory();
		cpu = new CPU(mem);
		lcd = new LCD(mem);
	}

	public void step() {
	    try {
	        cpu.step();
	        lcd.step();
	    } catch(Exception e) {
	        System.out.println(cpu);
	        System.out.println(lcd);
	        throw e;
	    }
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
