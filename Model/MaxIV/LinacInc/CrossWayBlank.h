/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/CrossWayBlank.h
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
#ifndef tdcSystem_CrossWayBlank_h
#define tdcSystem_CrossWayBlank_h

class Simulation;


namespace tdcSystem
{

  /*!
  \class CrossWayBlank
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief CrossWayBlank for Max-IV
*/

class CrossWayBlank :
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
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CrossWayBlank(const std::string&);
  CrossWayBlank(const std::string&,const std::string&);
  CrossWayBlank(const CrossWayBlank&);
  CrossWayBlank& operator=(const CrossWayBlank&);
  virtual ~CrossWayBlank();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
