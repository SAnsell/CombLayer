/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/Hutch.h
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
#ifndef hutchSystem_Hutch_h
#define hutchSystem_Hutch_h

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
namespace attachSystem
{
 class FixedComp;
 class FixedOffset;
 class ContainedComp;
 class CellMap;
}

namespace constructSystem
{
  class Jaws;
}

namespace hutchSystem
{

  class ChipSample;
  class InnerWall;
  class Table;
  class PreCollimator;
  class ColBox;
  class BeamStop;

/*!
  \class ChipIRHutch
  \version 1.0
  \author S. Ansell
  \date September 2009
  \brief Adds the ChipIR Hutch
*/

class ChipIRHutch : public attachSystem::FixedGroup,
   public attachSystem::ContainedComp,public attachSystem::CellMap
{
 private:
  
  const int hutchIndex;         ///< Index of surface offset
  int cellIndex;                ///< Index of cells
  int populated;                ///< populated or not

  std::shared_ptr<PreCollimator> PreColObj;      ///< PreCollimator
  std::shared_ptr<constructSystem::Jaws> Jaw;   ///< Vertial collimator
  std::shared_ptr<ColBox> ColB;                  ///< Collimator box
  std::shared_ptr<InnerWall> Trimmer;            ///< Inner Trimmer
  std::shared_ptr<Table> FTable;                 ///< Front table
  std::shared_ptr<Table> BTable;                 ///< Back table
  std::shared_ptr<BeamStop> BStop;               ///< BeamStop object
  /// Samples [if added]
  std::vector<std::shared_ptr<ChipSample>> SampleItems; 

  Geometry::Vec3D BeamCentPoint;  ///< Centre point for origin
  Geometry::Vec3D ImpactPoint;    ///< Impact point [effective origin]

  double xStep;         ///< Xstep of outer hutch
  double yStep;         ///< Ystep of outer hutch
  double zLine;         ///< Actual Z Point of outer hutch

  double beamAngle;     ///< Angle of the beam
  double screenY;       ///< Screen material thickness
  double hGuideWidth;   ///< Guide/Nose width (full)
  double hFLWidth;      ///< Front outer left width
  double hFRWidth;      ///< Front outer right width
  double hFullLWidth;   ///< Full width at wide point (left)
  double hPartRWidth;   ///< Part width at first step (right)
  double hFullRWidth;   ///< Full width at wide point (right)
  double hMainLen;      ///< Main length
  double hLSlopeLen;    ///< Slope length on the left side
  double hRSlopeLen;    ///< Slope length on the right side
  double hPartLen;      ///< Partial step length
  double hExtLen;       ///< Extended length
  double hRoof;         ///< Height of the roof [relative to target centre]
  double hFloor;        ///< Floor to Centre distance
  double hSurfFloor;    ///< Floor Al layer 
  double hVoidFloor;    ///< Floor Void layer

  double hRoofThick;    ///< Thickness  of the roof 
  double hFloorDepth;   ///< Floor to thickness
  double hRWallThick;   ///< Right-wall steel thickness
  double hLWallThick;   ///< Left-wall steel thickness
  double hFWallThick;   ///< Front-wall steel thickness
  double hBWallThick;   ///< Back-wall steel thickness

  double hEdgeVoid;     ///< Void out of the edge of the left wall

  // Wall
  double wDoor;            ///< Thickness of the door
  double wInWall;          ///< Step to the edge of the void corridor
  double wOutWall;         ///< Step to the outer wall line
  double wBlockExtent;     ///< Front protection block Y step into hutch
  double wBlockWidth;      ///< Front protection block X step into hutch

  double mBlockYBeg;       ///< Start of crossing block
  double mBlockYEnd;       ///< End of crossing block
  double mBlockOut;        ///< Step out into room
  double mBlockSndLen;     ///< Length of the second block [infront o beam stop]

  // Beamstop [VOID : Opening]
  double bsXStep;          ///< Step Across of hutch 
  double bsZStep;          ///< Step Up of hutch 
  double bsWidth;          ///< Width of beamstop open [x coord] 
  double bsHeight;         ///< Height of beamstop open [z coord] 

  // Feed Block [At the door]
  double fbLength;         ///< feed block Length

  int roofMat;          ///< Material for roof
  int floorMat;         ///< Material for floor
  int falseFloorMat;    ///< Material for false floor
  int wallMat;          ///< Material for walls
  int innerWallMat;     ///< Inner wall material 
  int rearVoidMat;      ///< Material surrounding beamstop
  int screenMat;        ///< Screen material
  int fbMat;            ///< Feed block material

  int collimatorVoid;      ///< Cell number of collimator void
  int tailVoid;            ///< Tail void

  double westExtraThick;    ///< Extra wall thick
  double westExtraLength;   ///< Extra wall length from hutch front [out]
  
  
  Geometry::Vec3D beamStopCent;  ///< centroid of the beamspot
  Geometry::Vec3D serverCent;    ///< centroid of the server room
  // ----------------------------------------------------
  // SURFACE DIVIDERS:
  // --------------------------------------------1--------

  ///< Cells layering
  std::map<std::string,int> layerCells;  

  size_t nLWallDivide;      ///< Left Wall divider
  size_t nRWallDivide;      ///< Right Wall divider
  size_t nRoofDivide;       ///< Roof divider
  size_t nFloorDivide;      ///< Floor divider
  size_t nTrimDivide;       ///< Trim divider
  size_t nWestDivide;       ///< West extra

  std::vector<double> lWallFrac; ///< guide Wall thickness (fractions)
  std::vector<int> lWallMatList; ///< guide Wall materials
  std::vector<double> rWallFrac; ///< guide Wall thickness (fractions)
  std::vector<int> rWallMatList; ///< guide Wall materials
  std::vector<double> roofFrac;  ///< guide Roof thickness (fractions)
  std::vector<int> roofMatList;  ///< guide Roof materials
  std::vector<double> floorFrac; ///< guide Floor thickness (fractions)
  std::vector<int> floorMatList; ///< guide Floor materials
  std::vector<double> westFrac;  ///< guide Floor thickness (fractions)
  std::vector<int> westMatList;  ///< West extra Floor materials
  std::vector<double> trimFrac;  ///< guide Trim thickness (fractions)
  std::vector<int> trimMatList;  ///< guide Trim materials

  int collActiveFlag;   ///< Collimator active flag

  void layerProcess(Simulation&);
  void procSurfDivide(Simulation&,ModelSupport::surfDivide&,
		      const size_t,const std::vector<std::pair<int,int> >&,
		      const std::string&,const std::string&);

  // ----------------------------------------------------------

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const Geometry::Vec3D&,
			const attachSystem::FixedComp&);

  void createWallSurfaces(const attachSystem::FixedComp&);

  void createWallObjects(Simulation&,const attachSystem::ContainedComp&);

  void addCollimators(Simulation&,const attachSystem::TwinComp&);
  void addOuterVoid();
  void addExtraWalls(Simulation&,const attachSystem::FixedComp&);
  Geometry::Vec3D calcCentroid(const int,const int,const int,
			       const int,const int,const int) const;
  
  void createFeedBlock(Simulation&);
  void createLinks();

 public:

  ChipIRHutch(const std::string&);
  ChipIRHutch(const ChipIRHutch&);
  ChipIRHutch& operator=(const ChipIRHutch&);
  ~ChipIRHutch();

  int isObjectContained(Simulation&,const int,const int);

  /// Set the collimator build flag [Bits ==> Pre:V:H ]
  void setCollFlag(const int F) { collActiveFlag=F; }
  
  Geometry::Vec3D calcIndexPosition(const int) const;
  /// Access central point:
  //  const Geometry::Vec3D& getCentPoint() const { return CentPoint; }
  /// Access central point:
  const Geometry::Vec3D& getImpactPoint() const { return ImpactPoint; }
  /// Access beam angle
  double getBeamAngle() const { return beamAngle; }

  /// Calc origin of collimator
  Geometry::Vec3D getCollOrigin() const {  return Origin; }
  /// Get the collimator cell
  int getCollCell() const { return collimatorVoid; }
  /// Get the Beamstop centroid
  const Geometry::Vec3D& getBeamStopCentre() const { return beamStopCent; }
  /// Get the server centre point
  const Geometry::Vec3D& getServerCentre() const { return serverCent; }

  Geometry::Vec3D calcSideIntercept(const int,const Geometry::Vec3D&,
				   const Geometry::Vec3D&) const;
  Geometry::Vec3D calcSurfNormal(const int) const;

  Geometry::Vec3D calcDoorPoint() const;

  int exitWindow(const double,std::vector<int>&,Geometry::Vec3D&) const;
  void writeMasterPoints() const;

  void createAll(Simulation&,const shutterSystem::GeneralShutter&,
		 const attachSystem::TwinComp&,		 
		 const attachSystem::ContainedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::TwinComp&,		 
		 const attachSystem::ContainedComp&);

  void createCommonAll(Simulation&,
		 const attachSystem::TwinComp&,		 
		 const attachSystem::ContainedComp&);

};

}

#endif
 
