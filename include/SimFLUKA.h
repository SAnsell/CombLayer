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
  class flukaTally;
  class flukaPhysics;
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
 public:

  /// Tally fortranIO : tally
  typedef std::map<int,flukaSystem::flukaTally*> FTallyTYPE;
  
 private:

  const std::string alignment;    ///< the alignemnt string

  std::string defType;            ///< Default physics type
  bool writeVariable;             ///< Prevent the writing of variables
  bool lowEnergyNeutron;          ///< Low energy neutron assigned
  size_t nps;                     ///< Number of particles
  long int rndSeed;               ///< Random number seed

  std::string sourceExtraName;    ///< Extra name if using combined sources
  
  FTallyTYPE FTItem;              ///< Fluka tally map

  flukaSystem::flukaPhysics* PhysPtr;   ///< Fluka physics

  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  void writeElements(std::ostream&) const;
  void writeWeights(std::ostream&) const;
  void writeTransform(std::ostream&) const;
  void writeTally(std::ostream&) const;
  void writePhysics(std::ostream&) const;
  void writeSource(std::ostream&) const;
  void writeVariables(std::ostream&) const;

  const std::string& getLowMatName(const size_t) const;
  std::string getLowMat(const size_t,const size_t,const std::string&) const;
  void clearTally();
  
 public:
  
  SimFLUKA();
  SimFLUKA(const SimFLUKA&);
  SimFLUKA& operator=(const SimFLUKA&);
  virtual ~SimFLUKA();

  // TALLY Processing 
  void addTally(const flukaSystem::flukaTally&);
  flukaSystem::flukaTally* getTally(const int) const;
  flukaSystem::flukaPhysics* getPhysics() { return PhysPtr; }

  /// Access tally items
  FTallyTYPE& getTallyMap() { return FTItem; }

  /// Access constant
  const FTallyTYPE& getTallyMap() const { return FTItem; }
  int getNextFTape() const;

  virtual void setExtraSourceName(const std::string&);
  /// get RND seed
  long int getRNDseed() const { return rndSeed; }

  /// set nps [move to physics]
  void setNPS(const size_t N) { nps=N; }
  /// set rndseed [move to physics]
  void setRND(const long int N) { rndSeed=N; }

  virtual void prepareWrite();
  /// no write variable
  void setNoVariables() { writeVariable=0; }
  /// no low energy neturon
  void setNoThermal() { lowEnergyNeutron=0; }
  void setDefaultPhysics(const std::string&);
  void setForCinder();
  void processActiveMaterials() const;
  
  virtual void write(const std::string&) const;

};

#endif
