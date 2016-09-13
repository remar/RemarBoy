package se.remar.remarboy;

public class LCD {

    private Memory mem;
    private int cycles = 0;

    private static final int IF = 0xFF0F;
    private static final int LCDC = 0xFF40;
    private static final int LY = 0xFF44;

    private int ly = 0;

    public LCD(Memory mem) {
        this.mem = mem;

        ly = mem.getByte(LY);
    }

    public void step() {
        cycles += mem.cycles;

        while(cycles > 114) {
            cycles -= 114;
            ly++;
            if(ly == 144) {
                mem.putByte(IF, mem.getByte(IF) | 0x01); // Vblank Interrupt Request
            }
            if(ly >= 154) {
                ly = 0;
            }
            if(ly == 0) {
                mem.putByte(IF, mem.getByte(IF) & ~0x01); // Turn off Vblank Interrupt Request
            }
            mem.putByte(LY, ly);
        }
    }

    @Override
    public String toString() {
        return "LCD, LY: "+ly + " (0x" + Util.formatByte(ly) + ")";
    }
}
