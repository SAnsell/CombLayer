/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Surface.h
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
#ifndef Geometry_Surface_h
#define Geometry_Surface_h

namespace Geometry
{
  class Vec3D;
  class Plane;
  class Quaternion;
  class Transform;

/*!
  \class  Surface
  \brief Fundermental Surface object
  \author S. Ansell
  \date March 2008
  \version 1.0

  The top level geometry surface
*/

class Surface 
{
 private:
  
  int Name;        ///< Surface number (MCNP identifier)
  int TransN;      ///< Transform number (-ve means applied)


 protected:
  
  void processSetHead(std::string&);

 public:

  Surface();
  Surface(const int,const int);
  Surface(const Surface&);
  virtual Surface* clone() const =0;   ///< Abstract clone function
  Surface& operator=(const Surface&);
  virtual ~Surface();

  /// Effective TYPENAME 
  static std::string classType() { return "Surface"; }
  /// Effective typeid
  virtual std::string className() const 
    { return "Surface"; }
  /// Accept visitor for output
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  void setName(const int N) { Name=N; }            ///< Set Name
  int getName() const { return Name; }             ///< Get Name
  void setTrans(const int N) { TransN=N; }         ///< Set Transform number
  int getTrans() const { return TransN; }          ///< Get Transform number

  // Processes Name/TransNumber
  std::string stripID(const std::string&);
  /// All surfraces are not-null except nullsurf
  virtual int isNull() { return 0; } 
  int applyTransform(const std::map<int,Transform>&);
  int sideDirection(const Geometry::Vec3D&,
		    const Geometry::Vec3D&) const;

  /// \cond ABSTRACT
  virtual int setSurface(const std::string&) =0; 
  virtual int side(const Geometry::Vec3D&) const =0; 
  virtual int onSurface(const Geometry::Vec3D&) const =0;
  virtual double distance(const Geometry::Vec3D&) const =0; 
  virtual Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const =0;
  
  virtual void displace(const Geometry::Vec3D&)  =0;
  virtual void rotate(const Geometry::Matrix<double>&) =0;
  virtual void mirror(const Geometry::Plane&) =0; 

  virtual void writeFLUKA(std::ostream&) const =0;
  virtual void writePOVRay(std::ostream&) const =0;
  virtual void write(std::ostream&) const =0;
  /// \endcond ABSTRACT

  virtual void rotate(const Geometry::Quaternion&);

  void writeHeader(std::ostream&) const;

  virtual void print() const; 
  virtual void writeXML(const std::string&) const;

};


std::ostream&
operator<<(std::ostream&,const Surface&);

}     // NAMESPACE Geometry

#endif
