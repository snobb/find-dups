#
# main.nim
# Author: Alex Kozadaev (2015)
#

import os, strutils, md5, tables

const VERSION = "0.01"

type
    CheckSumTable {.byRef.} = Table[string, seq[string]]
    EKeyboardInterrupt = object of Exception

var fCount: int64 = 0


# calculate md5 sum of a given file
proc getMd5Sum(fname: string) :string=
    const blockSize: int = 64 * 128;
    var
        c: MD5Context
        d: MD5Digest
        f: File
        bytesRead: int = 0
        bytesTotal: int64 = 0
        buf: array[blockSize, char]

    try:
        f = open(fname)

        md5Init(c)
        while true:
            bytesRead = f.readBuffer(buf.addr, blockSize)
            if bytesRead <= 0:
                break
            bytesTotal += bytesRead
            md5Update(c, buf, bytesRead)

        md5Final(c, d)
    except IOError:
        echo("error: $#" % getCurrentExceptionMsg())
        return ""
    finally:
        if f != nil:
            f.close()

    return $d


# initialize checksum table
proc initCheckSumTable() :CheckSumTable{.inline.}=
    initTable[string, seq[string]]()


# show progress (how many files have been processed)
proc showProgress() {.inline.}=
    fCount.inc()
    write(stderr, "\rprocessed files: ", fCount, " ")


# collect data from files
proc collect(path: string, data: var CheckSumTable) =
    for fpath in walkDirRec(path):
        let md5sum = getMd5Sum(fpath)
        if md5sum.len() == 0: continue
        if data.hasKey(md5sum):
            data.mget(md5sum).add(fpath)
        else:
            data[md5sum] = @[fpath]

        showProgress()


# show usage
proc usage() =
    echo("finddup v", VERSION)
    echo("Usage: finddup [directory/files to search]")
    quit()


# SIGINT (ctrl-c) handler
proc sigIntHandler() {.noconv.} =
  raise newException(EKeyboardInterrupt, "Keyboard Interrupt")


# entry point
when isMainModule:
    setControlCHook(sigIntHandler)
    let args = commandLineParams()

    if args.len() == 0:
        usage()

    var data = initCheckSumTable()

    try:
        for arg in args:
            collect(arg, data)

        # check if we need to print
        if data.len() == 0:
            quit()

        # print the results
        for key, values in data.pairs():
            if values.len() > 1:
                echo("\r", key, ":")
                for value in values:
                    echo("\t", value)
    except EKeyboardInterrupt:
        write(stderr, "\n", getCurrentExceptionMsg(), "\n")
    finally:
        write(stderr, "\n");

