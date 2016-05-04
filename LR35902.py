# LR35902 CPU emulation

#   0 1 2 3 4 5 6 7 8 9 a b c d e f
# 0 x x . . x x x . . . . x x x x . 0
# 1 . x x x x x x . x x x . x x x . 1
# 2 x x x x x x x . x . x . x x x x 2
# 3 . x x . . . x . . . . . x x x . 3
# 4 x x x x x x x x x x x x x x x x 4
# 5 x x x x x x x x x x x x x x x x 5
# 6 x x x x x x x x x x x x x x x x 6
# 7 x x x x x x . x x x x x x x x x 7
# 8 x x x x x x x x . . . . . . . . 8
# 9 . . . . . . . . . . . . . . . . 9
# a x x x x x x x x x x x x x x x x a
# b x x x x x x x x . . . . . . . . b
# c x x . x . x . x x x x - . x . x c
# d . x . . . x . x . x . . . . . x d
# e x x x . . x x x . x x . . . . x e
# f x x . - . x . x . . x - . . x x f
#   0 1 2 3 4 5 6 7 8 9 a b c d e f

regmap = {0:"B", 1:"C", 2:"D", 3:"E", 4:"H", 5:"L", 6:"(HL)", 7:"A"}

ZF = 0x80
NF = 0x40
HF = 0x20
CF = 0x10

VBLANK_INT = 0x01
LCD_STAT_INT = 0x02
TIMER_INT = 0x04
SERIAL_INT = 0x08
JOYPAD_INT = 0x10

class LR35902(object):
    def __init__(self, memoryMap):
        # Registers
        self.PC = 0x0100
        self.SP = 0xfffe

        self.IME = True # Interrupt Master Enable Flag
        self.interrupt_counter = 0

        self.regs = {'A':0x00, 'F':0x00,
                     'B':0x00, 'C':0x00,
                     'D':0x00, 'E':0x00,
                     'H':0x00, 'L':0x00}

        self.mem = memoryMap

    def __str__(self):
        F = self.regs['F']
        Z = (F & ZF) >> 7
        N = (F & NF) >> 6
        H = (F & HF) >> 5
        C = (F & CF) >> 4
        return "CPU\nPC: " + hex4(self.PC) + ", SP: " + hex4(self.SP) \
            + ", IME: " + str(self.IME) + "\n" \
            + "Flags, Z: %d, N: %d, H: %d, C: %d\n" % (Z, N, H, C) \
            + "A " + hex2(self.regs['A']) + "\n" \
            + "B %s, C %s\n" % (hex2(self.regs['B']), hex2(self.regs['C'])) \
            + "D %s, E %s\n" % (hex2(self.regs['D']), hex2(self.regs['E'])) \
            + "H %s, L %s" % (hex2(self.regs['H']), hex2(self.regs['L'])) \

    def step(self):
        self.interrupt_counter += 1
        if self.interrupt_counter == 100:
            self.interrupt_counter = 0
            if self.IME:
                IF = self.mem.get(0xff0f)
                IE = self.mem.get(0xffff)
                if ((IF & VBLANK_INT) == VBLANK_INT
                    and (IE & VBLANK_INT) == VBLANK_INT):
                    # VBLANK INTERRUPT!!
                    self.mem.put(0xff0f, IF & ~VBLANK_INT)
                    self.IME = False
                    self._push_word(self.PC)
                    self.PC = 0x0040
                    return

        op = self.mem.get(self.PC)

        if op == 0x00: # NOP
            self.PC += 1
        elif op == 0x01: # LD BC,d16
            low, high = self.mem.get_word(self.PC+1)
            self.regs['B'] = high
            self.regs['C'] = low
            self.PC += 3
        elif op == 0x0b: # DEC BC
            BC = self.getBC()
            BC -= 1
            if BC < 0:
                BC = 0xffff
            self.putBC(BC)
            self.PC += 1
        elif op == 0x11: # LD DE,d16
            low, high = self.mem.get_word(self.PC+1)
            self.regs['D'] = high
            self.regs['E'] = low
            self.PC += 3
        elif op == 0x12: # LD (DE),A
            DE = self.getDE()
            self.mem.put(DE, self.regs['A'])
            self.PC += 1
        elif op == 0x13: # INC DE
            DE = self.getDE()
            DE = (DE + 1) & 0xffff
            self.putDE(DE)
            self.PC += 1
        elif op == 0x18: # JR r8
            offset = self.mem.get(self.PC+1)
            self.PC += 2 + (offset if offset < 0x80 else offset - 0x100)
        elif op == 0x19: # ADD HL,DE
            HL = self.getHL()
            DE = self.getDE()
            F = self.regs['F'] & ZF # Keep Z flag
            H = HF if (HL & 0x0f00) + (DE & 0x0f00) > 0x0f00 else 0
            HL += DE
            if HL > 0xffff:
                HL &= 0xffff
                C = CF
            else:
                C = 0
            self.regs['F'] = F|H|C
            self.putHL(HL)
            self.PC += 1
        elif op == 0x1a: # LD A,(DE)
            self.regs['A'] = self.mem.get(self.getDE())
            self.PC += 1
        elif op == 0x20: # JR NZ,r8
            offset = self.mem.get(self.PC+1)
            self.PC += 2
            if self.regs['F'] & ZF != ZF:
                self.PC += offset if offset < 0x80 else offset - 0x100
        elif op == 0x28: # JR Z,r8
            offset = self.mem.get(self.PC+1)
            self.PC += 2
            if self.regs['F'] & ZF == ZF:
                self.PC += offset if offset < 0x80 else offset - 0x100
        elif op == 0x2f: # CPL
            self.regs['A'] ^= 0xff
            self.regs['F'] |= (NF | HF)
            self.PC += 1
        elif op in [0x04,0x0c,0x14,0x1c,0x24,0x2c,0x34,0x3c]: # INC B ...
            target = regmap[(op - 4) / 8]
            if op == 0x34:
                HL = self.getHL()
                operand = self.mem.get(HL)
            else:
                operand = self.regs[target]
            operand = (operand + 1) & 0xff
            Z = ZF if operand == 0 else 0
            H = HF if operand & 0xf == 0 else 0
            if op == 0x34:
                self.mem.put(HL, operand)
            else:
                self.regs[target] = operand
            self.regs['F'] = (self.regs['F'] & CF) | Z | H
            self.PC += 1
        elif op in [0x05,0x0d,0x15,0x1d,0x25,0x2d,0x35,0x3d]: # DEC B ...
            if op == 0x35:
                raise Exception,"0x35 not implemented"
            target = regmap[(op - 5) / 8]
            self.regs[target] -= 1
            if self.regs[target] < 0:
                self.regs[target] = 0xff
            Z = ZF if self.regs[target] == 0 else 0
            H = HF if self.regs[target] & 0xf == 0xf else 0
            self.regs['F'] = (self.regs['F'] & CF) | Z | NF | H
            self.PC += 1
        elif op in [0x06,0x0e,0x16,0x1e,0x26,0x2e,0x36,0x3e]: # LD B,d8 ...
            target = regmap[(op - 6) / 8]
            data = self.mem.get(self.PC+1)
            if op == 0x36:
                self.mem.put(self.getHL(), data)
            else:
                self.regs[target] = data
            self.PC += 2
        elif op == 0x21: # LD HL,d16
            low, high = self.mem.get_word(self.PC+1)
            self.regs['H'] = high
            self.regs['L'] = low
            self.PC += 3
        elif op == 0x22: # LD (HL+),A
            HL = self.getHL()
            self.mem.put(HL, self.regs['A'])
            self.putHL(self._inc_word(HL))
            self.PC += 1
        elif op == 0x23: # INC HL
            self.putHL(self._inc_word(self.getHL()))
            self.PC += 1
        elif op == 0x2a: # LD A,(HL+)
            HL = self.getHL()
            self.regs['A'] = self.mem.get(HL)
            self.putHL(self._inc_word(HL))
            self.PC += 1
        elif op == 0x31: # LD SP,d16
            low, high = self.mem.get_word(self.PC+1)
            self.SP = high * 0x100 + low
            self.PC += 3
        elif op == 0x32: # LD (HL-),A
            HL = self.getHL()
            self.mem.put(HL, self.regs['A'])
            HL -= 1
            if HL < 0:
                HL = 0xffff
            self.putHL(HL)
            self.PC += 1
        elif op == 0x76:
            raise Exception, "HALT not implemented"
        elif op == 0xc0: # RET NZ
            if self.regs['F'] & ZF != ZF:
                self.PC = self._pop_word()
            else:
                self.PC += 1
        elif op == 0xc1: # POP BC
            self.putBC(self._pop_word())
            self.PC += 1
        elif op == 0xc3:
            low, high = self.mem.get_word(self.PC+1)
            self.PC = high * 0x100 + low
        elif op == 0xc5: # PUSH BC
            self._push_word(self.getBC())
            self.PC += 1
        elif op == 0xc8: # RET Z
            if self.regs['F'] & ZF == ZF:
                self.PC = self._pop_word()
            else:
                self.PC += 1
        elif op == 0xc9: # RET
            self.PC = self._pop_word()
        elif op == 0xca: # JP Z,a16
            if self.regs['F'] & ZF == ZF:
                low, high = self.mem.get_word(self.PC+1)
                self.PC = high * 0x100 + low
            else:
                self.PC += 3
        elif op == 0xcb: # PREFIX CB
            self._cb_op()
        elif op == 0xcd: # CALL a16
            low, high = self.mem.get_word(self.PC+1)
            self._push_word(self.PC + 3)
            self.PC = high * 0x100 + low
        elif op == 0xd1: # POP DE
            self.putDE(self._pop_word())
            self.PC += 1
        elif op == 0xd5: # PUSH DE
            self._push_word(self.getDE())
            self.PC += 1
        elif op == 0xd9: # RETI
            self.IME = True
            self.PC = self._pop_word()
        elif op == 0xe0: # LDH (a8),A aka LD ($FF00+a8),A
            offset = self.mem.get(self.PC+1)
            self.mem.put(0xff00+offset, self.regs['A'])
            self.PC += 2
        elif op == 0xe1: # POP HL
            self.putHL(self._pop_word())
            self.PC += 1
        elif op == 0xe2: # LDH (C),A aka LD ($FF00+C),A
            self.mem.put(0xff00+self.regs['C'], self.regs['A'])
            self.PC += 1
        elif op == 0xe5: # PUSH HL
            self._push_word(self.getHL())
            self.PC += 1
        elif op == 0xe6: # AND d8
            operand = self.mem.get(self.PC+1)
            self.regs['A'] &= operand
            self.regs['F'] = ZF|HF if self.regs['A'] == 0 else HF
            self.PC += 2
        elif op == 0xe9: # JP (HL)
            self.PC = self.getHL()
        elif op == 0xea: # LD (a16),A
            low, high = self.mem.get_word(self.PC+1)
            address = high * 0x100 + low
            self.mem.put(address, self.regs['A'])
            self.PC += 3
        elif op in [0xc7, 0xcf, 0xd7, 0xdf, 0xe7, 0xef, 0xf7, 0xff]: # RST xxH
            self._push_word(self.PC + 1)
            self.PC = op - 0xc7
        elif op == 0xf0: # LDH A,(a8) aka LD A,($FF00+a8)
            offset = self.mem.get(self.PC+1)
            self.regs['A'] = self.mem.get(0xff00+offset)
            self.PC += 2
        elif op == 0xf1: # POP AF
            self.putAF(self._pop_word())
            self.PC += 1
        elif op == 0xf3: # DI
            self.IME = False
            self.PC += 1
        elif op == 0xf5: # PUSH AF
            self._push_word(self.getAF())
            self.PC += 1
        elif op == 0xfa: # LD A,(a16)
            low, high = self.mem.get_word(self.PC+1)
            self.regs['A'] = self.mem.get(high * 0x100 + low)
            self.PC += 3
        elif op == 0xfb: # EI
            self.IME = True
            self.PC += 1
        elif op == 0xfe: # CP d8
            comp = self.mem.get(self.PC+1)
            Z = ZF if self.regs['A'] == comp else 0
            self.regs['F'] = Z | NF | HF | CF # TODO: carry and halfcarry??
            self.PC += 2

        elif op >= 0x40 and op <= 0x7f:
            self._load(op)
        elif op >= 0x80 and op <= 0xbf:
            self._alu(op)

        else:
            raise Exception, "Operation not implemented (%s)" % hex2(op)

    def _push_word(self, word):
        self.mem.put(self.SP-1, (word & 0xff00) >> 8)
        self.mem.put(self.SP-2, word & 0xff)
        self.SP -= 2

    def _pop_word(self):
        self.SP += 2
        return self.mem.get(self.SP-1) * 0x100 + self.mem.get(self.SP-2)

    def _inc_word(self, word):
        return (word + 1) & 0xffff

    def getAF(self):
        return self._get_word_reg('A', 'F')
    def putAF(self, AF):
        self._put_word_reg('A', 'F', AF)

    def getBC(self):
        return self._get_word_reg('B', 'C')
    def putBC(self, BC):
        self._put_word_reg('B', 'C', BC)

    def getDE(self):
        return self._get_word_reg('D', 'E')
    def putDE(self, DE):
        self._put_word_reg('D', 'E', DE)

    def getHL(self):
        return self._get_word_reg('H', 'L')
    def putHL(self, HL):
        self._put_word_reg('H', 'L', HL)

    def _get_word_reg(self, high, low):
        return self.regs[high] * 0x100 + self.regs[low]
    def _put_word_reg(self, high, low, value):
        self.regs[high] = (value & 0xff00) / 0x100
        self.regs[low] = value & 0xff

    def _load(self, op):
        # LD operations 0x40-0x7f
        target = ((op & 0xf8) - 0x40) / 0x08
        source = op & 0x07

        if target == 6: # target = (HL)
            self.mem.put(self.getHL(), self.regs[regmap[source]])
        elif source == 6: # source = (HL)
            self.regs[regmap[target]] = self.mem.get(self.getHL())
        else:
            self.regs[regmap[target]] = self.regs[regmap[source]]

        self.PC += 1

    def _alu(self, op):
        # Logic operations 0x80-0xbf
        methods = {0x80:'ADD',0x88:'ADC',0x90:'SUB',0x98:'SBC',
                   0xa0:'AND',0xa8:'XOR',0xb0:'OR',0xb8:'CP'}
        method = methods[op & 0xf8]

        source = regmap[op & 0x07]

        if source == 6: # (HL)
            operand = self.mem.get(self.getHL())
        else:
            operand = self.regs[source]

        if method == 'ADD': # ADD
            A = self.regs['A']
            H = HF if (A & 0xf) + (operand & 0xf) > 0xf else 0
            A += operand
            if A > 0xff:
                C = CF
                A &= 0xff
            else:
                C = 0
            Z = ZF if A == 0 else 0
            self.regs['A'] = A
            self.regs['F'] = Z|H|C
#        elif method == 'ADC': pass #ADC
#        elif method == 'SUB': pass #SUB
#        elif method == 'SBC': pass #SBC
        elif method == 'AND': # AND
            self.regs['A'] &= operand
            self.regs['F'] = ZF|HF if self.regs['A'] == 0 else HF
        elif method == 'XOR': # XOR
            self.regs['A'] ^= operand
            self.regs['F'] = ZF if self.regs['A'] == 0 else 0
        elif method == 'OR': # OR
            self.regs['A'] |= operand
            self.regs['F'] = ZF if self.regs['A'] == 0 else 0
#        elif method == 'CP': pass #CP
        else:
            raise Exception, "%s not implemented" % method

        self.PC += 1

    def _cb_op(self):
        # PC points to 0xcb, PC+1 points to extended op
        op = self.mem.get(self.PC+1)

        # CB operations:
        # RLC   00-07
        # RRC   08-0f
        # RL    10-17
        # RR    18-1f
        # SLA   20-27
        # SRA   28-2f
        # SWAP  30-37 IMPLEMENTED
        # SRL   38-3f
        # BIT n 40-7f
        # RES n 80-bf IMPLEMENTED
        # SET n c0-ff

        method = op / 8
        target = op & 0x7

        if target == 6:
            operand = self.mem.get(getHL())
        else:
            operand = self.regs[regmap[target]]

        if method == 6: # SWAP
            operand = (operand & 0x0f) << 4 | (operand & 0xf0) >> 4
        elif method >= 16 and method < 24: # RES n
            bit = method - 16
            pattern = (~(1 << bit)) & 0xff
            operand &= pattern
        else:
            raise Exception, "CB method %s not implemented" % d8(op)

        if target == 6:
            self.mem.put(getHL(), operand)
        else:
            self.regs[regmap[target]] = operand

        self.PC += 2

def hex2(i):
    return format(i, '#04x')

def hex4(i):
    return format(i, '#06x')

def d8(i):
    return "$" + format(i, '02x')

def d16(i):
    return "$" + format(i, '04x')
