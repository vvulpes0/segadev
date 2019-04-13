# segadev

This repository contains a set of tools intended to ease development
of software for the Sega Genesis (Mega Drive) console.  Currently,
the following programs are included:

* mbitpad
* mdchksum
* mdconvert


## mbitpad

The `mbitpad` program simply appends a given byte (0xFF by default)
to its input until its size is at least a given number of megabits.
For a specific example,

    mbitpad -c 0xFF -s 4 rom.bin

pads the file `rom.bin` with an <FF> character
until it is 4Mbit in size (equivalent to 0.5 MB).
If this file is already at least this large, it is unchanged.
For more information, see

    mbitpad -h


## mdchksum

Correct the checksum in a non-interleaved ROM.
By default, the program merely computes and displays
the checksum that the given file should have,
which is a non-destructive operation.
If the `-f` switch is given, or `-w value`,
then by default a copy of the input file is written to the standard output
except that the location that corresponds to a ROM file's checksum field
is overwritten by either a computed checksum in the former case,
or the given value in the latter.
This is also a non-destructive operation,
though the `-i` switch requests an in-place edit.

For more information, see

    mdchksum -h

or

    man mdchksum


## mdconvert

This program consists of a driver script `mdconvert`
for a few different sub-programs.
See mdconvert/README for more information.
