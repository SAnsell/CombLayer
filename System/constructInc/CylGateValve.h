/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/CylGateValve.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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
#ifndef constructSystem_CylGateValve_h
#define constructSystem_CylGateValve_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class CylGateValve
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CylGateValve unit  
*/

class CylGateValve :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  double length;                ///< Void length
  double width;                 ///< Void width (full)
  double height;                ///< height 
  double depth;                 ///< depth
  
  double wallThick;             ///< Wall thickness
  double portRadius;            ///< Port inner radius (opening)
  double portThick;             ///< Port outer ring
  double portLen;               ///< Forward step of port
  
  bool closed;                  ///< Shutter closed
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius
  
  int voidMat;                  ///< Void material
  int bladeMat;                 ///< blade material
  int wallMat;                  ///< Pipe material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CylGateValve(const std::string&);
  CylGateValve(const CylGateValve&);
  CylGateValve& operator=(const CylGateValve&);
  virtual ~CylGateValve();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
