# aria

A collection of Oberon-2 modules and tools for Ofront compilers.

## Tools

* `OEF` - Ofront Error Filter. This small program takes error messages output by the compiler and converts to a human-readable format.
* `OMake` - [Oberon Make tool](doc/OMake.md). **OMake** is a tool for automating the process of building large Oberon-2 projects. **OMake** uses only the dependency information already expressed in the Oberon-2 source, and requires no additional scripts in order to compile and link projects.

### Installation

1. Set the variable `OFRONT` to point to your OfrontPlus installation
2. Set the variable `TARGET` to your build target (eg. macOS)
3. In the top directory: `make`

This should run a bootstrap script to build `OMake`, which is then used to build other targets: `OEF`, and `TestAll`. 

Configuration for `OMake` is in `omake.ini`. This might need to be adjusted if special C compiler options are required.

## Modules

These are some basic utility modules I am using for my own projects. The overall philosophy is to build on functionality that is already in libc.

### Text

Most Oberon systems have a particular way of handling text, based on the mutable documents. This is great if you want a text editor, but sometimes its an overkill for simple things.

`arText` provides a basic interface for writing Oberon primitive types as text, similar to `Out`, or `Console` in the standard library. `arText.Writer` has procedures for writing Char, String, Integer, Hex, Real, Boolean, Set, and Ln. `Writer` can be used to:

- Construct strings
- Output text to streams (stdout, stderr)
- Output text to files

`arText.Writer` is built on a simpler interface `arText.Stream` which basically just outputs Char and String. `arText.Writer` uses libc to convert Integer, Real, and Hex to text, and default conversions can be specified using `printf` style formats.

`arText.Buffer` is a mutable text string that can be built incrementally using the `Writer` interface. Text.Buffers are often instantiated statically as local variables, and are used for quickly constructing strings (like a Java StringBuffer).

`arStream` implements text streams for libc `FILE` objects: stdout, stderr, and files.

### Data Types

`arStrings` is a module for basic string operations, based on `ooc2Strings` but with some additional functions.

`arValue` is A set of abstract data types. This is useful for defining data resources for programs. Oberon lacks any structured constants, which can make it painful to define constant data. Types implemented here can be read directly from JSON strings or files, and can also be serialised to JSON text.

Boxed types for Oberon scalars:

* Set
* Boolean
* Real
* Integer
* String (backed by POINTER TO ARRAY OF CHAR)

Collection types:

* List, a variable length linked list
* Record, similar to a Javascript/JSON object
* Array, fixed length tuples, backed by POINTER TO ARRAY OF Object

Object base type supports:

* Equals - test for equality
* Copy and DeepCopy
* Convert to String
* Read structured object from JSON string
* Write structured object to text or JSON

`arJSON` is a JSON parser. `arJSON.ReadFile` reads a file and returns an `arValue.Object`. `arJSON.ParseString` parses a string and returns an `arValue.Object`. Thus JSON objects can be stored in external files, or converted directly from string literals within the program.

### Other utilities

`arConfiguration` implements a basic system for parsing command-line options, and configuration files.

`arSize` defines 2-argument minimum / maximum functions which are missing from Oberon-2, but implemented in Component Pascal.

`arMath` contains code procedures for `math.h` functions.

`arPath` contains functions for manipulating file and directory names, paths, and extensions.

`arFormat` attempts some basic formatting in the spirit of printf. Oberon doesn't have have variable argument lists, so there are families of function for specific numbers of parameters. Arguments are of type `arValue.Object`, so primitive types must be boxed like so:

    F.Out3("{} plus {} equals {}\n", V.int(x), V.int(y), V.int(x+y));

Use `OutN` to print a formatted string with N parameters. The corresponding `FormatN` functions return a formatted `arValue.String`. For example:

    text := F.Format2("Moved to position ({}, {})", V.real(x), V.real(y));

For Real and Integer, standard printf Formatting options are supported. Don't include the '%' as this is added automatically. For example:

    F.Out3("MIDI: {02x} {02x} {02x}\n", V.int(status), V.int(data1), V.int(data2));


## Libraries

`src/lib` contains bindings for a few of my favourite C libraries. Most of these are minimal bindings written by hand, with Ofront-style code procedures. Included are:

* libPortAudio
* libPortMidi
* libSDL
* libCairo

