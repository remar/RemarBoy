from Tkinter import *

WIDTH = 128
HEIGHT = 192

class VramView(object):
    def __init__(self, frame, mem):
        self.mem = mem
        self.canvas = Canvas(frame, width=WIDTH, height=HEIGHT, bg="#000000")
        self.canvas.pack()
        self.img = PhotoImage(width=WIDTH, height=HEIGHT)
        self.canvas.create_image((WIDTH/2, HEIGHT/2), image=self.img, state="normal")
        self.canvas.bind("<Button-1>", self.render)

    def render(self, event=None):
        for tiley in xrange(24):
            for tilex in xrange(16):
                for line in xrange(8):
                    addr = 0x8000 + tiley * 256 + tilex * 16 + line * 2
                    low, high = self.mem.get_word(addr)
                    for pixel in xrange(8):
                        color = ((high & 1) * 2 + (low & 1)) * 85
                        self.img.put("#%02x%02x%02x" % (255-color, 255-color, 255-color),
                                     (tilex*8+(7-pixel), tiley*8+line))
                        low >>= 1
                        high >>= 1
        self.img.put("#ff0000", (WIDTH/2, HEIGHT/2))
