CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

INPUTFILESIZEMEGABYTES = 1

KILO = 1024
MEGA = $(shell echo $(KILO)\*$(KILO) | bc)
INPUTFILESIZEBYTES = $(shell echo $(MEGA)\*$(INPUTFILESIZEMEGABYTES) | bc)
INPUTBLOCKSIZEBYTES = $(KILO)
INPUTBLOCKS = $(shell echo $(INPUTFILESIZEBYTES)\/$(INPUTBLOCKSIZEBYTES) | bc)

EXECUTABLES = pi pi-sched rw rw-sched mix mix-sched rr_quantum
OBJECTS = $(EXECUTABLES:%=%.o)
#OBJECTS = $(foreach exe,$(EXECUTABLES),$(exe).o)

.PHONY: all clean

all: $(EXECUTABLES)

pi: pi.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

pi-sched: pi-sched.o pi
	$(CC) $(LFLAGS) $@.o -o $@ -lm

rw: rw.o rwinput
	$(CC) $(LFLAGS) $@.o -o $@ -lm

rw-sched: rw-sched.o rw
	$(CC) $(LFLAGS) $@.o -o $@ -lm

mix: mix.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

mix-sched: mix-sched.o mix
	$(CC) $(LFLAGS) $@.o -o $@ -lm

rr_quantum: rr_quantum.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

rwinput: Makefile
	dd if=/dev/urandom of=./rwinput bs=$(INPUTBLOCKSIZEBYTES) count=$(INPUTBLOCKS)

%.o: %.c
	$(CC) $(CFLAGS) $<

clean: testclean
	rm -f $(EXECUTABLES)
	rm -f *.o
	rm -f *~
	rm -f handout/*~
	rm -f handout/*.log
	rm -f handout/*.aux

testclean:
	rm -f mixoutput*
	rm -f rwoutput*
	rm -f rwinput*
