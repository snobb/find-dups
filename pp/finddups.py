#!/usr/bin/env python
#
# { finddups.py }
# Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]
#

import sys, os, hashlib

VERSION = "1.00"

def get_md5sum(fpath, bufsize=128):
    m = hashlib.md5()
    with open(fpath) as f:
        while True:
            buf = f.read(bufsize)
            if len(buf) <= 0:
                break
            m.update(buf)
    return m.hexdigest()

def find_dups(top):
    record={}
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)
            md5sum = get_md5sum(path)
            record.setdefault(md5sum, []).append(path)
    return record

def usage():
    print "finddup v{} [python edition]".format(VERSION)
    print "Usage: finddup [directory/files to search]\n"

if __name__ == "__main__":
    if (len(sys.argv) > 1):
        directory = sys.argv[1]
    else:
        usage()
        exit(1)

    db = find_dups(directory)
    for chksum in db.iterkeys():
        if len(db[chksum]) > 1:
            print "{}\n\t".format(chksum),
            print "\n\t".join(db[chksum])

# vim: set ts=4 sts=4 sw=4 tw=80 ai smarttab et list
