#!/usr/bin/env python3
#
# finddups.py
# Author: Alex Kozadaev (2013)
#

import errno
import hashlib
import os
import sys
from typing import Dict, List

VERSION = "1.05"


def get_md5sum(filename: str) -> str:
    md5 = hashlib.md5()
    with open(filename, "rb") as f:
        for chunk in iter(lambda: f.read(128 * md5.block_size), b""):
            md5.update(chunk)
    return md5.hexdigest()


def find_dups(top: str, db: Dict[str, List[str]]) -> None:
    count = 1
    for root, dirs, files in os.walk(top):
        for name in files:
            path = os.path.join(root, name)

            if os.path.islink(path):
                continue

            md5sum = get_md5sum(path)
            db.setdefault(md5sum, []).append(path)

            print(f"\rProcessed files: {count} ", file=sys.stderr, end="")
            sys.stderr.flush()
            count += 1


def usage(prog: str) -> None:
    print(f"{prog} v{VERSION} [python edition]")
    print("Usage: finddup [directory/files to search]\n")


if __name__ == "__main__":
    db: Dict[str, List[str]] = {}
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
                print(f"{chksum}\n\t", end="")
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
