CombLayer
=========

MCNP(X) project builder using C++ 

The object of this program is to produce highly parametric space-filling geometry models described by quadratic surfaces. 
The models are primarily for used to build MCNPX models for nuclear simulation but it can be quickly addapted for any 
other such geometry-type problem.  

The program has its own internal tracking system, which allows simple validation, variance reduction to be done, and 
property checking. 

The current build has models that cover the ISIS neutron facility, the ESS, and others. 

Contributions, comments and bug issues welcome. Please see the documents section for an introduction to the program philosophy and useage.

Some basic tutorials and a bit of a gettting started guide is available at http://plone.esss.lu.se/

Requirements
------------

To compile, you will need CMake, a C++ compiler (clang/gcc for example), boost, and GSL (Gnu Scientific Library).

It can be built either in the top directory or in a separate build directory.
If built in the source directory, the command is

```
cmake ./
make
```

or for a separate build directory

```
cmake -B/path/to/buildDirectory -S/path/to/srcDirectory
make
```

obviously make can be decorated with normal stuff, like -j8 to speed things up, or VERBOSE=1 to see what is going on.


After than build a model e.g.
```
./maxiv --defaultConfig Single COSAXS AA
```
will build the CoSAXS beamline model for Max IV.