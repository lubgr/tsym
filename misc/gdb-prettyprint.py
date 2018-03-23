
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

def extractStrData(string):
    return string

class NumberPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['prettyStr']
        return extractStrData(prettyStr)

class BasePtrPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['prettyStr']
        return extractStrData(prettyStr)

class VarPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        prettyStr = self.val['rep']['_M_ptr']['prettyStr']
        return extractStrData(prettyStr)

def lookupVariations(base):
    base = 'tsym::' + base
    return [ base, 'const ' + base, 'const ' + base + ' &' ]

def lookup(val):
    typeStr = str(val.type)

    if 'shared_ptr<tsym::Base' in typeStr:
        return BasePtrPrinter(val)
    elif typeStr in lookupVariations('Var'):
        return VarPrinter(val)
    elif typeStr in lookupVariations('Number'):
        return NumberPrinter(val)
    elif typeStr in lookupVariations('BasePtrList'):
        return StdListPrinter('std::__cxx11::list', val['list'])
    elif 'std::__cxx11::list' in typeStr:
        return StdListPrinter('std::__cxx11::list', val)
    return None

gdb.pretty_printers.append(lookup)
