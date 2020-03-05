/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuildInc/ShieldRoom.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef exampleSystem_ShieldRoom_h
#define exampleSystem_ShieldRoom_h

class Simulation;

namespace exampleSystem
{
  
/*!
  \class ShieldRoom
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Bremsstralung Collimator unit  
*/

class ShieldRoom :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:

  double width;               ///< full width
  double height;              ///< roof height
  double depth;               ///<  Floor depth 
  double length;              ///< Main length  

  double sideThick;           ///< Optional width
  double roofThick;           ///< Optional height
  double floorThick;          ///< Thickness of floor
  
  double sideExtra;           ///< Optional side thickness
  double earthLevel;          ///< level ground [from void height]
  double earthDome;           ///< dome height [from concrete roof]
  double earthDepth;           ///< dome height [from concrete roof]
  double earthRadius;         ///< dome radius


  
  int voidMat;                ///< void material
  int innerMat;                ///< main material
  int earthMat;                ///< floor material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  ShieldRoom(const std::string&);
  ShieldRoom(const ShieldRoom&);
  ShieldRoom& operator=(const ShieldRoom&);
  virtual ~ShieldRoom();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
