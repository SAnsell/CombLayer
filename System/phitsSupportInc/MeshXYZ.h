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

namespace Geometry
{
  class Transform;
}

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

  std::array<size_t,3> nBins;   ///< Number of bins
  std::array<int,3> logSpace;   ///< log bins

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
  size_t size() const { return nBins[0]*nBins[1]*nBins[2]; }

  size_t getXSize() const { return nBins[0]; }    ///< number of X-cells
  size_t getYSize() const { return nBins[1]; }    ///< number of Y-cells
  size_t getZSize() const { return nBins[2]; }    ///< number of Z-cells

  size_t getZeroIndex() const;
  
  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;  

  void setLog(const size_t index) { logSpace[index % 3]=1; }
  void setLinear(const size_t index) { logSpace[index % 3]=0; }
  void setSize(const size_t,const size_t,const size_t);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  
  void write2D(std::ostream&) const;
  void write(std::ostream&) const;

};

}  

#endif
