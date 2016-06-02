__author__ = 'ctomas'

import sys
import os
import logging
import time
import const
import fileutils
import contextlib

# .......................................................... Constants ...
const.LEVELS = {'DEBUG'    : logging.DEBUG,
                'INFO'     : logging.INFO,
                'WARNING'  : logging.WARNING,
                'ERROR'    : logging.ERROR,
                'CRITICAL' : logging.CRITICAL
                }


def getLogger(moduleName, level):
    '''
    Get a logger for the specified module name and level.
    :param moduleName:  The module name for which the logger is created
    :param level:       The logging level ('DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL')
    :return:[[
    '''
    # Find the log directory
    fileDir = os.path.dirname(os.path.abspath(__file__))
    logDir = os.path.join(fileDir, '..', 'log')

    # Create the logger
    logger = logging.getLogger(moduleName)

    # Set the logging level
    logger.setLevel(const.LEVELS[level])

    # Create a logger file handler
    currentDate = time.strftime('%Y%m%d%H%M%S')
    fileName = "{0}_{1}.log".format(moduleName, currentDate)
    handler = logging.FileHandler(os.path.join(logDir, fileName))

    # Create a logging format
    formatter = logging.Formatter('%(asctime)-15s %(name)-12s %(levelname)-6s %(message)s')
    handler.setFormatter(formatter)

    # Add the handler to the logger
    logger.addHandler(handler)

    return logger


@contextlib.contextmanager
def redirectStdout(where):
    '''
    Create a context manager to set stdout to whatever object specified in the parameter
    when entering the context. When exiting the context, the context manager reset stdout.
    :param where:   A file-like object where the stdout writes to
    :return:
    '''
    sys.stdout = where
    try:
        yield where
    finally:
        sys.stdout = sys.__stdout__
