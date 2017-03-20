
import logging
import sys

libLogger = logging.getLogger('tsym')

libFilter = logging.Filter()
libFilter.filter = lambda logRecord: 0

for handler in libLogger.handlers:
    if isinstance(handler, logging.StreamHandler) and handler.stream == sys.stdout:
        handler.addFilter(libFilter)
