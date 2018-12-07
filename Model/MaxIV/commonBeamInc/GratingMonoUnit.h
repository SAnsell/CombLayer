/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/GratingMonoUnit.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef xraySystem_GratingMonoUnit_h
#define xraySystem_GratingMonoUnit_h

class Simulation;

namespace xraySystem
{

/*!
  \class GratingMonoUnit
  \author S. Ansell
  \version 1.0
  \date September 2018
  \brief Paired Mono-crystal mirror constant exit gap
*/

class GratingMonoUnit :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double grateTheta;             ///< Theta angle for grateing

  double gWidth;            ///< Radius of from centre
  double gThick;            ///< Radius of detector
  double gLength;           ///< Outer wall thickness

  double mainGap;           ///< Void gap between bars
  double mainBarXLen;
  double mainBarDepth;
  double mainBarYWidth;      ///< Size in main ba

  double slidePlateThick;
  double slidePlateLength;
  double slidePlateGapWidth;
  double slidePlateGapLength;  

  int mainMat;            ///< Main metal
  int slideMat;           ///< slide material
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GratingMonoUnit(const std::string&);
  GratingMonoUnit(const GratingMonoUnit&);
  GratingMonoUnit& operator=(const GratingMonoUnit&);
  virtual ~GratingMonoUnit();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
