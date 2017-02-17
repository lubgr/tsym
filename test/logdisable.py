
import logging
import sys

libLogger = logging.getLogger('tsym')

for handler in libLogger.handlers:
    if isinstance(handler, logging.StreamHandler) and handler.stream == sys.stdout:
        handler.setLevel(logging.CRITICAL)
