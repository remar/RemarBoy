package se.remar.remarboy;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

public class Cart {
	private byte[] rom;

	public Cart(String path) {
		loadRom(path);
	}

	public byte get(int i) {
		return rom[i];
	}

	public int size() {
		return rom.length;
	}

	private void loadRom(String path) {
		try {
			RandomAccessFile f = new RandomAccessFile(path, "r");
			rom = new byte[(int)f.length()];
			f.readFully(rom);
			f.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
