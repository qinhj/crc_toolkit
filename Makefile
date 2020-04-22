# compiler
CC  := $(CROSS_TOOL)gcc
CXX := $(CROSS_TOOL)g++
AR  := $(CROSS_TOOL)ar
LD  := $(CROSS_TOOL)ld
STRIP := $(CROSS_TOOL)strip

# user config:
CFLAG   += --sysroot=$(SYSROOT) -fmessage-length=0 -Wall
CPPFLAG += --sysroot=$(SYSROOT) -fmessage-length=0 -Wall
CFLAG   += -D _DEBUG #-D _GNU_SOURCE -fvisibility=hidden
CPPFLAG += -D _DEBUG #-D _GNU_SOURCE -fvisibility=hidden
ifdef CRC64
CFLAG   += -D CRC64
CPPFLAG += -D CRC64
endif#CRC64

# include settings
inc     := -I./inc
extinc  ?= #-I./extinc
INCFLAG := $(inc) $(extinc)

# library settings
lib     := #-L./lib
extlib  ?= #-L./extlib
LIBPATH := $(lib) $(extlib)
# Note: To link libdl.so, one need add -D _GNU_SOURCE to CFLAG.
LIBLINK :=

# ========================================================================================

# compile flag
LIBFLAG := $(CFLAG) -shared -fPIC
LDFLAG  := $(LIBPATH) -rdynamic -Wl,--as-needed $(LIBLINK)

# runtime rpath dir
RPATH_DIR ?= .

# targets: lib/bin/sample
LIBRARY := libcrc.so
BINARY  := crc_toolkit
SAMPLE  := demo crc_tester crc_collision
TARGETS := $(LIBRARY) $(BINARY) $(SAMPLE)

# header files
HEADERS := inc
HEADERS := $(foreach dir, $(HEADERS), $(wildcard $(dir)/*.h))

# source code files
SRC_DIRS := src $(extsrc)
SRC_FILE := $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
SRC_OBJS := $(patsubst %.c, %.o, $(SRC_FILE))

# resource directory
RES_DIRS := . src res extsrc

# ========================================================================================

default: all

strip: $(TARGETS)
	$(STRIP) $^

_clean:
	for subdir in $(RES_DIRS); do \
		(cd $$subdir && rm -rf *.o *~ *.d) \
	done;

all: clean info
	make lib
	make bin
	make sample
	#make _clean

lib: $(LIBRARY)
libcrc.so: $(SRC_OBJS)
	$(CC) -o $@ $^ $(LIBFLAG) $(LDFLAG)

bin: $(BINARY)
crc_toolkit:
	@echo "not support yet"
	#$(CC) -o $@ $^ $(CFLAG) $(LDFLAG) -L. -llibcrc -Wl,-rpath,$(RPATH_DIR)

sample: $(SAMPLE)
demo: $(SRC_OBJS) main.o
	$(CC) -o $@ $^ $(CFLAG) $(LDFLAG)
crc_tester: extsrc/crc_tester.o
	$(CC) -o $@ $^ $(CFLAG) $(LDFLAG)
crc_collision: extsrc/crc_test_collision.o
	$(CC) -o $@ $^ $(CFLAG) $(LDFLAG)

clean:
	make _clean
	rm -f $(TARGETS)

info:
	@echo "============================================================================="
	@echo "ROOT_DIR: $(ROOT_DIR)"
	@echo "COMPILER: $(CC)/$(CXX)"
	@echo "SYSROOT:  $(SYSROOT)"
	@echo "STAGING:  $(STAGING)"
	@echo "DESTDIR:  $(DESTDIR)"
	@echo "HEADERS:  $(HEADERS)"
	@echo "============================================================================="

install: info
	@#cp -vf $(BINARY) $(DESTDIR)/bin/
	@cp -vf $(LIBRARY) $(DESTDIR)/lib/
	@cp -vf $(HEADERS) $(STAGING)/usr/include/
	@cp -vf $(LIBRARY) $(STAGING)/usr/lib/

.NOTPARALLEL: clean info demo
.PHONY: demo all clean info install

# ========================================================================================

%.o: %.c
	$(CC) $(INCFLAG) -fPIC -O2 $(CFLAG) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(INCFLAG) -fPIC -O2 $(CPPFLAG) -o $@ -c $<
