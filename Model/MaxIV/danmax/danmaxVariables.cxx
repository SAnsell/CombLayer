/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   danmax/danmaxVariables.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>
#include <cassert>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "maxivVariables.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "BellowGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "DoublePortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamPairGenerator.h"
#include "MonoShutterGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "DCMTankGenerator.h"
#include "MonoBlockXstalsGenerator.h"
#include "MLMonoGenerator.h"
#include "XRayHutchBaseGenerator.h"
#include "OpticsHutchGenerator.h"
#include "ExptHutGenerator.h"
#include "MovableSafetyMaskGenerator.h"
#include "CrossGenerator.h"
#include "BeamMountGenerator.h"
#include "BremBlockGenerator.h"
#include "HeatAbsorberR3ToyamaGenerator.h"
#include "ProximityShieldingGenerator.h"
#include "BladeBPMToyamaGenerator.h"
#include "FlangePlateGenerator.h"

// References
// [1] CARATELLI Drawing 06769-01-000
// [2] CARATELLI Drawing 06769-03-000
// [3] CARATELLI Drawing 06769-02-000
// [4] S0-2-0AB01088_DanMAX.pdf
// [5] S4-2-2AJ00829.pdf
// [6] S7-4-2AJ00837.pdf
// [7] S7-3-0AF00293.pdf
// [8] CARATELLI Drawing 06769-00-000
// [9] CARATELLI Drawing 06769-04-000
// [10] CAD model of DanMAX/SINCRYS, J. Selberg, fall/winter 2025
// [11] https://www.gammavacuum.com/detail/index/sArticle/3381/number/100LCV6SSCNN/sCategory/17668 (accessed on 2026-01-14)
// [12] DanMAX Beamline, FMB Oxford Drawing ABM0070
// [13] DanMAX Diagnostics, Functional Specification, FMB Oxford S3716, Rev. 5.0
// [14] DanMAX HDCM, Funcation Specification, FMB Oxford S3716, Rev. 6.0
// [15] Detailed Design Report for DanMAX, v2.0, Dec. 2017
// [16] CAD model 01_OH.x_t /mxn/groups/rad/Beamlines/DanMAX/Simulations/01_OH.x_t

namespace setVariable
{

namespace danmaxVar
{
  namespace absY{
    constexpr double XBPM1 = 1201.99; // [4]
    const double FM1Y = 1104.75; // [4]
    const double FM2Y = 1597.08; // [4]
    const double heatAbsorberY = 1673.33; // [4]
    // Absolute position of the downstream side of Valve4 [4].
    // This value is used to determine the lengths of elements in the optics hutch that
    // are not shown in [12].
    double valve4BackY = 2290.23; // [4]
    // All following absolute coordinates taken from [12]
    const double bremColl1Y = 2330.0;
    const double highPassFilterY = 2347.48;
    // Assume that the Y coordinate given there is the central axis of the top-jaw
    // port.
    const double whiteBeamSlitsTopJawY = 2622.75;
    const double beamViewer1Y = 2647.72;
    const double HDCMY = 2715.50;
    const double beamViewer2Y = 2784.0;
    const double MLMY = 2834.8; // Position of the view port on the top
    const double whiteBeamStopY = 2954.83;
    const double bremColl2Y = 2965.03;
    const double monoSlitsY = 2987.56;
    const double beamViewer3Y = 3036.92;
    const double CRLY = 3091.77;
    const double monoSlits2Y = 3143.94;
    const double bremColl3Y = 3166.47;
  }
// "V3 Valve" [4] Determines the length of several valves of the same type.
constexpr double valve3Length = 7.2;
const double opticalAxisHeight = 131.88; // [1] (back view, MEASURED)
// Height of Optics Hutch and Expt. Hutches 1 and 2.
// The value that is passed as the 'Height' variable of each hutch is the height above
// the optical axis.
// From [8], it can be seen that the height of all the hutches is roughly the same.
// For the Optics Hutch and Expt. Hutch 1, the heights are given in [1] (Section B-B)
// and [2] (Coupe B-B), respectively, and they agree within 3 mm.
// For Expt. Hutch 2, the height is not shown in the drawing [3].
// It was decided to use the value from [1] for all heights.
const double hutchHeightAboveOpticalAxis = 411.0-opticalAxisHeight;
const double exptHut1WallThick = 0.4; // "Lead Thickness Side Wall", Section A-A [2]
// DanMAX has a horizontal double crystal monochromator (HDCM) and a multilayer
// monochromator (MLM). It is possible to use either the HDCM, or the MLM, or both in
// combination. In all cases, the nominal offset of the beam as it exits the MLM is
// 10 mm [15].
const double beamMirrorShift = -1.0;

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& frontKey)
  /*!
    Builds the variables for the undulator
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double undulatorLen(300.0);
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,frontKey+"UPipe",undulatorLen);
  Control.addVariable(frontKey+"UPipeYStep",-undulatorLen/2.0);

  Control.addVariable(frontKey+"UPipeWidth",6.0);
  Control.addVariable(frontKey+"UPipeHeight",0.6);
  Control.addVariable(frontKey+"UPipePipeThick",0.2);

  // undulator
  Control.addVariable(frontKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(frontKey+"UndulatorLength",270.0);   // 46.2mm*30*2
  Control.addVariable(frontKey+"UndulatorMagnetWidth",6.0);
  Control.addVariable(frontKey+"UndulatorMagnetDepth",3.0);
  Control.addVariable(frontKey+"UndulatorSupportWidth",12.0);
  Control.addVariable(frontKey+"UndulatorSupportThick",8.0);
  Control.addVariable(frontKey+"UndulatorSupportLength",4.0);  // extra
  Control.addVariable(frontKey+"UndulatorSupportVOffset",2.0);
  Control.addVariable(frontKey+"UndulatorStandWidth",6.0);
  Control.addVariable(frontKey+"UndulatorStandHeight",8.0);
  Control.addVariable(frontKey+"UndulatorVoidMat","Void");
  Control.addVariable(frontKey+"UndulatorMagnetMat","NbFeB");
  Control.addVariable(frontKey+"UndulatorSupportMat","Copper");
  Control.addVariable(frontKey+"UndulatorStandMat","Aluminium");

  Control.addVariable(frontKey+"UndulatorFlipX",1);

  return;
}

void
frontMaskVariables(FuncDataBase& Control,
		   const std::string& preName)
  /*!
    Variable for the front maste
    \param Control :: Database
    \param preName :: Beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","frontMaskVariables");

  setVariable::SqrFMaskGenerator FMaskGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::FlangePlateGenerator FPGen;

  Control.addVariable(preName+"BellowALength",10.0); // [4]

  constexpr double FM1Length(40.0); // [4]
  constexpr double FM2Length(50.5); // [5]
  // MSM not used
  // constexpr double MSMLength(40.0);

  const double FM1dist(danmaxVar::absY::FM1Y+FM1Length/2.0); // [4]
  const double FM2dist(danmaxVar::absY::FM2Y+FM2Length/2.0); // [4]
  // MSM not used
  // const double MSMdist(1600.0);

  FMaskGen.setCF<CF100>();
  FMaskGen.setFrontGap(2.53, 2.53); //
  double backWidth = 1.19; //
  double backHeight = backWidth;
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM1Length-CF100::flangeLength-Geometry::zeroTol,
		      backWidth, backHeight);
  FMaskGen.generateColl(Control,preName+"FM1",FM1dist,FM1Length);


  FPGen.setFlange(CF100::flangeRadius, 1.99); // [4]
  FPGen.setWindow(0.0, 0.0, "Void"); // [4]
  FPGen.setMat("SteelUnknownGrade"); // guess
  FPGen.setInnerRadius(1.9);  // guess (same as BellowA)
  FPGen.generateFlangePlate(Control,preName+"FlangePlateAA"); // TODO: move to Toyama DanMAX front-end




  Control.addVariable(preName+"BellowBLength",10.0); // [4]
  Control.addVariable(preName+"BellowCLength",10.0); // [4]
  Control.addVariable(preName+"BellowFLength",14.0); // [4]
  Control.addVariable(preName+"BellowHLength",14.0); // [4]
  Control.addVariable(preName+"PipeCLength",34.0); // [4]

  Control.addVariable(preName+"PermanentMagnetYStep",270.0); // Distance permanent
  // magnet center to FM2 front: 170.5 cm, measured by AR
  // TODO: Define position with absolute coordinates or relative to FM2.

  FMaskGen.setFrontGap(2.1, 2.1); // [5]
  backWidth = 0.16; // [5]
  backHeight = 0.16; // [5]
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM2Length-4.6-Geometry::zeroTol,
		      backWidth, backHeight); // [5] Min position: 46 mm
  FMaskGen.setMat("Copper"); // [5] TODO: should be GLIDCOP
  ELog::EM << "TODO: FM2 material set to copper, should be GLIDCOP" << ELog::endWarn;
  FMaskGen.generateColl(Control,preName+"FM2",FM2dist,FM2Length);
  Control.addVariable(preName+"FM2Width", 8.8); // [5]
  Control.addVariable(preName+"FM2Height", 6.8); // [5]

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setMat("SteelUnknownGrade", "SteelUnknownGrade");
  BellowGen.generateBellow(Control,preName+"BellowCA",10.0); // [4]

  HeatAbsorberR3ToyamaGenerator HAGen;

  constexpr double heatAbsorberLength = 26.5;  // [4]

  HAGen.generate(Control,preName+"HeatAbsorber",heatAbsorberLength);
  Control.addVariable(preName+"HeatAbsorberYStep",danmaxVar::absY::heatAbsorberY);

  // NOT PRESENT :::
  // FMaskGen.setFrontGap(0.84,0.582);
  // FMaskGen.setBackGap(0.750,0.357);

  // FMaskGen.setMinAngleSize(12.0,1600.0, 100.0, 100.0);
  // FMaskGen.generateColl(Control,preName+"CollC",17/2.0,17.0);

  // approx for 100uRad x 100uRad
  FMaskGen.setFrontAngleSize(FM2dist,150.0,150.0);
  FMaskGen.setMinAngleSize(12.0,FM2dist, 100.0, 100.0 );
  FMaskGen.setBackAngleSize(FM2dist, 160.0,160.0 );
  FMaskGen.generateColl(Control,preName+"CollC",17.0/2.0,17.0);

  setVariable::PipeGenerator PipeGen;
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>(); // dummy
  PipeGen.generatePipe(Control,preName+"MSMEntrancePipe",5.0); // dummy


  // Movable Safety Mask (not used)
  // BellowGen.setMat("SteelUnknownGrade", "SteelUnknownGrade");
  // BellowGen.setCF<setVariable::CF40>();
  // BellowGen.generateBellow(Control,preName+"BellowPreMSM",14.0); // guess

  // MovableSafetyMaskGenerator MSMGen;
  // MSMGen.generate(Control,preName+"MSM",MSMLength, "undulator"); //
  // Control.addVariable(preName+"MSMYStep",MSMdist);

  // BellowGen.generateBellow(Control,preName+"BellowPostMSM",14.0); // guess

  // PipeGen.setMat("SteelUnknownGradeL"); // dummy
  // PipeGen.generatePipe(Control,preName+"MSMExitPipe",100.0); // dummy

  return;
}


void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& hutName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param hutName :: Optics hut name
    \param xOffset  :: Wall step as beam line moved
  */
{
  ELog::RegMethod RegA("danmaxVariables","opticsHutVariables");

  OpticsHutchGenerator OGen;

  // Not given in [1]. Use same value as in other beamlines (where this value is also
  // used without reference).
  const double skinThick = 0.2;
  OGen.setSkin(skinThick);
  const double backLead = 5.0; // "Lead Thickness Back Wall" Section A-A [1]
  OGen.setBackLead(backLead);
  // "Lead Thickness Side Wall", Section A-A [1]
  const double opticsHutchWallThick = 1.2;
  OGen.setWallLead(opticsHutchWallThick);
  OGen.setRoofLead(1.2); // "Roof Lead Thickness", top view [1]
  OGen.addHole(Geometry::Vec3D(danmaxVar::beamMirrorShift,0,0),3.6); // Section D-D [1]
  const double opticsHutLength = 1010.0; // Section A-A in [1]

  // Section D-D or back view in [1] show all dimensions
  OGen.setBackPlateOuter(7.0, 200.0, 200.0);

  OGen.generateHut(Control,hutName, opticsHutLength);
  const double opticsHutOuterWidth = 259.7; // Section A-A in [1]
  Control.addVariable(hutName+"OutWidth", opticsHutOuterWidth);
  Control.addVariable(hutName+"Height", hutchHeightAboveOpticalAxis);

  // Section A-A [1]
  Control.addVariable(hutName+"RingStepLength", opticsHutLength-155.2);
  // Section A-A [1]
  Control.addVariable(hutName+"RingStepWidth",566.7-opticsHutOuterWidth-120.2);

  Control.addVariable(hutName+"BackPlateInnerActive", 0);

  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  const double chicaneHeight = 60.0; // Outside view [1]
  PGen.setHeight(chicaneHeight);
  PGen.setWidth(60.0); // Outside view [1]
  // Determined by AR and UFG 260112 to be the same as hutch wall thickness.
  // Measured on site to confirm.
  PGen.setPlateThick(opticsHutchWallThick);
  // Assume it is the same as the hutch skin thickness as in Expt. Hutches.
  PGen.setSkin(skinThick);

  // Reference x value for all chicanes
  const double x0 = (opticsHutLength+2.0*skinThick+backLead)/2.0;
  PGen.generatePortChicane(
    Control,hutName+"Chicane0","Left",
    // Section A-A [1], hutch back to center of chicane
    // 1552 mm + 2 x 12 mm + 1.5 x 1500 mm = 3802 mm
    x0-380.2,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Outside view [1]
  );
  PGen.generatePortChicane(
    Control,hutName+"Chicane1","Left",
    // Section A-A [1], hutch back to center of chicane
    // 1552 mm + 4 x 12 mm + 2950 mm + 2.5 x 1500 mm = 8300 mm
    x0-830.0,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Outside view [1]
  );

  Control.addVariable(hutName+"FloorShineThick", 0.6); // [1]
  Control.addVariable(hutName+"FloorShineLength", 50.0); // full length [1]

  Control.addVariable(hutName+"WallShineThick", 0.6); // Detail G [2]
  // In Detail G [2], the length of the wall-shine element is given as 650 mm.
  // It is not clear from [2] what the 650 mm correspond to. Here, it is assumed that
  // this value is measured from the outside wall like all other shielding of this
  // type.
  Control.addVariable(hutName+"WallShineLength", 65.0);
  Control.addVariable(hutName+"WallShineOutThick", 1.2); // measured by UFG 251201
  Control.addVariable(hutName+"WallShineOutLength", 20.0); // measured by UFG 251201

  return;
}

void
connectVariables(FuncDataBase& Control,
		 const std::string& beamName)
  /*!
    Connecting line variables
    \param Control :: DataBase to add
    \param beamName :: beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","connectVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PortItemGenerator PItemGen;

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setNoWindow();

  const std::string connectName(beamName+"ConnectShield");
  Control.addVariable(connectName+"SmallWidth", 22.0); // Detail D [9]
  Control.addVariable(connectName+"LargeWidth", 40.0); // Detail K [9]
  Control.addVariable(connectName+"SmallHeight", 23.5); // Detail D [9]
  Control.addVariable(connectName+"LargeHeight", 23.5); // Detail K [9]
  // Front wall inside to start of inner void of large region, Section B-B [9]
  // 103 mm + 2 x 1147.5 mm + 2 x 10 mm = 2418.0 mm
  Control.addVariable(connectName+"LargeRegionStart", 241.8);
  Control.addVariable(connectName+"LargeRegionLength", 55.4); // Section B-B [9]
  Control.addVariable(connectName+"TopThick", 0.7); // Detail D and Detail K [9]
  Control.addVariable(connectName+"BottomThick", 1.0); // Detail D and Detail K [9]
  Control.addVariable(connectName+"LeftThick", 0.7); // Detail D and Detail K [9]
  Control.addVariable(connectName+"RightThick", 0.7); // Detail D and Detail K [9]
  Control.addVariable(connectName+"FrontBackThick", 0.7); // Detail D and Detail K [9]
  Control.addVariable(connectName+"SkinThick", 0.1); // Estimated, not shown in [9]
  Control.addVariable(connectName+"Mat", "Lead"); // [9]
  Control.addVariable(connectName+"SkinMat", "SteelUnknownGrade");
  Control.addVariable(connectName+"VoidMat", "Void");

  // dummy, adjusted such that ion pump is centered in Experimental Hutch 2.
  // (Note that the shielding box around it is not exactly centered in the hutch)
  // PipeA length =
  //    Expt. Hutch 2 length (545.8 cm) / 2.0
  //    - length of JoinPipeB inside Expt. Hutch 2 (10.0 cm) (*)
  //    - ion pump length (32.59 cm) / 2.0
  //    - FlangeA length (5.0 cm)
  //    - BellowA length (16.0 cm)
  //
  // (*) see comment there
  PipeGen.generatePipe(Control,beamName+"PipeA",225.605);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,beamName+"BellowA",16.0);

  PipeGen.setBFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"FlangeA",5.0);

  // Gamma Vacuum 100L TiTan Ion Pump [9-11]
  // Geometry simplified to a single pipe with the appropriate standard.
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"IonPumpA", 32.59);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"FlangeB",5.0);

  BellowGen.generateBellow(Control,beamName+"BellowB",16.0);

  PipeGen.setCF<setVariable::CF40>();
  // dummy, see comment for PipeA
  PipeGen.generatePipe(Control,beamName+"PipeB",214.31);

  return;
}

void
exptHut1Variables(FuncDataBase& Control,
		 const std::string& beamName)
  /*!
    Experimental hutch 1 variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","exptHut1Variables");

  setVariable::ExptHutGenerator EGen;

  const std::string hutName(beamName+"ExptHut1");

  // See the comment for EGen.setBackLead in exptHut1Variables
  EGen.setFrontLead(0.4); // "Lead Thickness Upstream Wall", Section A-A [2]
  EGen.setFrontHole(danmaxVar::beamMirrorShift,0.0,3.6); // Coupe C-C [2]
  EGen.setCorner(atan(167.4/281.5)*180.0/M_PI,281.5); // Section A-A [2]
  const double backLead = 0.6; // "Lead Thickness Downstream Wall", Section A-A [2]
  EGen.setBackLead(backLead);
  const double skinThick = 0.1; // "Steel", Detail E [2]
  EGen.setSkin(skinThick);
  EGen.setRoofLead(0.4); // "Roof Thk Pb", Section A-A [2]
  EGen.setWallLead(exptHut1WallThick);
  EGen.setFloorShine(0.6, 20.0); // [2], full length [2]

  const double hutchLength = 1401.3; // Section A-A [2]
  EGen.generateHut(Control,hutName,0.0, hutchLength);
  Control.addVariable(hutName+"RingWidth",204.8); // Section A-A [2]
  Control.addVariable(hutName+"OutWidth",260.2); // Section A-A [2]
  Control.addVariable(hutName+"Height",hutchHeightAboveOpticalAxis);
  // In [2], the floor-shine length is measured from the outside of the wall, i.e. the
  // wall thickness is included. In this case here, where the front wall is the back
  // wall of Expt. Hutch 2, the wall thickness (steel layer inside and outside from
  // Detail E and lead thickness from Section A-A in [2]) needs to be subtracted.
  Control.addVariable(hutName+"FloorShineFrontLength",20.0-0.6);

  // 5 chicanes, 3 wide ones (#0 - #2) and 2 small ones (#3 and #4).
  // Apart from the values set explicitly here, using default values for, e.g. gap
  // sizes, that have been deemed reasonable by site visits.
  Control.addVariable(hutName+"NChicane",5);
  PortChicaneGenerator PGen;
  // Coupe B-B [2], height of wide chicanes. Only shown for leftmost chicane,
  // assumed to be valid for all.
  double chicaneHeight = 60.0;
  PGen.setHeight(chicaneHeight);
  const double chicaneWidth = 60.0; // Coupe B-B [2]
  PGen.setWidth(chicaneWidth);
  // Determined by AR and UFG 260112 to be the same as hutch wall thickness.
  // Measured on site to confirm.
  const double chicaneWallThick = exptHut1WallThick;
  PGen.setPlateThick(exptHut1WallThick);
  PGen.setSkin(skinThick); // Measured on site.

  // Reference x value for all chicanes
  const double x0 = -(hutchLength-2.0*skinThick-backLead)/2.0;

  PGen.generatePortChicane(
    Control,hutName+"Chicane0","Left",
    // Coupe B-B [2], hutch front to center of chicane
    // 1655 mm + 1750 mm + 4.5 x 1500 mm = 10155 mm
    x0+1015.5,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Coupe B-B [2]
  );
  PGen.generatePortChicane(
    Control,hutName+"Chicane1","Left",
    // Coupe B-B [2], hutch front to center of chicane
    // 1655 mm + 1750 mm + 0.5 x 1500 mm = 10155 mm
    x0+415.5,
    -opticalAxisHeight+110.0+chicaneHeight/2.0 // Coupe B-B [2]
  );
  const double smallChicaneWidth = 30.0;
  // Distance from hutch front wall to chicane vertical wall
  const double chicane4WallDist = 22.5;
  // Distance from chicane 2 to chicane 4, vertical wall to vertical wall
  const double chicane2chicane4Dist = 24.0;
  PGen.generatePortChicane(
    Control,hutName+"Chicane2","Left",
    x0+chicane4WallDist+smallChicaneWidth+3.0*chicaneWallThick+chicane2chicane4Dist
    +0.5*chicaneWidth,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Coupe B-B [2]
  );
  // Coupe B-B [2], height of small chicanes. It is assumed that the upper edge of the
  // small chicanes is 1400 mm above ground level as for the wide chicanes.
  chicaneHeight = 68.0;
  PGen.setHeight(chicaneHeight);
  PGen.setWidth(smallChicaneWidth); // Coupe B-B [2]
  PGen.generatePortChicane(
    Control,hutName+"Chicane3","Left",
    // Coupe B-B [2], hutch front to center of chicane
    // 1655 mm + 1750 mm + 2.5 x 1500 mm = 7155 mm
    x0+715.5,
    -opticalAxisHeight+72.0+chicaneHeight/2.0 // Coupe B-B [2]
  );
  PGen.generatePortChicane(
    Control,hutName+"Chicane4","Left",
    x0+smallChicaneWidth/2.0+chicaneWallThick+chicane4WallDist,
    -opticalAxisHeight+72.0+chicaneHeight/2.0 // Coupe B-B [2]
  );

  return;
}

void
exptHut2Variables(FuncDataBase& Control,
		 const std::string& beamName)
  /*!
    Experimental hutch 2 variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","exptHut2Variables");

  setVariable::ExptHutGenerator EGen;

  const std::string hutName(beamName+"ExptHut2");

  // Both experimental hutches share the same wall: it is the back
  // wall of hutch 1 and the front wall of hutch 2. Since hutch 2 is
  // narrower than hutch 1, the wall is built according to the wider
  // hutch to avoid gaps.
  EGen.setBackLead(0.0);
  // backLead is needed for chicane placement:
  const double backLead = Control.EvalVar<double>(beamName+"ExptHut1PbFrontThick");
  const double skinThick = 0.1; // "Steel", Detail E [3]
  EGen.setSkin(skinThick);
  EGen.setRoofLead(0.2); // "ROOF THK Pb", Section A-A [3]
  EGen.setWallLead(0.2); // "Lead Thickness Side Wall", Section A-A [3]
  EGen.setFloorShine(0.6, 20.0); // Detail J [3]

  // // "Lead Thickness", back view [1]
  // ELog::EM << "TODO: set correct dimensions and intersect cells: 7x200x200" << ELog::endWarn;
  // ELog::EM << "TODO: Why thickness==7 ?" << ELog::endWarn;
  // EGen.setFrontPlate(7.0, 50.0, 80.0); // TODO: dummy. Set correct values.

  // The actual length of hutch 2 is 5366 mm as shown in [3].
  // However, to match the simplified model of the optics hutch, use the slightly
  // larger value given in [8].
  const double hutchLength = 545.8;
  EGen.generateHut(Control,hutName,0.0,hutchLength);

  Control.addVariable(hutName+"RingWidth",47.3); // Section A-A [3]
  Control.addVariable(hutName+"OutWidth",260.2); // Section A-A [3]
  Control.addVariable(hutName+"Height",hutchHeightAboveOpticalAxis);

  // No floor shine at the front/back wall inside Expt. Hutch 2 shown in [1-3] or [8].
  // Confirmed during site visit.
  Control.addVariable(hutName+"FloorShineFrontLength",0.0);
  Control.addVariable(hutName+"FloorShineBackLength",0.0);

  Control.addVariable(hutName+"NChicane",3);
  PortChicaneGenerator PGen;
  double chicaneHeight = 60.0; // Outside view [3]
  PGen.setHeight(chicaneHeight);
  PGen.setWidth(60.0); // Outside view [3]
  // Determined by AR and UFG 260112 to be the same as the wall thickness of
  // Experimental Hutch 1 (!). Measured on site to confirm.
  // Supposedly, the chicane design from hutch 1 was reused when hutch 2 was added
  // later.
  const double chicaneWallThick = exptHut1WallThick;
  PGen.setPlateThick(chicaneWallThick);
  PGen.setSkin(skinThick); // Measured on site.

  // Reference x value for all chicanes
  const double x0 = (hutchLength-2.0*skinThick-backLead)/2.0;
  // Chicane0 and Chicane1 are within 1828 mm from the back wall of Expt. Hutch 2
  // (Section A-A [3]). Assume that their centers are located at 1/4*1828 and 3/4*1828.
  // Positioning uncertainty estimate from the drawing: += 50 mm.
  PGen.generatePortChicane(
    Control, hutName+"Chicane0", "Left",
    x0-182.8*0.25,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Outside view [3]
  );
  PGen.generatePortChicane(
    Control, hutName+"Chicane1", "Left",
    x0-182.8*0.75,
    -opticalAxisHeight+80.0+chicaneHeight/2.0 // Outside view [3]
  );

  chicaneHeight = 68.0; // Outside view [3]
  PGen.setHeight(chicaneHeight);
  PGen.setWidth(30.0); // Outside view [3]
  PGen.generatePortChicane(
    Control, hutName+"Chicane2", "Left",
    // Section A-A [3], hutch back to center of chicane
    // 1828 mm + 8 mm + 0.5 x 760 mm = 2216 mm
    x0-221.6,
    -opticalAxisHeight+72.0+chicaneHeight/2.0 // Outside view [3]
  );

  return;
}

double
viewPackage(FuncDataBase& Control,const std::string& viewKey,
  const double beamViewerFrontToPort)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
    \param beamViewerFrontToPort :: Distance from the front surface to the port that
    defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","viewPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  // will be rotated vertical
  const std::string pipeName=viewKey+"ViewTube";
  SimpleTubeGen.setCF<CF100>(); // [10]
  SimpleTubeGen.setCap();
  const double mainTubeAboveOpticalAxis = 15.0; // [10]
  const double mainTubeBelowOpticalAxis = 32.5; // [10]
  const double offsetZ = (mainTubeBelowOpticalAxis-mainTubeAboveOpticalAxis)/2.0;
  SimpleTubeGen.generateTube(
    Control,pipeName,mainTubeAboveOpticalAxis+mainTubeBelowOpticalAxis);


  Control.addVariable(pipeName+"NPorts",3);

  const double totalLength = 2.0*beamViewerFrontToPort; // [10]
  const double connectPortLength = (totalLength-2.0*CF100::outerRadius)/2.0;
  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+connectPortLength);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(0,0,-1));
  PItemGen.setPlate(0.0,"SteelUnknownGrade");
  PItemGen.setCF<setVariable::CF40>(sqrt(2.0)*CF100::outerRadius+10.0); // [10]
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(-1,0,1));

  FlangeGen.setNoPlate();
  // Most blade data from [13].
  //
  // Angle from [10].
  //
  // Material is actually given as "pCVD Diamond" in [13].
  FlangeGen.setBlade(3.3,1.3,0.2,-45.0,"Diamond",1);
  FlangeGen.generateMount(Control,viewKey+"ViewTubeScreen",1);

  return totalLength-beamViewerFrontToPort;
}

double
lensPackage(FuncDataBase& Control,const std::string& lensKey,
  const double CRLFrontToCenter)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param lensKey :: prename including view
    \param CRLFrontToCenter :: Distance from the front surface to the center, which
    defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","lensPackage");

  setVariable::MonoBoxGenerator MBoxGen;

  const double totalLength = 2.0*CRLFrontToCenter;
  const std::string lensName=lensKey+"LensBox";

  MBoxGen.setCF<CF40>();
  const double wallThick = 1.0; // [10]
  MBoxGen.setWallThick(wallThick);
  const double portABLength = 4.5; // [10]
  MBoxGen.setPortLength(portABLength, portABLength);
  const double bottomThick = 2.0;
  const double topThick = 2.4;
  MBoxGen.setLids(3.5,bottomThick, topThick); // [10]
  // All dimensions from [10]
  MBoxGen.generateBox(Control,lensName,
    21.0,12.0-2.0*topThick,8.2-2.0*bottomThick,
    totalLength-2.0*(wallThick+portABLength));

  return totalLength-CRLFrontToCenter;
}

double
viewBPackage(FuncDataBase& Control,const std::string& viewKey,
  const double mainTubeFrontToBeamViewerPort)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
    \param mainTubeFrontToBeamViewerPort :: Distance from the front surface to the
    port that defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","viewBPackage");

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  const std::string pipeName=viewKey+"ViewTubeB";
  PTubeGen.setMat("SteelUnknownGrade");
  PTubeGen.setPipeCF<CF150>();
  PTubeGen.setPortCF<CF40>();
  const double wallThick = 0.2; // [10]
  PTubeGen.setWallThick(wallThick);
  const double flangeLength = 3.0; // [10]
  PTubeGen.setPortLength(flangeLength,flangeLength);
  // ystep/width/height/depth/length
  const double totalLength = 36.0; // [10]
  PTubeGen.generateTube(
    Control,pipeName,0.0,totalLength-2.0*flangeLength-2.0*wallThick);


  // will be rotated vertical

  Control.addVariable(pipeName+"NPorts",3);

  PItemGen.setCF<setVariable::CF100>(14.0); // [10]
  PItemGen.setPlate(CF100::flangeLength, "SteelUnknownGrade");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-9,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setCF<setVariable::CF100>(18.0); // [10]
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,-totalLength/2.0+mainTubeFrontToBeamViewerPort,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setPlate(CF40::flangeLength, "SteelUnknownGrade");
  PItemGen.setCF<setVariable::CF40>(12.056/cos(M_PI_4)); // [10]
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,-totalLength/2.0+mainTubeFrontToBeamViewerPort,0),
			Geometry::Vec3D(-1,-1,0));

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(0.7,0.7,0.02,-45.0,"Diamond",1); // [13]
  FlangeGen.generateMount(Control,pipeName+"Screen",1);

  return totalLength-mainTubeFrontToBeamViewerPort;
}

double
beamStopPackage(FuncDataBase& Control,const std::string& viewKey,
  const double beamStopFrontToWBPort)
  /*!
    Builds the variables for the ViewTube 2
    \param Control :: Database
    \param viewKey :: prename
    \param beamStopFrontToWBPort :: Distance from the front surface to the
    port that defines the absolute position.
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","beamStopPackage");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::BeamPairGenerator BeamPairGen;

  const double port0Radius = 5.08; // [10]
  const double port0WallThick = 0.2; // [10]
  const double beamStopInPipeLength = 2.5; // [10]
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<CF40>(); // [10]
  PipeGen.setBFlange(port0Radius+port0WallThick, port0WallThick);
  PipeGen.generatePipe(Control,viewKey+"BeamStopInPipe",
    beamStopInPipeLength+CF40::flangeLength+port0WallThick); // [10]

  const double port0Length=15.4; // [10]
  const double port0SplitLength=8.0;

  std::string pipeName = viewKey+"BeamStopSection";
  SimpleTubeGen.setPipe(port0Radius, port0WallThick, 1.0, 0.0);
  SimpleTubeGen.generateTube(Control,pipeName,port0Length-port0SplitLength);
  Control.addVariable(pipeName+"NPorts",1);
  PItemGen.setCF<CF40>(14.0); // [10]
  PItemGen.setPlate(CF40::flangeLength, "SteelUnknownGrade");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-(port0Length-port0SplitLength)/2.0
      +beamStopFrontToWBPort-beamStopInPipeLength-CF40::flangeLength-port0WallThick,0),
			Geometry::Vec3D(1,0,0));

  // Demonstrate explicitly that the code sets the position of the bremsstrahlung
  // collimator correctly by re-calculating the distance between the bremsstrahlung
  // collimator and the white beam stop using local variables.
  assert(
    fabs(
      (
        port0Length - (beamStopFrontToWBPort-beamStopInPipeLength-CF40::flangeLength)
      ) // Calculated value
    - (danmaxVar::absY::bremColl2Y-danmaxVar::absY::whiteBeamStopY) // Reference value [12]
    ) < Geometry::zeroTol
  );

  // will be rotated vertical
  pipeName=viewKey+"BeamStopTube";
  SimpleTubeGen.setCF<CF160>(); // [10]
  SimpleTubeGen.setWallThick(0.2); // [10]
  SimpleTubeGen.setCap(1,1);
  const double tubeLengthAboveOpticalAxis = 11.2; // [10]
  const double tubeLengthBelowOpticalAxis = 32.8; // [10]
  const double tubeLength = tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis;
  SimpleTubeGen.generateTube(Control,pipeName,
    tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis);

  Control.addVariable(pipeName+"NPorts",2);

  PItemGen.setPort(port0SplitLength-port0WallThick, port0Radius, port0WallThick); // [10]
  PItemGen.setFlange(1.0, 0.0);
  PItemGen.setNoPlate();
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,1));

  const double port1Length = 19.575; // [10]
  const double port1ArtificialSplitLength = 12.0;
  const double port1Radius = 7.3; // [10]
  const double port1WallThick = 0.2; // [10]
  PItemGen.setPort(port1ArtificialSplitLength, port1Radius, port1WallThick); // [10]
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,-1));

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setPipe(port1Radius, port1WallThick);
  PipeGen.setAFlange(1.0, 0.0);
  PipeGen.setBFlangeCF<CF150>();
  PipeGen.generatePipe(Control,viewKey+"BeamStopOutPipe",
    port1Length-port1ArtificialSplitLength);

  // TODO: Material currently set to pure tungsten (default), but should be DENSIMET [13].
  BremGen.centre();
  BremGen.setCube(10.0,10.0); // [13]
  BremGen.setAperature(0.4); // [13]
  BremGen.generateBlock(Control,viewKey+"BeamStop",
    tubeLength/2.0-tubeLengthAboveOpticalAxis);
  Control.addVariable(viewKey+"BeamStopXAngle",90);

  // Tube for Monochromatic Slits
  const double monoSlitsTubeWallThick = 2.0; // [10]
  // CF150 to fit flange B of port 1 [10]
  SimpleTubeGen.setPipe(
    CF150::flangeRadius-monoSlitsTubeWallThick,monoSlitsTubeWallThick,1.0,0.0); // [10]
  pipeName = viewKey+"MonoSlitsTube";
  const double monoSlitsTubeLength = 2.0*(
    danmaxVar::absY::monoSlitsY-danmaxVar::absY::bremColl2Y-port1Length);
  SimpleTubeGen.generateTube(
    Control,pipeName,monoSlitsTubeLength);
  Control.addVariable(pipeName+"NPorts",4);
  PItemGen.setCF<CF16>(12.0); // Estimated
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  const double bladeThick = 0.2; // [10]
  const double bladeOffset = 0.1; // [10]
  const double bladeXZDist = 3.1; // [10]
  const double bladeWidth = 5.0; // [13]
  const double innerBladePos = 0.5*(bladeXZDist-bladeOffset-bladeThick);
  const double outerBladePos = 0.5*(bladeXZDist+bladeOffset+bladeThick);
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-outerBladePos,0.5*bladeWidth),
			Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,-innerBladePos,-0.5*bladeWidth),
			Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0.5*bladeWidth,innerBladePos,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port3",
			Geometry::Vec3D(-0.5*bladeWidth,outerBladePos,0),
			Geometry::Vec3D(0,0,1));

  // Slit width/height/material are the same as in Diagnostic Module 2, therefore
  // assume other properties are also similar if not indicated otherwise.
  BeamPairGen.setLift(0.6, 0.6); // "Maximum aperture" given as 10 mm x 10 mm [13]
  BeamPairGen.setGap(-0.1,-0.1); // "Maximum overlap" given as 2 mm in [13]
  BeamPairGen.setThread(0.5*bladeThick,"Nickel"); // Estimated
  // Height from [13]
  BeamPairGen.setBlock(bladeWidth,3.5,bladeThick,0.0,"TungstenCarbide"); // [13]
  // Cooling is achieved through water-cooled fingers attached to the blades
  // (not modeled). This is different from Diagnostic Module 2.
  BeamPairGen.setWaterPipes(0,0);

  // Seen from upstream, the beam hits the jaws in the following order [10]:
  // Right (MonoSlitsX A) -> Left (MonoSlitsX B)
  // -> Bottom (MonoSlitsZ B) -> Top (MonoSlitsZ A)
  BeamPairGen.setXYStep(
    -0.5*bladeThick-2.0*bladeOffset,-0.5*bladeWidth,
    0.0,-0.5*bladeWidth);
  BeamPairGen.generateMount(Control,viewKey+"MonoSlitsX",0);
  BeamPairGen.setXYStep(
    0.5*bladeWidth,-0.5*bladeThick-2.0*bladeOffset,
    0.5*bladeWidth,0.0);
  BeamPairGen.generateMount(Control,viewKey+"MonoSlitsZ",0);

  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  const double slitsAOutLength = 4.0; // Estimated
  PipeGen.generatePipe(Control,viewKey+"SlitsAOut",slitsAOutLength);

  return monoSlitsTubeLength/2.0+slitsAOutLength;
}

double
revBeamStopPackage(FuncDataBase& Control,
		   const std::string& viewKey, const double revMonoSlitsFrontToSlits)
  /*!
    Builds the variables for the reversed slit tube/beamstop

    The construction is largely analogous to Diagnostic Module 4.
    See that one for more information.

    \param Control :: Database
    \param viewKey :: prename
    \param revMonoSlitsFrontToSlits :: Distance from the front surface to the
    port that defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","revBeamStopPackage");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::BeamPairGenerator BeamPairGen;

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  const double slitsInLength = 4.0; // Estimated
  PipeGen.generatePipe(Control,viewKey+"RevMonoSlitsIn",slitsInLength);

  // Tube for Monochromatic Slits
  const double monoSlitsTubeWallThick = 2.0; // [10]
  // CF150 to fit flange B of port 1 [10]
  SimpleTubeGen.setPipe(
    CF150::flangeRadius-monoSlitsTubeWallThick,monoSlitsTubeWallThick,1.0,0.0); // [10]
  std::string pipeName = viewKey+"RevMonoSlitsTube";
  const double monoSlitsTubeLength = 2.0*(revMonoSlitsFrontToSlits-slitsInLength);
  SimpleTubeGen.generateTube(
    Control,pipeName,monoSlitsTubeLength);

  Control.addVariable(pipeName+"NPorts",4);
  PItemGen.setCF<CF16>(12.0); // Estimated
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  const double bladeThick = 0.2; // [10]
  const double bladeOffset = 0.1; // [10]
  const double bladeXZDist = 3.1; // [10]
  const double bladeWidth = 5.0; // [13]
  const double innerBladePos = 0.5*(bladeXZDist-bladeOffset-bladeThick);
  const double outerBladePos = 0.5*(bladeXZDist+bladeOffset+bladeThick);
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-outerBladePos,0.5*bladeWidth),
			Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,-innerBladePos,-0.5*bladeWidth),
			Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0.5*bladeWidth,innerBladePos,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port3",
			Geometry::Vec3D(-0.5*bladeWidth,outerBladePos,0),
			Geometry::Vec3D(0,0,1));

  BeamPairGen.setLift(0.6, 0.6); // "Maximum aperture" given as 10 mm x 10 mm [13]
  BeamPairGen.setGap(-0.1,-0.1); // "Maximum overlap" given as 2 mm in [13]
  BeamPairGen.setThread(0.5*bladeThick,"Nickel"); // Estimated
  // Height from [13]
  BeamPairGen.setBlock(bladeWidth,3.5,bladeThick,0.0,"TungstenCarbide");
  BeamPairGen.setWaterPipes(0,0);

  // Seen from upstream, the beam hits the jaws in the following order [10]:
  // Right (MonoSlitsX A) -> Left (MonoSlitsX B)
  // -> Bottom (MonoSlitsZ B) -> Top (MonoSlitsZ A)
  BeamPairGen.setXYStep(
    -0.5*bladeThick-2.0*bladeOffset,-0.5*bladeWidth,
    0.0,-0.5*bladeWidth);
  BeamPairGen.generateMount(Control,viewKey+"RevMonoSlitsX",0);
  BeamPairGen.setXYStep(
    0.5*bladeWidth,-0.5*bladeThick-2.0*bladeOffset,
    0.5*bladeWidth,0.0);
  BeamPairGen.generateMount(Control,viewKey+"RevMonoSlitsZ",0);

  // will be rotated vertical
  pipeName=viewKey+"RevBeamStopTube";
  SimpleTubeGen.setCF<CF160>(); // [10]
  SimpleTubeGen.setWallThick(0.2); // [10]
  SimpleTubeGen.setCap(1,1);
  const double tubeLengthAboveOpticalAxis = 11.2; // [10]
  const double tubeLengthBelowOpticalAxis = 32.8; // [10]
  const double tubeLength = tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis;
  SimpleTubeGen.generateTube(Control,pipeName,
    tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis);

  Control.addVariable(pipeName+"NPorts",2);

  PItemGen.setCF<CF150>(
    danmaxVar::absY::bremColl3Y-danmaxVar::absY::monoSlits2Y-0.5*monoSlitsTubeLength);
  PItemGen.setOuterVoid(0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,1));

  const double port1Length = 13.5; // [10]
  PItemGen.setCF<CF40>(port1Length);
  PItemGen.setNoPlate();
  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,-1));

  // TODO: Material currently set to pure tungsten (default), but should be
  // DENSIMET [13].
  BremGen.centre();
  BremGen.setCube(10.0,10.0); // [13]
  BremGen.setAperature(0.5); // [13]
  BremGen.generateBlock(Control,viewKey+"RevBeamStop",
    tubeLength/2.0-tubeLengthAboveOpticalAxis);
  Control.addVariable(viewKey+"RevBeamStopXAngle",90);

  return port1Length;
}

double
monoPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","monoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MonoBlockXstalsGenerator MXtalGen;

  const double HDCMTotalLength = 75.0; // [14]
  const double HDCMPortALength = 4.5; // [10]
  const double monoVesselWallThick = 0.5; // [10]
  const double monoVesselRadius = 30.0; // [10]
  MBoxGen.setCF<CF40>(); // [10]
  MBoxGen.setPortLength(
    HDCMPortALength,HDCMTotalLength-HDCMPortALength
    -2.0*monoVesselWallThick-2.0*monoVesselRadius
  );

  // Although the HDCM shifts the beam axis, the front and back ports of
  // are collinear [10]. No call to MBoxGen.setBPortOffset() or similar needed.

  // radius, height, depth
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",monoVesselRadius,0.0,17.5); // [10]
  Control.addVariable(monoKey+"MonoVesselWallThick", 0.5);

  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",0);
  PItemGen.setCF<setVariable::CF63>(5.0+31.2);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"SteelUnknownGrade","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));

  // crystals gap 7mm
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0);


  return HDCMTotalLength-HDCMPortALength
    -monoVesselWallThick-monoVesselRadius;
}

double
mirrorMonoPackage(FuncDataBase& Control,const std::string& monoKey,
  const double MLMFrontToTopViewPort)
  /*!
    Builds the variables for the mirror mono package (MLM)
    \param Control :: Database
    \param monoKey :: prename
    \param MLMFrontToTopViewPort :: Distance from the front surface to the
    port that defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","mirrorMonoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::VacBoxGenerator MBoxGen;
  setVariable::MLMonoGenerator MXtalGen;

  MBoxGen.setCF<CF40>();   // set ports
  const double MLMWallThick = 1.2; // Walls: front, side, back [10]
  // Roof/Base/Width/Front/Back
  const double MLMRoofThick = 1.5; // [10]
  const double MLMFloorThick = 2.7; // [10]
  MBoxGen.setAllThick(
    MLMRoofThick,MLMFloorThick,MLMWallThick,MLMWallThick,MLMWallThick);
  const double MLMPortLength = 2.4; // [10], same for A and B
  const double MLMTotalLength = 115.9; // [10]
  MBoxGen.setPortLength(MLMPortLength, MLMPortLength); // [10]
  const double MLMHeight = 47.0;
  const double MLMHeightAboveOpticalAxis = 11.55+CF40::innerRadius;
  MBoxGen.setBPortOffset(danmaxVar::beamMirrorShift, 0.0); // [10]
  // width / heigh / depth / length
  MBoxGen.generateBox
    (
      Control,monoKey+"MLMVessel",
      59.0-2.0*MLMWallThick, // [10]
      MLMHeightAboveOpticalAxis-MLMRoofThick,
      MLMHeight-MLMHeightAboveOpticalAxis-MLMFloorThick,
      MLMTotalLength-2.0*MLMWallThick-2.0*MLMPortLength
    );

  const std::string portName=monoKey+"MLMVessel";
  Control.addVariable(monoKey+"MLMVesselNPorts",0);
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.setPlate(4.0,"LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
			Geometry::Vec3D(0,5.0,-10.0),
			Geometry::Vec3D(1,0,0));

  // crystals gap 4mm
  MXtalGen.generateMono(Control,monoKey+"MLM",-10.0,0.3,0.3);

  return MLMTotalLength-MLMFrontToTopViewPort;
}

void
monoShutterVariables(FuncDataBase& Control,
		     const std::string& preName)
  /*!
    Construct Mono Shutter variables
    \param Control :: Database for variables
    \param preName :: Control ssytem
   */
{
  ELog::RegMethod RegA("danmaxVariables","monoShutterVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;

  // up / up (true)
  MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,preName+"MonoAdaptorA",4.0);
  PipeGen.setAFlangeCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,preName+"MonoAdaptorB",4.0);


  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowL",10.0);


  return;
}

template<typename JoinPipeCCF>
void
shieldVariables(FuncDataBase& Control)
  /*!
    Shield variables
    \param Control :: DataBase to add
  */
{
  ELog::RegMethod RegA("danmaxVariables","shieldVariables");

  const std::string preName("DanMAX");

  Control.addVariable(preName+"GuillotineLength",0.6); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWidth",40.0); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineHeight",40.0); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWallThick",0.0); // Coupe C-C [2], no guillotine wall shown
  Control.addVariable(preName+"GuillotineClearGap",4.1-2.0*(JoinPipeCCF::innerRadius+JoinPipeCCF::wallThick)); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWallMat","SteelUnknownGrade");
  Control.addVariable(preName+"GuillotineMat","Lead");

  Control.addVariable(preName+"NShieldYStep",10.2);
  Control.addVariable(preName+"NShieldLength",7.0);
  Control.addVariable(preName+"NShieldWidth",80.0);
  Control.addVariable(preName+"NShieldHeight",80.0);
  Control.addVariable(preName+"NShieldWallThick",0.5);
  Control.addVariable(preName+"NShieldClearGap",0.2);
  Control.addVariable(preName+"NShieldWallMat","SteelUnknownGrade");
  Control.addVariable(preName+"NShieldMat","Poly");

  Control.addVariable(preName+"OuterShieldYStep",10.2);
  Control.addVariable(preName+"OuterShieldLength",5.0);
  Control.addVariable(preName+"OuterShieldWidth",80.0);
  Control.addVariable(preName+"OuterShieldHeight",80.0);
  Control.addVariable(preName+"OuterShieldWallThick",0.5);
  Control.addVariable(preName+"OuterShieldClearGap",0.2);
  Control.addVariable(preName+"OuterShieldWallMat","SteelUnknownGrade");
  Control.addVariable(preName+"OuterShieldMat","Poly");
  return;
}


double
opticsSlitPackage(FuncDataBase& Control,
		  const std::string& opticsName, const double slitTubeFrontToTopPort)
  /*!
    Builds the DM2 slit package

    \param Control :: Function data base for variables
    \param opticsName :: PreName
    \param slitTubeFrontToTopPort :: Distance from the front surface to the
    port that defines the absolute position.
   */
{
  setVariable::PortTubeGenerator PortTubeGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BeamPairGenerator BeamMGen;

  const std::string sName=opticsName+"SlitTube";
  const double tubeLength = 48.5; // Outer length [10]
  PortTubeGen.setPipeCF<CF200>(); // [10]
  // [10] TODO: This is only the thickness of the hull.
  // The front and back walls are actually 1 mm thicker, but the present
  // PortTubeGenerator does not distinguish between these walls.
  const double wallThick = 0.2;
  PortTubeGen.setWallThick(0.2);

  PortTubeGen.setPortCF<CF40>(); // [10]
  const double topPortPipeToSlitTubeFront = 4.4; // [10]
  // Front port length determined by the Y offset.
  const double frontPortLength = slitTubeFrontToTopPort-CF150::outerRadius-topPortPipeToSlitTubeFront;
  const double backPortLength = 2.5; // [10]
  PortTubeGen.setPortLength(frontPortLength, backPortLength);
  const double totalLength = tubeLength+frontPortLength+backPortLength;
  PortTubeGen.generateTube(Control,sName,0.0,tubeLength-2.0*wallThick);

  Control.addVariable(sName+"NPorts",4);
  // [10] Length of Port0 and Port1 (they are the same within a few millimeters).
  PItemGen.setCF<setVariable::CF150>(CF200::innerRadius+wallThick+14.4);
  PItemGen.setPlate(setVariable::CF150::flangeLength,"SteelUnknownGrade");

  const Geometry::Vec3D topJaw(
    0.0,
    frontPortLength+topPortPipeToSlitTubeFront+CF150::outerRadius-totalLength/2.0,
    0.0
  );
  const Geometry::Vec3D sideJaw(
    0.0,
    frontPortLength+7.9+CF150::outerRadius-totalLength/2.0,
    0.0); // [10]
  const Geometry::Vec3D vacPort(0.0,0.0,0.0); // [10]
  const Geometry::Vec3D beamViewer1(
    0.0,
    frontPortLength+topPortPipeToSlitTubeFront+CF150::outerRadius
    +danmaxVar::absY::beamViewer1Y
    -danmaxVar::absY::whiteBeamSlitsTopJawY-totalLength/2.0,
    0.0
  );

  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,sName+"Port0",topJaw,ZVec);
  PItemGen.generatePort(Control,sName+"Port1",sideJaw,XVec);
  PItemGen.setCF<setVariable::CF150>(CF200::innerRadius+wallThick+24.0); // [10]
  PItemGen.generatePort(Control,sName+"Port2",vacPort,-ZVec);
  PItemGen.setCF<setVariable::CF100>(CF200::innerRadius+wallThick+5.9); // [10]
  PItemGen.generatePort(Control,sName+"Port3",beamViewer1,ZVec);

  // Jaw units:
  BeamMGen.setThread(0.5,"Nickel"); // Estimated
  BeamMGen.setLift(0.6, 0.6); // "Maximum aperture" given as 10 mm x 10 mm [13]
  BeamMGen.setGap(-0.1,-0.1); // "Maximum overlap" given as 2 mm in [13]
  // Width, height, and angle from [13]
  // Thickness: Estimated from the given coolant-connection radius of 8 mm in [13]
  BeamMGen.setBlock(5.0,3.5,1.0,0.0,"TungstenCarbide"); // [13]
  BeamMGen.setWaterPipes(2, 1);

  // Seen from upstream, the beam hits the jaws in the following order [10]:
  // Top (JawX B) -> Right (JawZ A) -> Bottom (JawX A) -> Left (JawZ B)
  const double bladeOffset = 3.6; // [10]
  BeamMGen.setXYStep(0.0,bladeOffset,0.0,-bladeOffset);
  BeamMGen.generateMount(Control,opticsName+"JawX",0);
  BeamMGen.setXYStep(-bladeOffset,0.0,bladeOffset,0.0);
  BeamMGen.generateMount(Control,opticsName+"JawZ",0);

  return totalLength-slitTubeFrontToTopPort;
}

void
opticsVariables(FuncDataBase& Control,
		const std::string& beamName)
  /*!
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
    \param beamName :: beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","opticsVariables");

  const std::string opticsName(beamName+"OpticsLine");

  Control.addVariable(opticsName+"OuterLeft",70.0);
  Control.addVariable(opticsName+"OuterRight",60.0);
  Control.addVariable(opticsName+"OuterTop",70.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::SqrFMaskGenerator FMaskGen;

  setVariable::GateValveGenerator GateGen;

  PipeGen.setNoWindow();
  PipeGen.setMat("SteelUnknownGrade");
  BellowGen.setCF<setVariable::CF40>(); // [10]
  BellowGen.setBellowStep(2.5); // [10]
  BellowGen.generateBellow(Control,opticsName+"InitBellow",10.0); // [4]

  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  // Total length of 200 mm given in [4], object is symmetric.
  TGen.setMainLength(10.0, 10.0);
  TGen.setPortCF<setVariable::CF40>(); // [10]
  TGen.setSideCF<setVariable::CF40>(10.0); // [10]
  TGen.generateTube(Control,opticsName+"TriggerUnit");

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve4"); // [10]

  const double bremColl1Length = 29.0; // [10]
  const double bremcoll1Height = 44.0; // [10]
  // Distance measured from top of beam pipe in [10], not from its center, therefore
  // need to add beam-pipe radius.
  const double bremColl1TopHeight = 8.2 + setVariable::CF40::innerRadius;
  const double bremColl1Z = bremcoll1Height/2.0-bremColl1TopHeight;

  PipeGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,opticsName+"BellowA",
    danmaxVar::absY::bremColl1Y-bremColl1Length/2.0-danmaxVar::absY::valve4BackY);
  // Reset bellow step to default value [10]. Large bellow step of InitBellow and
  // BellowA looked odd on the short bellows.
  BellowGen.setBellowStep(1.0);
  BellowGen.generateBellow(Control,opticsName+"BellowB",16.0);

  const std::string bremColl1TubeName = opticsName+"BremColl1Tube";
  SimpleTubeGen.setCF<setVariable::CF160>();
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,bremColl1TubeName,bremcoll1Height);
  Control.addVariable(bremColl1TubeName+"NPorts",2);
  PItemGen.setCF<setVariable::CF40>(bremColl1Length/2.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control, bremColl1TubeName+"Port0",
    Geometry::Vec3D(0,bremColl1Z,0),
		Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control, bremColl1TubeName+"Port1",
    Geometry::Vec3D(0,bremColl1Z,0),
		Geometry::Vec3D(0,0,-1));

  const std::string bremColl1Name = opticsName+"BremColl1";
  // TODO: Material currently set to pure tungsten (default), but should be
  // DENSIMET [13].
  BremGen.centre();
  BremGen.setLength(8.0); // Sec. 2.1 in [13]
  BremGen.setCube(10.0,10.0); // Sec. 2.1 in [13]
  // All dimensions from Sec. 2.1 in [13], except aperture position.
  // The aperture position is not mentioned in [13], but was read off from [10].
  // Aperture dimensions can be given in two ways, either as an angle or by
  // setting the cross-section dimensions at different positions. Tried both ways,
  // once with the angle from [13] and once with dimensions read off from [10].
  // Since the entrance/exit dimensions agreed within a few percent, decided to use
  // the angle, since its value is given explicitly.
  BremGen.setAperatureAngle(2.0, 0.2, 0.2, 5.0, 5.0);
  BremGen.generateBlock(Control,bremColl1Name,bremColl1Z);
  Control.addVariable(bremColl1Name+"XAngle",90);

  // High Pass Filter
  // Simplified to a pipe with two 'windows' corresponding to the two diamond filters.
  PipeGen.setRectWindow(0.6,0.6,0.06,0.6,0.6,0.04); // [13]
  PipeGen.setWindowMat("Diamond", "Diamond"); // [13]
  // Length adjusted to fit the position given in [12]
  const double highPassFilterLength =
    2.0*(
      danmaxVar::absY::highPassFilterY-danmaxVar::absY::bremColl1Y-bremColl1Length/2.0
  );
  PipeGen.generatePipe(Control,opticsName+"HighPassFilter",
    highPassFilterLength);

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve5"); // [10]

  const double bellowCDLength = 8.0; // Dummy
  // Offset of the slit tube along the Y axis.
  //
  // Front Port Length
  // + Distance Top Port Pipe to Slit Tube Front
  // + Top Port Radius
  const double slitTubeFrontToTopPort = 2.5+4.4+CF150::outerRadius; // [10]

  PipeGen.setNoWindow();
  PipeGen.generatePipe(Control,opticsName+"PipeA",12.5); // [16]
  BellowGen.generateBellow(Control,opticsName+"BellowC",11.8); // [16]


  // Laue monochromator
  PipeGen.generatePipe(
    Control,opticsName+"LauePipe",
    danmaxVar::absY::whiteBeamSlitsTopJawY-danmaxVar::absY::highPassFilterY
    -0.5*highPassFilterLength-valve3Length
    -2.0*bellowCDLength-slitTubeFrontToTopPort
  );
  BellowGen.generateBellow(Control,opticsName+"BellowD",8.0);

  const double slitTubeTopPortToBack = opticsSlitPackage(
    Control,opticsName,slitTubeFrontToTopPort);

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve6"); // [10]
  // Control.addVariable(opticsName+"Valve6YAngle", 90.0); // [10]

  // Distance from the front plane to the center of the mono vessel:
  // port A length + vessel radius + vessel wall thickness
  const double HDCMOffsetY = 4.5 + 30.0 + 0.5;
  BellowGen.generateBellow(
    Control,opticsName+"BellowE",
    danmaxVar::absY::HDCMY-danmaxVar::absY::whiteBeamSlitsTopJawY
    -slitTubeTopPortToBack-valve3Length-HDCMOffsetY
  );

  const double HDCMCenterToBack = monoPackage(Control,opticsName);

  const double beamViewer2FrontToPort = 9.9; // [10]
  BellowGen.generateBellow(
    Control,opticsName+"BellowAfterMono",
    danmaxVar::absY::beamViewer2Y-danmaxVar::absY::HDCMY
    -valve3Length-beamViewer2FrontToPort-HDCMCenterToBack
  );

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve7"); // [10]
  // Angle roughly adjusted to [10]. Found it difficult to read off from the model.
  // Control.addVariable(opticsName+"Valve7YAngle", -20.0);

  const double beamViewer2PortToBack = viewPackage(
    Control,opticsName,beamViewer2FrontToPort);

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve8"); // [10]
  // Control.addVariable(opticsName+"Valve8YAngle", 90.0); // [10]

  const double MLMFrontToTopViewPort = 21.95; // [10]
  BellowGen.generateBellow(Control,opticsName+"BellowF",
    danmaxVar::absY::MLMY-danmaxVar::absY::beamViewer2Y-beamViewer2PortToBack
    -valve3Length-MLMFrontToTopViewPort
  );

  const double MLMTopViewPortToBack = mirrorMonoPackage(
    Control,opticsName, MLMFrontToTopViewPort);
  const double beamStopFrontToWBPort = 2.5+CF40::flangeLength+5.2;
  BellowGen.generateBellow(Control,opticsName+"BellowG",
    danmaxVar::absY::whiteBeamStopY-danmaxVar::absY::MLMY
    -MLMTopViewPortToBack-valve3Length-beamStopFrontToWBPort
  );

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve9"); // [10]
  // Angle estimated from [10]
  // Control.addVariable(opticsName+"Valve9YAngle", 135.0);

  const double monoSlitsToBack = beamStopPackage(
    Control,opticsName,beamStopFrontToWBPort);

  const double mainTubeFrontToBeamViewerPort = 26.0; // [10]
  BellowGen.generateBellow(Control,opticsName+"BellowH",
    danmaxVar::absY::beamViewer3Y-danmaxVar::absY::monoSlitsY
    -mainTubeFrontToBeamViewerPort-monoSlitsToBack);

  const double mainTubeBeamViewerPortToBack = viewBPackage(
    Control,opticsName,mainTubeFrontToBeamViewerPort);

  const double CRLFrontToCenter = 49.445/2.0; // [10]
  const double CRLGateTotalLength = 3.5+2.0*CF40::flangeLength; // [10]
  BellowGen.generateBellow(Control,opticsName+"BellowI",
    danmaxVar::absY::CRLY-danmaxVar::absY::beamViewer3Y-mainTubeBeamViewerPortToBack
    -CRLFrontToCenter-CRLGateTotalLength);

  GateGen.setCylCF<setVariable::CF40>(); // [10]
  GateGen.setLength(CRLGateTotalLength-2.0*CF40::flangeLength);
  GateGen.generateValve(Control,opticsName+"CRLGateIn",0.0,0);

  const double CRLCenterToBack = lensPackage(Control,opticsName,CRLFrontToCenter);

  GateGen.generateValve(Control,opticsName+"CRLGateOut",0.0,0);

  // Connector length (estimated) + half the length of the pipe that contains the
  // monochromatic slits [10]
  const double revMonoSlitsFrontToSlits = 4.0+5.25;
  BellowGen.generateBellow(
    Control,opticsName+"BellowJ",danmaxVar::absY::monoSlits2Y
    -danmaxVar::absY::CRLY-CRLCenterToBack-CRLGateTotalLength-revMonoSlitsFrontToSlits);

  revBeamStopPackage(Control,opticsName,revMonoSlitsFrontToSlits);

  BellowGen.generateBellow(Control,opticsName+"BellowK",10.0);

  monoShutterVariables(Control,opticsName);
  GateGen.setBladeThick(0.3);
  GateGen.generateValve(Control,opticsName+"GateG",0.0,0);

  return;
}

}  // NAMESPACE danmaxVar


void
support7DanMAX(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the Toyama DanMAX-like front-end shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","support7");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::ProximityShieldingGenerator PSGen;
  setVariable::BremBlockGenerator BBGen;

  constexpr double bellowILength = 10.0; // [4]
  constexpr double florTubeALength = 12.0; // "Flourescent Screen" [4]
  constexpr double bellowJLength = 10.0; // [4]
  constexpr double proxiShieldAPipeLength = 20.0; // [4]
  constexpr double proxiShieldBPipeLength = 20.0; // [4]
  // safety shutter
  constexpr double shutterLength = 57.8; // [4]
  constexpr double offPipeALength = 6.8; // approx
  constexpr double shutterBoxLength = shutterLength - offPipeALength;

  constexpr double bremCollTotalLength = 21.0; //[4] (OffPipeB + BremCollPipe)
  constexpr double offPipeBLength = 2.6; // as small as possible
  constexpr double bremCollPipeLength = bremCollTotalLength - offPipeBLength;

  constexpr double proxiShieldBPipeEnd = 2110.0 - 2.97; // [4, page1]
  constexpr double bellowIYstep = proxiShieldBPipeEnd - proxiShieldBPipeLength -
    bremCollTotalLength - shutterLength - proxiShieldAPipeLength - danmaxVar::valve3Length -
    bellowJLength - florTubeALength - bellowILength;
  // same as counting from Movable Mask 2
  // 18692.8 + 300 + 140 + 17.5 + 340 = 19490.3
  assert(std::abs(bellowIYstep - 1949.03)<Geometry::zeroTol && "Wrong shutter table length."); // [4]

  BellowGen.setMat("SteelUnknownGrade", "SteelUnknownGrade");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",bellowILength);
  Control.addVariable(frontKey+"BellowIYStep",bellowIYstep);

  SimpleTubeGen.setCF<CF66_TDC>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",16.0);
  Control.addVariable(frontKey+"FlorTubeARadius",7.63/2-0.2);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+2.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  Control.addVariable(florName+"Port0Length",6);
  Control.addVariable(florName+"Port1Length",6);


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",bellowJLength);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",offPipeALength);
  Control.addVariable(frontKey+"OffPipeAFlangeBZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  SimpleTubeGen.setCF<CF200>(); // [7]
  SimpleTubeGen.setCap(0,0);
  SimpleTubeGen.setFlangeLength(0.0, 0.0); // Remove flanges [7]
  SimpleTubeGen.generateTube(Control,shutterName,shutterBoxLength);
  Control.addVariable(shutterName+"NPorts",2);
  Control.addVariable(shutterName+"WallThick",0.3); // [7]
  Control.addVariable(shutterName+"Radius",9.85); // [7]

  // 20cm above port tube
  PItemGen.setCF<setVariable::CF50>(14.0);
  PItemGen.setPlate(setVariable::CF50::flangeLength,"Stainless304");
  BeamMGen.setThread(1.0,"Nickel");
  BeamMGen.setLift(6.0,0.0); // [7]
  BeamMGen.setCentreBlock(6.0,6.0,20.0,0.0,"Tungsten"); // [7]

  // Build bocks symmetrically around center of shutter box
  // such that the center-center distance is 25 cm;
  Geometry::Vec3D CPos(0, -12.5,0); // [7]
  for(size_t i=0;i<2;i++)
    {
      const std::string name=frontKey+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=frontKey+"BS"+std::to_string(i+1);

      PItemGen.generatePort(Control,name,CPos,ZVec);
      BeamMGen.generateMount(Control,fname,0);      // out of beam
      CPos+=Geometry::Vec3D(0,25.0,0); // [7]
    }

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",offPipeBLength);
  Control.addVariable(frontKey+"OffPipeBFlangeAZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);
  Control.addVariable(frontKey+"OffPipeBFlangeBType",0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  PSGen.generate(Control,frontKey+"ProxiShieldA", 10.0); // CAD
  Control.addVariable(frontKey+"ProxiShieldAYStep",3.53); // CAD
  Control.addVariable(frontKey+"ProxiShieldABoreRadius",0.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,frontKey+"ProxiShieldAPipe",proxiShieldAPipeLength);

  Control.copyVarSet(frontKey+"ProxiShieldAPipe", frontKey+"ProxiShieldBPipe");
  Control.copyVarSet(frontKey+"ProxiShieldA", frontKey+"ProxiShieldB");
  Control.addVariable(frontKey+"ProxiShieldBPipeLength",proxiShieldBPipeLength);
  Control.addVariable(frontKey+"ProxiShieldBPipeFlangeARadius",7.5);
  Control.addVariable(frontKey+"ProxiShieldBYStep",setVariable::CF40::flangeLength+0.1); // approx

  // Bremsstrahlung collimator
  std::string name;
  name=frontKey+"BremCollPipe";
  constexpr double bremCollLength(20.0); // collimator block inside BremCollPipe:  CAD+[6]

  constexpr double bremCollRadius(3.0); // CAD and [6]
  PipeGen.setCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,name,bremCollPipeLength);
  constexpr double bremCollPipeInnerRadius = 4.35; // [6]
  Control.addVariable(name+"Radius",bremCollPipeInnerRadius);
  Control.addVariable(name+"FlangeARadius",bremCollPipeInnerRadius+CF100::wallThick);
  Control.addVariable(name+"FlangeALength",1.0); // Estimated from [6]
  Control.addVariable(name+"FlangeBRadius",7.5); // CAD
  Control.addVariable(name+"FlangeAInnerRadius",bremCollRadius);
  Control.addVariable(name+"FlangeBInnerRadius",bremCollRadius);

  BBGen.centre();
  BBGen.setMaterial("Tungsten", "Void");
  BBGen.setLength(bremCollLength);
  BBGen.setRadius(bremCollRadius);
  BBGen.setAperature(-1.0, 1.5, 0.7,  1.5, 0.7,   1.5, 0.7); // [6]
  BBGen.generateBlock(Control,frontKey+"BremColl",0);
  Control.addVariable(frontKey+"BremCollYStep",(bremCollPipeLength-bremCollLength)/2.0);

  return;
}


void
DANMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","danmaxVariables");

  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  //  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setNoWindow();

  const std::string beamLineName("DanMAX");
  const std::string frontKey(beamLineName+"FrontBeam");

  danmaxVar::undulatorVariables(Control,frontKey);
  setVariable::R3FrontEndToyamaVariables(Control,beamLineName);
  //  Control.addVariable("DanmaxFrontBeamXStep",beamXStep);

  support7DanMAX(Control,frontKey);

  setVariable::CrossGenerator CrossGen;

  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  constexpr double ionPump3Length = 20.0; // [4]
  // lengths of ports from origin (back, front):
  CrossGen.setTotalPorts(ionPump3Length/2.0, ionPump3Length/2.0);
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPump3",0.0,13.3,26.6); // yStep, height, depth (dummy)

  CrossGen.setMat("SteelUnknownGrade");
  CrossGen.setPlates(0.5,2.0,2.0);     // wall/Top/base
  CrossGen.setTotalPorts(7.0,7.0);     // len of ports (after main): 14 in total [4]
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPump4",0.0,15.74,28.70);   // height/depth


  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"PipeA",24+11.0); // [4] - replacement for pumping unit
  setVariable::BladeBPMToyamaGenerator XBPMGen;

  XBPMGen.generate(Control,frontKey+"XBPM1", danmaxVar::absY::XBPM1);


  danmaxVar::frontMaskVariables(Control,frontKey);

  Control.addVariable(frontKey+"ProxiShieldAWallMat","Void"); // [AR 251104: checked by JR 251103]
  Control.addVariable(frontKey+"ProxiShieldBWallMat","Void"); // [AR 251104: checked by JR 251103]

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,beamLineName+"JoinPipe",146.0); // [4]

  danmaxVar::opticsHutVariables(Control,beamLineName+"OpticsHut");
  danmaxVar::opticsVariables(Control,beamLineName);

  PipeGen.setCF<setVariable::CF40>();
  // dummy, adjusted such that JoinPipeB extends 100 mm into Experimental Hutch 2.
  PipeGen.generatePipe(Control,beamLineName+"JoinPipeB",38.93);

  danmaxVar::shieldVariables<setVariable::CF40>(Control);
  danmaxVar::connectVariables(Control,beamLineName+"ConnectUnit");

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setWindow(2.7, 0.005);
  PipeGen.setWindowMat("Diamond");
  PipeGen.generatePipe(Control,beamLineName+"JoinPipeC",94.0);

  danmaxVar::exptHut1Variables(Control,beamLineName);
  danmaxVar::exptHut2Variables(Control,beamLineName);

  const std::string exptName=beamLineName+"ExptLine";

  Control.addVariable(exptName+"BeamStopYStep",806.0);
  Control.addVariable(exptName+"BeamStopRadius",10.0);
  Control.addVariable(exptName+"BeamStopThick",5.0);
  Control.addVariable(exptName+"BeamStopMat","SteelUnknownGrade");

  Control.addVariable(exptName+"SampleYStep",406.0);
  Control.addVariable(exptName+"SampleRadius",10.0);
  Control.addVariable(exptName+"SampleMat","Copper");

  return;
}

}  // NAMESPACE setVariable
