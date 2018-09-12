/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/phitsTally.h
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
#ifndef phitsSystem_phitsTally_h
#define phitsSystem_phitsTally_h

namespace phitsSystem
{

  class auxScore;
  
/*!
  \class phitsTally
  \version 1.0
  \author S. Ansell
  \date September 2018
  \brief Holds a phits tally object as a base class
  
*/

class phitsTally
{
 protected:

  std::string keyName;              ///< tally name
  int outputUnit;                   ///< Fortran output number
  std::string comments;             ///< comment line

  std::string auxParticle;          ///< Auxillary particle
  std::string doseType;             ///< Auxillary dose type if used
  
 public:
  
  explicit phitsTally(const int);
  phitsTally(const std::string&,const int);
  phitsTally(const phitsTally&);
  phitsTally& operator=(const phitsTally&);
  virtual phitsTally* clone() const; 
  virtual ~phitsTally();
  
  void setKeyName(const std::string&);
  void setBinary();
  void setComment(const std::string&);
  void setAuxParticles(const std::string&);
  virtual void setDoseType(const std::string&,const std::string&);
  virtual void setEnergy(const bool,const double,const double,const size_t);
  virtual void setAngle(const bool,const double,const double,const size_t);

  const std::string& getKeyName() const;
  /// access out unit
  int getOutUnit() const { return outputUnit; }

  
  virtual void write(std::ostream&) const;
  virtual void writeAuxScore(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const phitsTally&);
  
}  // NAMESPACE phitsSystem

#endif
