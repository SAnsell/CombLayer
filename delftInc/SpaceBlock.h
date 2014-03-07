/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/SpaceBlock.h
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
#ifndef delftSystem_SpaceBlock_h
#define delftSystem_SpaceBlock_h

class Simulation;

namespace delftSystem
{

/*!
  \class SpaceBlock
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief SpaceBlock for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class SpaceBlock :  public attachSystem::FixedComp,
    public attachSystem::ContainedComp
  {
 private:
  
  const std::string baseName;   ///< Base name
  const int boxIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on X to Target
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< xyRotation angle
  double zAngle;                ///< zRotation angle

  double length;                ///< Total length
  double width;                ///< Total length
  double height;                ///< Total length
  int mat;                ///< Total length

  int populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const size_t);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SpaceBlock(const std::string&,const size_t);
  SpaceBlock(const SpaceBlock&);
  SpaceBlock& operator=(const SpaceBlock&);
  virtual ~SpaceBlock();

  int createAll(Simulation&,const attachSystem::FixedComp&,
		const size_t);

};

}

#endif
 
