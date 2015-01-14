What
====
Contains testing code for trying to do user-space dram corruption.
See http://users.ece.cmu.edu/~omutlu/pub/dram-row-hammer_isca14.pdf

Code is a bit messy, configured via #defines in hammertest.c

Use run.sh to get nicely formatted output with DIMM information. (You will need to edit run.sh to check the correct dmidecode entry)

Usage
=====
make

./hammertest64

See if you get errors!
