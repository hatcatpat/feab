![logo/example](https://github.com/hatcatpat/feab/assets/39860407/f9238e35-1954-494a-b412-e25f16dba28f)

# an 8bit fantasy console and assembly language

*feab* is a fantasy console, similar to [pico8](https://www.lexaloffle.com/pico-8.php) and [tic80](https://tic80.com/), and an 8bit assembly language inspired by (6502 or [uxntal](https://wiki.xxiivv.com/site/uxntal.html))

* the console and program are stored in a single array, meaning that you are free to modify any byte of the console or program whenever you want. the only exceptions being the return stack, stack counter (sc) and program counter (pc), and program length.

* there are no registers, so all opcodes are applied to a given location in memory.

* labels act as functions or variables, depending on how you use them. a label is simply a location in memory, so you can either jump to that location (function), or modify the byte at that location (variable).

* labels are unique (i.e., no 2 can have the same name), whereas sublabels (denoted by a '@') can be reused in each scope.

* macros are of the form # NAME A B C D ... #
  * ascii letters, useful memory locations, and key bitflags are avaiable as macros by default

* addresses are 16-bits wide (two 8-bits). if you use a label, it will be converted to a 16-bit number. numbers prefixed with a & will also be converted to a 16-bit number.

* numbers can be base-10, hexadecimal (ff), or binary (%01101100)

* some key opcodes:
  * SET dest what -> memory[dest] = what
  * GET dest source -> memory[dest] = memory[source]
  * REF dest location -> memory[dest] = HIGH(location), memory[dest + 1] = LOW(location)
  * DEREF dest pointer -> memory[dest] = memory[memory[pointer] << 8 | memory[pointer + 1]]
  * JUMP address -> sets the PC to the given address
  * CALL address -> same as jump, but pushes the previous location on the return stack
  * RET -> returns to the previous location on the return stack
  * WAIT -> waits until the next frame

* many opcodes also have a V (value) mode, which treats the first argument as a pointer to an address. this allows you to store addresses in variables, and modify the address that is pointed to.
