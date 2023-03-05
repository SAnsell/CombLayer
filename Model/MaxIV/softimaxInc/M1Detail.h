/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/M1Detail.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1Detail_h
#define xraySystem_M1Detail_h

class Simulation;

namespace xraySystem
{

  class MLMCrystal;
  class MLMRadialSupport;
  class MLMSupportWheel;
  class MLMWheelPlate;
  
/*!
  \class M1Detail
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class M1Detail :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  std::shared_ptr<M1Mirror> mirror;
  
  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Detail(const std::string&);
  M1Detail(const M1Detail&);
  M1Detail& operator=(const M1Detail&);
  virtual ~M1Detail();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
