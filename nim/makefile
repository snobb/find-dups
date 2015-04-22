TARGET = finddups
SRC = main.nim
OBJ = ${addprefix ${CACHEDIR}, ${OBJS}}
NIM = nim
NIMFLAGS =


all: debug

debug: NIMFLAGS += --debuginfo --verbosity:2 -d:debug
debug: clean build

release: NIMFLAGS += --verbosity:1 -d:release
release: clean build

build: ${TARGET}

${TARGET}:
	${NIM} ${NIMFLAGS} -o=${TARGET} c ${SRC}

clean:
	-rm -rf ${TARGET} nimcache

.PHONY: build clean debug release
