/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimPHITS.h
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
#ifndef SimPHITS_h
#define SimPHITS_h

class localRotate;

namespace magnetSystem
{
  class magnetUnit;
}

namespace phitsSystem
{
  class phitsTally;
  class phitsPhysics;
}

/*!
  \class SimPHITS
  \brief Modifides Simulation to output PHITS input file
  \author S. Ansell
  \version 1.0
  \date September 2009
 */
class SimPHITS : public Simulation
{
 public:

  /// Tally  : tally
  typedef std::map<std::string,phitsSystem::phitsTally*> PTallyTYPE;
  /// Name : magnet
  typedef std::map<std::string,
    std::shared_ptr<magnetSystem::magnetUnit>> MagTYPE;

 private:

  std::string fileName;                ///< Output file name [for tallies]
  int icntl;                           ///< ICNTL
  size_t nps;                          ///< number of particles to run
  long int rndSeed;                    ///< RND seed

  PTallyTYPE PTItem;                   ///< Phits tally map

  MagTYPE MagItem;                ///< Fluka magnetic map
  
  phitsSystem::phitsPhysics* PhysPtr;   ///< Phits physics 
  
  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeSource(std::ostream&) const;
  void writeImportance(std::ostream&) const;
  void writeMagnet(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  void writeWeights(std::ostream&) const;
  void writeTransform(std::ostream&) const;
  void writeTally(std::ostream&) const;
  void writePhysics(std::ostream&) const;
  void writeVariables(std::ostream&) const;
  
 public:
  
  SimPHITS();
  SimPHITS(const SimPHITS&);
  SimPHITS& operator=(const SimPHITS&);
  virtual ~SimPHITS() {}           ///< Destructor

  void setICNTL(const std::string&);
  /// set nps [move to physics]
  void setNPS(const size_t N) { nps=N; }
  /// set rndseed [move to physics]
  void setRND(const long int N) { rndSeed=N; }

  /// access to physics
  phitsSystem::phitsPhysics* getPhysics() { return PhysPtr; }

  virtual std::map<int,int>
  renumberCells(const std::vector<int>&,const std::vector<int>&);
  
  // TALLY Processing 
  void addTally(const phitsSystem::phitsTally&);
  phitsSystem::phitsTally* getTally(const int) const;
  PTallyTYPE& getTallyMap() { return PTItem; }            ///< Access tally map
  const PTallyTYPE& getTallyMap() const { return PTItem; }  ///< Access constant
  int getNextTallyID() const;

  void addMagnetObject(const MagTYPE::mapped_type&);
  
  /// set generic filename
  void setFileName(const std::string& FN) { fileName=FN; }
  /// access generic filename
  const std::string&  getFileName() const { return fileName; }
  
  virtual void write(const std::string&) const;

};

#endif
