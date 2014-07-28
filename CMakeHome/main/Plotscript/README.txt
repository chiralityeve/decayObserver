// Documentation of Plotscript

On lxplus first run 

    $ SetupProject DaVinci v34r0

(newer versions are not tested) to find the ROOT libraries. Now go to the decayObserver/CMakeHome/build directory
and run

    $ cmake -DCMAKE_BUILD_TYPE=Release ..

Compilation is done with

    $ make Plotscript

Now you can run the program with

    $ ./bin/Plotscript <parameter>

Currently for parameters there is daniel, vitali and kevin allowed which refer to the functions daniel_current, vitali_current and kevin_current
and run the settings and plots in this functions.

For informations of which informations should be provided in this functions go to plot_example.cpp and see the made comments in there.
The desired plots are created as .png, .pdf and .root files.
