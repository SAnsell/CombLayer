/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/CrossWayTube.h
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
#ifndef xraySystem_CrossWayTube_h
#define xraySystem_CrossWayTube_h

class Simulation;


namespace xraySystem
{

  /*!
  \class CrossWayTube
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief CrossWayTube for Max-IV
*/

class CrossWayTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double xRadius;               ///< radius of x port 
  double yRadius;               ///< radiu of Y (Beam) port

  double wallThick;             ///< pipe thickness

  double height;                ///< full to flange height above beam
  double depth;                 ///< full to flange depth below beam

  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length
  double sideALength;            ///< full to flange length
  double sideBLength;            ///< full to flange length

  double flangeXRadius;         ///< Joining Flange radius
  double flangeYRadius;         ///< Joining Flange radius
  double flangeZRadius;         ///< Joining Flange radius

  double flangeXLength;         ///< Joining Flange radius
  double flangeYLength;         ///< Joining Flange radius
  double flangeZLength;         ///< Joining Flange radius
  
  double plateThick;            ///< Joining Flange radius

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material
 
  void populate(const FuncDataBase&) override;  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CrossWayTube(const std::string&);
  CrossWayTube(const std::string&,const std::string&);
  CrossWayTube(const CrossWayTube&);
  CrossWayTube& operator=(const CrossWayTube&);
  ~CrossWayTube() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
