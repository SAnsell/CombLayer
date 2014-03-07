/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/muonTube.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_muonTube_h
#define muSystem_muonTube_h

class Simulation;

namespace muSystem
{

/*!
  \class muonTube
  \author G. Skoro
  \version 1.0
  \date October 2013
  \brief Simple Tube object
*/

class muonTube : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int tubeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step

  double radius;                ///< radius
  double thick;                 ///< thickness
  double length;                ///< length  
  int mat;                      ///< material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  muonTube(const std::string&);
  muonTube(const muonTube&);
  muonTube& operator=(const muonTube&);
  virtual ~muonTube();

  void createAll(Simulation&,const attachSystem::FixedComp&);  
};

}

#endif
 
