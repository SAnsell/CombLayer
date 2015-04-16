/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/Algebra.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef Algebra_h
#define Algebra_h


namespace MonteCarlo
{

/*!
  \class  Algebra
  \brief Computes Boolean algebra for simplification
  \author S. Ansell
  \date Aug 2005
  \version 1.0

  The is an facade class, it effectively allows a better
  interface into Acomp. This takes surface indexes into 
  the alphanumberic naming used in Acomp. 
*/ 
class Algebra
{
 private:

  std::map<int,std::string> SurfMap;    ///< Surface Map 
  Acomp F;                              ///< Factor
  Acomp FplusImpl;                      ///< Factor with implicates
  
 public:

  Algebra();
  Algebra(const Algebra&);
  Algebra& operator=(const Algebra&);
  ~Algebra();

  /// Accessor
  const Acomp& getComp() const { return F; }  
  
  int operator==(const Algebra&) const;
  int operator!=(const Algebra&) const;
  Algebra& operator+=(const Algebra&);
  Algebra& operator-=(const Algebra&);
  Algebra& operator*=(const Algebra&);
  Algebra operator+(const Algebra&) const;
  Algebra operator-(const Algebra&) const;
  Algebra operator*(const Algebra&) const;
  int logicalEqual(const Algebra&) const;

  void Complement();
  void makeDNF() { F.makeDNFobject(); }  ///< assessor to makeDNFobj
  void makeCNF() { F.makeCNFobject(); }  ///< assessor to makeCNFobj
  std::pair<Algebra,Algebra> algDiv(const Algebra&) const;
  int setFunctionObjStr(const std::string&);
  int setFunction(const std::string&);
  int setFunction(const Acomp&);

  void addImplicate(const int,const int);

  std::string display() const;
  std::ostream& write(std::ostream&) const;
  std::string writeMCNPX() const;

  // Debug Functions::
  int countLiterals() const;

};

std::ostream&
operator<<(std::ostream&,const Algebra&);

}  // NAMESPACE MonteCarlo

#endif 
