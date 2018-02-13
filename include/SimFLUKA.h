/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimFLUKA.h
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
#ifndef SimFLUKA_h
#define SimFLUKA_h

namespace flukaSystem
{
  class userTally;
}

/*!
  \class SimFLUKA
  \brief Modifides Simulation to output a Fluka input file
  \author S. Ansell
  \version 1.0
  \date September 2015
 */
class SimFLUKA : public Simulation
{
 private:

  /// Tally type
  typedef std::map<std::string,flukaSystem::userTally*> flukaTTYPE; 

  /// horrific string for fluka input file
  const std::string alignment;   

  std::map<std::string,flukaTTYPE> UTMap;       ///< User tally map
  
  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  void writeElements(std::ostream&) const;
  void writeWeights(std::ostream&) const;
  void writeTransform(std::ostream&) const;
  void writeTally(std::ostream&) const;
  void writePhysics(std::ostream&) const;
  void writeVariables(std::ostream&) const;

  const std::string& getLowMatName(const size_t) const;
  std::string getLowMat(const size_t,const size_t,const std::string&) const;

  
 public:
  
  SimFLUKA();
  SimFLUKA(const SimFLUKA&);
  SimFLUKA& operator=(const SimFLUKA&);
  virtual ~SimFLUKA() {}           ///< Destructor

  void addTally(const flukaSystem::userTally&);
  flukaSystem::userTally* getTally(const std::string&) const;

  virtual void write(const std::string&) const;

};

#endif
