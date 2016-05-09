from Tkinter import *
import tkFont

class AddressToIndex(object):
    def __init__(self):
        self.address_to_index = {}
        self.BIG_OFFSET = 100000000

    def __str__(self):
        return str(self.address_to_index)

    def put(self, address, index):
        self.address_to_index[address] = index
        self.address_to_index[index+self.BIG_OFFSET] = address

    def get_by_address(self, address):
        return self.address_to_index[address]

    def get_by_index(self, index):
        return self.address_to_index[index+self.BIG_OFFSET]

class DisassemblyView(object):
    def __init__(self, frame):
        self.address_to_index = AddressToIndex()
        self.prev_mark = 0x100
        self.break_points = set()

        scrollbar = Scrollbar(frame)
        scrollbar.grid(row=0, column=1, sticky=N+S+E)
        self.listbox = Listbox(frame,
                               yscrollcommand=scrollbar.set,
                               font=tkFont.Font(font = "TkFixedFont"),
                               selectmode=SINGLE)
        self.listbox.grid(row=0, column=0, sticky=NSEW)
        scrollbar.config(command=self.listbox.yview)

    def mark_pc(self, PC):
        if self.prev_mark in self.break_points:
            self._mark_break(self.prev_mark)
        else:
            self._mark_clear(self.prev_mark)
        self._mark_pc(PC)
        self.prev_mark = PC

    def insert(self, address, assembly):
        # TODO: Handle insertion in the middle
        self.address_to_index.put(address, self.listbox.size())
        print "Insert item at 0x%04x" % address
        self.listbox.insert(END, "[0x%04x]   " % address + str(assembly))
        if assembly.bytes_[0] in [0xc3, 0xc9, 0xd9]:
            self.listbox.insert(END, "...")

    def get_selected(self):
        items = map(int, self.listbox.curselection())
        if not items:
            return None

        try:
            address = self.address_to_index.get_by_index(items[0])
        except Exception:
            return None

        return address

    def set_break_point(self, addr):
        self._mark_break(addr)
        self.break_points.add(addr)

    def clear_break_point(self, addr):
        self._mark_clear(addr)
        self.break_points.remove(addr)

    def go_to_pc(self):
        self.listbox.see(self.address_to_index.get_by_address(self.prev_mark))

    def _mark_clear(self, addr):
        self._mark(addr, "white")

    def _mark_break(self, addr):
        self._mark(addr, "red")

    def _mark_pc(self, addr):
        self._mark(addr, "yellow")

    def _mark(self, addr, color):
        self.listbox.itemconfig(self.address_to_index.get_by_address(addr),
                                bg=color)
