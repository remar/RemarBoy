import os, json
from Disassembler import Instruction

class SaveState(object):
    def __init__(self, filename):
        # See if save state exists for rom, if so load it
        self.save_state_filename = self._make_filename(filename)

    def get_file_name(self):
        return self.save_state_filename

    def exists(self):
        return os.path.exists(self.save_state_filename)

    def read(self):
        f = open(self.save_state_filename, "r")
        obj = json.load(f)
        f.close()
        return obj

    def write(self, disasm, cpu, mem):
        f = open(self.save_state_filename, "w")

        obj = {
            "disasm":self._disasm_to_json(disasm),
            "cpu":cpu.to_json(),
            "mem":mem.to_json()
        }

        json.dump(obj, f)

        f.close()

    def disasm_from_json(self, instructions):
        disasm = {}
        for address, instruction in instructions.iteritems():
            disasm[int(address)] = Instruction().from_json(instruction)
        return disasm

    def _disasm_to_json(self, disasm):
        instructions = {}
        for address, instruction in disasm.iteritems():
            instructions[address] = instruction.to_json()
        return instructions

    def _make_filename(self, filename):
        return "".join(filename.split(".")[0:-1]) + ".rba"
