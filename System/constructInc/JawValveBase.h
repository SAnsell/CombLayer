/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawValveBase.h
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
#ifndef constructSystem_JawValveBase_h
#define constructSystem_JawValveBase_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class JawValveBase
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief JawValveBase unit  
*/

class JawValveBase :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 protected:
  
  double length;                ///< Void length
  
  double wallThick;             ///< Wall thickness

  double portARadius;            ///< Port inner radius (opening)
  double portAThick;             ///< Port outer ring
  double portALen;               ///< Forward step of port

  double portBRadius;            ///< Port inner radius (opening)
  double portBThick;             ///< Port outer ring
  double portBLen;               ///< Forward step of port

  JawUnit JItem;                ///< Paired Jaw [contolled by this]
  
  int voidMat;                  ///< Void material
  int wallMat;                  ///< Pipe material
  
  virtual void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,const long int);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&) =0;
  void createOuterObjects(Simulation&);
  virtual void createLinks();

  void createJaws(Simulation&);
  
 public:

  JawValveBase(const std::string&);
  JawValveBase(const JawValveBase&);
  JawValveBase& operator=(const JawValveBase&);
  virtual ~JawValveBase();

  using FixedComp::createAll;
  void createAll(Simulation& System,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
