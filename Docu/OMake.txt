# OMake - A build tool for Ofront+

**OMake** is a tool for automatically building large Oberon-2 projects.

**OMake** uses only the dependency information already expressed in the Oberon-2 source, and requires no extra scripts to compile and link projects.

To build any Oberon-2 module:

```
OMake Module
```

**OMake** locates the source code for `Module` (eg. `Module.ob2`) and parses the `IMPORT` statements to identify all the imported modules. If an imported module is part of a library, that library is added as a link dependency. Otherwise, each imported module is recursively pre-built from each of its dependencies.

**OMake** uses **OFront+** to compile each Oberon source module, producing a corresponding "C" language object file. Finally, it uses the C compiler to compile and link these object files into an executable.

Various options are available to define compiler options and link dependencies.

## Command-line Options

Basic usage is:

    OMake [ options ] { Module }

to automatically locate and process the source for `Module`. Here `Module` is the module name, and the location and extension of the source file is inferred from the project settings.

Alternatively:

    OMake [ options ] { path/to/Module.ob2 }

to process a specific source file.

`options` are one or more command-line or configuration options (see below). All modules included on the command-line are built as "main" modules using the `-m` compiler option, and are linked to an executable called `Module`. Oberon dialect is selected based on the file extension, which can be configured as outlined below. Other options are:

* `-read file`

    Reads configuration from the named file. By default, **OMake** looks for configuration in the file `omake.ini` in the current directory. This option allows alternative files to be specified, or for one file to include another file.

* `-script`

    By default **OMake** calls the compiler and linker to complete the build process. Specifying `-script` causes only the build commands to be output. Such scripts can be used to later build modules without the **OMake** command, for example to bootstrap **OMake** on a system for the first time.

## Configuration Options

**OMake** can be configured on the command-line, or via configuration files.

On the command line, an option is specified with a hyphen in front of the option name, and any additional parameters for that option are given in the following arguments. For example:

    -script
    -read omake.ini
    -link "gcc -g"
    -set TARGET macOS

Arguments that include spaces are enclosed within quotes, as required by the shell.

In configuration files, each option is specified on a separate line beginning with the option name followed by a colon. Any parameters of the option are separated by whitespace, and parameters that include space must be quoted. The following correspond to the above examples:

    script:
    read: omake.ini
    link: "gcc -g"
    set: TARGET macOS

### Compiler and linker commands

The basic compile command is defined by the `compile` option. Example:

    compile: "Ofront+ -88 -s -e"
    
The compile command should only define flags that are common to all modules in the project. Additional flags may be appended based on file type.

Similarly, the link command is defined by the `link` option:

    link: "gcc -Os -g"

### File types and paths

The set of valid module extensions is defined using `extension` options. Each extension defines a set of compiler flags, which are used when compiling modules of that type. This is used to set the language model. **Ofront+** supports Oberon (`-1`), Oberon-2 (`-2`), Oberon-07 (`-7`), Component Pascal (`-C`), and an experimental dialect Oberon-3 (`-3`). The following defines different file extensions for each dialect:

    extension: Mod "-2"
    extension: ob  "-1"
    extension: ob2 "-2"
    extension: ob3 "-3"
    extension: ob7 "-7"
    extension: cp  "-C"

**OMake** needs to know where to find the source code for each imported module. This is specified with the `source` option, which defines a comma-separated list of directories. To locate a module, **OMake** searches each directory in turn, trying each valid extension until it finds a match.

    source: .,test,src/lib,src/aria,[OFRONTLIB]

Certain built-in modules have no dependencies, and may sometimes have no source files. These can be excluded from module search using the `exclude` option:

    exclude: Platform,SYSTEM

### Settings and Environment

**OMake** uses the `set` option to define parameters which can be substituted elsewhere in other configuration options. This is useful to avoid repeating common settings such as directory paths. The following defines two settings: `OFRONT` for the location of the compiler installation, and `TARGET` for the target type. Two other settings (`OFRONTLIB` and `TARGETLIB`) are derived by substitution.

    set: OFRONT /usr/local/src/OfrontPlus
    set: TARGET macOS
    set: OFRONTLIB {OFRONT}/Mod/Lib
    set: TARGETLIB {OFRONT}/Target/{TARGET}/Lib
    
Each setting defines the name of a variable (eg. `OFRONT`) followed by its value (eg. `/usr/local/src/OfrontPlus`). This value can be substituted anywhere else in the configuration file by enclosing the variable name in curly braces (eg. `{OFRONT}`). This will be replaced by the value assigned to the original variable. For example, in the definition of `OFRONTLIB`, the value `{OFRONT}` is replaced by `/usr/local/src/OfrontPlus`, resulting in the value `/usr/local/src/OfrontPlus/Mod/Lib`.

Environment variables can be used to avoid hard-coded paths,  which defers evaluation of the value to the time that the compile and link commands are executed. For example, in the shell:

    export OFRONT=/usr/local/src/OfrontPlus
    export TARGET=macOS

Then in the configuration file, include:

    set: OFRONT $OFRONT
    set: TARGET $TARGET

This has the same effect as the previous example, but allows the variables `OFRONT` and `TARGET` to be controlled by the user's environment, making the configuration file independent of system paths. Occasionally, we need to evaluate an absolute path which may contain such environment variables. For example, when defining a source path to **OMake**. In this situation, we use square brackets instead of curly braces:

    {OFRONTLIB} = $OFRONT/Mod/Lib
    [OFRONTLIB] = /usr/local/src/OfrontPlus/Mod/Lib

Curly braces substitute the literal value; Square brackets causes the evaluation of all environment variables in the value before substitution.

See the main configuration example below. Currently only `lib` and `source` directives need to know absolute paths, and would therefore be written like this:

      lib: Ofront [OFRONTLIB]
      source: .,test,tool,src/lib,src/aria,[OFRONTLIB]

### Library Dependencies

There are two forms of library dependency:

* Pre-built Oberon-2 libraries such as libOfront
* Interface modules that require external C libraries

For pre-built libraries we use the `lib` option to define which modules are part of the library. This is done by associating a source directory (in the `source` path) with a link library:

      lib: Ofront [OFRONTLIB]

The first part of the `lib` option is the name of the library. The second part is the path to the directory containing the library source. If any imported source modules are located within that path, the specified library will be added as a link dependency.

For interface modules, **OMake** includes the option of defining additional flags and link dependencies using a special directive which can be embedded within the source module. This has the form:

    'OMAKE' [ flags ] [ 'LINK' lib { ',' lib } ]

The `OMAKE` directive is be included in a comment between the `MODULE` declaration, and the `IMPORT` list. To define compiler flags, include an optional string after the `OMAKE` keyword. To define link libraries, include the keyword `LINK` followed by a comma-separated list of strings. For Example:

    MODULE Test;
        (* OMAKE "-x" LINK "SDL" *)
    IMPORT ...

In this example, "-x" will be added to the compile flags for module `Test`. Any module importing `Test`, either directly or indirectly, will be linked with library "SDL" using the link option "-lSDL".

To link particular libraries, we may need to define additional paths to header files (-I) or library files (-L). This is done via the `linkflag` option:

    linkflag: Ofront -I{OFRONTLIB}
    linkflag: Ofront -I{TARGETLIB}/Obj
    linkflag: Ofront -L{TARGETLIB}
    linkflag: SDL2 -I/usr/local/include/SDL2
    linkflag: cairo -I/usr/local/include/cairo

The first part defines the name of a library, as identified either through a `lib` directive, or via `OMAKE LINK`. The second part defines the corresponding flags that should be used by the linker when including this library. Multiple flags can be defined by quoting a string of flags, or by repeating the `linkflag` directive.

These flags may vary between systems depending on the location of library files and headers. If a library is available as part of the operating system distribution, then additional flags may not be necessary. If instead the library has been installed by the user, it may be in a local directory. Of course, it is also possible to define such dependencies in the `link` command, or via the environment (eg. like `CFLAGS`).

Some libraries must always be linked regardless of imports, because they contain essential parts of the run-time system. Required link libraries can be declared using the `require` option:

    require: Ofront

Some "header-only" libraries have all implementation code contained in C header files. Such libraries require `linkflags` but have no corresponding `-l` library. Header-only libraries can be declared like this:

    linkflag: CK   -I/usr/local/include
    linkflag: CK   NOLIB

The `NOLIB` flag instructs the linker to omit the usual `-l` library, but all other `linkflag` values are output as usual.

Once the set of required libraries has been determined (via the `lib`, `OMAKE` and `require`) the following link command is executed:

    linkcmd { linkflags } [ objs ] -o Module { libs }

Where:

* *linkcmd* is the linker command defined by `link`
* *linkflags* are the flags declared for for all required libraries
* *objs* are all the required object files that are not contained in other libraries
* *main* is the name of the main module, and *main.c* is its object file
* *libs* are `-l` directives for each required library

## To Do

Features to add in the future:

* *Improved error reporting*. **OMake** reports the location of errors in the source file, but description of the error should be improved, and should not include token codes. This most often occurs when there is an error in the `MODULE` or `IMPORT` statement, or if an imported module cannot be found.
* *Build libraries*. Currently **OMake** compiles directly from `.c` to build an executable, but for building libraries it should generate intermediate `.o` files, and use the `ar` archiver. There should also be a method to describe which modules to include in the library. Some options might be:
    1. All source files in a module directory. This would require the ability to enumerate files in a directory, which is not currently in the Platform implementation.
    2. A "manifest" file that specifies a list of module names
    3. A "root" module that imports all of the modules to be included
* *Incremental build*. Currently **OMake** builds all imported modules that are not part of a pre-linked library. This is usually quite fast, but a module only really needs to be recompiled if it has changed since the last compilation, or if one of its imports have changed. One way to check this would be to compare the modification times of the module source and the symbol file.
* *Filter compiler options*. Some modules may disable run-time checks for speed. Optionally we may want to suppress such flags, to build a "safer" version of the code.
* *Better help message*. The default help message shows the default state for options, but at present this only works for simple "scalar" values. It should also output some of the more complex options which involved lists and/or associations (eg. "-set").
* *Simplify handling of absolute paths*. As described, square brackets must be used to resolve absolute paths for `lib` and `source` directives. This resolution should probably be built in to these commands, so that either `{}` or `[]` forms can be used with the same effect.

## Configuration example

```
# ---- BEGIN - OMake configuration file

# Location of OfrontPlus distribution
set: OFRONT $OFRONT

# Name of target (eg. macOS, Win64, Linux_amd64, BlackBox)
# see list of targets in $OFRONT/Target
set: TARGET $OTARGET

# Location of Ofront library source files.
# Required by OMake to locate library sources, and by linker for include files
set: OFRONTLIB {OFRONT}/Mod/Lib

# Location of Ofront library object files.
# Required by linker to locate library object files (.c)
set: TARGETLIB {OFRONT}/Target/{TARGET}/Lib

# Paths to source code files. Path elements are separated by commas

source: .,test,tool,src/lib,src/aria,[OFRONTLIB]

# ---- Module extensions
# Here we list the valid module extensions, and any compiler flags
# associated with file type. Use this to define Oberon dialects
#
# arg 1: Module extension without leading "."
# arg 2: List of associated compiler flags

extension: Mod "-2"
extension: ob  "-1"
extension: ob2 "-2"
extension: ob3 "-3"
extension: ob7 "-7"
extension: CP  "-C"

# ---- Ofront+ compile command

compile: "Ofront+ -88 -s -e"

# ---- Link command
# Always include OFRONTLIB for SYSTEM.oh

link: "gcc -Os -g"

# ---- Built-in modules without source files
# OMake will not attempt to locate and parse these modules

exclude: Platform,SYSTEM

# ---- Source directories that represent libraries
# A source file located in the directory is assumed part of the library
# which causes the library to be added to the linker command
#
# arg 1: Library name
# arg 2: Directory containing library source files

lib: Ofront [OFRONTLIB]

# ---- Libraries that must always be linked (eg. for the language runtime)

require: Ofront

# ---- Linker flags required by various libraries
# This defines any additional flags (eg. -I, -L) required by the link
# command (ie. gcc) to link the program. Do not add -l flags here as 
# these are added implicitly by OMake
# 
# arg 1: Library name
# arg 2: Linker flags (eg. -I, -L) required for library

linkflag: Ofront -I{OFRONTLIB}
linkflag: Ofront -I{TARGETLIB}/Obj
linkflag: Ofront -L{TARGETLIB}

linkflag: SDL2   -I/usr/local/include/SDL2
linkflag: CK     -I$HOME/local/include
linkflag: CK     NOLIB
linkflag: cairo  -I/usr/local/include/cairo

# ---- END
```
