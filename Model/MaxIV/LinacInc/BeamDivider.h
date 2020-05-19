/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/BeamDivider.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_BeamDivider_h
#define tdcSystem_BeamDivider_h

class Simulation;


namespace tdcSystem
{
/*!
  \class BeamDivider
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief BeamDivider for Max-IV
*/

class BeamDivider :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double boxLength;                ///< void length [total]
  double wallThick;                ///< void radius
  double mainWidth;                ///< box width on left [front]
  double exitWidth;                ///< box width on right from exitOrg
  double height;                   ///< box height

  double mainXStep;                ///< Main step [from box end]
  
  double exitXStep;                ///< Exit step [from box end]
  double exitAngle;                ///< angle on right

  double mainLength;               ///< Main length 
  double mainRadius;               ///< main radius
  double mainThick;                ///< main wall thickness

  double exitLength;               ///< Exit length [from box end]
  double exitRadius;               ///< exit radius
  double exitThick;                ///< exit wall thickness

  double flangeARadius;         ///< Joining Flange radius 
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius 
  double flangeBLength;         ///< Joining Flange length

  double flangeERadius;         ///< Exit flange radius
  double flangeELength;         ///< Exit flange length
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< electrode material
  int flangeMat;                ///< flange material  

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeamDivider(const std::string&);
  BeamDivider(const std::string&,const std::string&);
  BeamDivider(const BeamDivider&);
  BeamDivider& operator=(const BeamDivider&);
  virtual ~BeamDivider();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
