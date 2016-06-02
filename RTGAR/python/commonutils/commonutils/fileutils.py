'''
fileutils module: implements some utilities to deal with files
'''
import os
import fnmatch
import const

def find(pattern, path=os.curdir):
    '''
    Find a file searching from the specified directory path
    :param pattern: The file pattern to search
    :param path:    The directory path
    :return:        The file(s) as a result of the searching
    '''
    result = []

    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result


def resolveOutputFile(fileName, prefix, filePath, defaultName = "default_name.dat"):
    '''
    Fit output file name by forcing it to start by the OUTPUT_FILE_PREFIX
    and setting its path.
    :param filename:    The output file name
    :param prefix:      The prefix for output files
    :param filepath:    The path of the output file
    :param default:     The default file name
    :return:
    '''

    # If file name not specified, set it to the default name
    if not fileName:
        fileName = defaultName

    # The output file must start with the OUTPUT_FILE_PREFIX
    outName = (prefix + fileName) \
            if not fileName.startswith(prefix) \
            else fileName

    return os.path.join(str(filePath), outName)
