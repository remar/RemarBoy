from Tkinter import *
import DisassemblyView, SaveState
import LR35902, LCD, MemoryMap, Disassembler
import time, atexit

class MainWindow(object):
    def __init__(self, master):
        self.master = master

        self.mem = MemoryMap.MemoryMap()
        self.cpu = LR35902.LR35902(self.mem)
        self.lcd = LCD.LCD(self.mem)

        self.disasm = dict()
        self.break_points = set()

        list_frame = Frame(master)
        list_frame.grid(row=0,column=0,sticky=NSEW)
        list_frame.grid_columnconfigure(0, weight=1)
        list_frame.grid_rowconfigure(0, weight=1)

        self.disassembly_view = DisassemblyView.DisassemblyView(list_frame)

        button_frame = Frame(master)
        button_frame.grid(row=1, column=0, sticky=S)

        step = Button(button_frame, text="Step", command=self.step_one)
        step.pack(side=LEFT)

        breakpoint = Button(button_frame, text="Breakpoint",
                            command=self.break_point)
        breakpoint.pack(side=LEFT)

        run10000 = Button(button_frame, text="Run 10000",
                          command=self.run10000)
        run10000.pack(side=LEFT)

    def insert_cart(self, filename):
        self.master.title("RemarBoy - " + filename.split("/")[-1])
        self.mem.insertCart(filename)

        self.save_state = SaveState.SaveState(filename)
        print "Save state avail:", self.save_state.exists()

        atexit.register(self.save)

        self.disasm[0x100] = Disassembler.disassemble(0x100, self.mem)
        self.disassembly_view.insert(0x100, self.disasm[0x100])
        self.disassembly_view.mark_pc(0x100)

    def save(self):
        print "Saving state..."
        self.save_state.write(self.disasm, self.cpu)

    def step_one(self):
        self.step()

        addr = self.mem.get_rom_address(self.cpu.PC)
        if addr not in self.disasm:
            self._disassemble_at(addr)

        self.disassembly_view.mark_pc(self.cpu.PC)
        self.disassembly_view.go_to_pc()

    def step(self):
        self.cpu.step()
        self.lcd.step()

    def break_point(self):
        addr = self.disassembly_view.get_selected()
        if addr is not None:
            print "(De)select break point: 0x%04x" % addr
            if addr in self.break_points:
                self.break_points.remove(addr)
                self.disassembly_view.clear_break_point(addr)
            else:
                self.break_points.add(addr)
                self.disassembly_view.set_break_point(addr)
            print self.break_points

    def run10000(self):
        start = time.clock()
        visited = set()
        for x in xrange(10000):
            self.step()
            visited.add(self.cpu.PC)
            if self.cpu.PC in self.break_points:
                break

        for addr in visited:
            if addr not in self.disasm:
                self._disassemble_at(addr)

        self.disassembly_view.mark_pc(self.cpu.PC)
        self.disassembly_view.go_to_pc()

        end = time.clock()
        print "Time:", (end - start)
        self.disassembly_view.go_to_pc()
        print "Lines:", self.disassembly_view

    def _disassemble_at(self, addr):
        self._disassemble(addr)
        if self.disasm[addr].bytes_[0] in [0x20,0x28,0x30,0x38,
                                           0xc2,0xc4,0xca,0xd2,0xda]:
            self._disassemble(addr, len(self.disasm[addr].bytes_))

    def _disassemble(self, addr, offset=0):
        self.disasm[addr+offset] = Disassembler.disassemble(addr+offset, self.mem)
        self.disassembly_view.insert(addr+offset, self.disasm[addr+offset])
