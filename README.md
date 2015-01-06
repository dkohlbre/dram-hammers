What
====
Contains testing code for trying to do user-space dram corruption.
See http://users.ece.cmu.edu/~omutlu/pub/dram-row-hammer_isca14.pdf


Usage
=====
make

./benchmark32

./benchmark64

Then edit hammertest.c so that NUM_ITERS is ~ the number of iterations to take 10 seconds.

make

./hammertest32 or ./hammertest64

See if you get errors!
