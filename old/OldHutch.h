/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/OldHutch.h
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
#ifndef Hutch_h
#define Hutch_h

class Simulation;

/*!
  \namespace HutchSystem
  \brief Add hutches to the model
  \author S. Ansell
  \version 1.0
  \date September 2009
*/

namespace shutterSystem
{
  class GeneralShutter;
}
namespace zoomSystem
{
 class LinearComp;
}

namespace hutchSystem
{

void createWallsChipIR(Simulation&,const shutterSystem::GeneralShutter&);

/*!
  \class ChipIRHutch
  \version 1.0
  \author S. Ansell
  \date September 2009
  \brief Adds the ChipIR Hutch
*/

class ChipIRHutch : public zoomSystem::LinearComp
{
 private:
  
  const int hutchIndex;         ///< Index of surface offset
  const std::string keyName;    ///< Key name
  int cellIndex;                ///< Index of cells
  int populated;                ///< populated or not

  Geometry::Vec3D CentPoint;    ///< Centre point for origin
  Geometry::Vec3D ImpactPoint;  ///< Impact point [effective origin]
  Geometry::Vec3D BeamAxis;     ///< Centre beam line
  Geometry::Vec3D entryY;       ///< Entry vector

  double beamAngle;     ///< Angle of the beam
  double screenY;       ///< Screen material
  double hYLen;         ///< Length of main hutch
  double hYFlat;        ///< Length of flat wall section
  double hYNose;        ///< Length of nose cone
  double hGuideWidth;   ///< Guide/Nose width (full)
  double hFLWidth;      ///< Front outer left width
  double hFRWidth;      ///< Front outer right width
  double hFullLWidth;   ///< Full width at wide point (left)
  double hFullRWidth;   ///< Full width at wide point (right)
  double hYSlopeDist;   ///< Distance that stop extends before straight
  double hRXStep;       ///< Step down from roof (at front)
  double hRoof;         ///< Height of the roof [relative to target centre]
  double hCollRoof;     ///< Height of collimator ceiling
  double hFloor;        ///< Floor to target distance
  double hFloorDepth;   ///< Floor to target distance
  double hHangSteel;    ///< Width of hanging steel
  double hHangDepth;    ///< Depth of front hanging steel
  double hHangLift;     ///< Up height of front hanging steel
  double hRoofSteel;    ///< Roof-wall steel thickness
  double hRWallSteel;   ///< Right-wall steel thickness
  double hLWallSteel;   ///< Left-wall steel thickness
  double hBWallSteel;   ///< Back-wall steel thickness
  
  double iFrontTrimLen;   ///< Length of first trim piece
  double iFrontTrimWidth; ///< Width of first trim piece
  double iFrontTrimHeight; ///< Height of first trim piece
  double iFrontTrimDepth;  ///< Thickness of first trim piece

  // Tables:

  double t1Height;      ///< height realtive to beam
  double t1Width;       ///< width of table
  double t1Len;         ///< full-length
  double t1MidY;        ///< Mid point of table

  double t2Height;      ///< height realtive to beam
  double t2Width;       ///< width of table
  double t2Len;         ///< full-length of table
  double t2MidY;        ///< Mid point of table

  // BeamStop (walkthrough):
  double wYDist;       ///< Total walkthough distance
  double wXWrap;       ///< Corner length
  double wWallThick ;  ///< Wall thickness
  double wXSnd;        ///< Length to pod start

  double bsYFullLen;   ///< beam stop full length
  double bsYLen;       ///< beam stop  length
  double bsYMidThick;  ///< beam stop actual length
  double bsYBackWall;  ///< beam stop back wall thickness
  double bsYStart;     ///< beam stop Start point
  double bsXWidth;     ///< beam stop full width
  double bsXSideWall;  ///< beam stop Side wall thickness
  double bsRoof;       ///< roof height
  double bsRoofThick;  ///< roof thickness
  double bsBase;       ///< Thickness of base

  int roofMat;          ///< Material for roof
  int floorMat;          ///< Material for roof
  int trimMat;          ///< Material for trimmer
  int wallMat;          ///< Material for walls
  int tableMat;         ///< Wall material
  int bsWallMat;        ///< BeamStop wall material
  int bsMat;            ///< BeamStop material
  int screenMat;        ///< Screen material

  int collimatorVoid;   ///< Cell number of collimator void
  int beamStopCell;     ///< Cell number of beamstop
  Geometry::Vec3D beamStopCent; ///< centroid of the beamspot
  Geometry::Vec3D serverCent; ///< centroid of the server room
  // ----------------------------------------------------
  // SURFACE DIVIDERS:
  // ----------------------------------------------------

  int nWallDivide;       ///< Wall divider
  int nRoofDivide;       ///< Roof divider
  int nFloorDivide;      ///< Floor divider

  std::vector<double> wallFrac;  ///< guide Wall thickness (fractions)
  std::vector<int> wallMatList;  ///< guide Wall materials
  std::vector<double> roofFrac;  ///< guide Roof thickness (fractions)
  std::vector<int> roofMatList;  ///< guide Roof materials
  std::vector<double> floorFrac; ///< guide Floor thickness (fractions)
  std::vector<int> floorMatList; ///< guide Floor materials

  int leftWallCell;     ///< Cell to divide from the wall
  int rightWallCell;    ///< Cell to divide from the wall
  int backWallCell;     ///< Cell to divide from the wall
  int roofCell;         ///< Cell to divide from the roof
  void layerProcess(Simulation&);

  // ----------------------------------------------------------

  void populate(const Simulation&);
  void createUnitVector(const shutterSystem::GeneralShutter&,
			const zoomSystem::LinearComp&);

  void createWallSurfaces(const zoomSystem::LinearComp&);
  void createBeamStopSurfaces();

  void createWallObjects(Simulation&);
  void createBeamStopObjects(Simulation&);

  void addOuterVoid(Simulation&);
  Geometry::Vec3D calcCentroid(const int,const int,const int,
			       const int,const int,const int) const;
  

 public:

  ChipIRHutch(const int,const std::string&);
  ChipIRHutch(const ChipIRHutch&);
  ChipIRHutch& operator=(const ChipIRHutch&);
  ~ChipIRHutch();

  void createAll(Simulation&,const shutterSystem::GeneralShutter&,
		 const zoomSystem::LinearComp&);

  int isObjectContained(Simulation&,const int,const int);
  
  Geometry::Vec3D calcTable(const int) const;
  /// Access central point:
  const Geometry::Vec3D& getCentPoint() const { return CentPoint; }
  /// Access central point:
  const Geometry::Vec3D& getImpactPoint() const { return ImpactPoint; }
  /// Access central axis:
  const Geometry::Vec3D& getBeamAxis() const { return BeamAxis; }
  /// Access beam angle
  double getBeamAngle() const { return beamAngle; }

  /// Calc origin of collimator
  Geometry::Vec3D getCollOrigin() const
    {  return CentPoint+Y*hHangSteel; }
  /// Get the collimator cell
  int getCollCell() const { return collimatorVoid; }
  /// Get the Beamstop cell
  int getBeamStopCell() const { return beamStopCell; } 
  /// Get the Beamstop centroid
  const Geometry::Vec3D& getBeamStopCentre() const { return beamStopCent; }
  /// Get the server centre point
  const Geometry::Vec3D& getServerCentre() const { return serverCent; }

  int exitWindow(const double,std::vector<int>&,Geometry::Vec3D&) const;
  void writeMasterPoints() const;
};

}

#endif
 
