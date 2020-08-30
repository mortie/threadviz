# ThreadViz: Visualize your program's threading

ThreadViz consists of two parts: `libthreadviz.so`, an `LD_PRELOAD`able library
which intercepts various pthread functions, and `threadviz`, a utility wrapper
script.

## Usage

Run your program with the `threadviz` command:

	threadviz myprogram

This generates a log and writes it to `./threadviz-data/threadviz.<PID>.log`.
The `threadviz` tool can then be used to generate a visualization:

	threadviz --html ./threadviz-data/threadviz.<PID>.log

This generates a HTML file, `./threadviz-data/threadviz.<PID>.log.html`.

## Build/Install

Compile with `make`, then install with `sudo make install`.

By default, `sudo make install` will install to `/usr/local`. Set `PREFIX`
to change install location.

Uninstall with `sudo make uninstall`.

The `threadviz` command will only work after being installed, because the install
step modifies the installed `threadviz` script so that it knows about the location
of `libthreadviz.so` and `visualize.html`.
