#!/usr/bin/env python3

from shutil import copyfile

# "Normal" op codes

def generate_ld_rr_nn(op):
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    return make_case(op, "LD " + r1 + r2 + ",nn") + [
        indent(3), r2, " = mem.getByte(PC++);", nl(),
        indent(3), r1, " = mem.getByte(PC++);", nl()
    ] + make_cycles_and_break(3)

def generate_dec_rr(op):
    r1, r2 = get_wide_reg((op & 0x30) // 16)
    def r1r2(s):
        return s.replace("r1", r1).replace("r2", r2)

    return make_case(op, "DEC " + r1 + r2) + [
        indent(3), r1r2("r1r2 = (r1 & 0xff) * 0x100 + (r2 & 0xff);"), nl(),
        indent(3), r1r2("r1r2 = (r1r2 - 1) & 0xffff;"), nl(),
        indent(3), r1r2("r1 = (r1r2 & 0xff00) >> 8;"), nl(),
        indent(3), r1r2("r2 = r1r2 & 0x00ff;"), nl()
    ] + make_cycles_and_break(2)

def generate_inc_r(op):
    reg = get_reg((op & 0x38) // 8)
    return make_case(op, "INC " + reg) + [
        indent(3), reg, "++;", nl(),
        indent(3), reg, " &= 0xff;", nl(),
        indent(3), "F = (F & CF) | (r == 0 ? ZF : 0) | (r == 0x10 ? HF : 0);".replace("r", reg), nl()] + make_cycles_and_break(1)

def generate_dec_r(op):
    reg = get_reg((op & 0x38) // 8)
    return make_case(op, "DEC " + reg) + [
            indent(3), "r = (r - 1) & 0xff;".replace("r",reg), nl(),
            indent(3), "F = (F & CF) | (r == 0 ? ZF : 0) | NF | ((r & 0xf) == 0xf ? HF : 0);".replace("r",reg), nl()] + make_cycles_and_break(1)

def generate_ld_r_r(op):
    dest = get_reg((op & 0x38) // 8)
    src = get_reg(op & 0x07) if (op & 0x07) != 0x06 else "(HL)"
    if src == "(HL)":
        return make_case(op, "LD " + dest + "," + src) + [
            indent(3), dest, " = mem.getByte(", make_word("H", "L"), ");", nl()
        ] + make_cycles_and_break(2)
    elif dest == src:
        return make_case(op, "LD " + dest + "," + src) + make_cycles_and_break(1)
    else:
        return make_case(op, "LD " + dest + "," + src) + [
            indent(3), dest, " = ", src, ";", nl()
        ] + make_cycles_and_break(1)

def generate_and(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "AND " + r_name) + [
        indent(3),
        "A = (A & 0xff) & (" + r + " & 0xff);" if r != "A" else "", nl(),
        indent(3), "F = (A == 0 ? ZF : 0) | HF;", nl()
    ] + make_cycles_and_break(cycles)

def generate_xor(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "XOR " + r_name) + [
        indent(3),
        "A = (A & 0xff) ^ (" + r + " & 0xff);" if r != "A" else "A = 0;", nl(),
        indent(3), "F = (A == 0 ? ZF : 0);", nl()
    ] + make_cycles_and_break(cycles)

def generate_or(op):
    r = get_reg(op & 0x07) if (op & 0x07) != 0x06 else get_hl()
    r_name = get_reg(op & 0x07)
    cycles = 2 if (op & 0x07) == 0x06 else 1
    return make_case(op, "OR " + r_name) + ([
        indent(3), "A = (A & 0xff) | (" + r + " & 0xff);",
        nl() ] if r != "A" else []) + [
            indent(3), "F = A == 0 ? ZF : 0;", nl()
        ] + make_cycles_and_break(cycles)

# CB op codes
def generate_swap(op):
    def swap(s):
        return "((s & 0x0f) << 4) | ((s & 0xf0) >> 4)".replace("s", s)

    if op & 0x07 == 0x06:
        # (HL)
        return make_cb_case(op, "SWAP (HL)") + [
            indent(4), "HL = ", make_word("H", "L"), ";", nl(),
            indent(4), "temp = mem.getByte(HL);", nl(),
            indent(4), "mem.putByte(HL, "+swap("temp")+");", nl()
        ] + make_cb_cycles_and_break(4)
    else:
        r = get_reg(op & 0x07)
        return make_cb_case(op, "SWAP " + r) + [
            indent(4), ("r = "+swap("r")+";")
            .replace("r", r), nl(),
            indent(4), "F = (r == 0 ? ZF : 0);".replace("r", r), nl()
        ] + make_cb_cycles_and_break(2)

def get_hl():
    return "mem.getByte("+make_word("H", "L")+")"

def make_word(h, l):
    return "(" + h + " & 0xff) * 0x100 + (" + l + " & 0xff)"

def get_reg(r):
    return {0: "B", 1: "C", 2: "D", 3: "E", 4:"H", 5:"L", 6:"(HL)", 7:"A"}[r]

def get_wide_reg(r):
    return {0:("B", "C"), 1:("D", "E"), 2:("H", "L")}[r]

def make_case(op, title):
    return [indent(2), "case ", make_op(op), ": // 0x", format(op, "02x"),
            ", ", title, nl()]

def make_cb_case(op, title):
    return [indent(3), "case ", make_op(op), ": // 0x", format(op, "02x"),
            ", ", title, nl()]

def make_cycles_and_break(cycles):
    return [indent(3), "cycles += "+str(cycles)+";", nl(),
            indent(3), "break;", nl()]

def make_cb_cycles_and_break(cycles):
    return [indent(4), "cycles += "+str(cycles)+";", nl(),
            indent(4), "break;", nl()]

def make_op(op):
    return str(op if op < 128 else (-256 + op))

def indent(n):
    return " "*4*n

def nl():
    return "\n"

def generate_opcodes():
    ops = []

    for op in [0x01, 0x11, 0x21]:
        ops.extend(generate_ld_rr_nn(op))

    for op in [0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x3c]:
        ops.extend(generate_inc_r(op))

    for op in [0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x3d]:
        ops.extend(generate_dec_r(op))

    for op in [0x0b, 0x1b, 0x2b]:
        ops.extend(generate_dec_rr(op))

    for op in range(0x40, 0x70):
        ops.extend(generate_ld_r_r(op))

    for op in range(0x78, 0x80):
        ops.extend(generate_ld_r_r(op))

    for op in range(0xa0, 0xa8):
        ops.extend(generate_and(op))

    for op in range(0xa8, 0xb0):
        ops.extend(generate_xor(op))

    for op in range(0xb0, 0xb8):
        ops.extend(generate_or(op))

    return ops

def generate_cb_opcodes():
    ops = []

    for op in range(0x30, 0x38):
        ops.extend(generate_swap(op))

    return ops

def main():
    copyfile("CPU.java", "CPU.java.bak")

    f = open("CPU.java", "r")
    cpu = f.read().split("\n")
    f.close()

    begin = cpu.index("// --------- BEGIN GENERATED CODE ---------")
    end = cpu.index("// --------- END GENERATED CODE ---------")

    begin_cb = cpu.index("// --------- BEGIN GENERATED CB CODE ---------")
    end_cb = cpu.index("// --------- END GENERATED CB CODE ---------")

    opcodes = generate_opcodes()
    cb_opcodes = generate_cb_opcodes()

    f = open("CPU.java", "w")
    f.write("\n".join(cpu[0:begin+1]) + "\n")
    f.write("".join(opcodes))
    f.write("\n".join(cpu[end:begin_cb+1]) + "\n")
    f.write("".join(cb_opcodes))
    f.write("\n".join(cpu[end_cb:]))
    f.close()

def test():
    print("".join(generate_and(0xa6)))

main()
