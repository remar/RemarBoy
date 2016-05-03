from Tkinter import *
import LR35902, LCD, MemoryMap, Disassembler
import sys
import tkFont

if len(sys.argv) != 2:
    print "Usage: %s <rom file>" % sys.argv[0]
    exit()

mem = MemoryMap.MemoryMap()
cpu = LR35902.LR35902(mem)
lcd = LCD.LCD(mem)

mem.insertCart(sys.argv[1])

disasm = dict()

for i in xrange(1000):
    addr = mem.get_rom_address(cpu.PC)
    if cpu.PC < 0x8000 and addr not in disasm:
        disasm[addr] = Disassembler.disassemble(cpu.PC, mem)
    cpu.step()
    lcd.step()

print cpu

class App(object):
    def __init__(self, master):
        options = dict(sticky=NSEW, padx=3, pady=4)

        scrollbar = Scrollbar(master)
        #scrollbar.pack(side=RIGHT, fill=Y)
        scrollbar.grid(row=0, column=1, sticky=N+S+E)

        listbox = Listbox(master,
                          yscrollcommand=scrollbar.set,
                          font=tkFont.Font(font = "TkFixedFont"),
                          selectmode=SINGLE)

        self.address_to_index = {}

        prev = 0xff
        prev_instr = None
        for key in sorted(disasm.keys()):
            if prev_instr and (prev + len(prev_instr.bytes_) != key
                               or prev_instr.bytes_[0] in [0xc3, 0xc9, 0xd9]):
                listbox.insert(END, "...")
            self.address_to_index[key] = listbox.size()
            listbox.insert(END, "[0x%04x]   " % key + str(disasm[key]))
            prev = key
            prev_instr = disasm[key]

        #listbox.pack(side=LEFT, fill=BOTH)
        listbox.grid(row=0, column=0, sticky=NSEW)

        scrollbar.config(command=listbox.yview)

root = Tk()

app = App(root)

print app.address_to_index

root.grid_columnconfigure(0, weight=1)
root.grid_rowconfigure(0, weight=1)
root.geometry("800x500")
root.mainloop()
