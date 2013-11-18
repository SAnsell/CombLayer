/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/H2Water.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef delftSystem_H2Water_h
#define delftSystem_H2Water_h

class Simulation;

namespace delftSystem
{

/*!
  \class H2Water
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief H2Water [insert object]
*/

class H2Water : 
  public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int watIndex;               ///< Index of surface offset
  const std::string hydKeyName;     ///< KeyName [Hydrogen]
  int cellIndex;                    ///< Cell index
  
  double vacPosGap;         ///< Positive side length [Origin: MaxR]
  double vacNegGap;         ///< Negative side length [Origin: MaxR]
  double vacPosRadius;      ///< Positive side radius
  double vacNegRadius;      ///< Negative side radius  
  double vacLSide;       ///< Left Side thickness
  double vacRSide;       ///< Right Side thickness
  double vacTop;         ///< Top vac thickness
  double vacBase;        ///< Base vac thickness    

  double alPos;         ///< Positive al thickness
  double alNeg;         ///< Positive al thickness
  double alSide;        ///< Side thickness
  double alTop;         ///< Top Al thickness
  double alBase;        ///< Base Al thickness    

  double terPos;         ///< Positive tertiary thickness
  double terNeg;         ///< Positive tertiary thickness
  double terSide;        ///< Side tertiay thickness
  double terTop;         ///< Top tertiary thickness
  double terBase;        ///< Base tertiary thickness    

  double outPos;         ///< Positive outer thickness
  double outNeg;         ///< Positive outer thickness
  double outSide;        ///< Side thickness
  double outTop;         ///< Top outer thickness
  double outBase;        ///< Base outer thickness    

  double clearPos;         ///< Clearance Positive side
  double clearNeg;         ///< Positive outer thickness
  double clearSide;        ///< Side thickness
  double clearTop;         ///< Top outer thickness
  double clearBase;        ///< Base outer thickness    

  int alMat;             ///< Al (inner) material
  int outMat;            ///< Al (outer) material

  void populate(const Simulation&);

  void createUnitVector(const attachSystem::FixedComp&);
  void createBoundary(const attachSystem::FixedComp&);
  void createBoundary(const attachSystem::FixedComp&,
		      const attachSystem::FixedComp&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const std::string&);

  Geometry::Vec3D getDirection(const size_t) const;

 public:

  H2Water(const std::string&);
  H2Water(const H2Water&);
  H2Water& operator=(const H2Water&);
  virtual ~H2Water();

  Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  /// Accessor to divide surf:
  int getDivideSurf() const { return divideSurf; }

  void createAll(Simulation&,const Groove&,const Hydrogen&);
  void createAll(Simulation&,const Decoupled&);

};

}

#endif
 
