/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/TriggerTube.h
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
#ifndef tdcSystem_TriggerTube_h
#define tdcSystem_TriggerTube_h

class Simulation;


namespace xraySystem
{

  /*!
  \class TriggerTube
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief TriggerTube for Max-IV
*/

class TriggerTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius [vertical]
  double xRadius;               ///< radius of X (Side) port
  double yRadius;               ///< radius of Y (Beam) port

  double wallThick;             ///< pipe thickness

  double height;                ///< full to blank
  double depth;                 ///< full to flange depth below beam

  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length

  double flangeXRadius;         ///< Side join flange
  double flangeYRadius;         ///< Beam flange radius
  double flangeZRadius;         ///< Main radius

  double flangeXLength;         ///< Side Flange length
  double flangeYLength;         ///< Beam Flange length
  double flangeZLength;         ///< Main Flange length

  double sideZOffset;           ///< Second Z lift
  double sideLength;           ///< Second Z lift
  double plateThick;            ///< Top/Base plate thick

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TriggerTube(const std::string&);
  TriggerTube(const std::string&,const std::string&);
  TriggerTube(const TriggerTube&);
  TriggerTube& operator=(const TriggerTube&);
  virtual ~TriggerTube();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
