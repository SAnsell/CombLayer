/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/General.h
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
#ifndef Geometry_General_h
#define Geometry_General_h

namespace Geometry
{

/*!
  \class General
  \brief Holds a general quadratic surface
  \author S. Ansell
  \date April 2004
  \version 1.0

  Holds a general surface with equation form
  \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  which has been defined as a gq surface in MCNPX.
  It is a realisation of the Surface object.
*/

class General : public Quadratic
{
 private:
  
 public:
    
  General();
  explicit General(const int);
  General(const General&);
  General* clone() const override;
  General& operator=(const General&);
  ~General() override;
  
  /// Effective TYPENAME 
  static std::string classType() { return "General"; }
  /// Effective typeid
  std::string className() const override { return "General"; }
  /// fast index accessor
  SurfKey classIndex() const override { return SurfKey::General; }
  /// Visitor acceptance
  void acceptVisitor(Global::BaseVisit& A) const override
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  int setSurface(const std::string&) override;

  void setBaseEqn() override;

};

std::ostream&
operator<<(std::ostream&,const General&);

}  // NAMESPACE Geometry

#endif
