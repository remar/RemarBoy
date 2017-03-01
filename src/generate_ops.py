#!/usr/bin/env python3

from shutil import copyfile

# "Normal" op codes

def generate_jr_cond(op):
    def flag(op):
        return "CF" if op & 0x10 == 0x10 else "ZF"
    def cond(op):
        return "==" if op & 0x08 == 0x08 else "!="
    m = {0x20: "NZ", 0x28: "Z", 0x30: "NC", 0x38: "C"}
    return make_case(op, "JR "+m[op]+",n") + [
        indent(2), "if((AF.low & ", flag(op), ") ", cond(op), " ",
        flag(op), ") {", nl(),
        indent(3), "PC += (signed char)mem->getByte(PC) + 1;", nl(),
        indent(3), "mem->cycles = 3;", nl(),
        indent(2), "} else {", nl(),
        indent(3), "PC++;", nl(),
        indent(3), "mem->cycles = 2;", nl(),
        indent(2), "}", nl(),
        indent(2), "break;", nl()
    ]

def generate_ld_rr_nn(op):
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    return make_case(op, "LD " + r1 + r2 + ",nn") + [
        indent(2), r1, r2, ".low = mem->getByte(PC++);", nl(),
        indent(2), r1, r2, ".high = mem->getByte(PC++);", nl()
    ] + make_cycles_and_break(3)

def generate_ld_indexed(op):
    def get_wide(n):
        return ["BC", "DE", "HL", "HL"][n]
    wide = get_wide((op & 0x30) // 16)
    s = "" if (op & 0x20) == 0 else ("+" if (op & 0x10 == 0) else "-")
    mne = "("+wide+s+"),A" if (op & 0x0a) == 0x02 else "A,("+wide+s+")"
    return make_case(op, "LD " + mne) + (
        [
            indent(2), "AF.high = mem->getByte(", wide, ".word);", nl()
        ] if (op & 0x08) == 0x08 else [
            indent(2), "mem->putByte(", wide, ".word, AF.high);", nl()
        ]
    ) + ([] if s == "" else [indent(2), "HL.word",s,s,";", nl()]
    ) + make_cycles_and_break(2)

def generate_inc_rr(op):
    wide = ["BC", "DE", "HL"][(op & 0x30) // 16]
    return make_case(op, "INC " + wide) + [
        indent(2), wide, ".word++;", nl()
    ] + make_cycles_and_break(2)

def generate_ld_r_n(op):
    r = get_reg((op & 0x38) // 8)
    reg_name = get_reg_name((op & 0x38) // 8)
    if(r == "(HL)"):
        return make_case(op, "LD (HL),n") + [
            indent(2), "mem->putByte(HL.word, mem->getByte(PC++));", nl()
        ] + make_cycles_and_break(3)
    else:
        return make_case(op, "LD " + reg_name + ",n") + [
            indent(2), r, " = mem->getByte(PC++);", nl()
        ] + make_cycles_and_break(2)

def generate_add_hl_rr(op):
    def inner(rr):
        def gethl():
            return indent(2) + "HL = " + make_word("H", "L") + ";" + nl()
        def hc(rr):
            return (indent(2) + "halfcarry = (HL.word & 0x0f00) + (" + rr
                    + " & 0x0f00) > 0x0f00;" + nl())
        def carry(rr):
            return (indent(2) + "carry = (HL.word + " + rr + ") > 0xffff;" + nl())
        flags = (indent(2)
                 + "AF.low = (AF.low & ZF) | (halfcarry ? HF : 0) | (carry ? CF : 0);"
                 + nl())
        if rr == 3: # SP
            return [hc("SP"),
                    carry("SP"),
                    indent(2), "HL.word += SP;", nl(),
                    flags]
        elif rr == 2: # HL
            return [hc("HL.word"),
                    indent(2), "carry = HL.word > 0x7fff;", nl(),
                    indent(2), "HL.word += HL.word;", nl(),
                    flags]
        else: # BC or DE
            return [
                hc(r1+r2+".word"),
                carry(r1+r2+".word"),
                indent(2), "HL.word += ",r1,r2,".word;", nl(),
                flags
            ]
    rr = (op & 0x30) // 16
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    rr_name = "SP" if rr == 3 else (r1 + r2)
    return (make_case(op, "ADD HL," + rr_name) + inner(rr) + make_cycles_and_break(2))

def generate_dec_rr(op):
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    return make_case(op, "DEC " + r1 + r2) + [
        indent(2), r1+r2+".word--;", nl()
    ] + make_cycles_and_break(2)

def generate_inc_r(op):
    reg_name = get_reg_name((op & 0x38) // 8)
    reg = get_reg((op & 0x38) // 8)
    return make_case(op, "INC " + reg_name) + [
        indent(2), reg, "++;", nl(),
        indent(2), "AF.low = (AF.low & CF) | (r == 0 ? ZF : 0) | (r == 0x10 ? HF : 0);".replace("r", reg), nl()] + make_cycles_and_break(1)

def generate_dec_r(op):
    reg = get_reg((op & 0x38) // 8)
    reg_name = get_reg_name((op & 0x38) // 8)
    return make_case(op, "DEC " + reg_name) + [
            indent(2), reg, "--;", nl(),
            indent(2), "AF.low = (AF.low & CF) | (r == 0 ? ZF : 0) | NF | ((r & 0xf) == 0xf ? HF : 0);".replace("r",reg), nl()] + make_cycles_and_break(1)

def generate_ld_r_r(op):
    dest = get_reg((op & 0x38) // 8)
    dest_name = get_reg_name((op & 0x38) // 8)
    src = get_reg(op & 0x07) if (op & 0x07) != 0x06 else "(HL)"
    src_name = get_reg_name(op & 0x07) # if (op & 0x07) != 0x06 else "(HL)"
    if src == "(HL)":
        return make_case(op, "LD " + dest_name + "," + src_name) + [
            indent(2), dest, " = mem->getByte(HL.word);", nl()
        ] + make_cycles_and_break(2)
    elif dest == src:
        return make_case(op, "LD " + dest_name + "," + src_name) + make_cycles_and_break(1)
    else:
        return make_case(op, "LD " + dest_name + "," + src_name) + [
            indent(2), dest, " = ", src, ";", nl()
        ] + make_cycles_and_break(1)

def generate_ld_hl_r(op):
    src = get_reg(op & 0x07)
    src_name = get_reg_name(op & 0x07)
    return make_case(op, "LD (HL)," + src_name) + [
        indent(2), "mem->putByte(HL.word, " + src + ");", nl()
    ] + make_cycles_and_break(2)

def generate_add(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg_name(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "ADD " + r_name) + [
        indent(2), "n = " + r + ";", nl(),
        indent(2), "halfcarry = (AF.high & 0x0f) + (n & 0x0f) > 0x0f;", nl(),
        indent(2), "carry = (AF.high + n) > 0xff;", nl(),
        indent(2), "AF.high += n;", nl(),
        indent(2), "AF.low = (AF.high == 0 ? ZF : 0) | (carry ? CF : 0) | (halfcarry ? HF : 0);", nl()
    ] + make_cycles_and_break(cycles)

def generate_adc(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg_name(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "ADC " + r_name) + [
        indent(2), "n = (AF.low & CF) >> 4;", nl(),
        indent(2), "operand = " + r + ";", nl(),
        indent(2), "halfcarry = (AF.high & 0x0f) + (operand & 0x0f) + n > 0x0f;", nl(),
        indent(2), "carry = AF.high + operand + n > 0xff;", nl(),
        indent(2), "AF.high = AF.high + operand + n;", nl(),
        indent(2), "AF.low = (AF.high == 0 ? ZF : 0) | (carry ? CF : 0) | (halfcarry ? HF : 0);", nl()
    ] + make_cycles_and_break(cycles)

def generate_and(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg_name(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "AND " + r_name) + (
        [indent(2), "AF.high &= " + r + ";", nl()] if r != "AF.high" else []
    ) + [indent(2), "AF.low = (AF.high == 0 ? ZF : 0) | HF;", nl()
    ] + make_cycles_and_break(cycles)

def generate_xor(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg_name(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "XOR " + r_name) + [
        indent(2),
        "AF.high ^= " + r + ";" if r != "AF.high" else "AF.high = 0;", nl(),
        indent(2), "AF.low = (AF.high ? 0 : ZF);", nl()
    ] + make_cycles_and_break(cycles)

def generate_or(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg_name(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "OR " + r_name) + ([
        indent(2), "AF.high |= " + r + ";",
        nl() ] if r != "AF.high" else []) + [
            indent(2), "AF.low = AF.high == 0 ? ZF : 0;", nl()
        ] + make_cycles_and_break(cycles)

def generate_ret_cond(op):
    def flag(op):
        return "CF" if op & 0x10 == 0x10 else "ZF"
    def cond(op):
        return "==" if op & 0x08 == 0x08 else "!="
    m = {0xc0: "NZ", 0xc8: "Z", 0xd0: "NC", 0xd8: "C"}
    return make_case(op, "RET "+m[op]) + [
        indent(3), "if((AF.low & ", flag(op), ") ", cond(op), " ",
        flag(op), ") {", nl(),
        indent(4), "PC = mem->getWord(SP);", nl(),
        indent(4), "SP += 2;", nl(),
        indent(4), "mem->cycles = 5;", nl(),
        indent(3), "} else {", nl(),
        indent(4), "mem->cycles = 3;", nl(),
        indent(3), "}", nl(),
        indent(3), "break;", nl()
    ]

def generate_pop_rr(op):
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    return make_case(op, "POP " + r1 + r2) + [
        indent(2), r1, r2, ".low = mem->getByte(SP++);", nl(),
        indent(2), r1, r2, ".high = mem->getByte(SP++);", nl()
    ] + make_cycles_and_break(3)

def generate_jp_cond(op):
    def flag(op):
        return "CF" if op & 0x10 == 0x10 else "ZF"
    def cond(op):
        return "==" if op & 0x08 == 0x08 else "!="
    m = {0xc2: "NZ", 0xca: "Z", 0xd2: "NC", 0xda: "C"}
    return make_case(op, "JP "+m[op]+",nn") + [
        indent(2), "if((AF.low & ", flag(op), ") ", cond(op), " ",
        flag(op), ") {", nl(),
        indent(3), "PC = mem->getWord(PC);", nl(),
        indent(3), "mem->cycles = 4;", nl(),
        indent(2), "} else {", nl(),
        indent(3), "PC += 2;", nl(),
        indent(3), "mem->cycles = 3;", nl(),
        indent(2), "}", nl(),
        indent(2), "break;", nl()
    ]

def generate_push_rr(op):
    wide = ["BC", "DE", "HL", "AF"][(op & 0x30) // 16]
    return make_case(op, "PUSH " + wide) + [
        indent(2), "mem->putByte(SP-1, ", wide, ".high);", nl(),
        indent(2), "mem->putByte(SP-2, ", wide, ".low);", nl(),
        indent(2), "SP -= 2;", nl()
    ] + make_cycles_and_break(4)

def generate_rst(op):
    dest = (op & 0x38) // 8
    return make_case(op, "RST " + str(dest) + " (0x" + format(dest*8, "02x") + ")") + [
        indent(2), "mem->putByte(SP-1, (PC & 0xff00) >> 8);", nl(),
        indent(2), "mem->putByte(SP-2, PC & 0x00ff);", nl(),
        indent(2), "SP -= 2;", nl(),
        indent(2), "PC = 0x", format(dest*8, "02x"), ";", nl()
    ] + make_cycles_and_break(4)

# CB op codes
def generate_sla(op):
    if op & 0x07 == 0x06: # (HL)
        return make_cb_case(op, "SLA (HL)") + [
            indent(2), "temp = mem->getByte(HL.word);", nl(),
            indent(2), "carry = temp == 0x80;", nl(),
            indent(2), "temp <<= 1;", nl(),
            indent(2), "mem->putByte(HL.word, temp);", nl(),
            indent(2), "AF.low = (carry ? CF : 0) | (temp == 0 ? ZF : 0);", nl()
        ] + make_cb_cycles_and_break(4)
    else:
        reg = get_reg(op & 0x07)
        reg_name = get_reg_name(op & 0x07)
        return make_cb_case(op, "SLA " + reg_name) + [
            indent(2), "carry = ", reg, " == 0x80;", nl(),
            indent(2), reg, " <<= 1;", nl(),
            indent(2), "AF.low = (carry ? CF : 0) | (", reg, " == 0 ? ZF : 0);", nl()
        ] + make_cb_cycles_and_break(2)

def generate_swap(op):
    def swap(s):
        return "((s & 0x0f) << 4) | ((s & 0xf0) >> 4)".replace("s", s)

    if op & 0x07 == 0x06:
        # (HL)
        return make_cb_case(op, "SWAP (HL)") + [
            indent(2), "temp = mem->getByte(HL.word);", nl(),
            indent(2), "mem->putByte(HL.word, "+swap("temp")+");", nl(),
            indent(2), "AF.low = (temp == 0 ? ZF : 0);", nl()
        ] + make_cb_cycles_and_break(4)
    else:
        r = get_reg(op & 0x07)
        reg_name = get_reg_name(op & 0x07)
        return make_cb_case(op, "SWAP " + reg_name) + [
            indent(2), ("r = "+swap("r")+";")
            .replace("r", r), nl(),
            indent(2), "AF.low = (r == 0 ? ZF : 0);".replace("r", r), nl()
        ] + make_cb_cycles_and_break(2)

def generate_srl(op):
    if op & 0x07 == 0x06: # (HL)
        return make_cb_case(op, "SLA (HL)") + [
            indent(2), "temp = mem->getByte(HL.word);", nl(),
            indent(2), "carry = temp == 0x01;", nl(),
            indent(2), "temp >>= 1;", nl(),
            indent(2), "mem->putByte(HL.word, temp);", nl(),
            indent(2), "AF.low = (carry ? CF : 0) | (temp == 0 ? ZF : 0);", nl()
        ] + make_cb_cycles_and_break(4)
    else:
        reg = get_reg(op & 0x07)
        reg_name = get_reg_name(op & 0x07)
        return make_cb_case(op, "SLA " + reg_name) + [
            indent(2), "carry = ", reg, " == 0x01;", nl(),
            indent(2), reg, " >>= 1;", nl(),
            indent(2), "AF.low = (carry ? CF : 0) | (", reg, " == 0 ? ZF : 0);", nl()
        ] + make_cb_cycles_and_break(2)

def generate_bit(op):
    bit = (op - 0x40)//8
    r = get_reg(op & 0x07) if (op & 0x07 != 0x06) else "mem->getByte(HL.word)"
    reg_name = get_reg_name(op & 0x07)
    cycles = 3 if (op & 0x07) == 0x06 else 2
    return make_cb_case(op, "BIT " + str(bit) + "," + reg_name) + [
        indent(2), "AF.low = (AF.low & CF) | HF | (", r, " & 0x",
        format(1 << bit, "02x"), " ? 0 : ZF);", nl()
    ] + make_cb_cycles_and_break(cycles)

def generate_res(op):
    bit = (op - 0x80)//8
    r = get_reg(op & 0x07)
    reg_name = get_reg_name(op & 0x07)
    if(op & 0x07 == 0x06): # (HL)
        return make_cb_case(op, "RES " + str(bit) + ",(HL)") + [
            indent(2), "mem->putByte(HL.word, mem->getByte(HL.word) & 0x",
            format((0xff - (1 << bit)), "02x"), ");", nl()
        ] + make_cb_cycles_and_break(4)
    else:
        return make_cb_case(op, "RES " + str(bit) + "," + reg_name) + [
            indent(2), r, " &= 0x", format((0xff - (1 << bit)), "02x"),
            ";", nl()
        ] + make_cb_cycles_and_break(2)

def get_hl():
    return "mem->getByte(HL.word)"

def make_word(h, l):
    return "(" + h + " << 8) + " + l

def get_reg(r):
    return {0: "BC.high", 1: "BC.low", 2: "DE.high", 3: "DE.low", 4:"HL.high", 5:"HL.low", 6:"(HL)", 7:"AF.high"}[r]

def get_reg_name(r):
    return {0: "B", 1: "C", 2: "D", 3: "E", 4:"H", 5:"L", 6:"(HL)", 7:"A"}[r]

def get_wide_reg(r):
    return {0:("B", "C"), 1:("D", "E"), 2:("H", "L"), 3:("A", "F")}[r]

def make_case(op, title):
    return [indent(1), "case ", make_op(op), ": // ", title, nl()]

def make_cb_case(op, title):
    return [indent(1), "case ", make_op(op), ": // ", title, nl()]

def make_cycles_and_break(cycles):
    return [indent(2), "mem->cycles = "+str(cycles)+";", nl(),
            indent(2), "break;", nl()]

def make_cb_cycles_and_break(cycles):
    return [indent(2), "mem->cycles = "+str(cycles)+";", nl(),
            indent(2), "break;", nl()]

def make_op(op):
    return "0x" + format(op, "02X")

def indent(n):
    return " "*2*n

def nl():
    return "\n"

def generate_opcodes():
    ops = []

    for op in [0x01, 0x11, 0x21]:
        ops.extend(generate_ld_rr_nn(op))

    for op in [0x02, 0x0a, 0x12, 0x1a, 0x22, 0x2a, 0x32, 0x3a]:
        ops.extend(generate_ld_indexed(op))

    for op in [0x03, 0x13, 0x23]:
        ops.extend(generate_inc_rr(op))

    for op in [0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x3c]:
        ops.extend(generate_inc_r(op))

    for op in [0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x3d]:
        ops.extend(generate_dec_r(op))

    for op in [0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e]:
        ops.extend(generate_ld_r_n(op))

    for op in [0x09, 0x19, 0x29, 0x39]:
        ops.extend(generate_add_hl_rr(op))

    for op in [0x0b, 0x1b, 0x2b]:
        ops.extend(generate_dec_rr(op))

    for op in [0x20, 0x28, 0x30, 0x38]:
        ops.extend(generate_jr_cond(op))

    for op in range(0x40, 0x70):
        ops.extend(generate_ld_r_r(op))

    for op in [0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x77]:
        ops.extend(generate_ld_hl_r(op))

    for op in range(0x78, 0x80):
        ops.extend(generate_ld_r_r(op))

    for op in range(0x80, 0x88):
        ops.extend(generate_add(op))

    for op in range(0x88, 0x90):
        ops.extend(generate_adc(op))

    for op in range(0xa0, 0xa8):
        ops.extend(generate_and(op))

    for op in range(0xa8, 0xb0):
        ops.extend(generate_xor(op))

    for op in range(0xb0, 0xb8):
        ops.extend(generate_or(op))

    for op in [0xc0, 0xc8, 0xd0, 0xd8]:
        ops.extend(generate_ret_cond(op))

    for op in [0xc1, 0xd1, 0xe1, 0xf1]:
        ops.extend(generate_pop_rr(op))

    for op in [0xc2, 0xca, 0xd2, 0xda]:
        ops.extend(generate_jp_cond(op))

    for op in [0xc5, 0xd5, 0xe5, 0xf5]:
        ops.extend(generate_push_rr(op))

    for op in [0xc7, 0xcf, 0xd7, 0xdf, 0xe7, 0xef, 0xf7, 0xff]:
        ops.extend(generate_rst(op))

    return ops

def generate_cb_opcodes():
    ops = []

    for op in range(0x20, 0x28):
        ops.extend(generate_sla(op))

    for op in range(0x30, 0x38):
        ops.extend(generate_swap(op))

    for op in range(0x38, 0x40):
        ops.extend(generate_srl(op))

    for op in range(0x40, 0x80):
        ops.extend(generate_bit(op))

    for op in range(0x80, 0xc0):
        ops.extend(generate_res(op))

    return ops

def main():
#    copyfile("CPU.cpp", "CPU.cpp.bak")

    f = open("CPU.pre.cpp", "r")
    cpu = f.read().split("\n")
    f.close()

    begin = cpu.index("// --------- BEGIN GENERATED CODE ---------")
    end = cpu.index("// --------- END GENERATED CODE ---------")

    begin_cb = cpu.index("// --------- BEGIN GENERATED CB CODE ---------")
    end_cb = cpu.index("// --------- END GENERATED CB CODE ---------")

    opcodes = generate_opcodes()
    cb_opcodes = generate_cb_opcodes()

    f = open("CPU.cpp", "w")
    f.write("\n".join(cpu[0:begin+1]) + "\n")
    f.write("".join(opcodes))
#    f.write("\n".join(cpu[end:]))
    f.write("\n".join(cpu[end:begin_cb+1]) + "\n")
    f.write("".join(cb_opcodes))
    f.write("\n".join(cpu[end_cb:]))

    f.close()

def test():
    for op in range(0x88, 0x90):
        print("".join(generate_adc(op)))

#test()
main()
