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

disasm[0x100] = Disassembler.disassemble(0x100, mem)

class App(object):
    def __init__(self, master):
        list_frame = Frame(master)
        list_frame.grid(row=0,column=0,sticky=NSEW)
        list_frame.grid_columnconfigure(0, weight=1)
        list_frame.grid_rowconfigure(0, weight=1)

        scrollbar = Scrollbar(list_frame)
        scrollbar.grid(row=0, column=1, sticky=N+S+E)

        self.listbox = Listbox(list_frame,
                               yscrollcommand=scrollbar.set,
                               font=tkFont.Font(font = "TkFixedFont"),
                               selectmode=SINGLE)

        self.address_to_index = {}

        self.address_to_index[0x100] = self.listbox.size()
        self.listbox.insert(END, "[0x0100]   " + str(disasm[0x100]))

        self.listbox.itemconfig(self.address_to_index[cpu.PC], bg="yellow")

        self.listbox.grid(row=0, column=0, sticky=NSEW)

        scrollbar.config(command=self.listbox.yview)

        button_frame = Frame(master)
        button_frame.grid(row=1, column=0, sticky=S)

        step = Button(button_frame, text="Step", command=self.step)
        step.pack(side=LEFT)

        breakpoint = Button(button_frame, text="Breakpoint")
        breakpoint.pack(side=LEFT)

    def step(self):

        self.listbox.itemconfig(self.address_to_index[cpu.PC], bg="white")

        cpu.step()
        lcd.step()
        print "----------------------------------------"
        print cpu
        print "----------------------------------------"
        print lcd

        addr = mem.get_rom_address(cpu.PC)
        if cpu.PC < 0x8000 and addr not in disasm:
            disasm[addr] = Disassembler.disassemble(cpu.PC, mem)
            self.address_to_index[addr] = self.listbox.size()
            print "Insert item at 0x%04x" % addr
            self.listbox.insert(END, "[0x%04x]   " % addr + str(disasm[addr]))
            if disasm[addr].bytes_[0] in [0xc3, 0xc9, 0xd9]:
                self.listbox.insert(END, "...")

        self.listbox.itemconfig(self.address_to_index[cpu.PC], bg="yellow")

root = Tk()
root.title("RemarBoy")

app = App(root)

print app.address_to_index

root.grid_columnconfigure(0, weight=1)
root.grid_rowconfigure(0, weight=1)
root.geometry("800x500")
root.mainloop()
