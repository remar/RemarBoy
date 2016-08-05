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
        byte op = mem.getOp(PC++);

        if(debug) {
            System.out.println("-- OP: " + Util.formatByte(op));
        }

        //   0 1 2 3 4 5 6 7 8 9 A B C D E F
        // 0 x . . . . x x . . . . . x x x . 0
        // 1 . . . . . . . . . . . . . . . . 1
        // 2 x x . . . . . . . . x . . . . . 2
        // 3 . x x . . . x . . . . . . . x . 3
        // 4 . . . . . . . . . . . . . . . . 4
        // 5 . . . . . . . . . . . . . . . . 5
        // 6 . . . . . . . . . . . . . . . . 6
        // 7 . . . . . . . . . . . . . . . . 7
        // 8 . . . . . . . . . . . . . . . . 8
        // 9 . . . . . . . . . . . . . . . . 9
        // A . . . . . . . . . . . . . . . x A
        // B . . . . . . . . . . . . . . . . B
        // C . . . x . . . . . . . . . x . . C
        // D . . . . . . . . . . . . . . . . D
        // E x . x . . . . . . . x . . . . . E
        // F x . . x . . . . . . . . . . x . F
        //   0 1 2 3 4 5 6 7 8 9 A B C D E F

        switch(op) {
// --------- BEGIN GENERATED CODE ---------
// --------- END GENERATED CODE ---------
        case 0: // 0x00, NOP
            cycles += 1;
            break;
        case 5: // 0x05, DEC B
            B = (B - 1) & 0xff;
            F = (F & CF) | (B == 0 ? ZF : 0) | NF | ((B & 0xf) == 0xf ? HF : 0);
            cycles += 1;
            break;
        case 6: // 0x06, LD B,n
            B = mem.getByte(PC++);
            cycles += 2;
            break;
        case 12: // 0x0C, INC C
            C++;
            C &= 0xff;
            F = (F & CF) | (C == 0 ? ZF : 0) | (C == 0x10 ? HF : 0);
            cycles += 1;
            break;
        case 13: // 0x0D, DEC C
            C = (C - 1) & 0xff;
            F = (F & CF) | (C == 0 ? ZF : 0) | NF | ((C & 0xf) == 0xf ? HF : 0);
            cycles += 1;
            break;
        case 14: // 0x0E, LD C,n
            C = mem.getByte(PC++);
            cycles += 2;
            break;
        case 32: // 0x20, JR NZ,n
            if((F & ZF) != ZF) {
                byte offset = mem.getByte(PC++);
                PC += offset;
                cycles += 3;
            } else {
                PC++;
                cycles += 2;
            }
            break;
        case 33: // 0x21, LD HL,nn
            int word = mem.getWord(PC);
            PC += 2;
            H = (word & 0xff00) >> 8;
            L = word & 0xff;
            cycles += 3;
            break;
        case 42: // 0x2A, LD A,(HL+)
            int HL = H * 256 + L;
            A = mem.getByte(HL);
            HL++;
            H = (HL & 0xff00) >> 8;
            L = (HL & 0x00ff);
            cycles += 2;
            break;
        case 49: // 0x31, LD SP,nn
            SP = mem.getWord(PC);
            PC += 2;
            cycles += 3;
            break;
        case 50: // 0x32, LD (HL-),A
            HL = H * 256 + L;
            mem.putByte(HL, A);
            HL = (HL - 1) & 0xffff;
            H = (HL & 0xff00) >> 8;
            L = (HL & 0x00ff);
            cycles += 2;
            break;
        case 54: // 0x36, LD (HL),n
            int n = mem.getByte(PC++);
            mem.putByte((H & 0xff) * 0x100 + (L & 0xff), n);
            cycles += 3;
            break;
        case 62: // 0x3E, LD A,n
            A = mem.getByte(PC++);
            cycles += 1;
            break;
        case -81: // 0xAF, XOR A
            A = 0;
            F = (A == 0 ? ZF : 0);
            cycles += 1;
            break;
        case -61: // 0xC3, JP nn
            PC = mem.getWord(PC);
            cycles += 4;
            break;
        case -51: // 0xCD, CALL nn
            mem.putByte(SP-1, ((PC+2) & 0xff00) >> 8);
            mem.putByte(SP-2, (PC+2) & 0x00ff);
            SP -= 2;
            PC = mem.getWord(PC);
            cycles += 6;
            break;
        case -32: // 0xE0, LD (0xff00 + n),A
            int offset = (mem.getByte(PC++) & 0xff);
            mem.putByte(0xff00 + offset, A);
            cycles += 3;
            break;
        case -30: // 0xE2, LD (0xff00 + C),A
            mem.putByte(0xff00 + (C & 0xff), A);
            cycles += 2;
            break;
        case -22: // 0xEA, LD (nn),A
            int address = mem.getWord(PC);
            PC += 2;
            mem.putByte(address, A);
            cycles += 4;
            break;
        case -16: // 0xF0, LD A,(0xff00 + n)
            offset = (mem.getByte(PC++) & 0xff);
            A = (mem.getByte(0xff00 + offset) & 0xff);
            cycles += 3;
            break;
        case -13: // 0xF3, DI
            IME = false;
            cycles += 1;
            break;
        case -2: // 0xFE, CP n
            n = (mem.getByte(PC++) & 0xff);
            F = (A == n ? ZF : 0) | NF | ((A & 0xf) < (n & 0xf) ? HF : 0) | (A < n ? CF : 0);
            cycles += 2;
            break;
        default:
            System.out.println(this);
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
