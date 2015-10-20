/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WeightMesh.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef WeightSystem_WeightMesh_h
#define WeightSystem_WeightMesh_h


namespace WeightSystem
{

/*!
  \class WeightMesh
  \version 1.0
  \date April 2010
  \author S. Ansell
  \brief A WW-Mesh for neutron importance

*/

class WeightMesh 
{
 private:

  int tallyN;                  ///< Tally number required
  
  /// Types of weight mess
  enum GeomENUM { XYZ=1,Cyl=2,Sph=3 };
  
  GeomENUM type;               ///< Type of mesh
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

  size_t NX;                    ///< Total number of X
  size_t NY;                    ///< Total number of Y
  size_t NZ;                    ///< Total number of Z
  
  std::string getType() const;
  
  static double getCoordinate(const std::vector<double>&,
			      const std::vector<size_t>&,
			      const size_t);


  static void writeLine(std::ostream&,const double,size_t&);
  static void writeLine(std::ostream&,const int,size_t&);

  
 public:

  
  WeightMesh();
  WeightMesh(const WeightMesh&);
  WeightMesh& operator=(const WeightMesh&);
  virtual ~WeightMesh() {}   ///< Destructor

  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;

  void setMeshType(const GeomENUM&);
  void setMesh(const std::vector<double>&,const std::vector<double>&,
	       const std::vector<double>&,const std::vector<size_t>&,
	       const std::vector<size_t>&,const std::vector<size_t>&);
	       
  /// Set reference point
  void setRefPt(const Geometry::Vec3D& Pt) { RefPoint=Pt; } 

  
  void zeroCell(const int) { }      ///< Non-important return
  void write(std::ostream&) const;
  void writeWWINP() const;

};

}  

#endif
