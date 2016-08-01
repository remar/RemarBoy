package se.remar.remarboy;

public class Disassembler {
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

	public static void main(String[] args) {
		Disassembler disassembler = new Disassembler();

		boolean debug = false;

		long t = System.currentTimeMillis();
		for(int i = 0;i < 13000;i++) {
			if(debug) {
				System.out.println("---- STEP " + i + " ----");
			}
			disassembler.step();
			if(debug) {
				System.out.println(disassembler);
			}
		}
		System.out.println(disassembler);
		System.out.println("Time: " + (System.currentTimeMillis() - t));
	}
}
