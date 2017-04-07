/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/CylCan.h
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
#ifndef Geometry_CylCan_h
#define Geometry_CylCan_h

namespace Geometry
{

/*!
  \class CylCan
  \brief Holds a simple rectangle system
  \author S. Ansell
  \date March 2008
  \version 1.0

  Defines regular box and associated surfaces:
*/

class CylCan : public Surface
{
 private:
  
  Geometry::Vec3D OPt;                  ///< Origin
  Geometry::Vec3D unitD;                ///< Line vector (unit)
  double length;                        ///< Length 
  double radius;                        ///< radius
  
  std::vector<Geometry::Plane> Sides;   ///< Sides (only created if needed)

  void makeSides();
  
 public:

  /// Effective TYPENAME 
  static std::string classType() { return "CylCan"; }
  /// Effective typeid
  virtual std::string className() const 
    { return "CylCan"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  CylCan();
  CylCan(const CylCan&);
  CylCan* clone() const;
  CylCan& operator=(const CylCan&);
  bool operator==(const CylCan&) const;
  bool operator!=(const CylCan&) const;
  virtual ~CylCan();


  /// Access centre
  const Geometry::Vec3D& getCentre() const { return OPt; } 
  /// Access Normal
  const Geometry::Vec3D& getNormal() const { return unitD; }
  const Geometry::Plane& getPlane(const size_t) const;
  double getLength() const { return length; } ///< Access length
  double getRadius() const { return radius; } ///< Access radius


  int setSurface(const std::string&);
  int setCylCan(const Geometry::Vec3D&,const Geometry::Vec3D&,
		const double);
  int setCylCan(const Geometry::Vec3D&,const Geometry::Vec3D&,
		const double,const double);

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
