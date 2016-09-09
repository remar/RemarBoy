package se.remar.remarboy;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class Disassembler {
    public static void main(String[] args) {
        Disassembler disassembler = new Disassembler();

        boolean debug = false;

        long t = System.currentTimeMillis();
        for(int i = 0;i < 200000;i++) {
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

    public void disassembleVisited() {
        List<Integer> visited = new ArrayList<Integer>(emulator.getVisited());
        Collections.sort(visited);
        Iterator<Integer> it = visited.iterator();
        while(it.hasNext()) {
            int address = it.next();
            System.out.println(Util.formatWord(address) + ": " + disassemble(address, emulator.getMemory()));
        }
    }

    public Instruction disassemble(int address, Memory mem) {
        //   0 1 2 3 4 5 6 7 8 9 A B C D E F
        // 0 x x x x x x x . . x . . x x x . 0
        // 1 . x x x x x x . . x . . x x x . 1
        // 2 x x x x x x x . x x . . x x x . 2
        // 3 x x x x x x x . x x . . x x x . 3
        // 4 x x x x x x x x x x x x x x x x 4
        // 5 x x x x x x x x x x x x x x x x 5
        // 6 x x x x x x x x x x x x x x x x 6
        // 7 x x x x x x . x x x x x x x x x 7
        // 8 x x x x x x x x x x x x x x x x 8
        // 9 x x x x x x x x x x x x x x x x 9
        // A x x x x x x x x x x x x x x x x A
        // B x x x x x x x x x x x x x x x x B
        // C . x . x . x . x . . . . . x . x C
        // D . x . . . x . x . . . . . . . x D
        // E x x x . . x . x . x x . . . . x E
        // F x x . x . x . x . . . x . . x x F
        //   0 1 2 3 4 5 6 7 8 9 A B C D E F
        Map<Integer, String> codes = new HashMap<Integer, String>();

        codes.put(0x00, "NOP");
        codes.put(0xc3, "JP 0xnn");
        codes.put(0xcd, "CALL 0xnn");
        codes.put(0xe0, "LD (0xFFn),A");
        codes.put(0xe9, "JP (HL)");
        codes.put(0xe2, "LD (0xff00 + C),A");
        codes.put(0xea, "LD (0xnn),A");
        codes.put(0xf0, "LD A,(0xFFn)");
        codes.put(0xf3, "DI");
        codes.put(0xfb, "EI");
        codes.put(0xfe, "CP 0xn");

        byte op = mem.getByte(address);

        if((op & 0xe7) == 0x20) { // JR conditional,n
            String neg = (op & 0x08) == 0x08 ? "":"N";
            String flag = (op & 0x10) == 0x10 ? "C":"Z";
            byte n = mem.getByte(address + 1);
            return new Instruction("JR "+neg+flag+",0x"+Util.formatByte(n), op, n);
        } else if((op & 0xcf) == 0x01) { // LD rr,nn
            int nn = mem.getWord(address + 1);
            return new Instruction("LD " + getWideRegNameSP((op & 0x30) >> 4) + ",0x"+Util.formatWord(nn), mem.getBytes(address, 3));
        } else if((op & 0xcf) == 0x02) { // LD (rr+-),A
            return new Instruction("LD " + getIndirectRegName((op & 0x30) >> 4) + ",A", op);
        } else if((op & 0xcf) == 0x03) { // INC rr
            return new Instruction("INC " + getWideRegNameSP((op & 0x30) >> 4), op);
        } else if((op & 0xc7) == 0x04) { // INC r
            return new Instruction("INC " + getRegName((op & 0x38) >> 3), op);
        } else if((op & 0xc7) == 0x05) { // DEC r
            return new Instruction("DEC " + getRegName((op & 0x38) >> 3), op);
        } else if((op & 0xc7) == 0x06) { // LD r,n
            byte n = mem.getByte(address + 1);
            return new Instruction("LD " + getRegName((op & 0x38) >> 3)
                    + ",0x" + Util.formatByte(n), op, n);
        } else if((op & 0xcf) == 0x09) { // ADD HL,rr
            return new Instruction("ADD HL," + getWideRegNameSP((op & 0x30) >> 4), op);
        } else if((op & 0xcf) == 0x0a) { // LD A,(rr+-)
            return new Instruction("LD A," + getIndirectRegName((op & 0x30) >> 4), op);
        } else if((op & 0xc0) == 0x40 && (op & 0xff) != 0x76) { // LD r,r', LD (HL),r, and LD r,(HL)
            String target = getRegName((op & 0x38) >> 3);
            String source = getRegName(op & 0x07);
            return new Instruction("LD "+target+","+source, op);
        } else if((op & 0xff) >= 0x80 && (op & 0xff) <= 0xbf) { // ADD, ADC, SUB, SBC, AND, XOR, OR, CP
            String[] mne = {"ADD","ADC","SUB","SBC","AND","XOR","OR","CP"};
            return new Instruction(mne[((op & 0xff) - 0x80)/8] + " " + getRegName(op & 0x07), op);
        } else if((op & 0xcf) == 0xc1) { // POP rr
            return new Instruction("POP " + getWideRegNameAF((op & 0x30) >> 4), op);
        } else if((op & 0xcf) == 0xc5) { // PUSH rr
            return new Instruction("PUSH " + getWideRegNameAF((op & 0x30) >> 4), op);
        } else if((op & 0xc7) == 0xc7) {
            int dest = ((op & 0x28) >> 3);
            return new Instruction("RST " + dest + " (0x" + Util.formatByte(dest * 8) + ")", op);
        } else {
            String mnemonic = codes.get(op & 0xff);
            if(mnemonic == null) {
                throw new RuntimeException("Unable to disassemble " + op + " (" + Util.formatByte(op) + ")");
            }
            int length = 1;
            if(mnemonic.contains("nn")) {
                int nn = mem.getWord(address + 1);
                mnemonic = mnemonic.replace("nn", Util.formatWord(nn));
                length = 3;
            } else if(mnemonic.contains("n")) {
                int n = mem.getByte(address + 1);
                mnemonic = mnemonic.replace("n", Util.formatByte(n));
                length = 2;
            }
            return new Instruction(mnemonic, mem.getBytes(address, length));
        }
    }

    private String getRegName(int reg) {
        final String[] regs = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
        return regs[reg];
    }

    private String getWideRegNameAF(int widereg) {
        final String[] wideregs = {"BC", "DE", "HL", "AF"};
        return wideregs[widereg];
    }

    private String getWideRegNameSP(int widereg) {
        final String[] wideregs = {"BC", "DE", "HL", "SP"};
        return wideregs[widereg];
    }

    private String getIndirectRegName(int indirectreg) {
        final String[] indirectregs = {"(BC)", "(DE)", "(HL+)", "(HL-)"};
        return indirectregs[indirectreg];
    }
}
