class Instruction(object):
    def __init__(self, symbolic=None, bytes_=None):
        self.symbolic = symbolic
        self.bytes_ = bytes_

    def __str__(self):
        return "%-15s (%s)" % (self.symbolic,
                               ", ".join(hex2(x) for x in self.bytes_))

    def to_json(self):
        return {"sym":self.symbolic, "bytes":self.bytes_}

    def from_json(self, obj):
        self.symbolic = obj["sym"]
        self.bytes_ = obj["bytes"]
        return self

regmap = {0:"B", 1:"C", 2:"D", 3:"E", 4:"H", 5:"L", 6:"(HL)", 7:"A"}

multi_codes = {
    # mnemonic: (length, [op codes])
    "INC %s": (1, [0x04,0x0c,0x14,0x1c,0x24,0x2c,0x34,0x3c]),
    "DEC %s": (1, [0x05,0x0d,0x15,0x1d,0x25,0x2d,0x35,0x3d]),
    "LD %s,%%s": (2, [0x06,0x0e,0x16,0x1e,0x26,0x2e,0x36,0x3e])
}

codes = {
    0x00: ("NOP", 1),
    0x01: ("LD BC,%s", 3),
    0x0b: ("DEC BC", 1),

    0x11: ("LD DE,%s", 3),
    0x12: ("LD (DE),A", 1),
    0x13: ("INC DE", 1),
    0x18: ("JR %s", 2),
    0x19: ("ADD HL,DE", 1),
    0x1a: ("LD A,(DE)", 1),

    0x20: ("JR NZ,%s", 2),
    0x21: ("LD HL,%s", 3),
    0x22: ("LD (HL+),A", 1),
    0x23: ("INC HL", 1),
    0x28: ("JR Z,%s", 2),
    0x2a: ("LD A,(HL+)", 1),
    0x2f: ("CPL", 1),

    0x31: ("LD SP,%s", 3),
    0x32: ("LD (HL-),A", 1),

    0x76: ("HALT", 1),

    0xa8: ("XOR B", 1),
    0xa9: ("XOR C", 1),
    0xaa: ("XOR D", 1),
    0xab: ("XOR E", 1),
    0xac: ("XOR H", 1),
    0xad: ("XOR L", 1),
    0xae: ("XOR (HL)", 1),
    0xaf: ("XOR A", 1),

    0xc0: ("RET NZ", 1),
    0xc1: ("POP BC", 1),
    0xc3: ("JP %s", 3),
    0xc5: ("PUSH BC", 1),
    0xc8: ("RET Z", 1),
    0xc9: ("RET", 1),
    0xca: ("JP Z,%s", 3),
    0xcd: ("CALL %s", 3),

    0xd1: ("POP DE", 1),
    0xd5: ("PUSH DE", 1),
    0xd9: ("RETI", 1),

    0xe0: ("LDH (%s),A", 2),
    0xe1: ("POP HL", 1),
    0xe2: ("LDH (C),A", 1),
    0xe5: ("PUSH HL", 1),
    0xe6: ("AND %s", 2),
    0xe9: ("JP (HL)", 1),
    0xea: ("LD (%s),A", 3),
    0xef: ("RST 28H", 1),

    0xf0: ("LDH A,(%s)", 2),
    0xf1: ("POP AF", 1),
    0xf3: ("DI", 1),
    0xf5: ("PUSH AF", 1),
    0xfa: ("LD A,(%s)", 3),
    0xfb: ("EI", 1),
    0xfe: ("CP %s", 2),
    0xff: ("RST 38H", 1)
}

def disassemble(address, mem):
    op = mem.get(address)

    mnemonic = ""
    length = 1

    for mne in multi_codes.keys():
        if op in multi_codes[mne][1]:
            target = regmap[multi_codes[mne][1].index(op)]
            mnemonic = mne % target
            length = multi_codes[mne][0]
            break

    if not mnemonic:
        if op in codes:
            mnemonic = codes[op][0]
            length = codes[op][1]

    if op >= 0x40 and op <= 0x7f:
        target = regmap[((op & 0xf8) - 0x40) / 0x08]
        source = regmap[op & 0x07]
        mnemonic = "LD %s,%s" % (target, source)

    if op >= 0x80 and op <= 0xbf:
        method = ["ADD","ADC","SUB","SBC","AND","XOR","OR","CP"][(op-0x80)/8]
        source = regmap[op & 0x07]
        mnemonic = method + " " + source

    if op == 0xcb: # CB PREFIX
        cb_op = mem.get(address+1)
        return Instruction(disassemble_cb(cb_op), [0xcb, cb_op])

    if not mnemonic:
        raise Exception, "Unable to disassemble 0x%02x" % op

    if length == 1:
        return Instruction(mnemonic, [op])
    elif length == 2:
        operand = mem.get(address+1)
        return Instruction(mnemonic % d8(operand), [op, operand])
    elif length == 3:
        low, high = mem.get_word(address+1)
        operand = high * 0x100 + low
        return Instruction(mnemonic % d16(operand), [op, low, high])

    raise Exception, "Unable to disassemble 0x%02x" % op

def disassemble_cb(op):
    methods = {0x00: "RLC ", 0x08: "RRC ",
               0x10: "RL ", 0x18: "RR",
               0x20: "SLA ", 0x28: "SRA ",
               0x30: "SWAP ", 0x38: "SRL ",
               0x40: "BIT 0,", 0x48: "BIT 1,",
               0x50: "BIT 2,", 0x58: "BIT 3,",
               0x60: "BIT 4,", 0x68: "BIT 5,",
               0x70: "BIT 6,", 0x78: "BIT 7,",
               0x80: "RES 0,", 0x88: "RES 1,",
               0x90: "RES 2,", 0x98: "RES 3,",
               0xa0: "RES 4,", 0xa8: "RES 5,",
               0xb0: "RES 6,", 0xb8: "RES 7,",
               0xc0: "SET 0,", 0xc8: "SET 1,",
               0xd0: "SET 2,", 0xd8: "SET 3,",
               0xe0: "SET 4,", 0xe8: "SET 5,",
               0xf0: "SET 6,", 0xf8: "SET 7,"}
    return methods[op & 0xf8] + regmap[op & 0x07]

def hex2(i):
    return format(i, '#04x')

def d8(i):
    return "$" + format(i, '02x')

def d16(i):
    return "$" + format(i, '04x')
