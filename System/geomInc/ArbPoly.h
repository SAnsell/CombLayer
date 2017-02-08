/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/ArbPoly.h
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
#ifndef Geometry_ArbPoly_h
#define Geometry_ArbPoly_h

namespace Geometry
{

/*!
  \class ArbPoly
  \brief Holds an arbritary polyhendron
  \author S. Ansell
  \date August 2008
  \version 1.0

  Holds an arbitory polyhedron.
  Surfaces need to be defined clockwize on the
  outside.
*/

class ArbPoly : public Surface
{
 private:
  
  size_t nSurface;                    ///< Number of surfaces
  std::vector<Vec3D> CVec;                  ///< Corner Vector
  std::vector<std::vector<size_t> > CIndex;    ///< Corner Index
  std::vector<Plane> Sides;                 ///< Sides (only created if needed)

  void makeSides();
  int inLoop(const Vec3D&,const size_t) const;

 public:

  ArbPoly();
  ArbPoly(const ArbPoly&);
  ArbPoly* clone() const;
  ArbPoly& operator=(const ArbPoly&);
  bool operator==(const ArbPoly&) const;
  bool operator!=(const ArbPoly&) const;
  virtual ~ArbPoly();

  /// Effective TYPENAME 
  static std::string classType() { return "ArbPoly"; }
  /// Effective typeid
  virtual std::string className() const { return "ArbPoly"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  size_t getNSurf() const { return nSurface; }   ///< Number of surface
  const Geometry::Vec3D& getCVec(const size_t) const;
  const std::vector<size_t>& getCIndex(const size_t) const;
  const Geometry::Plane& getPlane(const size_t) const;
  
  int setSurface(const std::string&);
  
  virtual int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;

  // stuff for finding intersections etc.
  double distance(const Geometry::Vec3D&) const;     
  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const;
  
  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);
  void mirror(const Geometry::Plane&);

  void print() const;

  virtual void writeFLUKA(std::ostream&) const;
  virtual void writePOVRay(std::ostream&) const;        
  void write(std::ostream&) const;        
};

}  // NAMESPACE Geometry

#endif
