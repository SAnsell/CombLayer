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

#include "DNFlanges.h"
#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "BellowGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacuumBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamPairGenerator.h"
#include "MonoShutterR3Generator.h"
#include "MonoSlitsJJGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "FixedMaskHybridGenerator.h"
#include "PortChicaneGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "DCMTankGenerator.h"
#include "MonoBlockXstalsGenerator.h"
#include "MLMonoGenerator.h"
#include "XRayHutchBaseGenerator.h"
#include "OpticsHutchGenerator.h"
#include "ExptHutGenerator.h"
#include "CrossGenerator.h"
#include "BeamMountGenerator.h"
#include "BremBlockGenerator.h"
#include "HeatAbsorberR3ToyamaGenerator.h"
#include "ProximityShieldingGenerator.h"
#include "BladeBPMToyamaGenerator.h"
#include "FlangePlateGenerator.h"
#include "TwinPipeGenerator.h"
#include "SmallAngleBellowsGenerator.h"
#include "WhiteBeamStopGenerator.h"

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
// [10] CAD model of DanMAX/SINCRYS, J. Selberg, fall/winter 2025 (/mxn/groups/rad/Beamlines/DanMAX/Simulations)
// [11] https://www.gammavacuum.com/detail/index/sArticle/3381/number/100LCV6SSCNN/sCategory/17668 (accessed on 2026-01-14)
// [12] DanMAX Beamline, FMB Oxford Drawing ABM0070
// [13] DanMAX Diagnostics, Functional Specification, FMB Oxford S3716, Rev. 5.0
// [14] DanMAX HDCM, Funcation Specification, FMB Oxford S3716, Rev. 6.0
// [15] Detailed Design Report for DanMAX, v2.0, Dec. 2017
// [16] S1-6-1AB01567.pdf
// [17] S7-2-1AB01538.pdf
// [18] /mxn/groups/rad/Beamlines/DanMAX/Simulations/FE_02.STEP, see also [10]
// [19] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM3/aqm0222.stp
// [20] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/CRL/azm0089 - CRL TOP LEVEL.stp
// [21] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM5/aqm0223.stp
// [22] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM4/AQM0218.stp
// [23] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM6/AQM0221.stp
// [24] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_DCM_MLM/Stp Files/HDCM/ADM0341 - Top Level.stp
// [25] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_DCM_MLM/Stp Files/MLM/ADM0342 - MLM Top Level.stp
// [26] /mxn/groups/rad/Beamlines/DanMAX/Simulations/02_OH.STEP, see also [10]
// [27] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM2/aqm0217.stp
// [28] /mxn/groups/rad/Beamlines/DanMAX/Simulations/01_OH.STEP, see also [10]
// [29] /mxn/groups/rad/Beamlines/DanMAX/Drawings/DanMAX/Optics/FDR_Diagnostics/Stp Files/DM1/AQM0214.stp
// [30] CAD model 01_OH.x_t /mxn/groups/rad/Beamlines/DanMAX/Simulations/01_OH.x_t
// [31] SINCRYS layout CM1, Drawing 256569, 2025-05-06
// [32] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
// [33] JJ X-RAY, SINCRYS layout drawing, 23087, 2025-06-27 (Appendix to [32])
// [34] /mxn/groups/rad/Beamlines/DanMAX/Simulations/03_OH.STEP, see also [10]
// [35] S3716 DanMAX MLM Functional Specification Rev05_NEW.pdf
// [36] Verbal communication with Mads and Sergio 260311 (viewing window parameters: thickness 3 mm, standard Glass, radius: same as the pipe inner radius) + see email from KB 260311
// [37] CAD model CM2_chamber_2026-03-04.STEP /mxn/groups/rad/Beamlines/DanMAX/Simulations/CM2_chamber_2026-03-04.STEP

namespace setVariable
{

namespace danmaxVar
{
  namespace absY{
    constexpr double XBPM1 = 1201.99; // [4]
    constexpr double XBPM2 = 1510.63; // [4]
    const double beamPipe2Port = 1048.9; // [4]
    const double FM1 = 1104.75; // [4]
    const double FM2 = 1597.08; // [4]
    const double heatAbsorber = 1673.33; // [4]
    const double opticsHutFront = 2250.0; // [4]
    // Absolute position of the downstream side of Valve4 [4].
    // This value is used to determine the lengths of elements in the optics hutch that
    // are not shown in [12].
    double valve4Back = 2290.23; // [4]
    // All following absolute coordinates taken from [12] if not indicated otherwise.
    const double bremColl1 = 2330.0;
    const double highPassFilter = 2347.48;
    // Using information from [12] and [31]. In the latter, the distance of CM1 from
    // the bremsstrahlung collimator is shown.
    const double CM1 = bremColl1 + 62.26;
    // Assume that the Y coordinate given there is the central axis of the top-jaw
    // port.
    const double whiteBeamSlitsTopJaw = 2622.75;
    const double beamViewer1 = 2647.72;
    const double HDCM = 2715.50;
    const double beamViewer2 = 2784.0;
    const double MLM = 2834.8; // [12] Position of the view port viewing at the 1st crystal
    const double whiteBeamStop = 2954.83;
    const double bremColl2 = 2965.03;
    const double monoSlits = 2987.56;
    const double beamViewer3 = 3036.92;
    const double CRL = 3091.77;
    const double monoSlits2 = 3143.94;
    const double bremColl3 = 3166.47;
    const double monoShutter = 3211.25; // [31]
  }
// "V3 Valve" [4] Determines the length of several valves of the same type.
constexpr double valve3Length = 7.2;
const double SINCRYSBranchShift = -98.0; // [31,33]
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
// Drawings give the following values for the "outer width" of the DanMAX hutches:
// Optics Hutch  : 2957 mm (see below)
// Expt. Hutch 2 : 2602 mm (Section A-A in [3])
// Expt. Hutch 1 : 2602 mm (Section A-A in [2])
// As can be seen, there is a discrepancy that would create a gap.
// For simplicity, it was decided to use the outer width of the Optics Hutch for all
// hutches.
const double opticsHutOuterWidth = 259.7; // Section A-A in [1]
  // The actual length of hutch 2 is 5366 mm as shown in [3].
  // However, to match the simplified model of the optics hutch, use the slightly
  // larger value given in [8].
const double exptHut2Length = 545.8;

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

  setVariable::FixedMaskHybridGenerator FMHGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator PTGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::SqrFMaskGenerator FMaskGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::FlangePlateGenerator FPGen;

  PipeGen.setCF<CF40>();
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.generatePipe(Control,preName+"BeamPipe1",165.5); // [4]

  PTGen.setCF<CF40>(); // [4]
  PTGen.setMat("SteelUnknownGrade");
  const std::string pipeName = preName+"BeamPipe2";
  const double flangePlateAThick = CF63::flangeLength; // [18]
  const double bellowALength = 10.0; // [4]
  const double beamPipe2Length = 51.5; // [4]
  PTGen.generateTube(Control,pipeName,beamPipe2Length);
  Control.addVariable(pipeName+"NPorts",1);
  PItemGen.setCF<CF40>(7.0); // [4]
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  PItemGen.generatePort(Control,pipeName+"Port0",
    Geometry::Vec3D(
      0,
      -0.5*beamPipe2Length+danmaxVar::absY::FM1-danmaxVar::absY::beamPipe2Port
      -flangePlateAThick-bellowALength,
      0
    ),
    Geometry::Vec3D(1,0,0) // [4] and [18] show different orientations. Using [18].
  );

  Control.addVariable(preName+"BellowALength",bellowALength);
  Control.addVariable(preName+"BellowABellowStep",2.5); // [18]

  FPGen.setFlange(CF100::flangeRadius, flangePlateAThick); // [4]
  FPGen.setWindow(0.0, 0.0, "Void"); // [4]
  FPGen.setMat("SteelUnknownGrade");
  FPGen.setInnerRadius(CF40::innerRadius); // [4]
  FPGen.generateFlangePlate(Control,preName+"FlangePlateA");

  constexpr double FM1Length(40.0); // [4]
  constexpr double FM2Length(50.5); // [5]

  const double FM2dist(danmaxVar::absY::FM2+FM2Length/2.0); // [4]
  // MSM not used
  // const double MSMdist(1600.0);

  double backWidth = 1.1; // ???
  double backHeight = backWidth;
  FMHGen.setFrontRadius(2.9);
  FMHGen.setBackGap(backWidth, backHeight);
  FMHGen.generate(Control,preName+"FM1H",danmaxVar::absY::FM1,FM1Length);

  // Radius adjusted to XBPM flange [30], Thickness from [4]
  FPGen.setFlange(7.6, 1.99);
  FPGen.setWindow(0.0, 0.0, "Void"); // [4]
  FPGen.setMat("SteelUnknownGrade"); // guess
  FPGen.setInnerRadius(CF40::innerRadius);  // guess (same as BellowA)
  FPGen.generateFlangePlate(Control,preName+"FlangePlateAA"); // TODO: move to Toyama DanMAX front-end
  FPGen.generateFlangePlate(Control,preName+"FlangePlateXBPM2"); // TODO: move to Toyama DanMAX front-end

  Control.addVariable(preName+"BellowBLength",10.0); // [4]
  Control.addVariable(preName+"BellowCLength",10.0); // [4]
  Control.addVariable(preName+"BellowFLength",14.0); // [4]
  Control.addVariable(preName+"BellowHLength",14.0); // [4]
  Control.addVariable(preName+"PipeCLength",34.0); // [4]

  // magnet center to FM2 front: 170.5 cm, measured by AR
  Control.addVariable(preName+"PermanentMagnetYStep",-170.5); // 170.5 cm, measured by AR

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
  BellowGen.generateBellow(Control,preName+"BellowCA",10.0); // [4]

  FPGen.setCF<CF63>(CF40::innerRadius); // [4]
  FPGen.setFlangeLen(1.75); // [18]
  FPGen.generateFlangePlate(Control,preName+"FlangePlateD");
  BellowGen.setCF<setVariable::CF63>(); // [18]
  BellowGen.generateBellow(Control,preName+"BellowPreHA",14.0); // [4]
  BellowGen.generateBellow(Control,preName+"BellowPostHA",14.0); // [4]
  FPGen.generateFlangePlate(Control,preName+"FlangePlateE"); // [4]

  HeatAbsorberR3ToyamaGenerator HAGen;

  constexpr double heatAbsorberLength = 26.5;  // [4]

  HAGen.generate(Control,preName+"HeatAbsorber",heatAbsorberLength);
  Control.addVariable(preName+"HeatAbsorberYStep",danmaxVar::absY::heatAbsorber);

  FPGen.generateFlangePlate(Control,preName+"FlangePlateF"); // [4]
  FPGen.generateFlangePlate(Control,preName+"FlangePlateG"); // [4]

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
  Control.addVariable(connectName+"XStep",danmaxVar::beamMirrorShift);

  BellowGen.setMat("SteelUnknownGrade","SteelUnknownGrade%Void%10.0");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,beamName+"BellowA",16.0);

  PipeGen.setAFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"FlangeA",5.0);

  // Gamma Vacuum 100L TiTan Ion Pump [9-11]
  // Geometry simplified to a single pipe with the appropriate standard.
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<setVariable::CF100>();
  const double ionPumpALength = 32.59;
  PipeGen.generatePipe(Control,beamName+"IonPumpA", ionPumpALength);
  Control.addVariable(beamName+"IonPumpAXStep",
    danmaxVar::beamMirrorShift);
  Control.addVariable(beamName+"IonPumpAYStep",
    (danmaxVar::exptHut2Length-ionPumpALength)/2.0);

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
  Control.addVariable(hutName+"OutWidth",opticsHutOuterWidth);
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

  EGen.generateHut(Control,hutName,0.0,danmaxVar::exptHut2Length);

  Control.addVariable(hutName+"RingWidth",47.3); // Section A-A [3]
  Control.addVariable(hutName+"OutWidth",opticsHutOuterWidth);
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
  const double x0 = (danmaxVar::exptHut2Length*skinThick-backLead)/2.0;
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

void viewPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
    defines the absolute position.
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","viewPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  // will be rotated vertical
  const double beamViewerFrontToPort = 9.9; // [19]
  const std::string pipeName=viewKey+"ViewTube";
  SimpleTubeGen.setCF<CF100>(); // [19]
  SimpleTubeGen.setCap();
  const double mainTubeAboveOpticalAxis = 15.0; // [19]
  const double mainTubeBelowOpticalAxis = 32.5; // [19]
  const double offsetZ = (mainTubeBelowOpticalAxis-mainTubeAboveOpticalAxis)/2.0;
  SimpleTubeGen.generateTube(
    Control,pipeName,mainTubeAboveOpticalAxis+mainTubeBelowOpticalAxis);


  Control.addVariable(pipeName+"YStep",
    danmaxVar::absY::beamViewer2-beamViewerFrontToPort);
  Control.addVariable(pipeName+"NPorts",3);

    const double connectPortLength = beamViewerFrontToPort-CF100::outerRadius;

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+connectPortLength);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(0,0,-1));
  PItemGen.setPlate(0.0,"SteelUnknownGrade");
  PItemGen.setCF<setVariable::CF40>(sqrt(2.0)*CF100::outerRadius+10.0); // [19]
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,offsetZ,0),
			Geometry::Vec3D(-1,0,1));

  // Otherwise geometric error between port 0 and 2, even though we
  // intersect them in constructViewScreen.  If BuildZone cells become
  // too comples, will have to do more sophisticated (manual)
  // intersection
  Control.addVariable(pipeName+"Port0OuterVoid",0);
  Control.addVariable(pipeName+"Port2OuterVoid",0);

  FlangeGen.setNoPlate();
  // Most blade data from [13].
  //
  // Angle from [19].
  //
  // Material is actually given as "pCVD Diamond" in [13].
  FlangeGen.setBlade(3.3,1.3,0.2,-45.0,"Diamond",1);
  FlangeGen.generateMount(Control,viewKey+"ViewTubeScreen",1);
}

void
lensPackage(FuncDataBase& Control,const std::string& lensKey)
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

  const double CRLFrontToCenter = 49.445/2.0; // [20]
  const std::string lensName=lensKey+"LensBox";

  MBoxGen.setCF<CF40>();
  const double wallThick = 1.0; // [20]
  MBoxGen.setWallThick(wallThick);
  const double portABLength = 4.5; // [20]
  MBoxGen.setPortLength(portABLength, portABLength);
  const double bottomThick = 2.0;
  const double topThick = 2.4;
  MBoxGen.setLids(3.5,bottomThick, topThick); // [20]
  // All dimensions from [20]
  MBoxGen.generateBox(Control,lensName,
    21.0,12.0-2.0*topThick,8.2-2.0*bottomThick,
    2.0*(CRLFrontToCenter-(wallThick+portABLength)));

  Control.addVariable(lensName+"XStep",danmaxVar::beamMirrorShift);
  Control.addVariable(lensName+"YStep",danmaxVar::absY::CRL-CRLFrontToCenter);
}

void
viewBPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
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
  const double wallThick = 0.2; // [21]
  PTubeGen.setWallThick(wallThick);
  const double flangeLength = 3.0; // [21]
  PTubeGen.setPortLength(flangeLength,flangeLength);
  // ystep/width/height/depth/length
  const double totalLength = 36.0; // [21]
  PTubeGen.generateTube(
    Control,pipeName,0.0,totalLength-2.0*flangeLength-2.0*wallThick);

  const double mainTubeFrontToBeamViewerPort = 26.0; // [21]
  Control.addVariable(pipeName+"XStep",danmaxVar::beamMirrorShift);
  Control.addVariable(pipeName+"YStep",
    danmaxVar::absY::beamViewer3-mainTubeFrontToBeamViewerPort);
  Control.addVariable(pipeName+"NPorts",3);

  PItemGen.setCF<setVariable::CF100>(14.0); // [21]
  PItemGen.setPlate(CF100::flangeLength, "SteelUnknownGrade");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-9,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setCF<setVariable::CF100>(18.0); // [21]
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,-totalLength/2.0+mainTubeFrontToBeamViewerPort,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setPlate(CF40::flangeLength, "SteelUnknownGrade");
  PItemGen.setCF<setVariable::CF40>(12.056/cos(M_PI_4)); // [21]
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,-totalLength/2.0+mainTubeFrontToBeamViewerPort,0),
			Geometry::Vec3D(-1,-1,0));

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(0.7,0.7,0.02,-45.0,"Diamond",1); // [13]
  FlangeGen.generateMount(Control,pipeName+"Screen",1);
}

void
beamStopPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube 2
    \param Control :: Database
    \param viewKey :: prename
    port that defines the absolute position.
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","beamStopPackage");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::BeamPairGenerator BeamPairGen;

  const double port0Radius = 5.08; // [22]
  const double port0WallThick = 0.2; // [22]
  const double beamStopInPipeLength = 2.5; // [22]
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<CF40>(); // [22]
  PipeGen.setBFlange(port0Radius+port0WallThick, port0WallThick);
  PipeGen.generatePipe(Control,viewKey+"BeamStopInPipe",
    beamStopInPipeLength+CF40::flangeLength+port0WallThick); // [22]
  Control.addVariable(viewKey+"BeamStopInPipeXStep",danmaxVar::beamMirrorShift);
  const double beamStopFrontToWBPort =
    beamStopInPipeLength+CF40::flangeLength+5.2; // [22]
  Control.addVariable(viewKey+"BeamStopInPipeYStep",
    danmaxVar::absY::whiteBeamStop-beamStopFrontToWBPort);

  const double port0Length=15.4; // [22]
  const double port0SplitLength=8.0;

  std::string pipeName = viewKey+"BeamStopSection";
  SimpleTubeGen.setPipe(port0Radius, port0WallThick, 1.0, 0.0);
  SimpleTubeGen.generateTube(Control,pipeName,port0Length-port0SplitLength);
  Control.addVariable(pipeName+"NPorts",2);
  PItemGen.setCF<CF63>(14.0); // [22]
  PItemGen.setPlate(CF63::flangeLength, "SteelUnknownGrade");
  const double port0y = -(port0Length-port0SplitLength)/2.0+
    beamStopFrontToWBPort-beamStopInPipeLength-CF63::flangeLength-port0WallThick-0.8; // approx
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,port0y,0),
			Geometry::Vec3D(1,0,0));

  PItemGen.setWindowPlate(CF40::flangeLength,0.3,CF40::innerRadius,"SteelUnknownGrade","Glass"); // Window parameters: [36]

  //  const double port1angle = -116 * M_PI/180.0; // [22]
  const double port1angle = -115 * M_PI/180.0; // [22]
  PItemGen.setCF<CF40>(14.0-4.1);
  PItemGen.setPlate(CF40::flangeLength, "SteelUnknownGrade");
  const double port1y = -(port0Length-port0SplitLength)/2.0+
    beamStopFrontToWBPort-beamStopInPipeLength-CF40::flangeLength-port0WallThick; // approx
  PItemGen.generatePort(Control,pipeName+"Port1",Geometry::Vec3D(0,port1y,0),
			Geometry::Vec3D(sin(port1angle),cos(port1angle),0));
  PItemGen.setNoWindow();

  WhiteBeamStopGenerator WBSGen;
  WBSGen.generate(Control,viewKey+"WhiteBeamStop");

  // will be rotated vertical
  pipeName=viewKey+"BeamStopTube";
  SimpleTubeGen.setCF<CF160>(); // [22]
  SimpleTubeGen.setWallThick(0.2); // [22]
  SimpleTubeGen.setCap(1,1);
  const double tubeLengthAboveOpticalAxis = 11.2; // [22]
  const double tubeLengthBelowOpticalAxis = 32.8; // [22]
  const double tubeLength = tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis;
  SimpleTubeGen.generateTube(Control,pipeName,
    tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis);

  Control.addVariable(pipeName+"NPorts",2);

  PItemGen.setPort(port0SplitLength-port0WallThick, port0Radius, port0WallThick); // [22]
  PItemGen.setFlange(1.0, 0.0);
  PItemGen.setNoPlate();
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,1));

  const double port1Length = 19.575; // [22]
  const double port1ArtificialSplitLength = 12.0;
  const double port1Radius = 7.3; // [22]
  const double port1WallThick = 0.2; // [22]
  PItemGen.setPort(port1ArtificialSplitLength, port1Radius, port1WallThick); // [22]
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
  const double monoSlitsTubeWallThick = 2.0; // [22]
  // CF150 to fit flange B of port 1 [22]
  SimpleTubeGen.setPipe(
    CF150::flangeRadius-monoSlitsTubeWallThick,monoSlitsTubeWallThick,1.0,0.0); // [22]
  pipeName = viewKey+"MonoSlitsTube";
  const double monoSlitsTubeLength = 2.0*(
    danmaxVar::absY::monoSlits-danmaxVar::absY::bremColl2-port1Length);
  SimpleTubeGen.generateTube(
    Control,pipeName,monoSlitsTubeLength);
  Control.addVariable(pipeName+"NPorts",4);
  PItemGen.setCF<CF16>(12.0); // Estimated
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  const double bladeThick = 0.2; // [22]
  const double bladeOffset = 0.1; // [22]
  const double bladeXZDist = 3.1; // [22]
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

  // Seen from upstream, the beam hits the jaws in the following order [22]:
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

  FlangePlateGenerator flangePlateGen;
  flangePlateGen.setCF<CF150>(CF40::innerRadius); // [22]
  flangePlateGen.generateFlangePlate(Control,viewKey+"SlitsAOut");
}

void revBeamStopPackage(FuncDataBase& Control,
		   const std::string& viewKey)
  /*!
    Builds the variables for the reversed slit tube/beamstop

    The construction is largely analogous to Diagnostic Module 4.
    See that one for more information.

    \param Control :: Database
    \param viewKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","revBeamStopPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::BeamPairGenerator BeamPairGen;
  FlangePlateGenerator flangePlateGen;

  const double slitsInLength = CF150::flangeLength; // [23]
  flangePlateGen.setCF<CF150>(CF40::innerRadius); // [23]
  flangePlateGen.generateFlangePlate(Control,viewKey+"RevMonoSlitsIn");

  // Tube for Monochromatic Slits
  const double monoSlitsTubeWallThick = 2.0; // [23]
  // CF150 to fit flange B of port 1 [23]
  SimpleTubeGen.setPipe(
    CF150::flangeRadius-monoSlitsTubeWallThick,monoSlitsTubeWallThick,1.0,0.0); // [23]
  std::string pipeName = viewKey+"RevMonoSlitsTube";
  // Connector length + half the length of the pipe that contains the
  // monochromatic slits [23]
  const double revMonoSlitsFrontToSlits = CF150::flangeLength+5.25;
  const double monoSlitsTubeLength = 2.0*(revMonoSlitsFrontToSlits-slitsInLength);
  SimpleTubeGen.generateTube(Control,pipeName,monoSlitsTubeLength);

  Control.addVariable(pipeName+"XStep", danmaxVar::beamMirrorShift);
  Control.addVariable(pipeName+"YStep", danmaxVar::absY::monoSlits2-monoSlitsTubeLength*0.5);
  Control.addVariable(pipeName+"NPorts",4);

  Control.addVariable(pipeName+"NPorts",4);
  PItemGen.setCF<CF16>(12.0); // Estimated
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  const double bladeThick = 0.2; // [23]
  const double bladeOffset = 0.1; // [23]
  const double bladeXZDist = 3.1; // [23]
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

  // Seen from upstream, the beam hits the jaws in the following order [23]:
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
  SimpleTubeGen.setCF<CF160>(); // [23]
  SimpleTubeGen.setWallThick(0.2); // [23]
  SimpleTubeGen.setCap(1,1);
  const double tubeLengthAboveOpticalAxis = 11.2; // [23]
  const double tubeLengthBelowOpticalAxis = 32.8; // [23]
  const double tubeLength = tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis;
  SimpleTubeGen.generateTube(Control,pipeName,
    tubeLengthAboveOpticalAxis+tubeLengthBelowOpticalAxis);

  Control.addVariable(pipeName+"NPorts",2);

  // The length of this port could also have been read from [23], but here it is
  // calculated to place the main tube exactly at the given absolute position.
  PItemGen.setCF<CF150>(
    danmaxVar::absY::bremColl3-danmaxVar::absY::monoSlits2-0.5*monoSlitsTubeLength);
  PItemGen.setOuterVoid(0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,tubeLength/2.0-tubeLengthAboveOpticalAxis,0),
			Geometry::Vec3D(0,0,1));

  const double port1Length = 13.5; // [23]
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
}

void
hdcmPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","hdcmPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MonoBlockXstalsGenerator MXtalGen;

  const double HDCMTotalLength = 75.0; // [14]
  const double HDCMPortALength = 4.5; // [24]
  const double hdcmVesselWallThick = 0.5; // [24]
  const double hdcmVesselRadius = 30.0; // [24]
  MBoxGen.setCF<CF40>(); // [24]
  MBoxGen.setPortLength(
    HDCMPortALength,HDCMTotalLength-HDCMPortALength
    -2.0*hdcmVesselWallThick-2.0*hdcmVesselRadius
  );

  // Although the HDCM shifts the beam axis, the front and back ports of
  // are collinear [24]. No call to MBoxGen.setBPortOffset() or similar needed.
  // See rad/Beamlines/DanMAX/Pictures/OH/IMG_0718.JPG

  // radius, height, depth
  const std::string hdcmVesselKey = monoKey+"HDCMVessel";
  MBoxGen.generateBox(Control,hdcmVesselKey,hdcmVesselRadius,0.0,17.5); // [24]
  Control.addVariable(hdcmVesselKey+"WallThick", 0.5); // [24]
  Control.addVariable(hdcmVesselKey+"RoofThick", 0.3); // [24]
  Control.addVariable(hdcmVesselKey+"YStep",danmaxVar::absY::HDCM
    -HDCMPortALength-hdcmVesselRadius-hdcmVesselWallThick);

  const std::string portName=monoKey+"HDCMVessel";
  Control.addVariable(monoKey+"HDCMVesselNPorts",5); // the ports below are sorted by their rad. safety importance

  PItemGen.setCF<setVariable::CF100>(19.5); // [24]
  PItemGen.setWindowPlate(CF100::flangeLength,0.3,CF100::innerRadius,"SteelUnknownGrade","Glass"); // Window parameters: 36  - Mads: it's getting dark under irradiation -> normal (not Lead) glass
  PItemGen.generatePort(Control,portName+"Port0",Geometry::Vec3D(0,0.0,-0.2-3.5),Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF63>(34.1); // [24] approx
  PItemGen.setWindowPlate(CF63::flangeLength,0.3,CF63::innerRadius,"SteelUnknownGrade","Glass"); // Window parameters: 36
  const double port2angle = 45 * M_PI/180.0; // approx - not clear from [24]
  PItemGen.generatePort(Control,portName+"Port1",Geometry::Vec3D(0,0.0,-0.2),Geometry::Vec3D(sin(port2angle),cos(port2angle),0.0)); // [24]

  const double port3angle = 30 * M_PI/180.0; // approx - not clear from [24]
  PItemGen.generatePort(Control,portName+"Port2",Geometry::Vec3D(0,0.0,-0.2),Geometry::Vec3D(-sin(port3angle),-cos(port3angle),0.0)); // [24]

  PItemGen.setCF<setVariable::CF35_TDC>(34.1); // [24]
  PItemGen.setWindowPlate(CF35_TDC::flangeLength,0.3,CF35_TDC::innerRadius,"SteelUnknownGrade","Glass"); // Window parameters: 36
  PItemGen.generatePort(Control,portName+"Port3",Geometry::Vec3D(0,0.0,-10.4),Geometry::Vec3D(1,0,0)); // [24]

  PItemGen.setCF<setVariable::CF35_TDC>(34.1); // dummy, the port turns upwards, but not modelling it to save time
  PItemGen.setNoWindow();
  PItemGen.generatePort(Control,portName+"Port4",Geometry::Vec3D(0,0.0,-10.0),Geometry::Vec3D(-1,0,0));

  // crystals gap 7mm
  MXtalGen.setTopA(9.0, 5.0, 1.5); // [24]
  MXtalGen.setA(5.0, 5.0, 2.0); // [24]
  MXtalGen.setBaseA(11.0, 6.0, 1.5); // [24]

  MXtalGen.setTopB(12.5, 3.0, 1.5); // [24]
  MXtalGen.setB(10.0, 3.0, 2.0); // [24]
  MXtalGen.setBaseB(14.0, 6.0, 1.5); // [24]

  MXtalGen.setGap(0.7);
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0); // yStep, theta
}

void mirrorMonoPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the mirror mono package (MLM)
    \param Control :: Database
    \param monoKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","mirrorMonoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::VacuumBoxGenerator MBoxGen;
  setVariable::MLMonoGenerator MXtalGen;

  const double mlmFrontToPort2 = 21.95; // [25] distance b/w flange A front and  port 2 centre
  MBoxGen.setCF<CF40>();   // set ports
  const double MLMWallThick = 1.2; // Walls: front, side, back [25]
  // Roof/Base/Width/Front/Back
  const double MLMRoofThick = 1.5; // [25]
  const double MLMFloorThick = 2.7; // [25]
  MBoxGen.setAllThick(
    MLMRoofThick,MLMFloorThick,MLMWallThick,MLMWallThick,MLMWallThick);
  const double MLMPortLength = 2.4; // [25], same for A and B
  const double MLMTotalLength = 115.9; // [25]
  MBoxGen.setPortLength(MLMPortLength, MLMPortLength); // [25]
  const double MLMHeight = 47.0;
  const double MLMHeightAboveOpticalAxis = 11.55+CF40::innerRadius;
  MBoxGen.setBPortOffset(danmaxVar::beamMirrorShift, 0.0); // [25]
  const std::string monoVesselKey=monoKey+"MLMVessel";
  // width / heigh / depth / length
  MBoxGen.generateBox
    (
      Control,monoVesselKey,
      59.0-2.0*MLMWallThick, // [25]
      MLMHeightAboveOpticalAxis-MLMRoofThick,
      MLMHeight-MLMHeightAboveOpticalAxis-MLMFloorThick,
      MLMTotalLength-2.0*MLMWallThick-2.0*MLMPortLength
    );

  Control.addVariable(monoVesselKey+"YStep",danmaxVar::absY::MLM-mlmFrontToPort2); // [12]
  Control.addVariable(monoVesselKey+"WallMat", "Stainless316L");
  Control.addVariable(monoVesselKey+"PipeMat", "Stainless316L"); // only front/back ports

 // MLM Ports:
  Control.addVariable(monoVesselKey+"NPorts",5);

  // Ion pump port (top)
  PItemGen.setCF<setVariable::DN160CF>(17.69); // [25]
  PItemGen.setPlate(setVariable::DN160CF::flangeLength, "SteelUnknownGrade"); // [25] + not modelling vacuum pump (close the cap instead)
  PItemGen.generatePort(Control,monoVesselKey+"Port0",
			Geometry::Vec3D(0.0, -1.53, 0.0),
			Geometry::Vec3D(0,0,1));

  // View port (top of vessel, along Bragg axis)
  PItemGen.setCF<setVariable::DN100CF>(19.43); // [25]
  PItemGen.setWindowPlate(setVariable::DN100CF::flangeLength, // [25]
			  0.3, // [36]
			  DN100CF::innerRadius, // window radius [25, 36]
			  "SteelUnknownGrade", // TODO: dummy
			  "Glass");  // [36]
  PItemGen.generatePort(Control,monoVesselKey+"Port1",
			Geometry::Vec3D(0.0, -36.0, 0.0), // [25]
			Geometry::Vec3D(0,0,1));

  // View port (vessel side, viewing at 1st crystal surface)
  PItemGen.setCF<setVariable::CF63>(34.2);
  PItemGen.setWindowPlate(setVariable::CF63::flangeLength, // [25]
			  0.3, // [36]
			  CF63::innerRadius, // [25, 36]
			  "SteelUnknownGrade", // TODO: dummy
			  "Glass");  // [36]
  PItemGen.generatePort(Control,monoVesselKey+"Port2",
			Geometry::Vec3D(0.0, -36.0, 0.0), // [25]
			Geometry::Vec3D(-1,0,0));

  // View port (vessel side, viewing at 2nd crystal surface)
  PItemGen.setLength(33.4); // [25]
  PItemGen.generatePort(Control,monoVesselKey+"Port3",
			Geometry::Vec3D(0.0, 34.95, -0.15), // [25]
			Geometry::Vec3D(1,0,0));

  // Spare port (blind), IMG_0719.JPG
  PItemGen.setNoWindow();
  PItemGen.generatePort(Control,monoVesselKey+"Port4",
			Geometry::Vec3D(0.0, 0.0, -0.15), // [25]
			Geometry::Vec3D(-1,0,0));

  // crystals gap 4mm
  MXtalGen.generateMono(Control,monoKey+"MLM",-10.0,0.3,0.3);
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
  setVariable::MonoShutterR3Generator<CF200,CF63,CF40,CF40> MShutterGen;

  const std::string monoShutterName = preName+"MonoShutter";
  MShutterGen.generate(Control,monoShutterName);
  Control.addVariable(monoShutterName+"XStep",danmaxVar::beamMirrorShift);
  Control.addVariable(monoShutterName+"YStep",
    danmaxVar::absY::monoShutter-MShutterGen.getLength()/2.0);

  const double bellowLength = 12.0; // [26]
  BellowGen.setMat("SteelUnknownGrade","SteelUnknownGrade%Void%10.0");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowL",bellowLength);
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


void
opticsSlitPackage(FuncDataBase& Control,
		  const std::string& opticsName)
  /*!
    Builds the DM2 slit package

    \param Control :: Function data base for variables
    \param opticsName :: PreName
*/
{
  setVariable::PortTubeGenerator PortTubeGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BeamPairGenerator BeamMGen;

  const std::string sName=opticsName+"SlitTube";
  const double tubeLength = 48.5; // Outer length [27]
  PortTubeGen.setPipeCF<CF200>(); // [27]
  // [27] TODO: This is only the thickness of the hull.
  // The front and back walls are actually 1 mm thicker, but the present
  // PortTubeGenerator does not distinguish between these walls.
  const double wallThick = 0.2;
  PortTubeGen.setWallThick(0.2);

  PortTubeGen.setPortCF<CF40>(); // [27]
  const double topPortPipeToSlitTubeFront = 4.4; // [27]
  const double frontPortLength = 2.7; // [27]
  const double backPortLength = 2.5; // [27]
  PortTubeGen.setPortLength(frontPortLength, backPortLength);
  PortTubeGen.generateTube(Control,sName,0.0,tubeLength-2.0*wallThick);

  Control.addVariable(sName+"NPorts",4);
  // [27] Length of Port0 and Port1 (they are the same within a few millimeters).
  PItemGen.setCF<setVariable::CF150>(CF200::innerRadius+wallThick+14.4);
  PItemGen.setPlate(setVariable::CF150::flangeLength,"SteelUnknownGrade");

  const Geometry::Vec3D topJaw(
    0.0,CF150::outerRadius+topPortPipeToSlitTubeFront-tubeLength/2.0,0.0);
  const Geometry::Vec3D sideJaw(
    0.0,7.9+CF150::outerRadius-tubeLength/2.0,0.0); // [27]
  const Geometry::Vec3D vacPort(0.0,0.0,0.0); // [27]
  const Geometry::Vec3D beamViewer1(
    0.0,
    topPortPipeToSlitTubeFront+CF150::outerRadius
    +danmaxVar::absY::beamViewer1
    -danmaxVar::absY::whiteBeamSlitsTopJaw-tubeLength/2.0,
    0.0
  );
  Control.addVariable(sName+"YStep",
    danmaxVar::absY::whiteBeamSlitsTopJaw-frontPortLength-topPortPipeToSlitTubeFront-CF150::outerRadius);

  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,sName+"Port0",topJaw,ZVec);
  PItemGen.generatePort(Control,sName+"Port1",sideJaw,XVec);
  PItemGen.setCF<setVariable::CF150>(CF200::innerRadius+wallThick+24.0); // [27]
  PItemGen.generatePort(Control,sName+"Port2",vacPort,-ZVec);
  PItemGen.setCF<setVariable::CF100>(CF200::innerRadius+wallThick+5.9); // [27]
  PItemGen.generatePort(Control,sName+"Port3",beamViewer1,ZVec);

  // Jaw units:
  BeamMGen.setThread(0.5,"Nickel"); // Estimated
  BeamMGen.setLift(0.6, 0.6); // "Maximum aperture" given as 10 mm x 10 mm [13]
  BeamMGen.setGap(-0.1,-0.1); // "Maximum overlap" given as 2 mm in [13]
  // Width, height, and angle from [13]
  // Thickness: Estimated from the given coolant-connection radius of 8 mm in [13]
  BeamMGen.setBlock(5.0,3.5,1.0,0.0,"TungstenCarbide"); // [13]
  BeamMGen.setWaterPipes(2, 1);

  // Seen from upstream, the beam hits the jaws in the following order [27]:
  // Top (JawX B) -> Right (JawZ A) -> Bottom (JawX A) -> Left (JawZ B)
  const double bladeOffset = 3.6; // [27]
  BeamMGen.setXYStep(0.0,bladeOffset,0.0,-bladeOffset);
  BeamMGen.generateMount(Control,opticsName+"JawX",0);
  BeamMGen.setXYStep(-bladeOffset,0.0,bladeOffset,0.0);
  BeamMGen.generateMount(Control,opticsName+"JawZ",0);
}

void
splitterVariables(FuncDataBase& Control,
		  const std::string& splitKey)
  /*!
    Builds the variables for the splitter at
    the end of the opticsHut/opticsBeam
    \param Control :: Database
    \param splitKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","splitVariables");
  setVariable::TwinPipeGenerator TwinGen;

  TwinGen.setCF<CF40>();
  TwinGen.setJoinFlangeCF<CF100>();
  TwinGen.setAPos(-2.7*2,0);
  TwinGen.setBPos(0.0, 0.0);
  TwinGen.setXYAngle(15.0,0.0);
  TwinGen.generateTwin(Control,splitKey+"Splitter",0.0,42.0);
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

  Control.addVariable(opticsName+"OuterLeft",190.0);
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

  setVariable::GateValveGenerator GateGen;

  PipeGen.setNoWindow();
  PipeGen.setMat("SteelUnknownGrade");
  BellowGen.setMat("SteelUnknownGrade","SteelUnknownGrade%Void%10.0");
  BellowGen.setCF<setVariable::CF40>(); // [28]
  BellowGen.setBellowStep(2.5); // [28]
  // dummy length, but length is also given in [4]
  BellowGen.generateBellow(Control,opticsName+"InitBellow",10.0);

  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  // Total length of 200 mm given in [4], object is symmetric.
  TGen.setMainLength(10.0, 10.0);
  TGen.setPortCF<setVariable::CF40>(); // [28]
  TGen.setSideCF<setVariable::CF40>(10.0); // [28]
  TGen.generateTube(Control,opticsName+"TriggerUnit");

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve4"); // [28]
  // - Valve 4 is positioned w.r.t. its back surface.
  // - Valve 4 length = Valve 3 length.
  Control.addVariable(opticsName+"Valve4YStep",
    danmaxVar::absY::valve4Back-valve3Length);


  const double bremColl1Length = 29.0; // [29]
  const double bremcoll1Height = 44.0; // [29]
  // Distance measured from top of beam pipe in [29], not from its center, therefore
  // need to add beam-pipe radius.
  const double bremColl1TopHeight = 8.2 + setVariable::CF40::innerRadius;
  const double bremColl1Z = bremcoll1Height/2.0-bremColl1TopHeight;

  PipeGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,opticsName+"BellowA",0.0); // Dummy length
  PipeGen.generatePipe(Control,opticsName+"PipeA",38.3);
  // Reset bellow step to default value [28]. Large bellow step of InitBellow and
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
  Control.addVariable(bremColl1TubeName+"YStep",
    danmaxVar::absY::bremColl1-bremColl1Length/2.0);

  const std::string bremColl1Name = opticsName+"BremColl1";
  // TODO: Material currently set to pure tungsten (default), but should be
  // DENSIMET [13].
  BremGen.centre();
  BremGen.setLength(8.0); // Sec. 2.1 in [13]
  BremGen.setCube(10.0,10.0); // Sec. 2.1 in [13]
  // All dimensions from Sec. 2.1 in [13], except aperture position.
  // The aperture position is not mentioned in [13], but was read off from [29].
  // Aperture dimensions can be given in two ways, either as an angle or by
  // setting the cross-section dimensions at different positions. Tried both ways,
  // once with the angle from [13] and once with dimensions read off from [29].
  // Since the entrance/exit dimensions agreed within a few percent, decided to use
  // the angle, since its value is given explicitly.
  BremGen.setAperatureAngle(2.0, 0.2, 0.2, 5.0, 5.0);
  BremGen.generateBlock(Control,bremColl1Name,bremColl1Z);
  Control.addVariable(bremColl1Name+"XAngle",90);

  // High Pass Filter
  // Simplified to a pipe with two 'windows' corresponding to the two diamond filters.
  PipeGen.setOuterVoid(true);
  PipeGen.setRectWindow(0.6,0.6,0.06,0.6,0.6,0.04); // [13]
  PipeGen.setWindowMat("Diamond", "Diamond"); // [13]
  // Length adjusted to fit the position given in [12]
  const double highPassFilterLength =
    2.0*(
      danmaxVar::absY::highPassFilter-danmaxVar::absY::bremColl1-bremColl1Length/2.0
  );
  PipeGen.generatePipe(Control,opticsName+"HighPassFilter",
    highPassFilterLength);
  // When using absolute positioning, YStep variables needs to be adjusted.
  Control.addVariable(opticsName+"HighPassFilterYStep",0.0);

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"Valve5"); // [28]

  const double bellowCDLength = 8.0; // Dummy

  PipeGen.setNoWindow();
  PipeGen.generatePipe(Control,opticsName+"PipeA",12.5); // [16]
  BellowGen.generateBellow(Control,opticsName+"BellowC",10.0); // Dummy length

  //////// CM1
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D vDanMAX = -ZVec;
  // Starting with the SINCRYS-side port of CM1 and ending with SINCRYS beam viewer 1,
  // all beamline elements are at a fixed angle of 16.177 deg, which is the center of
  // motion between the two extreme angles 15.041 deg and 17.313 deg:
  // (17.313 deg - 15.041 deg) / 2 = 16.177 deg
  // Refs. [32] and [33] only contain evidence that this must be the fixed angle, but
  // I have not found a reference that states this explicitly.
  const double sinCrysBranchCenterAngleDeg = 16.177;
  const double sinCrysBranchMinAngleDeg = 15.041;
  const double sinCrysBranchMaxAngleDeg = 17.313;
  const double sinCrysBranchCenterAngle = sinCrysBranchCenterAngleDeg * M_PI/180.0;
  const Geometry::Vec3D vSinCrys(
    -sin(sinCrysBranchCenterAngle),0.0,-cos(sinCrysBranchCenterAngle));

  // will be rotated vertical
  std::string name=opticsName+"CM1";
  SimpleTubeGen.setCF<CF100>(); // [32]
  SimpleTubeGen.setCap();
  const double CM1MainTubeLength = 36.0; // [30]
  SimpleTubeGen.generateTube(Control,name,CM1MainTubeLength);
  Control.addVariable(name+"NPorts",3);

  // Front
  // Port lengths measured in [30], using the information that CM1 is centered on the
  // granite block whose length is given in [33].
  const double CM1FrontPortLength = 10.9;
  const double CM1Port12Length = 26.0;
  PItemGen.setCF<setVariable::CF40>(CM1FrontPortLength);
  PItemGen.setPlate(0.0,"Void");
  const double CM1PortY = CM1MainTubeLength/2.0-12.0; // [32]
  PItemGen.generatePort(Control,name+"Port0",Geometry::Vec3D(0,CM1PortY,0),ZVec);

  // Back: DanMAX branch
  PItemGen.setCF<setVariable::CF40>(CM1Port12Length);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,name+"Port1",Geometry::Vec3D(0,CM1PortY,0),vDanMAX);

  // Back: SinCrys branch
  PItemGen.setCF<setVariable::CF40>(CM1Port12Length);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,name+"Port2",Geometry::Vec3D(0,CM1PortY,0),vSinCrys);
  Control.addVariable(name+"YStep",danmaxVar::absY::CM1-CM1FrontPortLength);

  ///////////

  Control.copyVarSet(beamName+"FrontBeamValve3",opticsName+"ValveS1"); // [30]
  Control.addVariable(opticsName+"ValveS1YAngle",sinCrysBranchCenterAngleDeg);

  FlangePlateGenerator flangePlateGen;
  flangePlateGen.setFlangeCF<CF40>(); // [30]
  // Dimensions from [30]
  // Material from [32]
  flangePlateGen.setWindow(0.35,0.05,"Diamond");
  flangePlateGen.generateFlangePlate(Control,opticsName+"DiamondWindow");

  name=opticsName+"BeamViewerS1";
  // [30] In a good approximation (+- 1 mm), all ports have the same length,
  // no matter what one considers a port or a part of the main tube.
  const double beamViewerS1PortLength = 6.25;
  SimpleTubeGen.setCF<CF40>(); // [32]
  SimpleTubeGen.setCap(false, false);
  SimpleTubeGen.generateTube(Control,name,2.0*beamViewerS1PortLength);
  Control.addVariable(name+"NPorts",3);

  PItemGen.setCF<setVariable::CF40>(beamViewerS1PortLength);
  // Build ports with slightly smaller outer radius than the main pipe to avoid
  // geometry errors.
  PItemGen.setPort(beamViewerS1PortLength,CF40::innerRadius-1e-3,CF40::wallThick);
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  PItemGen.generatePort(
    Control,name+"Port0",Geometry::Vec3D(),Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(
    Control,name+"Port1",Geometry::Vec3D(),Geometry::Vec3D(0,0,-1));
  PItemGen.generatePort(
    Control,name+"Port2",Geometry::Vec3D(),Geometry::Vec3D(-1,0,0));

  // Screen dimensions from [30].
  const double beamViewerS1ScreenThick = 0.015; // [30]
  const double beamViewerS1ScreenSideLength = 1.0; // [30]
  FlangeGen.setNoPlate();
  // Thread is a conservative approximation. In reality, there is much more material
  // around the crystal, see [30] or [32].
  FlangeGen.setThread(beamViewerS1ScreenThick,0.0,"SteelUnknownGrade"); // Dummy length
  // Angle from [30]
  // Material is actually given as "CVD polycrystalline N doped diamond" in [32].
  FlangeGen.setBlade(
    beamViewerS1ScreenSideLength,beamViewerS1ScreenSideLength,
    beamViewerS1ScreenThick,45.0,"Diamond",1);
  FlangeGen.generateMount(Control,name+"Screen",1);

  // TODO: Add a corresponding Control.addVariable("...SINCRYSAngle",) command in a 
  // top-level class to make the variable available at runtime.
  const double SINCRYSAngleDeg = Control.EvalDefVar<double>(
    beamName+"SINCRYSAngle",sinCrysBranchCenterAngleDeg);
  if(
    SINCRYSAngleDeg < sinCrysBranchMinAngleDeg - Geometry::zeroTol ||
    SINCRYSAngleDeg > sinCrysBranchMaxAngleDeg + Geometry::zeroTol
  ){
    ELog::EM << "SINCRYS branch angle of " + std::to_string(SINCRYSAngleDeg)
    + " deg is outside the nominal range [ " + std::to_string(sinCrysBranchMinAngleDeg)
    + " deg, " + std::to_string(sinCrysBranchMaxAngleDeg) + " deg ]" << ELog::endWarn;
  }
  const double SINCRYSAngle = SINCRYSAngleDeg*M_PI/180.0;
  SmallAngleBellowsGenerator smallAngleBellowsGen;
  const double cardanBellowsLength = 18.0; // [30]
  const double CM2PortLength = 32.0-CF250::outerRadius; // [30]

  name = opticsName+"CM1BeamSplitter";
  int mode = 2;
  // TODO: Add a corresponding Control.addVariable("...Mode",) command in a 
  // top-level class to make the variable available at runtime.
  Control.EvalDefVar<int>(beamName+"Mode",mode); 
  Control.addVariable(name+"Mode",mode);
  Control.addVariable(name+"YStep",danmaxVar::absY::CM1);
  double cm1BeamSplitterZAngle = 0.0;
  if(mode == 2){
    // In mode 2, assume that the orientation of the CM1 splitter crystal corresponds
    // to Bragg scattering at the SINCRYS center angle. The actual relation between 
    // the crystal-surface angle and the crystal-lattice orientation is neither given 
    // in the main reference [32], nor is FLUKA capable of simulating Bragg scattering.
    // Therefore, this is more a cosmetic issue.
    cm1BeamSplitterZAngle = (SINCRYSAngleDeg-sinCrysBranchCenterAngleDeg)/2.0;
  }
  Control.addVariable(name+"ZAngle",cm1BeamSplitterZAngle);

  const Geometry::Vec3D SINCRYSBranchCenterVector(
    cos(sinCrysBranchCenterAngle),-sin(sinCrysBranchCenterAngle),0.0);
  const Geometry::Vec3D SINCRYSTransportPipeUpstreamJoint = (
    SINCRYSBranchCenterVector*(
      CM1Port12Length+valve3Length+2.0*beamViewerS1PortLength+0.5*cardanBellowsLength
    )
  );
  const Geometry::Vec3D SINCRYSTransportPipeDownstreamJoint = (
    Geometry::Vec3D(
      -SINCRYSBranchShift/tan(SINCRYSAngle),SINCRYSBranchShift,0.0
    )
    -SINCRYSBranchCenterVector*(CM2PortLength+0.5*cardanBellowsLength)
  );
  Geometry::Vec3D SINCRYSTransportPipeDirection = (
    SINCRYSTransportPipeDownstreamJoint-SINCRYSTransportPipeUpstreamJoint
  );
  SINCRYSTransportPipeDirection.makeUnit();
  const double cardanBellowsAngleSign = (
      SINCRYSTransportPipeDirection*SINCRYSBranchCenterVector).Z()
      /fabs((SINCRYSTransportPipeDirection*SINCRYSBranchCenterVector).Z()
    );
  const double cardanBellowsAngle = cardanBellowsAngleSign*acos(
    SINCRYSTransportPipeDirection.dotProd(SINCRYSBranchCenterVector));

  smallAngleBellowsGen.setLength(cardanBellowsLength);
  smallAngleBellowsGen.setAngle(cardanBellowsAngle*180.0/M_PI);
  name = opticsName+"CardanBellowsUpstream";
  smallAngleBellowsGen.generateBellows(Control,name);
  BellowGen.generateBellow(Control,opticsName+"BellowBA",10.0); // dummy TODO: fix length
  // Length at center angle from [30]
  PipeGen.generatePipe(Control,opticsName+"PipeSinCrys",180.0);
  name = opticsName+"LinearlyGuidedBellowsUpstream";
  // Actually, the linearly guided bellow consists of 7 single bellows. For simplicity,
  // it is modeled as a single, long bellow here.
  // Length at center angle from [30]
  BellowGen.generateBellow(Control,name,70.0);
  // Increase NFolds to be able to build long bellows
  // (see comment above about the unusual length).
  Control.addVariable(name+"NFolds",30);
  name = opticsName + "CardanBellowsDownstream";
  smallAngleBellowsGen.generateBellows(Control,name);
  // Compensate the rotation introduced by attaching the bellows to CM2.
  Control.addVariable(name+"YAngle",sinCrysBranchCenterAngleDeg);

  name=opticsName+"CM2";
  SimpleTubeGen.setCF<CF250>(); // [32]
  const double CM2MainTubeLength = 26.0; // [34]
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,name,CM2MainTubeLength);
  Control.addVariable(name+"NPorts",5);

  PItemGen.setCF<setVariable::CF40>(CM2PortLength);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,name+"Port0",Geometry::Vec3D(0,0,0),vSinCrys);
  PItemGen.generatePort(Control,name+"Port1",Geometry::Vec3D(0,0,0),ZVec);
  
  Geometry::Vec3D YVec(0.0,1.0,0.0);
  Geometry::Vec3D beamViewer2Dir = vSinCrys;
  // [32] gives the angle between the crystal and the incoming beam in CM2
  // as 45 degrees. From the way the crystal is mounted in the port (see [34]),
  // it is clear that the port's angle w.r.t. the beam port must also be 45 degrees.
  beamViewer2Dir.rotate(YVec,-M_PI_4);
  const Geometry::Vec3D beamViewer2CM2CenterOffset = vSinCrys*6.0;

  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade"); // [37]
  // Length of beam-viewer and camera ports [34]
  const double beamViewerS2PortLength = CF250::outerRadius+2.5;
  PItemGen.setLength(beamViewerS2PortLength);
  // Upstream distance of the beam-viewer screen from the center of CM2 roughly
  // read off as 6(1) cm from [34].
  PItemGen.generatePort(Control,name+"Port2",beamViewer2CM2CenterOffset,
    beamViewer2Dir);
  Geometry::Vec3D beamViewer2CameraDir = vSinCrys;
  // Angle of camera port roughly read off as 90(5) deg from [34].
  beamViewer2CameraDir.rotate(YVec,-M_PI_2);
  PItemGen.generatePort(Control,name+"Port3",beamViewer2CM2CenterOffset,
    beamViewer2CameraDir);

  PItemGen.setLength(17.0); // Read off as 170(5) from [37]
  Geometry::Vec3D viewPortDir = vSinCrys;
  viewPortDir.rotate(YVec,M_PI_2); // [37]
  PItemGen.generatePort(Control,name+"Port4",Geometry::Vec3D(0,0,0),viewPortDir);

  const double beamViewerS2ScreenThick = 0.005; // [34]
  const double beamViewerS2ScreenSideLength = 1.0; // [34]
  FlangeGen.setNoPlate();
  // Thread is a conservative approximation. In reality, there is much more material 
  // around the crystal, see [32] or [34].
  FlangeGen.setThread(beamViewerS2ScreenThick,
    beamViewerS2PortLength-beamViewerS2ScreenSideLength/2.0,
    "SteelUnknownGrade"
  );
  // Angle w.r.t. the beam is 45 degrees according to [32], but this angle is already
  // fixed by the beam-port angle. In reality, the angle is actually -45 degrees w.r.t.
  // the beam, because the screen is mounted in a way that is not possible with the 
  // FlangeMount class (I believe).
  FlangeGen.setBlade(
    beamViewerS2ScreenSideLength,beamViewerS2ScreenSideLength,
    beamViewerS2ScreenThick,0.0,"YAG",1); // [32]
  FlangeGen.generateMount(Control,opticsName+"BeamViewerS2Screen",1);
  Control.addVariable(opticsName+"BeamViewerS2ScreenBladeCentreActive",1);

  Control.addVariable(name+"XStep",SINCRYSBranchShift);
  Control.addVariable(name+"YStep",
    danmaxVar::absY::CM1
    // Refs. [31] and [33] give the distance from CM1 to CM2 along the beam axis as
    // 3395.0 mm (explicitly in [33], implicitly in [31]).
    // However, the center angle of 16.177 deg from Ref. [32], together with the
    // offset of 980 mm, results in a value of about 3378 mm that creates a visible
    // discrepancy in the geometry. For this reason, the optical-axis distance is
    // calculated here instead of taking it from [31] or [33].
    +fabs(SINCRYSBranchShift)/tan(SINCRYSAngleDeg*M_PI/180.0)
    +CM2PortLength
  );
  Control.addVariable(name+"ZAngle",180.0);

  // Gate valve 2 is a different type than valve 1, which can be seen by comparing 
  // [30] and [34]. Due to its outer shape, assumed that it is similar to 
  // FrontBeamValve1.
  name = opticsName+"ValveS2";
  Control.copyVarSet(beamName+"FrontBeamValve1",name); // [34]
  Control.addVariable(name+"YAngle",180.0);

  // Implement as stiff -in particular, non-bending- bellows, because transversal
  // stress is neglected in this part of the beamline.
  BellowGen.generateBellow(Control,opticsName+"CardanBellowsCM2",18.0); // [34]

  // See also comments on LinearlyGuidedBellowsUpstream
  name = opticsName+"LinearlyGuidedBellowsDownstream";
  // Length at center angle from [34]
  BellowGen.generateBellow(Control,name,70.0);
  Control.addVariable(name+"NFolds",30);

  PipeGen.generatePipe(Control,opticsName+"TransportPipe2",170.0); // [34]

  // See also comment on CardanBellowsCM2.
  BellowGen.generateBellow(Control,opticsName+"CardanBellowsTransfocator",18.0); // [34]

  name=opticsName+"Transfocator";
  setVariable::MonoBoxGenerator MBoxGen;
  MBoxGen.setCF<CF40>(); // [32,34]
  const double wallThick = 0.5;
  MBoxGen.setWallThick(wallThick);
  const double transfocatorLength = 74.0; // [34]
  const double transfocatorBoxLength = 62.6; // [34]
  const double transfocatorPortLength = (transfocatorLength-transfocatorBoxLength)/2.0;
  MBoxGen.setPortLength(transfocatorPortLength,transfocatorPortLength);
  const double bottomThick = 0.5; // [34]
  const double topThick = 2.0;
  // Actually, the transfocator vessel has no overhang at the bottom, see [34].
  MBoxGen.setLids(5.5,bottomThick,topThick); // [34]
  MBoxGen.generateBox(Control,name,
    16.4-2.0*wallThick,8.3,9.8,
    transfocatorBoxLength-2.0*wallThick); // [34]

  Control.addVariable(name+"XStep",SINCRYSBranchShift);
  Control.addVariable(name+"YStep",
    danmaxVar::absY::CM1+639.9); // [33]

  PipeGen.generatePipe(
    Control,opticsName+"TransfocatorToSlitsPipe1",12.5); // [34]
  BellowGen.generateBellow(
    Control,opticsName+"TransfocatorToSlitsBellows",10.0); // Dummy length
  // Between the bellows and the slits, there are actually two CF40 pipes.
  // The one upstream contains a port for a vacuum pump (see, for example, [34]).
  // Here, everything is implemented as a single pipe, and the pump and its 
  // port are not modeled.
  PipeGen.generatePipe(
    Control,opticsName+"TransfocatorToSlitsPipe2",25.0);

  // IB-C30-UHV slits
  name=opticsName+"SlitTubeS";
  MonoSlitsJJGenerator monoSlitsJJGenerator;
  monoSlitsJJGenerator.generate(Control,name);
  Control.addVariable(name+"XStep",SINCRYSBranchShift);
  Control.addVariable(name+"YStep",
    danmaxVar::absY::CM1+769.0-monoSlitsJJGenerator.getLength()/2.0); // [33]
  // According to [32], it is identical to beam viewer 1 except for the orientation.
  // See beam viewer 1 for more information.
  name=opticsName+"BeamViewerS3";
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setCap(false, false);
  SimpleTubeGen.generateTube(Control,name,2.0*beamViewerS1PortLength);
  Control.addVariable(name+"NPorts",3);

  PItemGen.setCF<setVariable::CF40>(beamViewerS1PortLength);
  PItemGen.setPort(beamViewerS1PortLength,CF40::innerRadius-1e-3,CF40::wallThick);
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  PItemGen.generatePort(
    Control,name+"Port0",Geometry::Vec3D(),Geometry::Vec3D(1,0,0));
  PItemGen.generatePort(
    Control,name+"Port1",Geometry::Vec3D(),Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(
    Control,name+"Port2",Geometry::Vec3D(),Geometry::Vec3D(0,0,1));

  FlangeGen.setNoPlate();
  FlangeGen.setThread(beamViewerS1ScreenThick,0.0,"SteelUnknownGrade"); // Dummy length
  FlangeGen.setBlade(
    beamViewerS1ScreenSideLength,beamViewerS1ScreenSideLength,
    beamViewerS1ScreenThick,45.0,"Diamond",1);
  FlangeGen.generateMount(Control,name+"Screen",1);

  // See also comments on ValveS2.
  name = opticsName+"ValveS3";
  Control.copyVarSet(beamName+"FrontBeamValve1",name); // [34]
  Control.addVariable(name+"YAngle",90.0);

  BellowGen.generateBellow(
    Control,opticsName+"SlitsToShutterBellows",10.0); // Dummy length

  // Mono shutter is identical to the one on the DanMAX branch [32].
  // See there for more information.
  setVariable::MonoShutterR3Generator<CF200,CF63,CF40,CF40> MShutterGen;
  name = opticsName+"MonoShutterS";
  MShutterGen.generate(Control,name);
  Control.addVariable(name+"XStep",SINCRYSBranchShift);
  Control.addVariable(name+"YStep",
    danmaxVar::absY::CM1+819.0-MShutterGen.getLength()/2.0);

  // Laue monochromator
  PipeGen.setNoWindow();
  BellowGen.generateBellow(Control,opticsName+"BellowC",bellowCDLength);
  // Dummy length
  PipeGen.generatePipe(
    Control,opticsName+"LauePipe",234.14);
  BellowGen.generateBellow(Control,opticsName+"BellowD",bellowCDLength);

  opticsSlitPackage(Control,opticsName);

  const std::string valve6Name = opticsName+"Valve6";
  Control.copyVarSet(beamName+"FrontBeamValve3",valve6Name); // [28]
  const double valve6Angle = 90.0;
  Control.addVariable(valve6Name+"YAngle", 90.0); // [28]

  const std::string bellowEName = opticsName+"BellowE";
  BellowGen.generateBellow(Control,bellowEName,10.0);  // Dummy length
  Control.addVariable(bellowEName+"YAngle", -valve6Angle);

  hdcmPackage(Control,opticsName);

  // Dummy length
  BellowGen.generateBellow(Control,opticsName+"BellowAfterHDCM",10.0);

  const std::string valve7Name = opticsName+"Valve7";
  Control.copyVarSet(beamName+"FrontBeamValve3",valve7Name); // [28]
  // Angle roughly adjusted to [28]. Found it difficult to read off from the model.
  Control.addVariable(valve7Name+"YAngle", -160.0);

  viewPackage(Control,opticsName);

  std::string valve8Name = opticsName+"Valve8";
  Control.copyVarSet(beamName+"FrontBeamValve3",valve8Name); // [33]
  // Neither visible on [26] or [28] as the other valves of this type, but the angle
  // can be seen in [33].
  Control.addVariable(valve8Name+"YAngle", 90.0);

  // Dummy length
  BellowGen.generateBellow(Control,opticsName+"BellowF",10.0);

  mirrorMonoPackage(Control,opticsName);
  BellowGen.generateBellow(Control,opticsName+"BellowG",10.0); // Dummy length

  const std::string valve9Name = opticsName+"Valve9";
  Control.copyVarSet(beamName+"FrontBeamValve3",valve9Name); // [26]
  // Angle estimated from [26]
  Control.addVariable(valve9Name+"YAngle", -135.0);

  beamStopPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowH",10.0); // Dummy length

  viewBPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowI",10.0); // Dummy length

  GateGen.setBladeMat("SteelUnknownGrade");
  GateGen.setCylCF<setVariable::CF40>(); // [26]
  const double CRLGateTotalLength = 3.5+2.0*CF40::flangeLength; // [26]
  GateGen.setLength(CRLGateTotalLength-2.0*CF40::flangeLength);
  const std::string CRLGatePrefix = opticsName+"CRLGate";
  GateGen.generateValve(Control,CRLGatePrefix+"In",0.0,0);
  Control.addVariable(CRLGatePrefix+"InYAngle",20.0); // Angle estimated from [26]

  lensPackage(Control,opticsName);

  GateGen.generateValve(Control,CRLGatePrefix+"Out",0.0,0);
  Control.addVariable(CRLGatePrefix+"OutYAngle",-20.0); // Angle estimated from [26]

  BellowGen.generateBellow(Control,opticsName+"BellowJ",10.0); // Dummy length

  revBeamStopPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowK",10.0); // Dummy length

  monoShutterVariables(Control,opticsName);
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
  constexpr double offPipeALength = 6.8; // [7]
  constexpr double shutterBoxLength = shutterLength - offPipeALength;

  constexpr double bremCollTotalLength = 21.0; //[4] (OffPipeB + BremCollPipe)
  constexpr double offPipeBLength = 2.6; // as small as possible
  constexpr double bremCollPipeLength = bremCollTotalLength - offPipeBLength;

  constexpr double proxiShieldBPipeEnd = 2110.0 - 2.97; // [4, page1]
  constexpr double bellowIYstep = proxiShieldBPipeEnd - proxiShieldBPipeLength -
    bremCollTotalLength - shutterLength - proxiShieldAPipeLength -
    danmaxVar::valve3Length - bellowJLength - florTubeALength - bellowILength;
  // same as counting from Movable Mask 2
  // 18692.8 + 300 + 140 + 17.5 + 340 = 19490.3
  assert(std::abs(bellowIYstep - 1949.03)<Geometry::zeroTol && "Wrong shutter table length."); // [4]

  BellowGen.setMat("SteelUnknownGrade", "SteelUnknownGrade");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",bellowILength);
  Control.addVariable(frontKey+"BellowIYStep",bellowIYstep);

  SimpleTubeGen.setCF<CF63>(); // Closest match with ICF114 given in [17]
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.setAFlange(CF63::outerRadius, 0.0);
  const double florTubeATotalHeight = 15.0; // [18]
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",florTubeATotalHeight);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  const double florTubeAHeight = 11.0; // [17]
  const Geometry::Vec3D portPos(0,0.5*florTubeATotalHeight-florTubeAHeight,0);
  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+2.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,florName+"Port0",portPos,ZVec);
  PItemGen.generatePort(Control,florName+"Port1",portPos,-ZVec);
  PItemGen.setPlate(CF40::flangeLength,"SteelUnknownGrade");
  PItemGen.generatePort(Control,florName+"Port2",portPos,XVec);
  PItemGen.generatePort(Control,florName+"Port3",portPos,-XVec);

  Control.addVariable(florName+"Port0Length",0.5*florTubeALength);
  Control.addVariable(florName+"Port1Length",0.5*florTubeALength);
  Control.addVariable(florName+"Port2Length",5.85); // [17]
  Control.addVariable(florName+"Port3Length",5.85); // [17]


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",bellowJLength);

  PipeGen.setMat("SteelUnknownGrade");
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
  const double shutterBoxWallThick = 0.3; // [7]
  Control.addVariable(shutterName+"WallThick",shutterBoxWallThick); // [7]
  Control.addVariable(shutterName+"Radius",
    CF150::flangeRadius-shutterBoxWallThick); // [7]

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

  // Off Pipe B is built from the same class as Off Pipe A for simplicity.
  // Due to its shortness, could also be simplified to a FlangePlate.
  PipeGen.setCF<setVariable::CF63>(); // [7]
  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setAFlangeCF<setVariable::CF150>(); //
  PipeGen.setBFlange(1.0, 0.0);
  constexpr double bremCollPipeInnerRadius = 4.35; // [6]
  // Built to match Bremsstrahlung Collimator pipe
  PipeGen.setPipe(CF63::innerRadius,bremCollPipeInnerRadius
    +CF100::wallThick-CF63::innerRadius);
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",offPipeBLength);
  Control.addVariable(frontKey+"OffPipeBFlangeAZStep",3.0); // [7]
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0); // [7]

  Control.addVariable(frontKey+"BremBlockRadius",3.0); // [6]
  Control.addVariable(frontKey+"BremBlockLength",20.0); // [6]
  Control.addVariable(frontKey+"BremBlockHoleWidth",1.5); // [6]
  Control.addVariable(frontKey+"BremBlockHoleHeight",0.7); // [6]
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten"); // [6]

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
  PipeGen.setCF<setVariable::CF100>(); // ICF152 in [6]
  PipeGen.generatePipe(Control,name,bremCollPipeLength);
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
    (Control,frontKey+"IonPump4",0.0,10.4,55.0);   // height/depth


  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"PumpingUnit1ReplacementPipe",35.0); // [4] - replacement for pumping unit, length is dummy (defined by FM1 absolute y-offset)
  PipeGen.generatePipe(Control,frontKey+"PumpingUnit2ReplacementPipe",35.0); // [4] - replacement for pumping unit, length is dummy (defined by FM1 absolute y-offset)

  setVariable::BladeBPMToyamaGenerator XBPMGen;

  XBPMGen.generate(Control,frontKey+"XBPM1", danmaxVar::absY::XBPM1);
  XBPMGen.generate(Control,frontKey+"XBPM2", danmaxVar::absY::XBPM2);


  danmaxVar::frontMaskVariables(Control,frontKey);

  // [AR 251104: checked by JR 251103]
  Control.addVariable(frontKey+"ProxiShieldAWallMat","Void");
  // [AR 251104: checked by JR 251103]
  Control.addVariable(frontKey+"ProxiShieldBWallMat","Void");

  PipeGen.setMat("SteelUnknownGrade");
  PipeGen.setCF<setVariable::CF40>(); // [18]
  PipeGen.generatePipe(Control,beamLineName+"JoinPipe",146.0); // [4]

  danmaxVar::opticsHutVariables(Control,beamLineName+"OpticsHut");
  danmaxVar::opticsVariables(Control,beamLineName);

  PipeGen.generatePipe(Control,beamLineName+"JoinPipeB",10.0); // Dummy length

  const std::string guillotineName = beamLineName + "GuillotineOHToEH2";
  // All dimensions from Section D-D in [1] if not indicated otherwise.
  Control.addVariable(guillotineName+"Length",12.0);
  Control.addVariable(guillotineName+"Width",40.0);
  Control.addVariable(guillotineName+"Height",40.0);
  // Roughly estimated from drawing. Uncertainty +- 2 mm.
  Control.addVariable(guillotineName+"WallThick",2.0);
  // Hole diameter of 41 mm is the same as JoinPipeB outer diameter.
  Control.addVariable(guillotineName+"ClearGap",0.0);
  Control.addVariable(guillotineName+"WallMat","SteelUnknownGrade");
  Control.addVariable(guillotineName+"Mat","Lead");

  danmaxVar::shieldVariables<setVariable::CF40>(Control);
  danmaxVar::connectVariables(Control,beamLineName+"ConnectUnit");

  PipeGen.setCF<setVariable::CF40>();
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
