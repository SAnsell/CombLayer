/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Material.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
  class particle;

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
 protected:
 
  int matID;                        ///< Material Number (Necessary)  
  std::string Name;                ///< Material Name (un-necessary)
  double atomDensity;               ///< Calculated atom density
  
 private:
  
  std::vector<Zaid> zaidVec;       ///< vector of zaids
  std::map<std::string,MXcards> mxCards;     ///< particle:MX card
  
  std::vector<std::string> Libs;    ///< Library extra
  std::vector<std::string> SQW;     ///< s(q,w) 


  int getExtraType(std::string&,std::string&);
  void calcAtomicDensity();

  size_t getZaidIndex(const size_t,const size_t,const char) const;
  static void writeZaid(std::ostream&,const double,const size_t);

 public:
  
  Material();
  Material(const int,const std::string,const double D);
  Material(const Material&);
  Material& operator=(const Material&);
  /// Clone function
  Material* clone() const { return new Material(*this); }
  virtual ~Material();

  Material& operator*=(const double);
  Material& operator+=(const Material&);

  static int lineType(std::string&);

  /// Special to decide if void
  bool isVoid() const { return (matID==0); }
  

  /// Set the material name
  void setName(const std::string& A) { Name=A; }  
  /// Get the material name
  const std::string& getName() const { return Name; } 

  /// Assesor function to Number
  int getID() const { return matID; }
  
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

  // stuff for tracking [default void]
  virtual double calcAtten(MonteCarlo::particle&,const double) const
  { return 1.0; } 
  
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
