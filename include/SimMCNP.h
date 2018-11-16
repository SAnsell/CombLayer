/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimMCNP.h
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
#ifndef SimMCNP_h
#define SimMCNP_h

namespace Geometry
{
  class Transform;
}

namespace tallySystem
{
  class Tally;
  class sswTally;
}

namespace Surface 
{
  class Surface;
}

namespace physicsSystem
{
  class PhysicsCards;
}

namespace ModelSupport
{
  class ObjSurfMap;
}

namespace WeightSystem
{
  class WeightMesh;
  class WeightControl;
}

namespace MonteCarlo
{
  class Object;
  class Material;
  class Object;
}

/*!
  \class SimMCNP
  \version 1.3
  \date February 2017
  \author S.Ansell
  \brief Specialization of Simulation for MCNP

*/

class SimMCNP : public Simulation
{
 public:

  typedef std::map<int,tallySystem::Tally*> TallyTYPE; ///< Tally type

 private:

  int mcnpVersion;                      ///< version of mcnp

  TallyTYPE TItem;                        ///< Tally Items

  physicsSystem::PhysicsCards* PhysPtr;   ///< Physics Cards


  void deleteTally();
  
  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  void writeWeights(std::ostream&) const;
  void writeTransform(std::ostream&) const;
  void writeTally(std::ostream&) const;
  void writeSource(std::ostream&) const;
  void writePhysics(std::ostream&) const;


  // The Cinder Write stuff
  void writeCinderMat() const;

 public:

  SimMCNP();
  SimMCNP(const SimMCNP&);
  SimMCNP& operator=(const SimMCNP&);
  virtual ~SimMCNP();
  
  virtual void resetAll();

  /// is the system MCNP6
  bool isMCNP6() const { return mcnpVersion!=10; }

  // processing for Simulation
  virtual void removeCell(const int);
  
  // Tally processing

  void removeAllTally();
  int removeTally(const int);

  int addTally(const tallySystem::Tally&);
  tallySystem::Tally* getTally(const int) const;
  tallySystem::sswTally* getSSWTally() const;
  /// Access tally items
  TallyTYPE& getTallyMap() { return TItem; }
  /// Access constant
  const TallyTYPE& getTallyMap() const { return TItem; }
  void setForCinder();
  int nextTallyNum(int) const;

  virtual void setEnergy(const double);

  
  void setMCNPversion(const int);
  virtual void substituteAllSurface(const int,const int);
  virtual std::map<int,int> renumberCells(const std::vector<int>&,
					  const std::vector<int>&);

  /// Get PhysicsCards
  physicsSystem::PhysicsCards& getPC() { return *PhysPtr; }
  
  virtual void writeCinder() const;          

  virtual void write(const std::string&) const;  
    
};

#endif
