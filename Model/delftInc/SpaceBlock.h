/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/SpaceBlock.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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

class SpaceBlock :  public attachSystem::FixedOffset,
    public attachSystem::ContainedComp
  {
 private:
  
  const std::string baseName;   ///< Base name

  int activeFlag;               ///< Active block
  double length;                ///< Total length
  double width;                ///< Total length
  double height;                ///< Total length
  int mat;                ///< Total length

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SpaceBlock(const std::string&,const size_t);
  SpaceBlock(const SpaceBlock&);
  SpaceBlock& operator=(const SpaceBlock&);
  virtual ~SpaceBlock();

  /// accessor to flag
  int getActiveFlag() { return activeFlag; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
