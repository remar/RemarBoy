package se.remar.remarboy;

public class Instruction {
    private String mnemonic;
    private byte[] bytes;

    public Instruction(String mnemonic, byte[] bytes) {
        this.mnemonic = mnemonic;
        this.bytes = bytes;
    }

    public Instruction(String mnemonic, byte byte1) {
        this.mnemonic = mnemonic;
        this.bytes = new byte[] {byte1};
    }

    public Instruction(String mnemonic, byte byte1, byte byte2) {
        this.mnemonic = mnemonic;
        this.bytes = new byte[] {byte1, byte2};
    }

    public Instruction(String mnemonic, byte byte1, byte byte2, byte byte3) {
        this.mnemonic = mnemonic;
        this.bytes = new byte[] {byte1, byte2, byte3};
    }

    public String toString() {
        String bytesString = "";
        if(bytes.length == 1) {
            bytesString = Util.formatByte(bytes[0]);
        } else if(bytes.length == 2) {
            bytesString = Util.formatByte(bytes[0]) + ", " + Util.formatByte(bytes[1]);
        } else if(bytes.length == 3) {
            bytesString = Util.formatByte(bytes[0]) + ", " + Util.formatByte(bytes[1])
                    + ", " + Util.formatByte(bytes[2]);
        }
        return String.format("%-15s %s", mnemonic , " (" + bytesString + ")");
    }
}
