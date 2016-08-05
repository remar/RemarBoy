package se.remar.remarboy;

public class Util {
    public static String formatWord(int word) {
        return formatByte((word & 0xff00) >> 8)
                + formatByte(word & 0xff);
    }

    public static String formatByte(int b) {
        int high = (b & 0xf0) >> 4;
        int low = b & 0x0f;
        return formatNibble(high) + formatNibble(low);
    }

    private static String formatNibble(int nibble) {
        String[] mapping = {"0", "1", "2", "3", "4", "5", "6", "7",
                            "8", "9", "A", "B", "C", "D", "E", "F"};
        return mapping[nibble];
    }
}
