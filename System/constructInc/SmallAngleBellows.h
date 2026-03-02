/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/SmallAngleBellows.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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
#ifndef constructSystem_SmallAngleBellows_h
#define constructSystem_SmallAngleBellows_h

class Simulation;

namespace constructSystem
{

/*!
  \class SmallAngleBellows
  \version 0.1
  \author U. Friman-Gayer
  \date February 2026
  \brief 
*/

class SmallAngleBellows:
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{

 private:
  double angle;
  double angleDeg;
  double bellowsMaterialThick;
  double bellowStep;
  double bellowThick;
  double bellowsVolumeFraction;
  double flangeLength;
  double flangeRadius;
  double length;
  double pipeInnerRadius;
  double pipeWallThick;

  int bellowBaseMat;
  int nFolds;
  int nSectors;
  int pipeMat;

  std::vector<int> bellowMat;
  std::vector<double> bellowsThick;

  Geometry::Vec3D Yp;

  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  double bellowLength() const;
  double bellowsMaterialVolume() const;
  double bellowsThickness(const double volume, const double length) const;
  void createSectors();
  std::pair<int,int> cylindricOuterSurf() const;
  double sectorAngle(const int nSector,const bool centerAngle) const;
  double sectorLength(const int nSector) const;
  int sectorPlaneID(const int nSector, const int base, const int offset) const;

 public:

  SmallAngleBellows(const std::string&);
  SmallAngleBellows(const SmallAngleBellows&);
  SmallAngleBellows& operator=(const SmallAngleBellows&);
  ~SmallAngleBellows();


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};
}

#endif
