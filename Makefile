# Universal Makefile for C + unit testing
# The directory layout should be as follows:
# /
# |\_ src       - main sources (set the name of the <main function>.c file via $(MAIN). Dir overriden via SRCDIR
# |\_ test      - test sources. Dir overriden via TSTDIR
# |__ makefile  - this makefile
#
# Quite simple/timesaving solution in order not to use autohell for every project.

SRCDIR = src
TSTDIR = test
MAIN = $(SRCDIR)/main.c
MAINOBJ = $(MAIN:.c=.o)
TARGET = $(SRCDIR)/finddups
TST_TARGET = $(TSTDIR)/tstprog

SRC = $(wildcard $(SRCDIR)/*.c)
TMPSRC = $(filter-out $(MAIN), $(SRC))
OBJ = $(TMPSRC:.c=.o)

TSTSRC = $(wildcard test/*.c)
TSTOBJ = $(TSTSRC:.c=.o)

HDR = $(wildcard *.h)

CC = cc
CFLAGS += -Wall
LFLAGS = -lcrypto
TSTLFLAGS = -g -lcrypto

INSTALL = install
INSTALL_ARGS = -o root -g root -m 755
INSTALL_DIR = /usr/local/bin/

ifeq ($(CC), $(filter $(CC), cc clang gcc))
	CFLAGS += -std=gnu99 -pedantic
endif

REVCNT = $(shell git rev-list --count master 2>/dev/null)
REVHASH = $(shell git log -1 --format="%h" 2>/dev/null)

ifeq ($(REVCNT),)
	VERSION = devel
else
	VERSION = "$(REVCNT).$(REVHASH)"
endif

all: debug

debug: CFLAGS += -g -DDEBUG
debug: LFLAGS += -g
debug: build

release: CFLAGS += -Os
release: LFLAGS += -s
release: clean build

build: build_host.h $(TARGET)

build_host.h:
	@echo "#define BUILD_HOST \"`hostname`\""      > $(SRCDIR)/build_host.h
	@echo "#define BUILD_OS \"`uname`\""          >> $(SRCDIR)/build_host.h
	@echo "#define BUILD_PLATFORM \"`uname -m`\"" >> $(SRCDIR)/build_host.h
	@echo "#define BUILD_KERNEL \"`uname -r`\""   >> $(SRCDIR)/build_host.h
	@echo "#define BUILD_VERSION \"$(VERSION)\""  >> $(SRCDIR)/build_host.h

$(TST_TARGET): clean $(OBJ) $(TSTOBJ)
	$(CC) $(TSTLFLAGS) -o $@ $(OBJ) $(TSTOBJ)

test: check

check: $(TST_TARGET)
	@echo ===========================
	@./$(TST_TARGET)

run: all
	@echo ===========================
	@./$(TARGET)

$(TARGET): build_host.h $(OBJ) $(MAINOBJ)
	$(CC) $(LFLAGS) -o $@ $(OBJ) $(MAINOBJ)

$(?D)%.o : $(?D)%.c
	$(CC) $(CFLAGS) -c $? -o $@

install: release
	${INSTALL} ${INSTALL_ARGS} ${TARGET} ${INSTALL_DIR}
	@echo "DONE"

testclean:
	-rm -f $(TSTDIR)/*.o $(TSTDIR)/*core $(TST_TARGET)

clean: testclean
	-rm -f $(SRCDIR)/build_host.h
	-rm -f $(SRCDIR)/*.o $(SRCDIR)/*core $(TARGET)

.PHONY : all debug release build run check clean testclean
