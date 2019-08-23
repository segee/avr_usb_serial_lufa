# avr_usb_serial_lufa
A very simple/minimal virtual serial example based on the LUFA library

You must download the LUFA framework to use this code.  
You can find it here (near the bottom).

http://www.fourwalledcubicle.com/LUFA.php

When you unzip the file, you will get a directory structure that tarts:

lufa-LUFA-version/

that directory contains (among other things) a directory called LUFA (this is what I mean by the LUFA directory below) and a directory called Projects.
The makefile in this repository assumes that the repository is located in the Projects directory.

If it is not, you must edit the makefile to point to the LUFA directory.

(and, of course if you use a different AVR or clock speed or whatever, you'll need to edit the makefile as well).

The actual code is a moderately brain-dead high-low guessing game played over a serial port (with a terminal emulator) and is of no value except as an example of: virtual serial port with LUFA, com port status lines, standard input and standard output 
