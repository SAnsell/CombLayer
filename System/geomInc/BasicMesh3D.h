/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/BasicMesh3D.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef Geometry_BasicMesh3D_h
#define Geometry_BasicMesh3D_h

namespace Geometry
{

/*!
  \class BasicMesh3D
  \version 1.0
  \date April 2021
  \author S. Ansell
  \brief A WW-Mesh [without fine bine + linear grid]
*/

class BasicMesh3D 
{
 private:

  size_t ID;                   ///< ID index [if used]
  size_t writeFlag;            ///< write out header info 

  Geometry::Vec3D RefPoint;    ///< Reference point
  Geometry::Vec3D Origin;      ///< Low corner
  Geometry::Vec3D OuterPoint;  ///< top corner 

  double xSize;                ///< size in x [full]
  double ySize;                ///< size in y [full]
  double zSize;                ///< size in z [full]
   
  size_t NX;                    ///< Total number of X BINS
  size_t NY;                    ///< Total number of Y BINS
  size_t NZ;                    ///< Total number of Z BINS
    
 public:

  
  BasicMesh3D();
  BasicMesh3D(const size_t);
  BasicMesh3D(const size_t,const Geometry::Vec3D&,
	      const Geometry::Vec3D&,const std::array<size_t,3>&);
  BasicMesh3D(const BasicMesh3D&);
  BasicMesh3D& operator=(const BasicMesh3D&);
  virtual ~BasicMesh3D() {}   ///< Destructor

  size_t getID() const { return ID; }
    
  /// total number of cells
  size_t size() const { return NX*NY*NZ; }
  size_t getXSize() const { return NX; }    ///< number of X-cells
  size_t getYSize() const { return NY; }    ///< number of Y-cells
  size_t getZSize() const { return NZ; }    ///< number of Z-cells

  const Geometry::Vec3D& getLow() const { return Origin; }
  const Geometry::Vec3D& getHigh() const { return OuterPoint; }
  Geometry::Vec3D point(const size_t,const size_t,const size_t) const;
  Geometry::Vec3D point(const long int,const long int,const long int) const;

  void setMesh(const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       const std::array<size_t,3>&); 
  void setMesh(const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       const size_t,const size_t,const size_t); 
    
  std::vector<Geometry::Vec3D> midPoints() const;
  /// Set reference point 
  void setRefPt(const Geometry::Vec3D&);  

  double getXCoordinate(const size_t) const;
  double getYCoordinate(const size_t) const;
  double getZCoordinate(const size_t) const;
  
  void write(std::ostream&) const;
  void writeVTK(std::ostream&) const;
  void writePHITS(std::ostream&) const;
  void writeWWINP(std::ostream&) const;

};

}  

#endif
