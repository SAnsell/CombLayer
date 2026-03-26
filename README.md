CombLayer
------------

CombLayer - A Constructive Solid Geometry project builder using C++

The purpose of CombLayer is to easily and rapidly build highly parametric complex
geometric models which are fast to run in Monte Carlo codes utilising
CGS-based geometry description used in e.g. FLUKA, MCNP and PHITS.

The program uses modifiable plug-and-play component modules to produce
a highly parametric space-filling geometry models described by
quadratic surfaces. The modules are described in a local coordinate
system and then can be inserted anywhere, and with any orientation.

The code exports models for the MCNP, FLUKA, PHITS nuclear simulation
programs, as well as for the POV-Ray raytracer and VTK file
format. The internal tracking system allows geometry validation and
variance reduction. Full featured examples cover the ISIS and ESS
neutron facilities, MAX IV synchrotron light source, Delft nuclear
reactor and others.

Geometry simulation methodology is similar to GEANT4 in that component
types are defined in a class and used in multiple locations but the
output is optimised for a CSG system.

Please see the documents section for an introduction to the program philosophy and usage.

## Features
List of features advertises the code to the unfamiliar users. This list is very incomplete:

* Export geometry to
  - MCNP
  - FLUKA
  - PHITS
  - POV-Ray
  - VTK

* In addition, for the Monte Carlo codes, both materials and tallies (estimators) are exported.
* When possible, the source terms are also exported.
* Complex object-object intersection which maintains simulation speed
* Variance reduction system, which can be used with MCNP, FLUKA and PHITS
 - Cell based
 - Mesh based
 - ext (angular bias on a cell/cell level, MCNP and PHITS only)
* Variable system to build fully parameterised geometries

## Source code
https://github.com/SAnsell/CombLayer

## Documentation

Selected features are documented in this guide:

https://github.com/SAnsell/CombLayer/blob/master/Documents/Guide


## Installation Requirements

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
will build the CoSAXS beamline model for MAX IV.
