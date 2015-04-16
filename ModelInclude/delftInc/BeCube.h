/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/BeCube.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef delftSystem_BeCube_h
#define delftSystem_BeCube_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeCube
  \version 1.0
  \author S. Ansell
  \date May 2014
  \brief BeCube for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeCube : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  int active;                   ///< To be created etc
  const int insertIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index


  double xStep;                 ///< Offset on X to front
  double yStep;                 ///< Offset on Y to front
  double zStep;                 ///< Offset on Z top front

  double innerRadius;           ///< Inner radius
  double outerWidth;            ///< Otuer Width [total]
  double outerHeight;           ///< Outer HEight [total]
  double length;                ///< Total length

  int mat;                      ///< Material 

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const std::string&);
  void createLinks();

 public:

  BeCube(const std::string&);
  BeCube(const BeCube&);
  BeCube& operator=(const BeCube&);
  virtual ~BeCube();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const std::string&);

};

}

#endif
 
