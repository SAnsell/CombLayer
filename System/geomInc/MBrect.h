/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/MBrect.h
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
#ifndef Geometry_MBrect_h
#define Geometry_MBrect_h

namespace Geometry
{

/*!
  \class MBrect
  \brief Holds a simple rectangle system
  \author S. Ansell
  \date March 2008
  \version 1.0

  Defines regular box and associated surfaces:
*/

class MBrect : public Surface
{
 private:
  
  Geometry::Vec3D Corner;               ///< Corner 
  Geometry::Vec3D LVec[3];              ///< LVec
  std::vector<Geometry::Plane> Sides;   ///< Sides (only created if needed)

  int boxType() const;
  void makeSides();
  
 public:

  MBrect();
  MBrect(const MBrect&);
  MBrect* clone() const;
  MBrect& operator=(const MBrect&);
  bool operator==(const MBrect&) const;
  bool operator!=(const MBrect&) const;
  virtual ~MBrect();

  /// Effective TYPENAME 
  static std::string classType() { return "MBrect"; }
  /// Effective typename 
  virtual std::string className() const { return "MBrect"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  const Geometry::Vec3D& getCorner() const { return Corner; } ///< Access corner
  const Geometry::Vec3D& getLVec(const size_t) const;
  const Geometry::Plane& getPlane(const size_t) const;

  int setSurface(const std::string&);
  
  void setCorner(const Geometry::Vec3D&);
  int setMBrect(const Geometry::Vec3D&,const Geometry::Vec3D&);
  int setMBrect(const Geometry::Vec3D&,const Geometry::Vec3D&,
		const Geometry::Vec3D&,const Geometry::Vec3D&);
  virtual int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;

  Triple<Geometry::Vec3D> calcMainPlane() const;
  
  // stuff for finding intersections etc.
  double distance(const Geometry::Vec3D&) const;     
  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const;
  
  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);
  void mirror(const Geometry::Plane&);

  void print() const;

  virtual void writeFLUKA(std::ostream&) const;
  virtual void writePOVRay(std::ostream&) const;
  virtual void write(std::ostream&) const;       


};

}  // NAMESPACE Geometry

#endif
