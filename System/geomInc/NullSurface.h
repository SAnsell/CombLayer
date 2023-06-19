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
  NullSurface* clone() const override;
  NullSurface& operator=(const NullSurface&);
  int operator==(const NullSurface&) const;
  ~NullSurface() override;

  /// Effective TYPENAME 
  static std::string classType() { return "NullSurface"; }
  /// Public identifier
  std::string className() const override { return "NullSurface"; }
  /// Visitor acceptance
  void acceptVisitor(Global::BaseVisit& A) const override
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  /// All surfraces are not-null except nullsurf
  int isNull() override { return 1; } 
  int setSurface(const std::string&) override;
  int side(const Geometry::Vec3D&) const override;
  int onSurface(const Geometry::Vec3D&) const override;
  double distance(const Geometry::Vec3D&) const override;

  void rotate(const Geometry::Matrix<double>&) override;
  void displace(const Geometry::Vec3D&) override;
  /// Null op mirror
  void mirror(const Geometry::Plane&) override { }
  Vec3D surfaceNormal(const Geometry::Vec3D&) const override;
  
  void print() const override;

  void writePOVRay(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;
  void write(std::ostream&) const override;
};

}  // NAMESPACE Geometry

#endif
