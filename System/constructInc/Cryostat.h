/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Cryostat.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_Cryostat_h
#define constructSystem_Cryostat_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class InnerPort;
  
/*!
  \class Cryostat
  \version 1.0
  \author S. Ansell
  \date January 2017
  \brief Cryostat unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class Cryostat :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int cryIndex;        ///< Index of surface offset
  int cellIndex;             ///< Cell index  

  int active;                ///< Active flag
  
  double sampleZOffset;      ///< Z-offset from centre line
  double sampleRadius;       ///< Radius of sample holder
  double sampleHeight;       ///< Total height of sample
  
  double voidRadius;         ///< Radius of void for sample
  double voidDepth;          ///< Radius of void for sample
  double voidHeight;         ///< Radius of void for sample
  double voidWallThick;      ///< thickness of sample wall
  
  double stickLen;           ///< length of sample stick to top
  double stickRadius;        ///< radius of sample stick

  double stickBoreRadius;    ///< Radius of void for sample
  double stickBoreHeight;    ///< Stick bore height
  double stickBoreThick;     ///< Stick bore wall thick

  double heatRadius;         ///< Main radius of shield
  double heatDepth;          ///< Depth of shield
  double heatHeight;         ///< Height above sample
  double heatThick;          ///< Wall thickness
  
  double heatOuterRadius;    ///< Main radius of outer shield
  double heatOuterLift;      ///< Distance of first cone

  double liqHeInnerRadius;   ///< inner radius
  double liqHeOuterRadius;   ///< Outer radius
  double liqHeWallThick;     ///< Thickness of walls
  double liqHeHeight;        ///< Height
  double liqHeExtra;         ///< lift above N2 tank

  double liqN2InnerRadius;   ///< inner radius
  double liqN2OuterRadius;   ///< Outer radius
  double liqN2WallThick;     ///< Thickness of walls
  double liqN2Height;        ///< Height

  double tailRadius;         ///< Main radius of tail (outer)
  double tailDepth;          ///< Depth of tail (outer)
  double tailHeight;         ///< Height above sample
  double tailThick;          ///< Wall thickness

  double tailOuterRadius;
  double tailOuterHeight;
  double tailOuterLift;

  double mainThick;        ///< Main (outer body) Wall thickness
  double roofThick;        ///< Main (outer body) Wall thickness
  
  int sampleMat;         ///< Sample Material
  int liqN2Mat;          ///< liquid nitrogen
  int liqHeMat;          ///< liquid He Material
  int wallMat;           ///< Wall Material
  int stickMat;          ///< stick Material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createNullObjects();
  void createLinks();

 public:

  Cryostat(const std::string&);
  Cryostat(const Cryostat&);
  Cryostat& operator=(const Cryostat&);
  virtual ~Cryostat();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
