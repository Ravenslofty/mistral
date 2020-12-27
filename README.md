# Mistral - A Cyclone V bitstream library

## What is it?

It's the very first version of a library/command line utility to
compile and decompile Cyclone V bitstreams, as used in the de-10 nano
(used in MiSTer) and the future Analogue Pocket.


## How to use it?

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


### Basic stuff

* Add a real build system, if possible make it portable to
  linux/windows/mac (everything is done on linux only at this point).
  Possibly make it installable.
* Find a better way to look for the routing files.  Possibly embed
  them, if it can be done sanely on different OSes.
* Try different methods to compress the routing files, so that loading
  is faster.  Possibly generate them with code, but that would be a
  lot of work (the contents are regular, but not *that* regular)


### Library interface

* There's a lot of stuff missing from the library interface.  One
  can't enumerate routes, the package access interface is inexistant,
  etc.  Method useful to interface with nextpnr in particular would be
  very welcome.


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
