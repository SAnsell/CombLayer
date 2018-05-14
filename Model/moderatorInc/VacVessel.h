/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/VacVessel.h
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
#ifndef moderatorSystem_VacVessel_h
#define moderatorSystem_VacVessel_h

class Simulation;

namespace moderatorSystem
{

  class Groove;
  class Hydrogen;
  class Decoupled;

/*!
  \class VacVessel
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief VacVessel [insert object]
*/

class VacVessel : 
  public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int vacIndex;               ///< Index of surface offset
  const std::string grooveKeyName;  ///< KeyName [Groove]
  const std::string hydKeyName;     ///< KeyName [Hydrogen]
  int cellIndex;                    ///< Cell index
  int populated;                    ///< 1:var
  
  std::array<Geometry::Vec3D,6> BVec;     ///< Boundary Points
  int divideSurf;                           ///< Divide surface

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

  VacVessel(const std::string&);
  VacVessel(const VacVessel&);
  VacVessel& operator=(const VacVessel&);
  virtual ~VacVessel();

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  /// Accessor to divide surf:
  int getDivideSurf() const { return divideSurf; }

  void createAllPair(Simulation&,const Groove&,const Hydrogen&);
  void createAll(Simulation&, const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);


};

}

#endif
 
