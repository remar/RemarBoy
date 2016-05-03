# Memory map for the Game Boy (DMG)

# 64k accessible memory

#	$0000-$3FFF 	Cartridge ROM - Bank 0 (fixed)
#	  $0000-$00FF 	Restart and Interrupt Vectors
#	  $0100-$014F 	Cartridge Header Area
#	$4000-$7FFF 	Cartridge ROM - Switchable Banks 1-xx
#       $8000-$9FFF     VRAM
#	  $8000-$97FF 	Character RAM
#	  $9800-$9BFF 	BG Map Data 1
#	  $9C00-$9FFF 	BG Map Data 2
#	$A000-$BFFF 	Cartridge RAM (If Available)
#	$C000-$CFFF 	Internal RAM - Bank 0 (fixed)
#	$D000-$DFFF 	Internal RAM - Bank 1-7 (switchable - CGB only)
#	$E000-$FDFF 	Echo RAM - Reserved, Do Not Use
#	$FE00-$FE9F 	OAM - Object Attribute Memory
#	$FEA0-$FEFF 	Unusable Memory
#	$FF00-$FF7F 	Hardware I/O Registers
#	$FF80-$FFFE 	Zero Page - 127 bytes
#       $FFFF 	        Interrupt Enable Flag

# First implementation without bank switching (MBC)

class MemoryMap(object):
    def __init__(self):
        self.rom = None          # $0000-$7FFF
        self.ram = {0xff44: 148} # $8000-$FFFF
        self.bank_lower = 1
        self.bank_higher = 0
        self.rom_ram_select = 0

    def insertCart(self, filename):
        self.rom = open(filename, "rb").read()

    def get_rom_address(self, address):
        if address >= 0x0000 and address <= 0x3fff:
            return address
        else:
            return address + (self.bank_lower - 1) * 0x4000

    def get(self, address):
        if address >= 0x0000 and address <= 0x3fff:
            return ord(self.rom[address])
        elif address >= 0x4000 and address <= 0x7fff:
            return ord(self.rom[address + (self.bank_lower - 1) * 0x4000])
        elif address >= 0x8000 and address <= 0xffff:
            return self.ram[address] if address in self.ram else 0

        raise Exception, "Out of bounds reading from %x" % address

    def get_word(self, address):
        return (self.get(address), self.get(address+1))

    def put(self, address, value):
        if address >= 0x8000:
            self.ram[address] = value
        elif address >= 0x2000 and address <= 0x3fff:
            self.bank_lower = value & 0x1f
            print "Selected bank (lower): %d" % self.bank_lower
        elif address >= 0x4000 and address <= 0x5fff:
            self.bank_higher = value & 0x03
            print "Selected bank (higher): %d" % self.bank_higher
        elif address >= 0x6000 and address <= 0x7fff:
            self.rom_ram_select = value & 0x01
        else:
            raise Exception, "Out of bounds writing to %x" % address
