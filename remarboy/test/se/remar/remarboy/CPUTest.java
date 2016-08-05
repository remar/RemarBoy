package se.remar.remarboy;

import static org.junit.Assert.*;
import junit.framework.TestCase;
import org.junit.Test;

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

	public void testTakeOneCPUStep() {
		cpu.step();
	}
}
