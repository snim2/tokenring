#!/usr/bin/env python

"""
Compile all benchmarks.
"""

from __future__ import print_function

import os
import os.path

__author__ = 'Sarah Mount <s.mount@wlv.ac.uk>'
__date__ = 'March 2014'

_BASEPATH = os.path.dirname(__file__)

_PATH = os.path.abspath(os.path.join(_BASEPATH, '../benchmarks/'))


def compile_benchmarks():
    """Compile all benchmarks.
    """
    os.chdir(_PATH)
    retval = os.system('make clean')
    if retval != 0:
        print('make clean did not exit cleanly.')
    retval = os.system('make')
    if retval != 0:
        print('make could not compile all benchmarks.')
    return


if __name__ == '__main__':
    compile_benchmarks()
