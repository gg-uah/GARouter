__author__ = 'ctomas'

import os, sys

# Import etree from lxml package
try:
    from lxml import etree
except ImportError:
    sys.exit("Failed to import ElementTree from lxml")


def getLanesFromNetFile(netfile):
    '''
    Collects into a list all the lane ids and lengths from the network file.
    :param edgfile:     The network file
    :return:            A list containing the lane ids and lengths in the network
    '''
    if not os.path.isfile(netfile):
        sys.exit("Missing network file \'%s\'" % netfile)

    # Parse the edge file
    tree = etree.parse(netfile)

    # The root points to the <net> tag
    doc = tree.getroot()

    # Create list containing the lane identifiers and lengths
    lanes = list()
    for lane in doc.findall('edge/lane'):
        laneId  = lane.attrib['id']
        laneLen = lane.attrib['length']

        # Append a dictionary id/length to the lane list
        if not laneId.startswith(':'):
            laneData = {'id' : laneId, 'length' : laneLen}
            lanes.append(laneData)

    return lanes
