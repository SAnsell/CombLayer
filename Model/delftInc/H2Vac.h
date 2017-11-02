/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/H2Vac.h
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
#ifndef delftSystem_H2Vac_h
#define delftSystem_H2Vac_h

class Simulation;

namespace delftSystem
{


/*!
  \class H2Vac
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief H2Vac [insert object] 
  Cylindrical vacuum vessel system
*/

class H2Vac : 
  public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int vacIndex;               ///< Index of surface offset
  int cellIndex;                    ///< Cell index

  double sideRadius;        ///< Side radius 
  
  double vacPosGap;         ///< Positive side length [Origin: MaxR]
  double vacNegGap;         ///< Negative side length [Origin: MaxR]
  double vacPosRadius;      ///< Positive side radius
  double vacNegRadius;      ///< Negative side radius  
  double vacSide;           ///< Radius of side gap

  double alPos;         ///< Positive al thickness
  double alNeg;         ///< Negative al thickness
  double alSide;        ///< Side thickness

  double terPos;         ///< Positive tertiary thickness
  double terNeg;         ///< Positive tertiary thickness
  double terSide;        ///< Side tertiay thickness

  double outPos;         ///< Positive outer thickness
  double outNeg;         ///< Negagive outer thickness
  double outSide;        ///< Side thickness

  double clearPos;         ///< Clearance Positive side
  double clearNeg;         ///< Positive outer thickness
  double clearSide;        ///< Side thickness

  int alMat;             ///< Al (inner) material
  int outMat;            ///< Al (outer) material

  void populate(const FuncDataBase&);

  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces(const attachSystem::FixedComp&);
  void createLinks();
  void createObjects(Simulation&,const std::string&);

  Geometry::Vec3D getDirection(const size_t) const;

 public:

  H2Vac(const std::string&);
  H2Vac(const H2Vac&);
  H2Vac& operator=(const H2Vac&);
  virtual ~H2Vac();

  virtual Geometry::Vec3D getSurfacePoint(const attachSystem::FixedComp&,
					  const size_t,const long int) const;

  void createAll(Simulation&, const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 
