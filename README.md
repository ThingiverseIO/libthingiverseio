# libthingiverseio
ThingiverseIO shared library

## Build

### Requirements

* [Zeromq 4.x](http://zeromq.org/)
* [Golang](https://golang.org/) 1.5+ (Windows 1.7+)
* Windows: GCC Compiler, e.g. CygWIN, MingW or TDM-GCC (MSVC will NOT work)

### Linux

Compiling:

    make

Installation:

    sudo make install

Testing after install:

    make test

### Windows

Make sure that you have gcc in your PATH. First you need ZeroMQ, which is difficult too compile with GCC on Windows. 

The painless way is using CygWIN, which brings zeromq via it's package manager: just install 'libzmq-devel'.

To compile it from source, do the following:

* Get the [Ruby Devkit](http://rubyinstaller.org/downloads/) and extract it to `c:\rubydevkit`
* Get the ZeroMQ Windows Sources (this instructions are for 4.1.5) and extract them
* Open MSYS shell by executing `c:\rubydevkit\msys.bat` and navigate to the ZeroMQ source directory
* Execute `./configure`
* Open the file `Makefile` with a text editor, search for '-Werror' and remove it, save
* Execute `Make`
* a folder named `.libs` is created, from there copy `libzmq.dll.a` to the folder `c:\rubydevkit\mingw\x86_64-w64-mingw32\lib` and `libzmq.dll` to the folder `c:\rubydevkit\bin`  
* copy the content of the folder `include` to `c:\rubydevkit\mingw\x86_64-w64-mingw32\include`
* execute `c:\rubydevkit\devkitvars.bat` or add `c:\rubydevkit\bin` to your PATH manually


After ZeroMQ is installed, navigate to the libthingiverseio folder and execute

    built.bat

After this, you can located the DLL in the folder `bin`. To test, execute 

    test.bat

To use the DLL, will need in addition to `libzmq.dll` the libraries `libgcc_s_sjlj-1.dll` and `libstdc++-6.dll` from the folder `c:\rubydevkit\mingw\bin`. Either have them in your PATH or put all 4 DLLs next to application executable.
