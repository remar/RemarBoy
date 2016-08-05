package se.remar.remarboy;

import junit.framework.TestCase;

public class CPUTest extends TestCase {
    CartMock cart;
    Memory mem;
    CPU cpu;

    public void setUp() {
        cart = new CartMock(0x8000);
        mem = new Memory();
        mem.addCart(cart);
        cpu = new CPU(mem);
    }

    public void tearDown() {
        mem = null;
        cpu = null;
    }

    // DEC B, 0x05 /////////////////////////////////////////////////
    public void testDecB() {
        decBTest(0xff, 0xfe, 0x40);
    }

    public void testDecBUnderflow() {
        decBTest(0x00, 0xff, 0x60);
    }

    public void testDecBToZero() {
        decBTest(0x01, 0x00, 0xc0);
    }

    private void decBTest(int B, int expectedB, int expectedF) {
        cpu.B = B;
        cpu.F = 0x00;
        cart.putByte(0x100, 0x05);
        cpu.step();
        assertEquals(expectedB, cpu.B);
        assertEquals(expectedF, cpu.F);
    }

    public void testCall() {
        cart.putBytes(0x100, 0xcd, 0x40, 0x20);
        cpu.step();
        assertEquals(0x2040, cpu.PC);
        assertEquals(0x01, mem.getByte(0xfffd));
        assertEquals(0x03, mem.getByte(0xfffc));
        assertEquals(0xfffc, cpu.SP);
    }
}
