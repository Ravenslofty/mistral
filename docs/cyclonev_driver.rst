The mistral-cv command-line program
===================================

The mistral-cv command line program allows for a minimal interfacing
with the library.  Calling it without parameters shows the possible
usages.


models
------

.. code-block::

    mistral-cv models

Lists the known models with their SKU, IDCODE, die, variant, package,
number of pins, temperature grade and speed grade.


routes
------

.. code-block::

    mistral-cv routes <model> <file.rbf>

Dumps the active routes in a rbf.


routes2
-------

.. code-block::

    mistral-cv routes <model> <file.rbf>

Dumps the active routes in a rbf where a GIN/GOUT/etc does not have a
port mapping associated.


cycle
-----

.. code-block::

    mistral-cv cycle <model> <file.rbf> <file2.rbf>

Loads the rbf in file1.rbf and saves is back in file2.rbf.  Useful to
test if the framing/unframing of oram/pram/cram works correctly.


bels
----

.. code-block::

    mistral-cv bels <model>

Dumps a list of all the logic elements of a model (only depends on the
die in practice).


decomp
------

.. code-block::

    mistral-cv decomp <model> <file.rbf> <file.bt>

Decompiles a bitstream into a compilable source.  Only writes down
what is identified as not being in default state.


comp
----

.. code-block::

    mistral-cv comp <file.bt> <file.rbf>

Compiles a source into a bitstream.  The source includes the model
information.


diff
----

.. code-block::

    mistral-cv diff <model> <file1.rbf> <file2.rbf>

Compares two rbf files and identifies the differences in terms of
oram, pram and cram.  Useful to list mismatches after a decomp/comp
cycle.

