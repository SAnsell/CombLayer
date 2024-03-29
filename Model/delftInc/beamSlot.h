/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/beamSlot.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef delftSystem_beamSlot_h
#define delftSystem_beamSlot_h

class Simulation;

namespace delftSystem
{
  /*!
    \class beamSlot
    \version 1.0
    \author S. Ansell
    \date July 2012
    Creates a beamslot
  */
  
class beamSlot :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut  
{
 private:

  const std::string baseName;  ///< Base name
  
  double xSize;          ///< Size of the slot [long]    
  double zSize;          ///< Size of the slot [short]

  double endThick;       ///< End thickness
  double divideThick;    ///< End thickness
  size_t NChannels;      ///< Number of open channels

  int glassMat;          ///< Glass material

  void populate(const FuncDataBase&) override;
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:
  
  beamSlot(const std::string&,const int);
  beamSlot(const beamSlot&);
  beamSlot& operator=(const beamSlot&);
  ~beamSlot() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;  

};

}

#endif
 
