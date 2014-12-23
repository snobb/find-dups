#!/usr/bin/env python3
#
# finddups.py
# Author: Alex Kozadaev (2013)
#

import sys, os, hashlib
import errno

VERSION = "1.04"


def get_md5sum(filename):
    md5 = hashlib.md5()
    with open(filename,"rb") as f:
        for chunk in iter(lambda: f.read(128*md5.block_size), b""):
            md5.update(chunk)
    return md5.hexdigest()


def find_dups(top, db):
    count = 1
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)

            if os.path.islink(path):
                continue

            md5sum = get_md5sum(path)
            db.setdefault(md5sum, []).append(path)

            print("\rProcessed files: {} ".format(count),
                    file=sys.stderr, end="")
            sys.stderr.flush()
            count += 1


def usage(prog):
    print("{} v{} [python edition]".format(prog, VERSION))
    print("Usage: finddup [directory/files to search]\n")


if __name__ == "__main__":
    db = {}
    prog = sys.argv.pop(0)

    if (len(sys.argv) == 0):
        usage(prog)
        exit(1)

    try:
        for directory in sys.argv:
            find_dups(directory, db)

        print("\r", file=sys.stderr, end="")
        sys.stderr.flush()

        for chksum in db.keys():
            if len(db[chksum]) > 1:
                print("{}\n\t".format(chksum), end="")
                print("\n\t".join(db[chksum]))
    except IOError as e:
        if e.errno == errno.EPIPE:
            pass    # ignoring SIGPIPE
        else:
            print("ERROR: {}".format(e), file=sys.stderr)
            exit(1)
    except KeyboardInterrupt:
        print("interrupted...", file=stderr)
        exit(1)


# vim: ts=4 sts=4 sw=4 tw=80 ai smarttab et fo=rtcq list
