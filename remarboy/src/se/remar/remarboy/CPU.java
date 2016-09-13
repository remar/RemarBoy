package se.remar.remarboy;

public class CPU {
    private boolean debug = false;

    private Memory mem;

    public int PC;
    public int SP;
    public int A, F;
    public int B, C;
    public int D, E;
    public int H, L;

    private final int ZF = 0x80; // Zero flag
    private final int NF = 0x40; // Negative op flag
    private final int HF = 0x20; // Half-carry flag
    private final int CF = 0x10; // Carry flag

    private int cycles = 0;

    private boolean IME = false;

    public CPU(Memory mem) {
        this.mem = mem;

        // Program starts at 0x100
        PC = 0x100;
        SP = 0xfffe;
    }

    public void step() {
        if(IME) {
            byte IF = mem.getByte(0xFF0F); // Interrupt Request
            byte IE = mem.getByte(0xFFFF); // Interrupt Enable
            int i = 0x01;
            int j = 0;
            while(i < 0x20) {
                if((IF & i) != 0 && (IE & i) != 0) {
                    IME = false;
                    mem.putByte(0xFF0F, IF & ~i); // Reset Interrupt Request bit
                    mem.putByte(SP-1, (PC & 0xff00) >> 8);
                    mem.putByte(SP-2, PC & 0x00ff);
                    SP -= 2;
                    PC = 0x0040 + j * 0x08; // Jump to interrupt routine
                    return;
                }
                i <<= 1;
                j++;
            }
        }

        byte op = mem.getOp(PC++);

        int BC;
        int DE;
        int HL;

        byte temp;
        int operand;
        int c;
        boolean carry;
        boolean halfcarry;

        if(debug) {
            System.out.println("-- OP: " + Util.formatByte(op));
        }

        //   0 1 2 3 4 5 6 7 8 9 A B C D E F
        // 0 x g . g g g g . . g . g g g g . 0
        // 1 . g . g g g g . . g . g g g g . 1
        // 2 g g . g g g g . g g x g g g g . 2
        // 3 g x x . . . g . g g . . g g g . 3
        // 4 g g g g g g g g g g g g g g g g 4
        // 5 g g g g g g g g g g g g g g g g 5
        // 6 g g g g g g g g g g g g g g g g 6
        // 7 . . . . . . . . g g g g g g g g 7
        // 8 g g g g g g g g g g g g g g g g 8
        // 9 . . . . . . . . . . . . . . . . 9
        // A g g g g g g g g g g g g g g g g A
        // B g g g g g g g g . . . . . . . . B
        // C g g g x . g . g g x g . . x . g C
        // D g g g   . g . g g . g   .   . g D
        // E x g x     g . g . x x       . g E
        // F x g . x   g . g . . . .     x g F
        //   0 1 2 3 4 5 6 7 8 9 A B C D E F

        switch(op) {

// --------- BEGIN GENERATED CODE ---------
// --------- END GENERATED CODE ---------

        case 0: // 0x00, NOP
            mem.cycles = 1;
            break;
        case 42: // 0x2A, LD A,(HL+)
            HL = (H & 0xff) * 0x100 + (L & 0xff);
            A = mem.getByte(HL);
            HL++;
            H = (HL & 0xff00) >> 8;
            L = (HL & 0x00ff);
            mem.cycles = 2;
            break;
        case 47: // 0x2F, CPL
            A = ~(A & 0xff);
            F = (F & (ZF | CF)) | NF | HF;
            mem.cycles = 1;
            break;
        case 49: // 0x31, LD SP,nn
            SP = mem.getWord(PC);
            PC += 2;
            mem.cycles = 3;
            break;
        case 50: // 0x32, LD (HL-),A
            HL = (H & 0xff) * 0x100 + (L & 0xff);
            mem.putByte(HL, A);
            HL = (HL - 1) & 0xffff;
            H = (HL & 0xff00) >> 8;
            L = (HL & 0x00ff);
            mem.cycles = 2;
            break;
        case -61: // 0xC3, JP nn
            PC = mem.getWord(PC);
            mem.cycles = 4;
            break;
        case -55: // 0xC9, RET
            PC = mem.getWord(SP);
            SP += 2;
            mem.cycles = 4;
            break;
        case -53: // 0xCB, CB prefix, 256 more opcodes!
            op = mem.getByte(PC++);

            //   0 1 2 3 4 5 6 7 8 9 A B C D E F
            // 0 . . . . . . . . . . . . . . . . 0
            // 1 . . . . . . . . . . . . . . . . 1
            // 2 . . . . . . . . . . . . . . . . 2
            // 3 g g g g g g g g . . . . . . . . 3
            // 4 . . . . . . . . . . . . . . . . 4
            // 5 . . . . . . . . . . . . . . . . 5
            // 6 . . . . . . . . . . . . . . . . 6
            // 7 . . . . . . . . . . . . . . . . 7
            // 8 g g g g g g g g g g g g g g g g 8
            // 9 g g g g g g g g g g g g g g g g 9
            // A g g g g g g g g g g g g g g g g A
            // B g g g g g g g g g g g g g g g g B
            // C . . . . . . . . . . . . . . . . C
            // D . . . . . . . . . . . . . . . . D
            // E . . . . . . . . . . . . . . . . E
            // F . . . . . . . . . . . . . . . . F
            //   0 1 2 3 4 5 6 7 8 9 A B C D E F

            switch(op) {
// --------- BEGIN GENERATED CB CODE ---------
// --------- END GENERATED CB CODE ---------
            case 0:
                break;
            default:
                System.out.println(this);
                throw new RuntimeException("Not implemented CB: " + op + " (" + Util.formatByte(op) + ")");
            }
            break;
        case -51: // 0xCD, CALL nn
            mem.putByte(SP-1, ((PC+2) & 0xff00) >> 8);
            mem.putByte(SP-2, (PC+2) & 0x00ff);
            SP -= 2;
            PC = mem.getWord(PC);
            mem.cycles = 6;
            break;
        case -32: // 0xE0, LD (0xff00 + n),A
            int offset = (mem.getByte(PC++) & 0xff);
            mem.putByte(0xff00 + offset, A);
            mem.cycles = 3;
            break;
        case -30: // 0xE2, LD (0xff00 + C),A
            mem.putByte(0xff00 + (C & 0xff), A);
            mem.cycles = 2;
            break;
        case -26: // 0xE6, AND n
            A = (A & 0xff) & (mem.getByte(PC++) & 0xff);
            F = (A == 0 ? ZF : 0) | HF;
            mem.cycles = 2;
            break;
        case -23: // 0xE9, JP (HL)
            PC = (H & 0xff) * 0x100 + (L & 0xff);
            mem.cycles = 1;
            break;
        case -22: // 0xEA, LD (nn),A
            int address = mem.getWord(PC);
            PC += 2;
            mem.putByte(address, A);
            mem.cycles = 4;
            break;
        case -16: // 0xF0, LD A,(0xff00 + n)
            offset = (mem.getByte(PC++) & 0xff);
            A = (mem.getByte(0xff00 + offset) & 0xff);
            mem.cycles = 3;
            break;
        case -13: // 0xF3, DI
            IME = false;
            mem.cycles = 1;
            break;
        case -5: // 0xFB, EI
            IME = true;
            mem.cycles = 1;
            break;
        case -2: // 0xFE, CP n
            int n = (mem.getByte(PC++) & 0xff);
            F = (A == n ? ZF : 0) | NF | ((A & 0xf) < (n & 0xf) ? HF : 0) | (A < n ? CF : 0);
            mem.cycles = 2;
            break;
        default:
            throw new RuntimeException("Not implemented: " + op + " (" + Util.formatByte(op) + ")");
        }
    }

    public String toString() {
        return "cycles: " + cycles + "\n"
                + "PC: " + Util.formatWord(PC) + ", SP: " + Util.formatWord(SP) + "\n"
                + "A: " + Util.formatByte(A) + ", F: " + Util.formatByte(F) + "\n"
                + "B: " + Util.formatByte(B) + ", C: " + Util.formatByte(C) + "\n"
                + "D: " + Util.formatByte(D) + ", E: " + Util.formatByte(E) + "\n"
                + "H: " + Util.formatByte(H) + ", L: " + Util.formatByte(L);
    }
}
