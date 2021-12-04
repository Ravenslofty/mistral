# Mistral - A Cyclone V bitstream library

## What is it?

It's the very first version of a library/command line utility to
compile and decompile Cyclone V bitstreams, as used in the de-10 nano
(used in MiSTer) and the future Analogue Pocket.


## Compile and install

It's a normal cmake project.  Create a build directory, run cmake
<source directoty> from it then your preferred builder (make,
ninja...).  The only dependency is liblzma.

It takes the usual cmake variables (CMAKE_INSTALL_PREFIX,
CMAKE_BUILD_TYPE) and also proposes the variable COMPRESSION_LEVEL to
choose the level of compression of the internal data.  It's a
size/time choice.  Default is 1.

Some quick tests on a good laptop are in the following table.  The run
time is for running the bels command for a 5CGTFD9C5F23I7 (gt300f, the
biggest).

+--------------------+--------------+----------+--------------+
| Compression level  | Compile time | Run time | Library size |
+--------------------+--------------+----------+--------------+
| 0 (no compression) | 0:52         | 0.71     | 2G           |
| 1 (default)        | 1:30         | 7.61     | 269M         |
| 6 (lzma default)   | 5:20         | 7.33     | 153M         |
| 9                  | 6:39         | 7.22     | 154M         |
+--------------------+--------------+----------+--------------+


Go into the lib directory, run 'make'.  This will generate a
command-line utility named mistral-cv.  You can start it from that
directory, but nowhere else because it looks for the routing databases
in ../gdata.  Read the doc, that should help.  It's very, very raw at
this point.

The makefile does not currently build the library as a library.  But
everything that's not main.cc is in fact the library.  There is only
one header file, cyclonev.h.


## How to help?

There are a lot of things that can be done to help without having to
look inside quartus.  Meanwhile mapping of the blocks and work on the
timings will go on.

### Logic block RE

A bunch of stuff is unsure or not know at all on logic blocks.  They
can be analyzed through creating appropriate verilog files, compiling
them with quartus and decompiling them with mistral-cv.

* The common part of LABs (routing of clocks, enables, clears) is very unsure.
* Mlabs in rom/ram mode are unknown.
* DSPs and M10ks are really unknown, neither the mux usage nor the
  ports mapping is unknown.
* Most peripheral blocks have their ports mapped, or are going to have
  them, but the mux usage is not understood.
