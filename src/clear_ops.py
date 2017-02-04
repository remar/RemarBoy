#!/usr/bin/env python3

from shutil import copyfile

copyfile("CPU.cpp", "CPU.cpp.bak")

f = open("CPU.cpp", "r")
cpu = f.read().split("\n")
f.close()

begin = cpu.index("// --------- BEGIN GENERATED CODE ---------")
end = cpu.index("// --------- END GENERATED CODE ---------")
#begin_cb = cpu.index("// --------- BEGIN GENERATED CB CODE ---------")
#end_cb = cpu.index("// --------- END GENERATED CB CODE ---------")

f = open("CPU.cpp", "w")
f.write("\n".join(cpu[0:begin+1]) + "\n")
f.write("\n".join(cpu[end:]))
f.close()

#f.write("\n".join(cpu[end:begin_cb+1]) + "\n")
#f.write("\n".join(cpu[end_cb:]))
