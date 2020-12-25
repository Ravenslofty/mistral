ALLINC =
ALLLIB = -lbz2
DEPLIB =

PROG = mistral-cv
OBJS = main.o cyclonev.o cv-files.o cv-opt.o cv-rmux.o cv-bmux.o cv-inv.o cv-rpats.o cv-pkg.o cv-models.o cv-bmux-data.o \
       cvd-e50f.o cvd-gt75f.o cvd-gt150f.o cvd-gt300f.o cvd-gx25f.o cvd-sx50f.o cvd-sx120f.o \
       cvd-e50f-blk.o cvd-gt75f-blk.o cvd-gt150f-blk.o cvd-gt300f-blk.o cvd-gx25f-blk.o cvd-sx50f-blk.o cvd-sx120f-blk.o \
       cvd-e50f-pkg.o cvd-gt75f-pkg.o cvd-gt150f-pkg.o cvd-gt300f-pkg.o cvd-gx25f-pkg.o cvd-sx50f-pkg.o cvd-sx120f-pkg.o \
       cvd-e50f-inv.o cvd-gt75f-inv.o cvd-gt150f-inv.o cvd-gt300f-inv.o cvd-gx25f-inv.o cvd-sx50f-inv.o cvd-sx120f-inv.o

SRCS = main.cc cyclonev.cc cv-files.cc cv-opt.cc cv-rmux.cc cv-bmux.cc cv-inv.cc cv-rpats.cc cv-pkg.cc cv-models.cc cv-bmux-data.cc \
       cvd-e50f.cc cvd-gt75f.cc cvd-gt150f.cc cvd-gt300f.cc cvd-gx25f.cc cvd-sx50f.cc cvd-sx120f.cc \
       cvd-e50f-blk.cc cvd-gt75f-blk.cc cvd-gt150f-blk.cc cvd-gt300f-blk.cc cvd-gx25f-blk.cc cvd-sx50f-blk.cc cvd-sx120f-blk.cc \
       cvd-e50f-pkg.cc cvd-gt75f-pkg.cc cvd-gt150f-pkg.cc cvd-gt300f-pkg.cc cvd-gx25f-pkg.cc cvd-sx50f-pkg.cc cvd-sx120f-pkg.cc \
       cvd-e50f-inv.cc cvd-gt75f-inv.cc cvd-gt150f-inv.cc cvd-gt300f-inv.cc cvd-gx25f-inv.cc cvd-sx50f-inv.cc cvd-sx120f-inv.cc

JUNK =

OPT=-O3

CC=gcc
AS=as
CXX=g++
CFLAGS=-g -Wall -std=c++14 ${OPT}

all: ${PROG}

.SUFFIXES: .cc .hh .o .a .ui .m.cc .m.o .y .s
.PRECIOUS: ${JUNK}

${PROG}: ${OBJS} ${DEPLIB}
	${CXX} ${CFLAGS} ${OBJS} ${ALLLIB} ${ALLINC} -o ${PROG} 

.cc.o:
	${CXX} -c ${CFLAGS} ${DEFS} ${ALLINC} $<

.c.o:
	${CC} -c ${CFLAGS} ${DEFS} ${ALLINC} $<

.y.c:
	bison -d -o $@ $<

.y.h:
	bison -d -o $@ $<

clean:
	rm -f ${PROG} ${OBJS} ${JUNK}

distclean:
	rm -f ${PROG} ${OBJS} ${JUNK}

###
main.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cyclonev.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-files.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-opt.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-rmux.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-bmux.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-rpats.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-models.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cv-bmux-data.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-e50f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt75f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt150f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt300f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gx25f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx50f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx120f.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-e50f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt75f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt150f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt300f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gx25f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx50f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx120f-blk.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-e50f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt75f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt150f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt300f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gx25f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx50f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx120f-pkg.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-e50f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt75f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt150f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gt300f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-gx25f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx50f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
cvd-sx120f-inv.o: cyclonev.h cv-rnodetypes.ipp cv-blocktypes.ipp cv-porttypes.ipp cv-bmuxtypes.ipp
