/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: LinacInc/InjectionHall.h
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
#ifndef tdcSystem_InjectionHall_h
#define tdcSystem_InjectionHall_h

class Simulation;

namespace tdcSystem
{

/*!
  \class InjectionHall
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Main 1.5GeV ring building and inner void
*/

class InjectionHall :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double mainLength;             ///< total length

  double linearRCutLength;       ///< Length to right out step
  double linearLTurnLength;      ///< Length to left angle out (inner)
  double spfAngleLength;         ///< Length to left off angle
  double spfMidLength;           ///< Length to  mid of angle [calculted]
  double spfLongLength;          ///< Extra length in spf (for InnerZone)
  double spfAngle;               ///< SPF hall diagonal wall

  double rightWallStep;          ///< Extra out step on right

  double linearWidth;            ///< Wall - Wall width

  double floorDepth;             ///< Depth (floor to under roof)
  double roofHeight;             ///< Height (floor to under roof)

  double wallThick;             ///< Wall thickness
  double roofThick;             ///< roof thickness
  double floorThick;            ///< floor thickness

  double midTXStep;             ///< Step to centre of T
  double midTYStep;             ///< Step to centre of T
  double midTThick;             ///< Thickness
  double midTAngle;             ///< angle of offset
  double midTLeft;              ///< left flat
  double midTRight;             ///< right flat
  double midTFrontAngleStep;    ///< angle at front extent from mid line
  double midTBackAngleStep;     ///< angle at back extent from mid line

  double klysDivThick;      ///< Thickness of klystrong divder

  double midGateOut;            ///< Size outwards from wall
  double midGateWidth;          ///< Gate width
  double midGateWall;           ///< Gate wall

  double backWallYStep;         ///< Y offset of the back wall
  double backWallThick;         ///< Back wall thickness
  int backWallMat;              ///< Back wall material

  double klystronXStep;         ///< Step of inner klystron wall
  double klystronLen;           ///< Length of klystron wall
  double klystronFrontWall;      ///< Thick of front kylstron wall
  double klystronSideWall;      ///< Thickness of klystron divder

  double boundaryWidth;           ///< Width after walls
  double boundaryHeight;          ///< Height after roof

  size_t nPillars;              ///< Number of pillars
  std::vector<double> pRadii;   ///< Pillar radii
  std::vector<int> pMat;        ///< Pillar materials
  std::vector<Geometry::Vec3D> pXY; ///< Pillar coordinates (with respect to building origin)

  double thzWidth;              ///< Width of THz penetration
  double thzHeight;             ///< Height of THz penetration
  double thzXStep;              ///< THz penetration X offset with respect to FMidPt
  double thzZStep;              ///< THz penetration X offset with respect to FMidPt
  double thzZAngle;             ///< THz penetration Z angle
  int thzMat;                   ///< THz penetration material

  int voidMat;               ///< void material
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material

  void createFloor(Simulation&);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  InjectionHall(const std::string&);
  InjectionHall(const InjectionHall&);
  InjectionHall& operator=(const InjectionHall&);
  virtual ~InjectionHall();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
