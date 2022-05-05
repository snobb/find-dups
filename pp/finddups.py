#!/usr/bin/env python
#
# finddups.py
# Author: Alex Kozadaev (2013)
#

import sys
import os
import hashlib
import errno

VERSION = "1.05"


def get_md5sum(filename):
    md5 = hashlib.md5()
    with open(filename, "rb") as f:
        for chunk in iter(lambda: f.read(128 * md5.block_size), b""):
            md5.update(chunk)
    return md5.hexdigest()


def find_dups(top, db):
    count = 0
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)

            if os.path.islink(path):
                continue

            md5sum = get_md5sum(path)
            db.setdefault(md5sum, []).append(path)

            count += 1
            print(f"\rProcessed files: {count} ", end=' ', file=sys.stderr)
            sys.stderr.flush()


def usage(prog):
    print(f"{prog} v{VERSION} [python edition]")
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

        print("\r", end=' ', file=sys.stderr)

        for chksum in db.keys():
            if len(db[chksum]) > 1:
                print(f"{chksum}\n\t", end=' ')
                print("\n\t".join(db[chksum]))
    except IOError as e:
        if e.errno == errno.EPIPE:
            pass  # ignoring SIGPIPE
        else:
            print(f"ERROR: {e}", file=sys.stderr)
            exit(1)
    except KeyboardInterrupt:
        print("interrupted...", file=sys.stderr)
        exit(1)
