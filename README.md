# CombLayer

**CombLayer** is a high-performance C++ framework for building and exporting parametric [CSG](https://en.wikipedia.org/wiki/Constructive_solid_geometry) geometry to Monte Carlo codes.  
Designed for flexibility, reuse, and speed—ideal for complex scientific facilities and simulations.

Designed for speed, flexibility, and scalability, CombLayer enables users to rapidly generate sophisticated geometric models optimized for leading Monte Carlo codes such as [MCNP](https://mcnp.lanl.gov), [FLUKA](https://fluka.eu), and [PHITS](https://phits.jaea.go.jp).

At its core, CombLayer uses modular, plug-and-play components that can be combined, reused, and reoriented with ease. Each component is defined in its own local coordinate system, allowing you to assemble large, intricate models with minimal effort while maintaining full parametric control.

CombLayer goes beyond geometry construction. It provides integrated support for **materials, tallies (estimators), and source definitions**, helping streamline the entire simulation setup process. Built-in validation and variance reduction tools ensure both accuracy and performance.

The framework has been successfully used to model real-world, large-scale facilities, including:

- [ISIS](https://www.isis.stfc.ac.uk) and [ESS](https://ess.eu) neutron sources  
- [MAX IV](https://maxiv.lu.se) synchrotron light source  
- [Delft](https://www.tudelft.nl/en/faculty-of-applied-sciences/business/facilities/tu-delft-reactor-institute) nuclear reactor  

While the workflow is conceptually similar to [GEANT4](https://cern.ch/geant4) — where reusable component classes define geometry — CombLayer is specifically optimized for **CSG-based simulation engines**, delivering efficient and production-ready output.

👉 See [the documentation](https://github.com/SAnsell/CombLayer/blob/master/Documents/Guide) for a deeper introduction to the design philosophy and usage.

---

## 🚀 Features

**CombLayer helps you go from idea to simulation faster:**

- **Multi-code export**  
  Seamlessly generate geometry for:
  - MCNP  
  - FLUKA  
  - PHITS  
  - POV-Ray (visualization)  
  - VTK (analysis & visualization)

- **Full simulation support**  
  - Automatic export of materials and tallies  
  - Source term generation (when applicable)

- **High-performance geometry handling**  
  - Efficient object–object intersections  
  - Optimized for fast Monte Carlo execution  

- **Advanced variance reduction tools**  
  Compatible with MCNP, FLUKA, and PHITS:
  - Cell-based methods  
  - Mesh-based methods  
  - Angular biasing (MCNP & PHITS)

- **Fully parametric design system**  
  Build flexible, reusable, and configurable geometries with ease.

---

## 📦 Source Code

https://github.com/SAnsell/CombLayer

---

## ⚙️ Installation

### Requirements

- CMake  
- C++ compiler (e.g., Clang or GCC)  
- Boost  
- GSL (GNU Scientific Library)

### Build Instructions

Build directly in the source directory:

```bash
cmake ./
make
```

Or use a separate build directory:
```
cmake -B /path/to/buildDirectory -S /path/to/srcDirectory
make
```
You can speed up compilation with options like ```-j8```, or enable verbose output with ```VERBOSE=1```.

