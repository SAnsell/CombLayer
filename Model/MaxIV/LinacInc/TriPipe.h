/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/TriPipe.h
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
#ifndef xraySystem_TriPipe_h
#define xraySystem_TriPipe_h

class Simulation;

namespace tdcSystem
{
  
/*!
  \class TriPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief TriPipe unit  
*/

class TriPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double axisXStep;                ///< Rotation about Z
  double axisZStep;                 ///< Rotation about X

  double axisXYAngle;                ///< Rotation about Z
  double axisZAngle;                 ///< Rotation about X
  
  double frontWidth;             ///< void width at front  [inner]
  double backWidth;              ///< void width at back [inner]
  double frontHeight;            ///< void height [front/inner]
  double backHeight;             ///< void height [back/inner]
  double length;                 ///< void length [total]
  
  double wallThick;              ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius 
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius 
  double flangeBLength;          ///< Joining Flange length
    
  int voidMat;                   ///< Void material
  int wallMat;                   ///< Pipe material
  int flangeMat;                 ///< Flange material

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:

  TriPipe(const std::string&);
  TriPipe(const TriPipe&);
  TriPipe& operator=(const TriPipe&);
  virtual ~TriPipe();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
