#!/usr/local/bin/python

import os

prefix = 'pt_'
for i in range(100):
    name = prefix + str(i)
    print "creating name ..."
    os.mkdir(name)
