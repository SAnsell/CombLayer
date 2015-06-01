/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/ChipIRGuide.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef hutchSystem_ChipIRGuide_h
#define hutchSystem_ChipIRGuide_h

class Simulation;

namespace ModelSupport
{
  class surfDivide;
}

namespace shutterSystem
{
  class GeneralShutter;
  class BulkShield;
}

namespace constructSystem
{
  class WallCut;
}

namespace hutchSystem
{


/*!
  \class ChipIRGuide
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Adds the ChipIR Guide
  
  The guide is defined along a line that leads from the centre line
  of the shutter. The origin is the point that the shutter centre 
  line impacts the 40000 outer surface
*/

class ChipIRGuide : public attachSystem::TwinComp,
    public attachSystem::ContainedGroup
{
 private:

  /// layer type
  typedef std::map<std::string,int> LCTYPE;

  const int guideIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  ChipIRFilter Filter;          ///< Filter component
  
  double beamAngle;             ///< Angle of the beam
  double sideBeamAngle;         ///< SideAngle of the beam
  double shutterAngle;          ///< Angle of the beam
  double gLen;                  ///< Guide length
  double hYStart;               ///< Distance to the start of the hutch
  double hFWallThick;           ///< Thickness hut front wall
 
  double xShift;                ///< Global xshift
  double zShift;                ///< Global zshift

  double xBeamShift;            ///< Inner xshift
  double zBeamShift;            ///< Inner zshift

  double innerARoofZ;          ///< Top roof height
  double innerAFloorZ;         ///< Base (floor)
  double innerBRoofZ;          ///< Top roof height
  double innerBFloorZ;         ///< Base (floor)
  double innerALWall;          ///< Left inner wall
  double innerARWall;          ///< Right inner wall
  double innerBLWall;          ///< Left inner wall
  double innerBRWall;          ///< Right inner wall

  // Liner:
  std::vector<double> LThick;     ///< Liner Thickness
  std::vector<int> LMat;          ///< Liner Material

  // Steel inner:
  double roofSteel;              ///< Steel in roof
  double floorSteel;             ///< Steel layer on floor
  double leftSteelInner;         ///< Left steel [close]
  double leftSteelFlat;          ///< Left steel [long]
  double rightSteelInner;        ///< Right steel [close]
  double rightSteelFlat;         ///< Right steel [long]
  double leftSteelAngle;         ///< Left steel angle from beamcentre
  double rightSteelAngle;        ///< Right steel angle from beamcentre

  // Concrete inner:
  double roofConc;
  double floorConc;
  double leftConcInner;
  double rightConcInner;
  double rightConcFlat; 
  double leftConcAngle;
  double rightConcAngle;
 
  double blockWallThick;       ///< Wall thickness
  double blockWallHeight;      ///< Height [from ground]
  double blockWallLen;       ///< Wall length

  // extra wall on TSA side (after block wall):
  double extraWallThick;         ///< Wall thickness
  double extraWallHeight;        ///< Height [from ground]
  double extraWallLen;           ///< Wall length
  double extraWallSideAngle;     ///<angle of side wall
  double extraWallEndAngle;      ///<angle of end wall  
//extra wall on RHS [W2 side]
  double rightWallThick;       ///< Wall thickness
  double rightWallHeight;      ///< Height [from ground]
  double rightWallLen;         ///< Wall length

  int remedialVoid;             ///< Flag to build
  double remedialWestWallThick;    ///< Remedial wall
  double remedialWallHeight;       ///< Remedial wall height
  
  size_t nCuts;                  ///< Number of wall cuts [inner]
  /// Wall cut objects
  std::vector<std::shared_ptr<constructSystem::WallCut> > WCObj; 
  
  size_t nLayers;                ///< number of layers
  std::vector<double> guideFrac; ///< guide Layer thicknesss (fractions)
  std::vector<int> guideMat;     ///< guide Layer materials

  size_t nRemedialWestLayers;            ///< number of layers
  std::vector<double> remedialWestFrac; ///< west Layer thicknesss (fractions)
  std::vector<int> remedialWestMat;     ///< west Layer materials


  size_t nConcLayers;              ///< number of layers
  std::vector<double> concFrac; ///< concrete Layer thicknesss (fractions)
  std::vector<int> concLayMat;     ///< Conctrete Layer materials

  int steelMat;           ///< Material for steel
  int concMat;           ///< Material for steel
  int wallMat;           ///< Material for steel

  int monoWallSurf;              ///< Montolith Exit surface
  std::vector<int> voidCells;    ///< Liners/Steel etc
  LCTYPE layerCells;   ///< Layered cells

  void populate(const FuncDataBase&);
  void createUnitVector(const shutterSystem::BulkShield&,
			const shutterSystem::GeneralShutter&);
  void createUnitVector(const attachSystem::FixedComp&,
			const double);
  
  void createSurfaces(const shutterSystem::GeneralShutter&);
  void createSurfaces();
  void createSurfacesCommon();
  void createObjects(Simulation&);
  void createLinks();
  void addInsertPlate(Simulation&);
  void addFilter(Simulation&);
  void layerProcess(Simulation&);
  void createLiner(const int,const double);
  void createCommon(Simulation&);
  
  void populateWallItems(const FuncDataBase&);
  
  void procSurfDivide(Simulation&,
		      ModelSupport::surfDivide&,const size_t, 
		      const std::vector<std::pair<int,int> >&,
		      const std::string&,
		      const std::string&);

  void addWallCuts(Simulation&);

 public:

  ChipIRGuide(const std::string&);
  ChipIRGuide(const ChipIRGuide&);
  ChipIRGuide& operator=(const ChipIRGuide&);
  ~ChipIRGuide();

  /// Set surface
  void setMonoSurface(const int M) { monoWallSurf=M; }

  /// Access beam angle
  double getBeamAngle() const { return beamAngle; }

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  int calcTallyPlanes(const int,std::vector<int>&) const;
  void writeMasterPoints();

  void createAll(Simulation&,const shutterSystem::BulkShield&,
		 const size_t);
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
