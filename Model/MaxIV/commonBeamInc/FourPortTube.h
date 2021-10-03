/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/FourPortTube.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_FourPortTube_h
#define xraySystem_FourPortTube_h

class Simulation;


namespace xraySystem
{

  /*!
  \class FourPortTube
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief FourPortTube for Max-IV
*/

class FourPortTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;               ///< void radius
  double linkRadius;           ///< void radius on 4 cross way
  double wallThick;            ///< pipe thickness
  double linkWallThick;        ///< wall thickness of side
  
  double frontLength;           ///< length -Y
  double backLength;            ///< length +Y
  double sideALength;           ///< length -X
  double sideBLength;           ///< length +X

  double flangeARadius;         ///< Joining Flange radius
  double flangeBRadius;         ///< Joining Flange radius
  double flangeSRadius;         ///< Joining Flange radius

  double flangeALength;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange radius
  double flangeSLength;         ///< Joining Flange radius
  
  double plateThick;           ///< Joining Flange radius

  int voidMat;                 ///< void material
  int mainMat;                 ///< main material
  int flangeMat;               ///< flange material
  int plateMat;                ///< plate material

  bool sideVoidFlag;          ///< Make the side inner void complete
  
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FourPortTube(const std::string&);
  FourPortTube(const std::string&,const std::string&);
  FourPortTube(const FourPortTube&);
  FourPortTube& operator=(const FourPortTube&);
  virtual ~FourPortTube();

  /// set to have the side void as the main void
  void setSideVoid() { sideVoidFlag=1; }
  
  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
