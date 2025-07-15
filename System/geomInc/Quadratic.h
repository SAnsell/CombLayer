/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Quadratic.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef Geometry_Quadratic_h
#define Geometry_Quadratic_h

namespace Geometry 
{

/*!
  \class  Quadratic
  \brief Holds a basic quadratic surface
  \author S. Ansell
  \date April 2004
  \version 1.0

  Holds a basic surface with equation form
  \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  
*/

class Quadratic : public Surface
{
 private:
  
  double eqnValue(const Geometry::Vec3D&) const;
  void matrixForm(Geometry::M3<double>&,
		  Geometry::Vec3D&,double&) const;          

 protected:

  std::vector<double> BaseEqn;     ///< Base equation (as a 10 point vector)

 public:

  static const int Nprecision=10;        ///< Precision of the output

  Quadratic();
  explicit Quadratic(const int);
  Quadratic(const Quadratic&);
  Quadratic* clone() const override;
  Quadratic& operator=(const Quadratic&);
  bool operator==(const Quadratic&) const;
  bool operator!=(const Quadratic&) const;
  ~Quadratic() override;

  /// Effective TYPENAME 
  static std::string classType() { return "Quadratic"; }
  /// Effective typeid
  std::string className() const override 
    { return "Quadratic"; }

  /// Accept visitor for line calculation
  void acceptVisitor(Global::BaseVisit& A) const override
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  /// access baseEqn
  std::vector<double>& getBaseEqn() { return BaseEqn; }
  /// access BaseEquation vector
  const std::vector<double>& copyBaseEqn() const { return BaseEqn; } 

  int setSurface(const std::string&) override;
  int side(const Geometry::Vec3D&) const override; 

  virtual void setBaseEqn();      ///< Abstract set baseEqn 
  
  int onSurface(const Geometry::Vec3D&) const override;          ///< is point valid on surface 
  double distance(const Geometry::Vec3D&) const override;        ///< distance between point and surface (approx)
  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const override;    ///< Normal at surface

  void displace(const Geometry::Vec3D&) override;
  void rotate(const Geometry::M3<double>&) override;
  void rotate(const Geometry::Quaternion&) override;
  void mirror(const Geometry::Plane&) override;

  void normalizeGEQ(const size_t);
  
  void print() const override;

  void writeXML(const std::string&) const override;
  void writeFLUKA(std::ostream&) const override;
  void writePOVRay(std::ostream&) const override;
  void write(std::ostream&) const override;

};

}  // NAMESPACE Geometry

#endif
