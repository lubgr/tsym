class IntPrinter:
    def __init__(self, val):
        self.val = val
        self.rep = self.val['rep']
        self.overflowed = self.val['overflow']

    def to_string(self):
        if self.overflowed:
            return "Overflowed"
        else:
            return str(self.rep)

class NumberPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['prettyStr']
        return prettyStr

class BasePtrPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['prettyStr']
        return prettyStr

class VarPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['basePtr']
        return prettyStr

def lookup(val):
    typeStr = str(val.type)
    if typeStr in ['tsym::BasePtr', 'const tsym::BasePtr', 'const tsym::BasePtr &']:
        return BasePtrPrinter(val)
    elif typeStr in ['tsym::Var', 'const tsym::Var', 'const tsym::Var &']:
        return VarPrinter(val)
    elif typeStr in ['tsym::Number', 'const tsym::Number', 'const tsym::Number &']:
        return NumberPrinter(val)
    elif typeStr in ['tsym::Int', 'const tsym::Int', 'const tsym::Int &']:
        return IntPrinter(val)
    return None

gdb.pretty_printers.append(lookup)
