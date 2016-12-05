
import sys
import glob

# The std::list type can be named std::__cxx11::list, in this case the stl-pretty-printer provided
# by the gcc package (usually located in /usr/share/gcc*/python/libstdcxx/v*/printers.py) doesn't
# return the correct printer ('StdListPrinter') upon type lookup, and the raw data for the list is
# printed. To fix this behavior, the stl-pretty-printers are manually loaded and added for plain
# lists and tsym::BasePtrList wrappers.

searchPath = glob.glob('/usr/share/gcc-*/python')
if len(searchPath) > 0:
    sys.path.insert(0, searchPath[-1])
    from libstdcxx.v6.printers import register_libstdcxx_printers, StdListPrinter
else:
    gdb.write('Couldn\'t find path to include libstd++ pretty printers\n')

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

def lookupVariations(base):
    base = 'tsym::' + base
    return [ base, 'const ' + base, 'const ' + base + ' &' ]

def lookup(val):
    typeStr = str(val.type)

    if typeStr in lookupVariations('BasePtr'):
        return BasePtrPrinter(val)
    if typeStr in lookupVariations('Var'):
        return VarPrinter(val)
    elif typeStr in lookupVariations('Number'):
        return NumberPrinter(val)
    elif typeStr in lookupVariations('Int'):
        return IntPrinter(val)
    elif typeStr in lookupVariations('BasePtrList'):
        return StdListPrinter('std::__cxx11::list', val['list'])
    elif 'std::__cxx11::list' in typeStr:
        return StdListPrinter('std::__cxx11::list', val)
    return None

gdb.pretty_printers.append(lookup)
