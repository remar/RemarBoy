package se.remar.remarboy;

import java.util.HashSet;
import java.util.Set;

// Memory map
// 0x0000-0x7FFF    Game rom (32 kB)
// 0x8000-0x9FFF    VRAM (8 kB)
// 0xA000-0xBFFF    External Expansion Working RAM (8 kB)
// 0xC000-0xDFFF    Unit Working RAM (8 kB)
// 0xE000-0xFDFF    (Use prohibited)
// 0xFE00-0xFE9F    OAM (160 bytes)
// 0xFF00-0xFF7F    Port/Mode/Control/Sound Registers
// 0xFF80-0xFFFE    Working & Stack RAM (127 bytes)
// 0xFFFF           Interrupt Enable

public class Memory {
    public int cycles;

    private Cart cart;

    private byte[] ram = new byte[0x8000];
    private byte[] metarom;

    private Set<Integer> visited = new HashSet<Integer>();

    private static final byte VISITED = 1;

    public void insertCart(String path) {
        addCart(new Cart(path));
    }

    public void addCart(Cart cart) {
        this.cart = cart;
        // TODO: Metarom will have to be more competent than this when dealing with MBCs...
        metarom = new byte[Math.max(cart.size(), 65536)];
    }

    public byte getOp(int address) {
        // TODO: Keep rom and ram separate in metarom (yes, code will be placed in ram from rom...)
        metarom[address] = VISITED;
        return getByte(address);
    }

    public byte getByte(int address) {
        if(address < 0x8000) {
            return cart.get(address);
        } else {
            return ram[address - 0x8000];
        }
    }

    public int getWord(int i) {
        return (getByte(i) & 0xff) + (getByte(i+1) & 0xff) * 0x100;
    }

    public void putByte(int address, int data) {
        if(address >= 0x8000) { // Ignore other writes for now
            ram[address - 0x8000] = (byte) (data & 0xff);
        } else {
            System.out.println(String.format("Caught write to address 0x%x", address) + ", data: " + Util.formatByte(data));
        }
    }

    public Set<Integer> getVisited() {
        visited.clear();
        for(int i = 0;i < metarom.length;i++) {
            if(metarom[i] == VISITED) {
                visited.add(i);
            }
        }
        return visited;
    }

    public byte[] getBytes(int address, int length) {
        byte[] bytes = new byte[length];
        for(int i = 0;i < length;i++) {
            bytes[i] = getByte(address + i);
        }
        return bytes;
    }
}
