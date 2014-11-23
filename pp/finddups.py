#!/usr/bin/env python
#
# finddups.py
# Author: Alex Kozadaev (2013)
#

import sys, os, hashlib

VERSION = "1.03"

record = {}

def get_md5sum(filename):
    md5 = hashlib.md5()
    with open(filename,"rb") as f:
        for chunk in iter(lambda: f.read(128*md5.block_size), b""):
            md5.update(chunk)
    return md5.hexdigest()


def find_dups(top):
    global record
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)
            if not os.path.islink(path):
                md5sum = get_md5sum(path)
                record.setdefault(md5sum, []).append("{}".format(path))
    return record


def usage(prog):
    print "{} v{} [python edition]".format(prog, VERSION)
    print "Usage: finddup [directory/files to search]\n"


if __name__ == "__main__":
    prog = sys.argv.pop(0)
    if (len(sys.argv) == 0):
        usage(prog)
        exit(1)

    try:
        for directory in sys.argv:
            db = find_dups(directory)

        for chksum in record.iterkeys():
            if len(db[chksum]) > 1:
                print "{}\n\t".format(chksum),
                print "\n\t".join(db[chksum])
    except IOError as e:
        print "ERROR: {}".format(e.message)
        exit(1)
    except KeyboardInterrupt:
        print "interrupted..."
        exit(1)


# vim: ts=4 sts=4 sw=4 tw=80 ai smarttab et fo=rtcq list
