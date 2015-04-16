/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuildInc/BColdMod.h
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
#ifndef bibSystem_bibBColMod_h
#define bibSystem_bibBColMod_h

class Simulation;

namespace bibSystem
{

/*!
  \class BColMod
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Specialized for wheel
*/

class BColMod : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step

  double xSize;                   ///< Max volume X
  double ySize;                   
  double zSize;

  double modYStep;
  double preWidth;
  double preHeight;
  double preDepth;
  double preWall;
  double preXGap;
  double preYGap;
  double preZGap;

  double modWidth;
  double modHeight;
  double modDepth;
  double modWall;

  int preMat;                      ///< Pre Material    
  int modMat;                      ///< Water material
  int wallMat;                     ///< Wall material


  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  public:

  BColMod(const std::string&);
  BColMod(const BColMod&);
  BColMod& operator=(const BColMod&);
  virtual ~BColMod();

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
