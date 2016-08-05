from shutil import copyfile

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

def get_reg(r):
    return {0: "B", 1: "C", 2: "D", 3: "E", 4:"H", 5:"L", 7:"A"}[r]

def make_case(op, title):
    return [indent(2), "case ", make_op(op), ": // 0x", format(op, "02x"),
            ", ", title, nl()]

def make_cycles_and_break(cycles):
    return [indent(3), "cycles += "+str(cycles)+";", nl(),
            indent(3), "break;", nl()]

def make_op(op):
    return str(op if op < 128 else (-256 + op))

def indent(n):
    return " "*4*n

def nl():
    return "\n"

def generate_opcodes():
    ops = []
    for op in [0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x3d]:
        ops.extend(generate_dec_r(op))

    for op in [0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x3c]:
        ops.extend(generate_inc_r(op))

    return ops
        
copyfile("CPU.java", "CPU.java.bak")

f = open("CPU.java", "r")
cpu = f.read().split("\n")
f.close()

begin = cpu.index("// --------- BEGIN GENERATED CODE ---------")
end = cpu.index("// --------- END GENERATED CODE ---------")

f = open("CPU.java", "w")
f.write("\n".join(cpu[0:begin+1]) + "\n")
f.write("".join(generate_opcodes()))
f.write("\n".join(cpu[end:]))
f.close()
