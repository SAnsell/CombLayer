/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/ConcreteDoor.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef xraySystem_ConcreteDoor_h
#define xraySystem_ConcreteDoor_h

class Simulation;

namespace MAXIV::GunTestFacility
{
  /*!
    \class ConcreteDoor
    \version 1.0
    \author K. Batkov
    \date Sep 2023
    \brief a copy of RingDoor modified for Gun Test Facility
  */

class ConcreteDoor :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap

{
 private:

  double innerHeight;                 ///< height of inner gap
  double innerWidth;                  ///< width of inner gap
  double innerThick;                  ///< thickness of first door

  double innerTopGap;                 ///< top gap inner
  double outerTopGap;                 ///< top gap outer
  double innerSideGapLeft;            ///< left-hand inner side gap
  double innerSideGapRight;           ///< right-hand inner side gap
  double outerSideGapLeft;            ///< left-hand outer side gap
  double outerSideGapRight;           ///< right-hand outer side gap
  double innerThickGap;               ///< inner gap between jamb and outer door layer

  double outerHeight;                 ///< height of outer gap
  double outerWidth;                  ///< width of outer gap

  double underStepHeight;             ///< height of gap
  double underStepWidth;              ///< Width of under gap
  double underStepXStep;        ///< x-offset of gap
  double innerSideAngle;          ///< inner side wall cut angle
  double outerSideAngle;        ///< outer side wall cut angle
  double innerXStep;            ///< x-offset of the inner part

  double cornerCut;             ///< corner cut length
  double jambCornerCut;         ///< corner cut length on the doorjamb

  int underMat;                       ///< Filling of under material
  int doorMat;                        ///< wall material

  Geometry::Vec3D getCorner(const int, const int, const int) const;
  void testDistances() const;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ConcreteDoor(const std::string&);
  ConcreteDoor(const ConcreteDoor&) = delete;
  ConcreteDoor& operator=(const ConcreteDoor&) = delete;
  virtual ~ConcreteDoor() {}  ///< Destructor

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
