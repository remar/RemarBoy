import os, json

class SaveState(object):
    def __init__(self, filename):
        # See if save state exists for rom, if so load it
        self.save_state_filename = self._make_filename(filename)

    def exists(self):
        return os.path.exists(self.save_state_filename)

    def write(self, disasm, cpu):
        f = open(self.save_state_filename, "w")

        obj = {
            "disasm":self._disasm_to_json(disasm),
            "cpu":cpu.to_json()
        }

        json.dump(obj, f)

        f.close()

    def _disasm_to_json(self, disasm):
        instructions = {}
        for address, instruction in disasm.iteritems():
            instructions[address] = instruction.to_json()
        return instructions

    def _make_filename(self, filename):
        return "".join(filename.split(".")[0:-1]) + ".rba"
