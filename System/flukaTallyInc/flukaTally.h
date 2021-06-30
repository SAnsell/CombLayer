/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTallyInc/flukaTally.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef flukaSystem_flukaTally_h
#define flukaSystem_flukaTally_h

namespace flukaSystem
{

  class auxScore;
  
/*!
  \class flukaTally
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Holds a fluka tally object as a base class
  
*/

class flukaTally
{
 private:

  static std::string idForm(const std::string&,const int);  

 protected:


  
  std::string keyName;              ///< tally name
  int ID;                           ///< ID number
  int outputUnit;                   ///< Fortran output number
  std::string comments;             ///< comment line
  std::string auxParticle;          ///< Auxillary particles
  std::string doseType;             ///< Auxillary dose type if used

  std::string userName;             ///< Selected for fluscw treatment

 public:
  
  flukaTally(const std::string&,const int,const int);
  flukaTally(const flukaTally&);
  flukaTally& operator=(const flukaTally&);
  virtual flukaTally* clone() const; 
  virtual ~flukaTally();

  /// return fluka name
  virtual std::string getType() const { return "VOID"; }

  void setKeyName(const std::string&);
  /// Simple assignment to userName
  void setUserName(const std::string& A)
    { userName=A;}

  void setAscii();
  void setBinary();
  void setComment(const std::string&);
  void setAuxParticles(const std::string&);
  virtual void setDoseType(const std::string&,const std::string&);
  virtual void setEnergy(const bool,const double,const double,const size_t);
  virtual void setAngle(const bool,const double,const double,const size_t);

  const std::string& getKeyName() const;
  const std::string& getUserName() const { return userName; }
  /// access out unit
  int getID() const { return ID; }
  /// access out unit
  int getOutUnit() const { return outputUnit; }
  
  virtual void write(std::ostream&) const;
  virtual void writeAuxScore(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const flukaTally&);
  
}  // NAMESPACE flukaSystem

#endif
