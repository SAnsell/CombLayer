/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1Mirror.h
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
#ifndef xraySystem_M1Mirror_h
#define xraySystem_M1Mirror_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1Mirror
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1Mirror :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double theta;            ///< Theta angle
  double phi;              ///< phi angle

  double width;               ///< Width of mirror
  double height;              ///< Depth into mirror
  double length;              ///< Length along mirror
  double slotXStep;           ///< Length along mirror
  double slotWidth;           ///< Length along mirror
  double slotDepth;           ///< Length along mirror
  double pipeXStep;           ///< step from mirror surface to base join
  double pipeYStep;           ///< step from mirror end
  double pipeZStep;           ///< step from lower base up.

  double pipeWallThick;       ///< Pipe thickness
  double pipeSideRadius;      ///< radius at side

  double pipeBaseLen;         ///< radius at base
  double pipeBaseRadius;      ///< radius at base

  double pipeOuterLen;       ///< Length of outer pipe vertical
  double pipeOuterRadius;     ///< radius of outer pipe vertical

  size_t nWaterChannel;      ///< water channels
  double waterLength;         ///< water width
  double waterWidth;         ///< water width
  double waterHeight;        ///< water height
  double waterDrop;          ///< water gap to Si surface
  double waterGap;           ///< water gap between
  
  int mirrorMat;             ///< XStal material
  int waterMat;              ///< material in pipe
  int pipeMat;               ///< Base material
  int outerMat;              ///< outer pipe material
  int voidMat;              ///< outer pipe material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Mirror(const std::string&);
  M1Mirror(const M1Mirror&);
  M1Mirror& operator=(const M1Mirror&);
  ~M1Mirror() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
