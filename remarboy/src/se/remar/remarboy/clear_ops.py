#!/usr/bin/env python3

from shutil import copyfile

copyfile("CPU.java", "CPU.java.bak")

f = open("CPU.java", "r")
cpu = f.read().split("\n")
f.close()

begin = cpu.index("// --------- BEGIN GENERATED CODE ---------")
end = cpu.index("// --------- END GENERATED CODE ---------")

f = open("CPU.java", "w")
f.write("\n".join(cpu[0:begin+1]) + "\n")
f.write("\n".join(cpu[end:]))
f.close()
