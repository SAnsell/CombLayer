/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawValve.h
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
#ifndef xraySystem_JawValve_h
#define xraySystem_JawValve_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class JawValve
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief JawValve unit  
*/

class JawValve :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double length;                ///< Void length
  double width;                 ///< Void width (full)
  double height;                ///< height 
  double depth;                 ///< depth
  
  double wallThick;             ///< Wall thickness
  double portRadius;            ///< Port inner radius (opening)
  double portThick;             ///< Port outer ring
  double portLen;               ///< Forward step of port

  JawUnit JItem;                ///< Paired Jaw [contolled by this]
  
  int voidMat;                  ///< Void material
  int wallMat;                  ///< Pipe material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createJaws(Simulation&);
  
 public:

  JawValve(const std::string&);
  JawValve(const JawValve&);
  JawValve& operator=(const JawValve&);
  virtual ~JawValve();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
