This file describes how to run CMake and compile my prototype. If you're on
lxplus, first run

    $ SetupProject DaVinci

This needs to be done once and is necessary for CMake to find the ROOT and
RooFit libraries. Now go into the decayObserver/CMakeHome/build/ directory and
run

    $ cmake -DCMAKE_BUILD_TYPE=Release ..

To compile, just run

    $ make kevinproto

Now you can run my prototype by executing 

    $ ./bin/kevinproto

When it is done, there should be a plot inside the
decayObserver/CMakeHome/plots/ directory.
