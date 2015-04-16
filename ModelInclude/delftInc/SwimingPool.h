/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/SwimingPool.h
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
#ifndef delftSystem_SwimingPool_h
#define delftSystem_SwimingPool_h

class Simulation;

namespace delftSystem
{

/*!
  \class SwimingPool
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief SwimingPool for reactor
  
  Door side is the back / widths are in the beam direction (Y)
*/

class SwimingPool : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int poolIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double base;                  ///< Base distance 
  double surface;               ///< Surface distance
  
  double xStep;                 ///< Centre step in X
  double yStep;                 ///< Centre step in Y
  double zStep;                 ///< Centre step in Z

  double frontWidth;            ///< front cut plane
  double backWidth;             ///< back cut plane [before door]
  double beamSide;              ///< Beam sides 
  double extendBeamSide;        ///< Extended beam side
  double beamSideBackLen;       ///< distance to start of width change
  double beamSideExtendLen;     ///< distance to start of width change
  double beamSideFrontLen;      ///< distance to start of width change
  double frontLength;           ///< distance to start of width change
  double doorWidth;             ///< door width
  double doorLength;            ///< door length

  int waterMat;                  ///< Material number

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,
		     const attachSystem::ContainedComp&);

 public:

  SwimingPool(const std::string&);
  SwimingPool(const SwimingPool&);
  SwimingPool& operator=(const SwimingPool&);
  virtual ~SwimingPool();

  // Accessor to the cell
  int getPoolCell() const { return poolIndex+1; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 
