/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/NullSurface.h
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
#ifndef Geometry_NullSurf_h
#define Geometry_NullSurf_h

namespace Geometry
{

/*!
  \class NullSurface
  \brief Holds a null surface
  \author S. Ansell
  \date January 2009
  \version 1.0


*/

class NullSurface : public Surface
{
 private:
    
 public:

  NullSurface();
  NullSurface(const int,const int);
  NullSurface(const NullSurface&);
  NullSurface* clone() const;
  NullSurface& operator=(const NullSurface&);
  int operator==(const NullSurface&) const;
  ~NullSurface();

  /// Effective TYPENAME 
  static std::string classType() { return "NullSurface"; }
  /// Public identifier
  virtual std::string className() const { return "NullSurface"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  /// All surfraces are not-null except nullsurf
  virtual int isNull() { return 1; } 
  int setSurface(const std::string&);
  int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;
  double distance(const Geometry::Vec3D&) const;

  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);
  /// Null op mirror
  void mirror(const Geometry::Plane&) { }
  Vec3D surfaceNormal(const Geometry::Vec3D&) const;
  
  void print() const;

  virtual void writePOVRay(std::ostream&) const;
  virtual void writeFLUKA(std::ostream&) const;
  virtual void write(std::ostream&) const;
};

}  // NAMESPACE Geometry

#endif
