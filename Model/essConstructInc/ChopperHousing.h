/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/ChopperHousing.h
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
#ifndef constructSystem_ChopperHousing_h
#define constructSystem_ChopperHousing_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class ChopperHousing
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief ChopperHousing unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class ChopperHousing :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  

  double voidHeight;            ///< void height from chopper rot centre
  double voidWidth;             ///< void width [across]
  double voidDepth;             ///< void depth 
  double voidThick;             ///< void length [total]
  
  double wallThick;             ///< wall thickness  
  int wallMat;                  ///< Wall material layer
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ChopperHousing(const std::string&);
  ChopperHousing(const ChopperHousing&);
  ChopperHousing& operator=(const ChopperHousing&);
  ~ChopperHousing() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
