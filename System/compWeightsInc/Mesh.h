/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   compWeightsInc/Mesh.h
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
#ifndef compSystem_Mesh_h
#define compSystem_Mesh_h


namespace compSystem
{

/*!
  \class Mesh
  \version 1.0
  \date April 2010
  \author S. Ansell
  \brief A WW-Mesh for neutron importance

*/

class Mesh 
{
 private:

  /// Types of weight mess
  enum GeomENUM { XYZ=1,Cyl=2,Sph=3 };
  
  GeomENUM type;               ///< Type of mesh
  Geometry::Vec3D RefPoint;    ///< Reference point
  Geometry::Vec3D Origin;      ///< Origin
  Geometry::Vec3D Axis;        ///< Axis
  Geometry::Vec3D Vec;         ///< Orthog Axis

  // NOTE : in cylindric Y/Z are rotations e.g 0->0.5 and 0->1
  std::vector<double> X;     ///< X/R coordinates
  std::vector<double> Y;     ///< Y/Z/phi coordinates
  std::vector<double> Z;     ///< Z/theta coordinates

  /// Mesh values  [Note many for different energy arrangements ???]
  boost::multi_array<double,3> MData;

  std::string getType() const;
  
 public:

  Mesh();
  Mesh(const Mesh&);
  Mesh& operator=(const Mesh&);
  virtual ~Mesh() {}   ///< Destructor

  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;

  void setMeshType(const GeomENUM&);
  /// Set reference point
  void setRefPt(const Geometry::Vec3D& Pt) { RefPoint=Pt; } 
  void setCylinder(const Geometry::Vec3D&,const Geometry::Vec3D&,
		   const Geometry::Vec3D&,const double,
		   const size_t,const size_t,const size_t);

  void zeroCell(const int) { }      ///< Non-important return
  void write(std::ostream&) const;
};

}  

#endif
