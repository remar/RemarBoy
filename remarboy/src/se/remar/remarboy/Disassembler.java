package se.remar.remarboy;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

public class Disassembler {
	public static void main(String[] args) {
		Disassembler disassembler = new Disassembler();

		boolean debug = false;

		long t = System.currentTimeMillis();
		for(int i = 0;i < 10;i++) {
			if(debug) {
				System.out.println("---- STEP " + i + " ----");
			}
			disassembler.step();
			if(debug) {
				System.out.println(disassembler);
			}
		}
		System.out.println(disassembler);
		System.out.println("Visited: " + disassembler.getVisited());
		System.out.println("Time: " + (System.currentTimeMillis() - t));

		disassembler.disassembleVisited();
	}

	private Emulator emulator;

	public Disassembler() {
		emulator = new Emulator();
		emulator.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");
		// Hardcode LY to 0x94... for Tetris...
		emulator.getMemory().putByte(0xff44, 0x94);
	}

	public void step() {
		emulator.step();
	}

	public String toString() {
		return emulator.toString();
	}

	public Set<Integer> getVisited() {
		return emulator.getVisited();
	}

	public void disassembleVisited() {
		Set<Integer> visited = getVisited();
		Iterator<Integer> it = visited.iterator();
		while(it.hasNext()) {
			System.out.println(disassemble(it.next(), emulator.getMemory()));
			it.next();
		}
	}

	public Instruction disassemble(int address, Memory mem) {
		Map<Integer, String> codes = new HashMap<Integer, String>();
		codes.put(0x00, "NOP");
		return new Instruction(codes.get(mem.getByte(address)), new byte[] {0});
	}

}
