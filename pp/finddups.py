#!/usr/bin/env python
#
# finddups.py
# Author: Alex Kozadaev (2013)
#

import sys, os, hashlib
import errno

VERSION = "1.03a"

counter = 0

def get_md5sum(filename):
    md5 = hashlib.md5()
    with open(filename,"rb") as f:
        for chunk in iter(lambda: f.read(128*md5.block_size), b""):
            md5.update(chunk)
    return md5.hexdigest()


def print_progress():
    global counter
    print >>sys.stderr, "\rProcessing ... {} ".format("-/|\\"[counter]),
    sys.stderr.flush()
    counter = (counter + 1) % 4


def find_dups(top, db):
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)

            if os.path.islink(path):
                continue

            print_progress()

            md5sum = get_md5sum(path)
            db.setdefault(md5sum, []).append("{}".format(path))


def usage(prog):
    print "{} v{} [python edition]".format(prog, VERSION)
    print "Usage: finddup [directory/files to search]\n"


if __name__ == "__main__":
    db = {}
    prog = sys.argv.pop(0)

    if (len(sys.argv) == 0):
        usage(prog)
        exit(1)

    try:
        for directory in sys.argv:
            find_dups(directory, db)

        print >>sys.stderr, "\r",

        for chksum in db.iterkeys():
            if len(db[chksum]) > 1:
                print "{}\n\t".format(chksum),
                print "\n\t".join(db[chksum])
    except IOError as e:
        if e.errno == errno.EPIPE:
            pass
        else:
            print >>sys.stderr, "ERROR: {}".format(e)
            exit(1)
    except KeyboardInterrupt:
        print >>sys.stderr, "interrupted..."
        exit(1)


# vim: ts=4 sts=4 sw=4 tw=80 ai smarttab et fo=rtcq list
