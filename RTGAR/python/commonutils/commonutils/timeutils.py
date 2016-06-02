__author__ = 'ctomas'

import sys
import datetime
import time
import re
import const

const.TIME_PATTERN = r'([01]?[0-9]|2[0-3])\.[0-5][0-9]'

def validateTime(time):
    '''
    Get the time value from a time option in the command line. The time must match the format 'HH.MM'
    :param time:    The time value from the command line option
    :return:        The suitable time value that matches the format 'HH.MM'
    '''
    try:
        res = re.match(const.TIME_PATTERN, time)
        if (res.group() != time):
            return False
    except AttributeError as ex:
        print "Bad time expression \'%s\'. Time expression must match the format <HH.MM>" % time
        return False
    else:
        return True


def getTimeSeconds(stime):
    """
    Get the time of the day in seconds for the given time in format <HH.MM>
    :param stime: The time of the day in format <HH.MM>
    :return:        The time of the day in seconds
    """
    timesecs = None

    if not validateTime(stime):
        sys.exit("Bad time expression \'%s\'. Time expression must match the format <HH.MM>" % stime)

    # Get seconds from time in format 'HH.MM'
    try:
        tm = time.strptime(stime, '%H.%M')
        timesecs = datetime.timedelta(hours = tm.tm_hour,
                                      minutes = tm.tm_min,
                                      seconds = tm.tm_sec)\
            .total_seconds()
    except Exception as ex:
        print "Fail to convert time \'%s\' into seconds: %s" % (stime, ex.message)
    finally:
        return int(timesecs)
