package se.remar.remarboy;

public class Instruction {
	private String mnemonic;
	private byte[] bytes;

	public Instruction(String mnemonic, byte[] bytes) {
		this.mnemonic = mnemonic;
		this.bytes = bytes;
	}

	public String toString() {
		return mnemonic;
	}
}
