/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/GratingUnit.h
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
#ifndef xraySystem_GratingUnit_h
#define xraySystem_GratingUnit_h

class Simulation;

namespace xraySystem
{
  class GrateHolder;

/*!
  \class GratingUnit
  \author S. Ansell
  \version 1.0
  \date September 2018
  \brief Paired Mono-crystal mirror constant exit gap
*/

class GratingUnit :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double grateTheta;        ///< Theta angle for grating
  int grateIndex;           ///< Offset position of grating

  std::array<std::shared_ptr<GrateHolder>,3> grateArray;
  
  double mainGap;           ///< Void gap between bars
  double mainBarXLen;       ///< X length of bars (to side support)
  double mainBarDepth;      ///< Depth Z direction
  double mainBarYWidth;     ///< Bar extent in beam direction

  double slidePlateZGap;       ///< lift from the mirror surface
  double slidePlateThick;      ///< slide bar
  double slidePlateWidth;      ///< slide bar extra width 
  double slidePlateLength;     ///< slide bar extra length


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

  GratingUnit(const std::string&);
  GratingUnit(const GratingUnit&);
  GratingUnit& operator=(const GratingUnit&);
  virtual ~GratingUnit();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
