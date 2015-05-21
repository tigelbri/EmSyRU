EmSyRU
======

Embedded System Remote Updater

Extreme lightweight tool for updating applications on embedded sytems.
It uses two enviroments for automatic and secure updating.
Also it delivers an option for a sanity check.



REQUIREMENTS
============

- GCC 4.8
- CMake 2.8
- CurlPP 0.2




INSTALL
===========

mkdir build && cd build
cmake ../
make



Example use
===========

./EmSyRU "user@ftp://path/to/jobfile" "password"
./EmSyRU "user@ftp://path/to/jobfile" "password" ["/path/for/creating/workbench"]

If no path for creating the workbench is given the workbench will be created next to executable
