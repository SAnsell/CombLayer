/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BIFROST.h
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
#ifndef essSystem_BIFROST_h
#define essSystem_BIFROST_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace constructSystem
{  
  class Aperture;
  class SimpleBoxChopper;
  class ChopperPit;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class LineShieldPlus;
  class InnerShield;
  class RotaryCollimator;
  class VacuumPipe;
  class VacuumWindow;
  class E02Base;
  class E02PillarCover;
  class D03SupportBlock;
  class D03BaseBlock;
  class D03UpperBlock;
  class D03SteelInsert;
  class D03SteelSpacing;
  class Beamstop;
  class Corner;
  class ConicPipe;
}
namespace shutterSystem
{
  class BlockShutter;
  class GeneralShutter;
}


namespace cryoSystem
{
 class CryoMagnetBase;
 class cryoSample;
}


namespace simpleCaveSystem
{
  class simpleCave;
  //  class CryoMagnetBase;
  //  class cryoSample;
  class caveDetector;
  class radialCollimator;
  class brickShape;
}
namespace essSystem
{
  class CompBInsert;
  class GuideItem;
  class DetectorTank;
  class BifrostHut;
  class BoxShutter;
  /*!
    \class BIFROST
    \version 1.0
    \author S. Ansell
    \date July 2016
    \brief BIFROST beamline constructor for the ESS
  */
  
class BIFROST : public attachSystem::CopiedComp
{
 private:

  const size_t nGuideSection;      ///< Guide section in rect unit
  const size_t nSndSection;        ///< Guide section in snd-rec unit
  const size_t nEllSection;        ///< Guide section in final elliptic unit
  // const size_t nShutterB4CPlate;      /// Number of lead plates in the beam shutter 
  //  const size_t nShutterPlate;      /// Number of lead plates in the beam shutter  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> bifrostAxis;

  /// Elliptic focus in bulkshield [m3]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe in the Light shutter gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide in the light shutter
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Vacuum pipe to PSC after light shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;  
  /// Elliptic guide to the PSC
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;
 
  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppA;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SimpleBoxChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  
  /// Pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBA;
  /// Elliptic guide leaving first chopper
  /// Bender start after first chopper
  std::shared_ptr<beamlineSystem::GuideLine> BenderA;

  /// 10.5m FOC 
  std::shared_ptr<constructSystem::SimpleBoxChopper> ChopperB;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskB;

  /// Pipe from first chopper [6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBB;
  /// Straight guide leaving second
  //  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  //Bender continues
  std::shared_ptr<beamlineSystem::GuideLine> BenderB;
  
  /// Second pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBC;
  /// Straight guide in second length
  //  std::shared_ptr<beamlineSystem::GuideLine> FocusE;
  //Bender continues
  std::shared_ptr<beamlineSystem::GuideLine> BenderC;
  
  /// 20.5m FOC-2
  std::shared_ptr<constructSystem::SimpleBoxChopper> ChopperC;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskC;

  /// Pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBD;
  /// 4m elliptic leaving FOC2
  //  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  std::shared_ptr<beamlineSystem::GuideLine> BenderD;

  std::shared_ptr<constructSystem::VacuumPipe> VPipeBE;
  /// 4m elliptic leaving FOC2
  //  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  std::shared_ptr<beamlineSystem::GuideLine> BenderE;

  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppB;

  /// Bunker insert
  std::shared_ptr<essSystem::CompBInsert> BInsert;
  /// Pipe in bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> TPipeWall;
  std::shared_ptr<beamlineSystem::GuideLine> TPipeWall1;
  std::shared_ptr<beamlineSystem::GuideLine> TPipeWall2;
  std::shared_ptr<beamlineSystem::GuideLine> TPipeWall3;
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall1;
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall2;

  /// Cut in the bunker wall on the outer side
  // std::shared_ptr<constructSystem::Aperture> WallCut;
    std::shared_ptr<beamlineSystem::GuideLine> WallCut;
  std::shared_ptr<constructSystem::D03SupportBlock> D03FirstSupport;  
  std::shared_ptr<constructSystem::D03BaseBlock> D03FirstBase;  
  std::shared_ptr<constructSystem::D03UpperBlock> D03FirstUpper;  
  std::shared_ptr<constructSystem::D03UpperBlock> D03SecondUpper;
  std::shared_ptr<constructSystem::D03SteelSpacing> D03SteelVoid;
  std::shared_ptr<constructSystem::D03SteelInsert> D03SteelFirstA;
  std::shared_ptr<constructSystem::D03SteelInsert> D03B4CFirstA;
  std::shared_ptr<constructSystem::D03SteelInsert> D03SteelFirstB;
  std::shared_ptr<constructSystem::D03SteelInsert> D03LeadFirstA;
  std::shared_ptr<constructSystem::D03SteelInsert> D03SteelFirstC;
  
  /// Gap between lateral shielding and bunker wall
  //  std::shared_ptr<constructSystem::LineShieldPlus> ShieldGap1;
  std::shared_ptr<constructSystem::InnerShield> ShieldGap1;  
  std::shared_ptr<constructSystem::InnerShield> InnerShieldGap1;  
  std::shared_ptr<constructSystem::LineShieldPlus> ShieldGap2;  
  std::shared_ptr<constructSystem::InnerShield> InnerShieldGap2;  
  /// First Shield wall outside bunker
  std::shared_ptr<constructSystem::LineShieldPlus> ShieldA;  
  std::shared_ptr<constructSystem::InnerShield> InnerShieldA;  

  //Expansion for beam shutter
  std::shared_ptr<constructSystem::LineShieldPlus> ShutterFrontWall;  
  std::shared_ptr<constructSystem::LineShieldPlus> ShutterRearWall;  
  std::shared_ptr<constructSystem::LineShieldPlus> ShutterExpansion;  
  std::shared_ptr<constructSystem::InnerShield> ShutterInnerShield;  
  std::shared_ptr<constructSystem::InnerShield> ShutterFrontHorseshoe;  
  std::shared_ptr<constructSystem::InnerShield> ShutterRearHorseshoe;  
  std::shared_ptr<constructSystem::InnerShield> ShutterFrontB4C;  
  std::shared_ptr<constructSystem::InnerShield> ShutterRearB4C;  
 
  
  /// First vacuum pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// First guide leaving bunker
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

  /// First vacuum pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// First guide leaving bunker
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;

  /// Last ellispe vacuum pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Last ellipse section
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;
  //Beam shutter
  //  std::shared_ptr<shutterSystem::GeneralShutter> ShutterA;
 /// Tungsten apperature after gamma focus

  std::shared_ptr<constructSystem::Aperture> ShutterAFrame;

  std::shared_ptr<constructSystem::Aperture> ShutterARails;
  
  std::shared_ptr<essSystem::BoxShutter> ShutterAB4C;
  std::shared_ptr<essSystem::BoxShutter> ShutterALead;
  std::shared_ptr<constructSystem::D03SteelInsert> ShutterAPedestal;

  /*
  /// Last ellispe vacuum pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  /// Last ellipse section
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutD;
  */
  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,8> RecPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,8> RecFocus;

  /// Chopper pit
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACutFront;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACutBack;
  /// First out of bunker chopper 
  std::shared_ptr<constructSystem::SimpleBoxChopper> ChopperOutA;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskOutA;
  
  /// Second shield wall : chopperOutA to Cave
  std::shared_ptr<constructSystem::LineShieldPlus> ShieldB;  
  std::shared_ptr<constructSystem::InnerShield> InnerShieldB;  

  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,10> SndPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,10> SndFocus;

  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,6> EllPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,6> EllFocus;

  /// single disk chopper
  std::shared_ptr<BifrostHut> Cave;
  /// Enterance hole to cave
  std::shared_ptr<constructSystem::HoleShape> CaveCut;

  /// Vacuum pipe in the front of the cave
  //  std::shared_ptr<constructSystem::VacuumPipe> VPipeCave;
  // std::shared_ptr<beamlineSystem::GuideLine> FocusCave;
  /// Sample environment
  //  std::shared_ptr<cryoSystem::CryoMagnetBase> SampleMagnetBase;   ///< Aluminium magnet base
  // std::shared_ptr<cryoSystem::cryoSample> ASample;   ///< Sample holder with sample  /********   Cave variables ********/       

    std::shared_ptr<simpleCaveSystem::simpleCave> ACave;
  std::shared_ptr<simpleCaveSystem::simpleCave> CaveLining;
  //  std::shared_ptr<beamlineSystem::GuideLine> InletCut;
    std::shared_ptr<constructSystem::InnerShield> InletCut;
    std::shared_ptr<constructSystem::InnerShield> InletCut2;
  std::shared_ptr<beamlineSystem::GuideLine> BeamstopCut;
  std::shared_ptr<constructSystem::VacuumPipe> VBeamstopPipe;
  std::shared_ptr<constructSystem::ConicPipe> VGetLostPipe;
  std::shared_ptr<cryoSystem::CryoMagnetBase> AMagnetBase;   ///< Aluminium magnet base
    std::shared_ptr<essSystem::BoxShutter> SampleSheet;
  std::shared_ptr<cryoSystem::cryoSample> ASample;   ///< Sample holder with sample
  std::shared_ptr<simpleCaveSystem::brickShape> BrickShapeIn;
  std::shared_ptr<simpleCaveSystem::brickShape> BrickShapeOut;
  /// Vacuum pipe in the front of the cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCave;
  std::shared_ptr<beamlineSystem::GuideLine> FocusCave1;
  std::shared_ptr<constructSystem::Aperture> Slits;
  std::shared_ptr<constructSystem::Aperture> B4CChopper;
  std::shared_ptr<constructSystem::Aperture> BeamMonitorChopper;

  std::shared_ptr<beamlineSystem::GuideLine> FocusCave2;
  /// Enterance hole to cave
  //  std::shared_ptr<constructSystem::HoleShape> CaveCut;
  std::shared_ptr<simpleCaveSystem::caveDetector> Detector;
  std::shared_ptr<simpleCaveSystem::radialCollimator> Collimator;
  //Labyrinth to sample position
    std::shared_ptr<constructSystem::Corner> SampleLabyrinth;
    std::shared_ptr<constructSystem::Corner> SampleLabyrinthB4C;
    std::shared_ptr<constructSystem::InnerShield> SampleCorridor;
  //Labyrinth to detector
    //   std::shared_ptr<beamlineSystem::GuideLine> DetectorAccess;
    std::shared_ptr<constructSystem::InnerShield> DetectorAccess;
     std::shared_ptr<constructSystem::Beamstop> DetectorLabyrinth;
     std::shared_ptr<constructSystem::Corner> DetectorLabyrinthB4C;
     std::shared_ptr<constructSystem::Corner> DetectorLabyrinthB4Cexit;
   std::shared_ptr<beamlineSystem::GuideLine> DetectorAccessOuter;
    // Hatch
    std::shared_ptr<constructSystem::InnerShield> Hatch;    
    std::shared_ptr<constructSystem::InnerShield> Hatch2;    
    std::shared_ptr<constructSystem::InnerShield> Hatch3;    
    std::shared_ptr<constructSystem::InnerShield> HatchRails;    
    // Cable tracing system on the roof
    std::shared_ptr<constructSystem::InnerShield> UpperBlockWall;
    std::shared_ptr<constructSystem::InnerShield> GallerySteel;
    std::shared_ptr<constructSystem::InnerShield> GalleryDoor;
    std::shared_ptr<constructSystem::InnerShield> UpperBlockRoof1;
    std::shared_ptr<constructSystem::InnerShield> UpperBlockRoof2;
    std::shared_ptr<constructSystem::InnerShield> UpperBlockHatch;
    //    std::shared_ptr<constructSystem::InnerShield> UpperBlockHatchB4C;
    std::shared_ptr<constructSystem::InnerShield> UpperBlockHatch2;
    std::shared_ptr<constructSystem::Corner> RoofCableB4C1;
    std::shared_ptr<constructSystem::Corner> RoofCableB4C2;
    std::shared_ptr<constructSystem::InnerShield> CavePlatform;
    std::shared_ptr<constructSystem::InnerShield> CavePlatformFence;
    std::shared_ptr<constructSystem::InnerShield> CavePlatformB4C;
    std::shared_ptr<constructSystem::InnerShield> CableLow1;
    std::shared_ptr<constructSystem::InnerShield> CableLow2;
    std::shared_ptr<constructSystem::InnerShield> CableLow3;
    std::shared_ptr<constructSystem::Corner> CableLowBlock1;
    std::shared_ptr<constructSystem::Corner> CableLowBlock2;
        std::shared_ptr<constructSystem::InnerShield> CableLowBlock3;
    std::shared_ptr<constructSystem::Corner> CableLowBlock11;
    std::shared_ptr<constructSystem::Corner> CableLowBlock21;
    std::shared_ptr<constructSystem::Corner> UpperCableRouteR;
    std::shared_ptr<constructSystem::Corner> UpperCableRouteL;
    std::shared_ptr<essSystem::BoxShutter> BeamstopTshape;
    std::shared_ptr<essSystem::BoxShutter> BeamstopPlug;
    std::shared_ptr<beamlineSystem::GuideLine> BeamstopInnerDoughnut;
    std::shared_ptr<constructSystem::InnerShield> BeamstopDoughnut2;
    std::shared_ptr<constructSystem::InnerShield> BeamstopDoughnut;
    std::shared_ptr<essSystem::BoxShutter> HatchLock;
    std::shared_ptr<essSystem::BoxShutter> HatchLock1;
    std::shared_ptr<essSystem::BoxShutter> HatchLock2;
    std::shared_ptr<essSystem::BoxShutter> HatchStreamBlocker;
    std::shared_ptr<essSystem::BoxShutter> LabyrinthPlate;
    std::shared_ptr<essSystem::BoxShutter> Beamstop;

  int buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& ,
                        const long int,
                        const int);
  void buildOutGuide(Simulation&, const Bunker&,
                     const int);
  int buildCommonShield(Simulation&, const Bunker&,
                     const int);

 void  buildCave(Simulation* System,
		       //		       const attachSystem::FixedComp& FTA,
		       //                       const long int thermalIndex,
		       //		       const CompBInsert& BInsert,
		 const constructSystem::D03UpperBlock  ShieldInterface,
		 const int voidCell, const int, const int);
 
 public:
  
  BIFROST(const std::string&);
  BIFROST(const BIFROST&);
  BIFROST& operator=(const BIFROST&);
  ~BIFROST();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
