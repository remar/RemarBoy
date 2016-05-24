# 160x144 pixels

class LCD(object):
    def __init__(self, mem):
        self.mem = mem
        self.counter = 0
        mem.put(0xff44, 0) # LCD Y pos

    def __str__(self):
        return "LCD\nLY: %d" % self.mem.get(0xff44)

    def step(self):
        self.counter += 1
        if self.counter >= 100:
            self.counter = 0
            LY = self.mem.get(0xff44)
            LY += 1
            if LY == 144:
                self.mem.put(0xff0f, self.mem.get(0xff0f) | 0x01) # VBLANK
            if LY > 153:
                LY = 0
            self.mem.put(0xff44, LY)
