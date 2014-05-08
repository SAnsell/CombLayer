/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/BeFullBlock.h
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
#ifndef delftSystem_BeFullBlock_h
#define delftSystem_BeFullBlock_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeFullBlock
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief BeFullBlock for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeFullBlock : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int insertIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< Offset on X to front
  double yStep;                 ///< Offset on Y to front
  double zStep;                 ///< Offset on Z top front

  double width;                 ///< Width
  double height;                ///< Height
  double length;                ///< length

  int mat;                      ///< Material 

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeFullBlock(const std::string&);
  BeFullBlock(const BeFullBlock&);
  BeFullBlock& operator=(const BeFullBlock&);
  virtual ~BeFullBlock();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
