/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/GaugeTube.h
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
#ifndef xraySystem_GaugeTube_h
#define xraySystem_GaugeTube_h

class Simulation;


namespace xraySystem
{

  /*!
  \class GaugeTube
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief GaugeTube for Max-IV
*/

class GaugeTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double yRadius;               ///< radiu of Y (Beam) port
  double xRadius;               ///< radius of x port 

  double wallThick;             ///< pipe thickness
  
  double length;                ///< full to flange length (Y axis)

  double sideYStep;             ///< step in Y direction from centre
  double sideLength;            ///< beam to flange length
  double sideAngle;             ///< rotation angle of side
  
  double flangeXRadius;         ///< Joining Flange radius
  double flangeYRadius;         ///< Joining Flange radius

  double flangeXLength;         ///< Joining Flange radius
  double flangeYLength;         ///< Joining Flange radius
  
  double plateThick;            ///< Joining Flange radius

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GaugeTube(const std::string&);
  GaugeTube(const std::string&,const std::string&);
  GaugeTube(const GaugeTube&);
  GaugeTube& operator=(const GaugeTube&);
  virtual ~GaugeTube();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
