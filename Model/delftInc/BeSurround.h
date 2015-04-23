/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/BeSurround.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef delftSystem_BeSurround_h
#define delftSystem_BeSurround_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeSurround
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief BeSurround for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeSurround : public attachSystem::ContainedComp,
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
  double outerRadius;           ///< Radius of Be outer
  double length;                ///< Total length

  int mat;                      ///< Material 

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const std::string&);
  void createLinks();

 public:

  BeSurround(const std::string&);
  BeSurround(const BeSurround&);
  BeSurround& operator=(const BeSurround&);
  virtual ~BeSurround();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const std::string&);

};

}

#endif
 
