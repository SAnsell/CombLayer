/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/Mesh3D.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef Geometry_Mesh3D_h
#define Geometry_Mesh3D_h

namespace Geometry
{

/*!
  \class Mesh3D
  \version 1.0
  \date October 2015
  \author S. Ansell
  \brief A WW-Mesh 
*/

class Mesh3D 
{
 private:

  /// Types of weight mess
  enum GeomENUM { XYZ=1,Cyl=2,Sph=3 };
  
  GeomENUM type;               ///< Type of mesh

  size_t writeFlag;            ///< write out header info 
  Geometry::Vec3D RefPoint;    ///< Reference point
  Geometry::Vec3D Origin;      ///< Origin
  Geometry::Vec3D Axis;        ///< Axis
  Geometry::Vec3D Vec;         ///< Orthog Axis

  std::vector<double> X;     ///< X/R coordinates [+origin]
  std::vector<double> Y;     ///< Y/Z/phi coordinates [+origin]
  std::vector<double> Z;     ///< Z/theta coordinates [+origin]

  std::vector<size_t> XFine;    ///< Number of fine X bins 
  std::vector<size_t> YFine;    ///< Number of fine Y bins 
  std::vector<size_t> ZFine;    ///< Number of fine z bins 

  size_t NX;                    ///< Total number of X BINS
  size_t NY;                    ///< Total number of Y BINS
  size_t NZ;                    ///< Total number of Z BINS
  
  std::string getType() const;
  
  static double getCoordinate(const std::vector<double>&,
			      const std::vector<size_t>&,
			      const size_t);
  
 public:

  Mesh3D();
  Mesh3D(const Mesh3D&);
  Mesh3D& operator=(const Mesh3D&);
  virtual ~Mesh3D() {}   ///< Destructor

  /// total number of cells
  size_t size() const { return NX*NY*NZ; }
  size_t getXSize() const { return NX; }    ///< number of X-cells
  size_t getYSize() const { return NY; }    ///< number of Y-cells
  size_t getZSize() const { return NZ; }    ///< number of Z-cells
  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;
  double getXCoordinate(const size_t) const;
  double getYCoordinate(const size_t) const;
  double getZCoordinate(const size_t) const;

  
  void setMeshType(const GeomENUM&);
  void setMesh(const std::vector<double>&,const std::vector<size_t>&,
	       const std::vector<double>&,const std::vector<size_t>&,
	       const std::vector<double>&,const std::vector<size_t>&);
  std::vector<Geometry::Vec3D> midPoints() const;
  /// Set reference point 
  void setRefPt(const Geometry::Vec3D&);  
  
  void write(std::ostream&) const;
  void writeWWINP(std::ostream&) const;

};

}  

#endif
