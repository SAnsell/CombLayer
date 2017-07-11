/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/PressureVessel.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_PressureVessel_h
#define delftSystem_PressureVessel_h

class Simulation;

namespace delftSystem
{

/*!
  \class PressureVessel
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief PressureVessel for reactor
  
  This is a Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class PressureVessel : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int pressIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int reshiftOrigin;          ///< Reshift origin [-1 front / 1 back ]
  double frontLength;         ///< Offset of front cap [y axis]
  double frontRadius;         ///< Radius of front cap
  double frontWall;           ///< Radius of front cap

  double backLength;          ///< Length to circle centre
  double backRadius;          ///< Inner radius of back cap
  double backWall;            ///< Thickness of back bcap

  double sideRadius;          ///< Inner Radius of side wall
  double sideWall;            ///< Thickness of side wall
  

  int wallMat;                  ///< Wall Material number

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PressureVessel(const std::string&);
  PressureVessel(const PressureVessel&);
  PressureVessel& operator=(const PressureVessel&);
  virtual ~PressureVessel();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
