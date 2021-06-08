/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: LinacInc/InjectionHall.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
  \author S. Ansell / K. Batkov
  \date February 2020
  \brief TDC/SPF building (byggnader B och C)
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
  double spfAngleStep;           ///< x-offset of the wall after spfAngle

  double spfMazeWidthTDC;       ///< SPF access maze width from the TDC side
  double spfMazeWidthSide;      ///< SPF access maze side width
  double spfMazeWidthSPF;       ///< SPF access maze width from the SPF hallway
  double spfMazeLength;         ///< SPF access maze full length (along the x-axis)

  double fkgDoorWidth;          ///< Future klystron gallery access door width
  double fkgDoorHeight;         ///< Future klystron gallery access door height
  double fkgMazeWidth;          ///< Future klystron gallery maze width
  double fkgMazeLength;         ///< Future klystron gallery maze length
  double fkgMazeWallThick;      ///< Future klystron gallery maze wall thickness

  double btgThick;          ///< PREFAB BTG-BLOCK thick
  double btgHeight;         ///< BTG-BLOCK height
  double btgLength;         ///< BTG-BLOCK length
  double btgYOffset; ///< BTG-BLOCK Y+ offset (relative to the back wall front concrete surface 21)
  int btgMat;                   ///< BTG-BLOCK material
  size_t btgNLayers;               ///< BTG-BLOCK number of imp layers

  double spfParkingFrontWallLength; ///< Wall thickness between C080011 and C080012
  double spfParkingLength;      ///< Length of SPF concrete door parking space (C080012)
  double spfParkingWidth;       ///< Width  of SPF concrete door parking space (C080012)

  double spfExitLength;         ///< Length of the SPF emergency exit room
  double spfExitDoorLength;     ///< Length of the SPF emergency exit door
  double spfExitDoorHeight;     ///< Height of the SPF emergency exit door

  double femtoMAXWallThick;      ///< SPF/FemtoMAX wall thickness
  double femtoMAXWallOffset;      ///< X-offset of the SPF/FemtoMAX wall

  double bsp01WallOffset;       ///< BSP01 wall offset
  double bsp01WallLength;       ///< BSP01 wall length (before maze starts)
  double bspWallThick;        ///< Thickness of wall between FemtoMAX (BSP02) and BSP01 beamline areas
  double bspMazeWidth;          ///< Maze width (passage) in the BSP beamline areas
  double bspFrontMazeThick;     ///< Thick of the BSP front maze walls
  double bspFrontMazeIronThick;    ///< Thick of the iron layer in the end of the BSP0[12] rooms
  double bspMidMazeIronThick1;   ///< Thick of the first iron layer in the BSP maze
  double bspMidMazeIronThick2;   ///< Thick of the second iron layer in the BSP maze
  double bspMidMazeThick;       ///< Thick of the BSP middle maze walls
  double bspMidMazeDoorHeight;  ///< Height of the BSP middle maze doors
  double bspBackMazeThick;      ///< Thick of the BSP back maze walls

  double rightWallStep;          ///< Extra out step on right

  double linearWidth;            ///< Wall - Wall width

  double floorDepth;             ///< Depth (floor to under roof)
  double roofHeight;             ///< Height (floor to under roof)

  double wallThick;             ///< Wall thickness
  double roofThick;             ///< roof thickness
  double floorThick;            ///< floor thickness

  double midTXStep;             ///< Step to centre of T
  double midTYStep;             ///< Step to centre of T
  double midTThick;             ///< T-shape wall thickness (short segment perp.to Y + inclined segments)
  double midTThickX;            ///< T-shape wall thickness (long segment perp.to X axis)
  double midTAngle;             ///< angle of offset
  double midTLeft;              ///< left flat
  double midTRight;             ///< right flat
  double midTFrontAngleStep;    ///< angle at front extent from mid line
  double midTBackAngleStep;     ///< angle at back extent from mid line
  size_t midTNLayers;           ///< Number of layers in the MidT wall [for cell-based biasing]

  double midTFrontLShieldThick; ///< Local shielding thickness of MidTFrontWall
  double midTFrontLShieldHeight; ///< MidTFrontWall local shielding height
  double midTFrontLShieldWidth; ///< MidTFrontWall local shielding width
  int midTFrontLShieldMat;      ///< MidTFrontWall local shielding material

  size_t midTNDucts;            ///< Number of ducts in the MidT wall
  std::vector<double> midTDuctRadius;  ///< MidT duct radii
  std::vector<double> midTDuctYStep;   ///< MidT duct y steps
  std::vector<double> midTDuctZStep;   ///< MidT duct z steps
  std::vector<int> midTDuctMat;        ///< MidT duct material

  double klysDivThick;      ///< Thickness of klystrong divder

  double midGateOut;            ///< Size outwards from wall
  double midGateWidth;          ///< Gate width
  double midGateWall;           ///< Gate wall

  double backWallYStep;         ///< Y offset of the back wall
  double backWallThick;         ///< Back wall thickness
  double backWallIronThick;     ///< Thickness of iron layer before the back wall
  int backWallMat;              ///< Back wall material
  size_t backWallNLayers;       ///< Number of layers in the back wall

  double klystronXStep;         ///< Step of inner klystron wall
  double klystronLen;           ///< Length of klystron wall
  double klystronFrontWall;      ///< Thick of front kylstron wall
  double klystronSideWall;      ///< Thickness of klystron divder

  double boundaryWidth;           ///< Width after walls
  //  double boundaryHeight;          ///< Height after roof

  double bdRoomHeight;          ///< Under-the-floor beam dump room void height
  double bdRoomLength;          ///< Beam dump room void length
  double bdRoomFloorThick;      ///< Beam dump room floor thickness
  double bdRoomRoofThick;       ///< Beam dump room steel roof thickness
  double bdRoomRoofSteelWidth;  ///< Width of roof steel layers
  double bdRoomFrontWallThick;  ///< Beam dump room front wall thickness
  double bdRoomSideWallThick;   ///< Thickness of the beam dump room side walls
  double bdRoomBackSteelThick;  ///< Beam dump room back steel layer thickness
  double bdRoomHatchLength;     ///< Length of penetration in the floor to the beam dump room
  double bdRoomXStep;           ///< Beam dump room x-offset
  double bdRoomInnerWallThick;  ///< Thickness of inner walls
  double bdRoomInnerWallLength; ///< Length of inner walls
  double bdRoomTDCWidth;        ///< Width of area with the TDC beam dump
  double bdRoomSPFWidth;        ///< Width of area with the SPF beam dump
  double bdRoomNewWidth;        ///< Width of area with the NEW beam line beam dump

  double wasteRoomLength;       ///< Inner length of the radioactive waste room
  double wasteRoomWidth;        ///< Inner width of the radioactive waste room
  double wasteRoomWallThick;    ///< WallThick of the radioactive waste room
  double wasteRoomYStep;        ///< Offset to Origin

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
  
  int voidMat;                  ///< Void material
  int wallMat;                  ///< Wall material
  int wallIronMat;              ///< Material of the iron layer before the back wall
  int bdRoofIronMat;            ///< Material of iron in the beam dump roof
  int roofMat;                  ///< Roof material
  int floorMat;                 ///< Floor material
  int soilMat;                  ///< Earth material

  
  void createFloor(Simulation&);
  void layerProcess(Simulation&,const std::string&,
                    const int,const int,const size_t);


  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  InjectionHall(const std::string&);
  InjectionHall(const InjectionHall&);
  InjectionHall& operator=(const InjectionHall&);
  virtual ~InjectionHall();

  size_t getBackWallNLayers() const {return backWallNLayers; }
  bool addPillars(Simulation&,const int) const;

  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
