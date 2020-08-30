# ThreadViz: Visualize your program's threading

ThreadViz consists of two parts: `libthreadviz.so`, an `LD_PRELOAD`able library
which intercepts various pthread functions, and `threadviz`, a utility wrapper
script.

![](https://raw.githubusercontent.com/mortie/threadviz/master/threadviz.jpg)

_A visualization of a program with one main thread and 4 worker threads._

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

## Future Work

The `libthreadviz.so` part is probably basically done; it just intercepts
pthreads calls and logs to a file. However, there's still lots of work to be
done on the visualization side of things.

* ThreadViz currently gives a decent idea of _when_ threads are blocked, but
  not _why_. It would be interesting to correlate one thread's `pthread_mutex_unlock`
  with another thread's `pthread_mutex_lock` etc.
* The plot should be interactive, making it easier to explore a part of it in detail.
* The plot needs to provide more information; how long is the thread blocked,
  which mutex is being waited on, etc.
* The plot needs to show the name of the threads (requires intercepting
  `pthread_setname_np`).
* If you have an idea for a feature, or a use-case which is currently unsolved,
  please create an issue and we can discuss a solution.
