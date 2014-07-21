This file describes how to run CMake and compile secateur. If you're on
lxplus, first run

    $ SetupProject DaVinci

This needs to be done once and is necessary for CMake to find the ROOT and
RooFit libraries. Now go into the decayObserver/CMakeHome/build/ directory and
run

    $ cmake -DCMAKE_BUILD_TYPE=Release ..

To compile, just run

    $ make secateur

Now you can run secateur by executing 

    $ ./bin/secateur

This will print a usage text that explains the command line arguments. If you
want to generate a configuration file to prune /path/yourfile.root to
/path/yourfile_pruned.root, run

    $ ./bin/secateur -c -i /path/yourfile.root -o /path/yourfile_pruned.root /path/configfile.conf

This will show a listbox in which you can select which trees to keep. Then it
will show a listbox in which you can select which branches to copy to the new
root file. You can modify the configuration file manually to add cuts. How to do
this can be seen in secateur_example.conf. If you only want to remove or add
branches, you can modify the configuration by running

    $ ./bin/secateur -m /path/configfile.conf

This will show a listbox again in which you can search and select branches. To
actually prune your ROOT file, run

    $ ./bin/secateur /path/configfile.conf