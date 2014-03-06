#!/usr/bin/env python

"""
Gather version numbers for every runtime we are benchmarking.
"""

from __future__ import print_function

import csv
import os
import os.path
import subprocess

__author__ = 'Sarah Mount <s.mount@wlv.ac.uk>'
__date__ = 'March 2014'

_BASEPATH = os.path.dirname(__file__)

_PATH = os.path.abspath(os.path.join(_BASEPATH, '../benchmarks/'))

_DATA = os.path.abspath(os.path.join(_BASEPATH, '../data/'))

def get_version_info():
    """Get version info for all runtimes.
    """
    versions = dict()
    subdirs = [folder[0] for folder in os.walk(_PATH)]
    for subdir in subdirs:
        if subdir == _PATH:
            continue
        try:
            os.chdir(subdir)
            long_v = subprocess.check_output('make version', shell=True)
            short_v = subprocess.check_output('make version-short', shell=True)
            versions[os.path.basename(subdir)] = (str(long_v).strip(),
                                                  str(short_v).strip())
        except subprocess.CalledProcessError:
            pass
    return versions


def write_version_info(versions, filename='versions.csv'):
    """Write version info to a CSV file formatted like this:

    Benchmark, Version, Version Short
    Scala, scala-2.9.1, 2.9.1
    Go,    golang-1.5, 1.5
    """
    with open(os.path.join(_DATA, filename), 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerow(('Benchmark', 'Version', 'Version Short'))
        for bench in versions:
            writer.writerow((bench, versions[bench][0], versions[bench][1]))
    return


if __name__ == '__main__':
    write_version_info(get_version_info())
