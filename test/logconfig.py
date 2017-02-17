
import logging
import copy
import sys

tsymFormatter = logging.Formatter('%(name)s-%(levelname)-7s - %(cfilename)s:%(clineno)d - %(message)s')

testFormatter = logging.Formatter('%(name)-8s: %(message)s')

tsymDebugFileHandler = logging.FileHandler('misc/test-logfiles/debug.log', 'w')
tsymDebugFileHandler.setFormatter(tsymFormatter)
tsymDebugFileHandler.setLevel(logging.DEBUG)

cppuDebugFileHandler = copy.copy(tsymDebugFileHandler)
cppuDebugFileHandler.setFormatter(testFormatter)

tsymInfoFileHandler = logging.FileHandler('misc/test-logfiles/info.log', 'w')
tsymInfoFileHandler.setFormatter(tsymFormatter)
tsymInfoFileHandler.setLevel(logging.INFO)

cppuInfoFileHandler = copy.copy(tsymInfoFileHandler)
cppuInfoFileHandler.setFormatter(testFormatter)

streamHandler = logging.StreamHandler(sys.stdout)
streamHandler.setFormatter(tsymFormatter)
streamHandler.setLevel(logging.WARNING)

libLogger = logging.getLogger('tsym')
libLogger.addHandler(tsymDebugFileHandler)
libLogger.addHandler(tsymInfoFileHandler)
libLogger.addHandler(streamHandler)
libLogger.setLevel(logging.DEBUG)

cppUTestLogger = logging.getLogger('CppUTest')
cppUTestLogger.addHandler(cppuDebugFileHandler)
cppUTestLogger.addHandler(cppuInfoFileHandler)
cppUTestLogger.setLevel(logging.DEBUG)
