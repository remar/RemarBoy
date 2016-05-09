from Tkinter import *
import LR35902, LCD, MemoryMap, Disassembler, DisassemblyView
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

break_points = set()

class App(object):
    def __init__(self, master):
        list_frame = Frame(master)
        list_frame.grid(row=0,column=0,sticky=NSEW)
        list_frame.grid_columnconfigure(0, weight=1)
        list_frame.grid_rowconfigure(0, weight=1)

        self.disassembly_view = DisassemblyView.DisassemblyView(list_frame)
        self.disassembly_view.insert(0x100, disasm[0x100])
        self.disassembly_view.mark_pc(0x100)

        button_frame = Frame(master)
        button_frame.grid(row=1, column=0, sticky=S)

        step = Button(button_frame, text="Step", command=self.step)
        step.pack(side=LEFT)

        breakpoint = Button(button_frame, text="Breakpoint",
                            command=self.break_point)
        breakpoint.pack(side=LEFT)

        run10000 = Button(button_frame, text="Run 10000",
                          command=self.run10000)
        run10000.pack(side=LEFT)

    def step(self):
        cpu.step()
        lcd.step()

        addr = mem.get_rom_address(cpu.PC)
        #if cpu.PC < 0x8000 and addr not in disasm:
        if addr not in disasm:
            disasm[addr] = Disassembler.disassemble(cpu.PC, mem)
            self.disassembly_view.insert(addr, disasm[addr])

        self.disassembly_view.mark_pc(cpu.PC)

    def break_point(self):
        addr = self.disassembly_view.get_selected()
        if addr is not None:
            print "Selected address: 0x%04x" % addr
            if addr in break_points:
                break_points.remove(addr)
                self.disassembly_view.clear_break_point(addr)
            else:
                break_points.add(addr)
                self.disassembly_view.set_break_point(addr)
            print break_points

    def run10000(self):
        for x in xrange(10000):
            self.step()
            if cpu.PC in break_points:
                break
        self.disassembly_view.go_to_pc()

root = Tk()
root.title("RemarBoy")

app = App(root)

root.grid_columnconfigure(0, weight=1)
root.grid_rowconfigure(0, weight=1)
root.geometry("800x500")
root.mainloop()
