/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1FrontShield.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1FrontShield_h
#define xraySystem_M1FrontShield_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1FrontShield
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1FrontShield :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double cubePlateThick;    ///< Inner cube plate thickness (full)
  
  double cubeThick;         ///< Thickness in Y direction
  double cubeWidth;         ///< Full width [back of xstal]
  double cubeHeight;        ///< Full height [xstal centre]
  double cubeSideWall;      ///< Thickness of side wall
  double cubeBaseWall;      ///< Thickness of base wall

  double innerThick;       ///< inner block thickness
  double innerHeight;       ///< inner block height
  double innerWidth;        ///< inner block width

  double pipeYStep;        ///< Symmetic y-step
  double pipeLength;        ///< Pipe length
  double pipeRadius;        ///< inner radius
  double pipeWall;          ///< Wall thickness
  
  int mat;                  ///< Material
  int voidMat;              ///< outer pipe material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1FrontShield(const std::string&);
  M1FrontShield(const M1FrontShield&);
  M1FrontShield& operator=(const M1FrontShield&);
  ~M1FrontShield() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
