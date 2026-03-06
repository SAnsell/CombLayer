/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/SmallAngleBellows.h
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
#ifndef xraySystem_SmallAngleBellows_h
#define xraySystem_SmallAngleBellows_h

class Simulation;

namespace xraySystem
{

/*!
  \class SmallAngleBellows
  \version 1.0
  \author U. Friman-Gayer
  \date March 2026
  \brief Fixed-length cylindric bellows that bend at a small angle

  This class approximates the geometry of cylindric bellows with a small angle between 
  the front- and back-surface normal.
  By default, the initial main axis of the bellows is along the Y axis, and positive 
  angles bend the bellows towards negative X values.
  Both at the front and at the back, the bellows have regular cylindric-pipe adapters.
  In reality, the shape of bent bellows is complex. Here, it is approximated by an 
  instantaneous step at the bellows' center, as shown below.
       _________
      /         |
     /    ______|
    /    /
   /    /
  /____/

  By adopting a simple stepwise model for the shape of the bellows' folds, this class 
  models the radius- and effective-density variations in the bellows when they are 
  bent.
  The bellows' folds' shape is assumed to be a zero-order stepwise function as shown 
  below (like simple merlons on a castle wall, for example).
       ________      ________      ________
      |  ____  |    |  ____  |    |  ____  |
      | |    | |    | |    | |    | |    | |
      | |    | |    | |    | |    | |    | |
  ____| |    | |____| |    | |____| |    | |
  ______|    |________|    |________|    |__

  This shape is not modeled, however, but approximated as a material with an effective
  density.
  With the assumption that the thickness of the folds and the total amount of material
  is conserved, the change in radius and effective density as the bellows are bent can
  be calculated with simple, analytical expressions.

  This class takes into account that bending breaks axial symmetry by dividing the 
  bellows into a user-defined amount of sectors with constant radius and effective 
  density.

  The aforementioned assumptions are only a good approximation at small (few degrees) 
  angles where ..

  - ... the path through the bellows is not obstructed too much by the unrealistic 
        corners.
  - ... the bending is not so strong that the folds interfere with each other.
  - ... the difference in radius and effective density between the sectors is not too 
        large.

  As a rule of thumb, this class should only be used for bellows in which there is a 
  line of sight between the front and the back.
  However, bellows that bend at large angles could be approximated by a combination 
  of small-angle bellows.

  Version history:
  1.0 - 2026-03-05
*/

class SmallAngleBellows:
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{

 private:
  // Angle between the front- and back-surface normals in radians.
  double angle;
  // Angle between the front- and back-surface normals in degrees.
  double angleDeg;
  // Thickness of the sheets that are folded to create the bellows.
  double bellowsMaterialThick;
  // Length of the piece of pipe between the front/back flanges and the bellows.
  double bellowsStep;
  // Nominal difference between inner and outer radius in the bellows section.
  double bellowsThick;
  // Length of the front/back flange.
  double flangeLength;
  // Radius of the front/back flange.
  double flangeRadius;
  // Total length of the bellows. Note that the total length is conserved when the 
  // bellows are bent.
  double length;
  // Radius of the inner volume in all parts of the bellows.
  double pipeInnerRadius;
  // Wall thickness of the front/back pipe.
  double pipeWallThick;

  // Base material of the bellows. Will be mixed with void material to create an
  // effective density.
  int bellowsBaseMat;
  // Number of folds of the bellows. The actual folds will not be modeled in the 
  // geometry, but this value is needed to estimate the effective density and radius.
  int nFolds;
  // Number of sectors for approximation of the effective-density and radius variations
  // of the bellows.
  // For nSectors = 1, the nominal bellows thickness is used everywhere.
  // The first sector is always centered around the radial direction where the bending 
  // is maximum. To model the minimum bending accurately, it is recommended to set an 
  // even number for nSectors.
  int nSectors;
  // Material of the front/back pipes and flanges.
  int pipeMat;

  // List of material with effective density for each sector.
  std::vector<int> bellowsMatPerSector;
  // List of bellows thickness for each sector.
  std::vector<double> bellowsThickPerSector;

  // Y' axis after bend.
  Geometry::Vec3D Yp;

  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  double bellowsLength() const;
  double bellowsMaterialVolume() const;
  double bellowsThickness(const double volume, const double length) const;
  void checkInput() const;
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
