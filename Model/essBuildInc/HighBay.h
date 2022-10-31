/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/HighBay.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef essSystem_HighBay_h
#define essSystem_HighBay_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  
/*!
  \class HighBay
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bunker wall 
*/

class HighBay :
    public attachSystem::FixedComp,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  double length;                 ///< Y direction of roof
  double height;                 ///< Height 
  double roofThick;              ///< Wall thickness

  int wallMat;                   ///< Default wall material
  int roofMat;                   ///< Default wall material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  HighBay(const std::string&);
  HighBay(const HighBay&);
  HighBay& operator=(const HighBay&);
  virtual ~HighBay();

  void createAll(Simulation&,const FixedComp&,const long int);

};

}

#endif
 

