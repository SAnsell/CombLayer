/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupportInc/MeshXYZ.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef phitsSystem_MeshXYZ_h
#define phitsSystem_MeshXYZ_h

namespace phitsSystem
{

/*!
  \class MeshXYZ
  \version 1.0
  \date Septebmer 2018
  \author S. Ansell
  \brief Phits mesh for tally/source
*/

class MeshXYZ 
{
 private:

  bool logSpace;                ///< log bins
  size_t NX;                    ///< Total number of X BINS
  size_t NY;                    ///< Total number of Y BINS
  size_t NZ;                    ///< Total number of Z BINS

  Geometry::Vec3D minPoint;    ///< lower point
  Geometry::Vec3D maxPoint;    ///< top point


  Geometry::Transform* TransPtr;
  
  static double getCoordinate(const std::vector<double>&,
			      const std::vector<size_t>&,
			      const size_t);
  
 public:

  MeshXYZ();
  MeshXYZ(const MeshXYZ&);
  MeshXYZ& operator=(const MeshXYZ&);
  virtual ~MeshXYZ() {}   ///< Destructor

  /// total number of cells
  size_t size() const { return NX*NY*NZ; }

  size_t getXSize() const { return NX; }    ///< number of X-cells
  size_t getYSize() const { return NY; }    ///< number of Y-cells
  size_t getZSize() const { return NZ; }    ///< number of Z-cells
  
  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;  
  
  void write(std::ostream&) const;

};

}  

#endif
