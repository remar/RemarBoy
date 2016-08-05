package se.remar.remarboy;

public class CartMock extends Cart {
    public CartMock(int size) {
        rom = new byte[size];
    }

    public void putByte(int address, int data) {
        rom[address] = (byte)data;
    }

    public void putBytes(int address, int data1, int data2) {
        rom[address] = (byte)data1;
        rom[address + 1] = (byte)data2;
    }

    public void putBytes(int address, int data1, int data2, int data3) {
        rom[address] = (byte)data1;
        rom[address + 1] = (byte)data2;
        rom[address + 2] = (byte)data3;
    }
}
