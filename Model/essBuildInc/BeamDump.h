/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BeamDump.h
 *
 * Copyright (c) 2016 by Konstantin Batkov
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
#ifndef essSystem_BeamDump_h
#define essSystem_BeamDump_h

class Simulation;

namespace essSystem
{

/*!
  \class BeamDump
  \version 1.0
  \author Konstantin Batkov
  \date DATE
  \brief LINAC4 commissioning beam dump assembly
*/

class BeamDump : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  int    steelMat;              ///< Steel material
  int    concMat;               ///< Concrete material
  int    alMat;                 ///< Aluminium material

  double frontWallLength;       ///< Front wall length (+y, vol 6 in SPLTDISH0001)
  double frontWallHeight;       ///< Front wall height (+z)
  double frontWallDepth;        ///< Front wall depth (-z)
  double frontWallWidth;        ///< Front wall width (+-x)

  double backWallLength;        ///< Back wall length (+y, vol 6 in SPLTDISH0001)
  double backWallDepth;         ///< Back wall depth (-z)

  double frontInnerWallDepth;   ///< pl29
  double frontInnerWallLength;  ///< pl29
  double backInnerWallLength;   ///< thickness of back inner wal (plate 24+30+31)
  double backInnerWallGapLength; ///< thickness of gap b/w back inner and outer walls
  double sideWallThick;         ///< thickness of each of the side walls (plates 21+27)

  double floorLength;           ///< Floor length (+y, vol 6 in SPLTDISH0001)
  double floorDepth;            ///< Floor thickness (+z)

  double plate25Length;         ///< Length of plate 25 (small plate under the floor)
  double plate25Depth;          ///< Thickness of small plate below the floor (+z)
 
  double plate38Depth;          ///< Thickness of Al plate 38

  double roofThick;           ///< Thickness (height) of roof
  double roofOverhangLength;  ///< Length of overhang of roof
  double innerRoofThick;        ///< Thickness (height) of inner roof (plates 19 and 20)

  double vacPipeFrontInnerWallDist;   ///< distance b/w front inner wall and vacuum pipe
  double vacPipeLength;          ///< vacuum pipe length
  double vacPipeRad;             ///< vacuum pipe radius
  double vacPipeSideWallThick;   ///< vacuum pipe side wall thickness

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

  
 public:

  BeamDump(const std::string&);
  BeamDump(const BeamDump&);
  BeamDump& operator=(const BeamDump&);
  virtual BeamDump* clone() const;
  virtual ~BeamDump();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int&);

};

}

#endif
 

