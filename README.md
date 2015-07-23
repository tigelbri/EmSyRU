EmSyRU
======

Embedded System Remote Updater 

Extreme lightweight package manager for updating applications on embedded sytems.
It uses two enviroments for automatic and secure updating.
It automatically starts a recover version in case of updating failure 
Also it delivers an option for a sanity check. 

Applications are stored as Packages (see Packages.conf for example)
Applications can be updated by Job files (see examples/ for example Job files)

Job files are tars which has to deliver a Job.conf 
file with defined Tasks in it (see Template.conf)
Following Tasks are supported (Add, Update and Remove)
New or update files has to be in a folder with their package name.
One Job.conf can content multiply Tasks.

The EmSyRU downloads the given Job File and automaticlly checks the Packages
versions and update them. 
A Report of the updating process is been uploaded back again.

Copyright (c) 2015, Tristan Igelbrink

This Application uses the CurlPP Packages (Copyright (c) <2002-2009> <Jean-Philippe Barrette-LaPierre>)

REQUIREMENTS
============

- GCC 4.8
- CMake 2.8
- CurlDev (debian libcurl4-openssl-dev)
- CurlPP 0.2 (debian libcurlpp-dev)




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
