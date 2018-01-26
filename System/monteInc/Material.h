/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Material.h
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
#ifndef MonteCarlo_Material_h
#define MonteCarlo_Material_h

namespace MonteCarlo
{

/*!
  \class Material
  \version 1.0
  \date September 2005
  \author S. Ansell
  \brief Holds individual Material components

  Holds materials along with Zaid and s(q,w)
  componenents
*/

class Material 
{ 
 private:
  
  int Mnum;                        ///< Material Number (Necessary)  
  std::string Name;                ///< Material Name (un-necessary)
  std::vector<Zaid> zaidVec;       ///< vector of zaids
  std::map<std::string,MXcards> mxCards;     ///< particle:MX card
  
  std::vector<std::string> Libs;    ///< Library extra
  std::vector<std::string> SQW;     ///< s(q,w) 

  double atomDensity;               ///< Calculated atom density 
  int getExtraType(std::string&,std::string&);
  void calcAtomicDensity();

  size_t getZaidIndex(const size_t,const size_t,const char) const;
  static void writeZaid(std::ostream&,const double,const size_t);

 public:
  
  Material();
  Material(const Material&);
  Material& operator=(const Material&);
  /// Clone function
  Material* clone() const { return new Material(*this); }
  virtual ~Material();

  Material& operator*=(const double);
  Material& operator+=(const Material&);

  /// Effective TYPENAME 
  static std::string classType() { return "Material"; }
  /// Effective typeid
  virtual std::string className() const { return "Material"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const 
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  static int lineType(std::string&);

  /// Set the material name
  void setName(const std::string& A) { Name=A; }  
  /// Get the material name
  const std::string& getName() const { return Name; } 

  /// set the Material number
  void setNumber(const int nA) { Mnum=nA; } 
  /// Assesor function to Number
  int getNumber() const { return Mnum; }

  int setMaterial(const std::vector<std::string>&);
  int setMaterial(const int,const std::string&,
		  const std::string&,const std::string&,
		  const std::string&);
  int setMaterial(const int,const std::string&,const std::string&,
		  const std::string&);

  void setMXitem(const size_t,const size_t,const char,const std::string&,
		 const std::string&);
  /// Get atomic density
  double getAtomDensity() const { return atomDensity; }
  double getMacroDensity() const;
  double getMeanA() const;
  void setENDF7();
  void setDensity(const double);
  bool hasZaid(const size_t,const size_t,const char) const;
  std::vector<Zaid> getZaidVec() const { return zaidVec; }

  /// remove mt cards
  void removeSQW() { SQW.clear(); }
  void removeMX(const std::string&);
  void removeLib(const std::string&);
    
  void changeLibrary(const size_t,const char);

  void listComponent() const;
  void print() const;
  void write(std::ostream&) const;               
  void writeCinder(std::ostream&) const;
  void writeFLUKA(std::ostream&) const;
  void writePHITS(std::ostream&) const;
  void writePOVRay(std::ostream&) const;               
  
};

std::ostream&
operator<<(std::ostream&,const Material&);

}

#endif
