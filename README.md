ARM Thumb2 disassembler tool for the Pebble
===========================================

This is just a toy project to provide a full featured hexeditor,
assembler and disassembler for the Pebble, The plan is to integrate
this into the SysInfo app, but it needs to be upgraded to support
the 3rd version of the SDK.

The disassembler is a fork from the winedbg one. It is handling
most commont Thumb and Thumb2 instructions, which are the only
instruction set supported by the Pebble CPU.

At the moment this is just a tool to disassemble user defined data.

For more info

* https://github.com/rigel314/pebbleSystemInfo
