/****************************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/HeavyShutter.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef essSystem_HeavyShutter_h
#define essSystem_HeavyShutter_h

class Simulation;

namespace essSystem
{

/*!
  \class HeavyShutter
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief Heavy Shutter
*/

class HeavyShutter : public attachSystem::ContainedComp,
                     public attachSystem::FixedOffset,
                     public attachSystem::CellMap
{
 private:
   
  const int heavyIndex;           ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  
  double length;                  ///< Total length without casing
  double width;                   ///< Total width without casing
  double height;                  ///< Total height without casing
  double wallThick;               ///< External wall thickness
  double guideLodging;            ///< Vertical space for guide housing
  double separatorThick;          ///< Separators thicknesses
  std::vector<double> slabThick;  ///< Vector of slab thicknesses
  
  int mainMat;                    ///< Main material
  std::vector<int> slabMat;       ///< Slabs materials
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  
 public:
  
  HeavyShutter(const std::string&);
  HeavyShutter(const HeavyShutter&);
  HeavyShutter& operator=(const HeavyShutter&);
  virtual ~HeavyShutter();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
};

}

#endif
