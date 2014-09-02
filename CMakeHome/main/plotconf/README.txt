// Documentation of plotconf (plotting with a given configname)

On lxplus first run 

    $ SetupProject DaVinci v34r0

(newer versions are not tested) to find the ROOT libraries. Now go to the decayObserver/CMakeHome/build directory
and run

    $ cmake -DCMAKE_BUILD_TYPE=Release ..

Compilation is done with

    $ make plotconf

Now you can run the program with

    $ ./bin/plotconf <configfile>



For informations of which informations should be provided in the config file go to EXAMPLE_CONFIG.conf and see the made comments in there.
The desired plots are created as .png, .pdf and .root files.
