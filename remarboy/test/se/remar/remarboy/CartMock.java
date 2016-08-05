package se.remar.remarboy;

public class CartMock extends Cart {
	public CartMock(int size) {
		rom = new byte[size];
	}

	public void putByte(int address, int data) {
		rom[address] = (byte)data;
	}
}
