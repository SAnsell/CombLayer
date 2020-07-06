/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/targetVariables.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell/Konstantin Batkov
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
EssWheel(FuncDataBase& Control)
  /*!
    Variables that are used for the segmented wheel
    \param Control :: Segment variables
   */
{
  Control.addVariable("BilbaoWheelShaftHeight",435.0);

  const size_t nSectors(36);
  const std::string ss316l("SS316L");

  Control.addVariable("TargetTopClearanceHeight", 6.0); // TSV32IS
  Control.addVariable("TargetTopClearanceMat", "Void");

  Control.addVariable("TargetLowClearanceHeight", 9.2); // TSV32IS
  Control.addVariable("TargetLowClearanceMat", "Void");

  Control.addVariable("BilbaoWheelNShaftLayers",6);
  Control.addVariable("BilbaoWheelShaftRadius1",5.0);
  Control.addVariable("BilbaoWheelShaftMat1","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius2",13.5);
  Control.addVariable("BilbaoWheelShaftMat2","SS316L3925");
  Control.addVariable("BilbaoWheelShaftRadius3",14.0);
  Control.addVariable("BilbaoWheelShaftMat3","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius4",20.0); // ESS-0124024 page 23 (diam 400)
  Control.addVariable("BilbaoWheelShaftMat4","SS316L3925");
  const double shaftR5(23.0);
  Control.addVariable("BilbaoWheelShaftRadius5",shaftR5);
  Control.addVariable("BilbaoWheelShaftMat5","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius6",25.0);
  Control.addVariable("BilbaoWheelShaftMat6","Void");

  Control.addVariable("BilbaoWheelShaft2StepHeight",15);  // TSM141108V3000
  Control.addVariable("BilbaoWheelShaft2StepConnectionHeight",6.4);  // TSV31-TargetUpdate02
  Control.addVariable("BilbaoWheelShaft2StepConnectionDist", 25);  // TSV31-TargetUpdate02
  Control.addVariable("BilbaoWheelShaft2StepConnectionRadius", 28.25);  // ESS-0124024 page 2 + TSV31-TargetUpdate02
  
  Control.addVariable("BilbaoWheelShaftConnectionFlangeRingHeight",3.0); // TSV31-TargetUpdate02
  Control.addVariable("BilbaoWheelShaftConnectionFlangeRingRadius",31.0); // ESS-0124024 page 23

  // Upper Stiffener shape and dimensions correspond to
  // ESS-0153983
  const double ubsLength(20.0);
  const double ubsThick(2.0);
  Control.addVariable("BilbaoWheelShaftUpperBigStiffLength",ubsLength); // ESS-0124024 page 19
  Control.addVariable("BilbaoWheelShaftUpperBigStiffThick",ubsThick); // ESS-0124024 page 19
  Control.addVariable("BilbaoWheelShaftUpperBigStiffHeight",15); // ESS-0124024 page 19

  const double fracUp(nSectors/2*ubsThick/M_PI/(2*shaftR5+ubsLength)*100.0);
  Control.addVariable("BilbaoWheelShaftUpperBigStiffHomoMat",
		      ss316l + "%Void%" + std::to_string(fracUp));

  const double lbsThick(2.0); // ESS-0124024 page 19
  const double lbsHeight(15.0);  // TSV32IS
  const double lbsSL(8.0);  // TSV32IS
  const double lbsLL(20.0);  // TSV32IS
  Control.addVariable("BilbaoWheelShaftLowerBigStiffShortLength",lbsSL);
  Control.addVariable("BilbaoWheelShaftLowerBigStiffLongLength",lbsLL);
  Control.addVariable("BilbaoWheelShaftLowerBigStiffHeight",lbsHeight);
  Control.addVariable("BilbaoWheelShaftLowerBigStiffThick",lbsThick);

  // Fraction of total volume of stiffeners / total volume of (stiffeners + void between them)
  const double R1(shaftR5);
  const double R2(R1+lbsSL);
  const double R3(R1+lbsLL);
  const size_t N(nSectors/2);
  double fracLow = N*lbsThick / 2.0 / M_PI;
  fracLow *= 0.5*(R2+R3)-R1;
  fracLow /= 0.5*(pow(R2,2)+pow(R3,2))-pow(R1,2);
  fracLow *= 100; // checked [total volume 3% above MonteCarlo estimate]
  Control.addVariable("BilbaoWheelShaftLowerBigStiffHomoMat",
		      ss316l + "%Void%" + std::to_string(fracLow));

  Control.addVariable("BilbaoWheelShaftHoleHeight",4.5); // TSM141108V3000
  Control.addVariable("BilbaoWheelShaftHoleSize",0.25); // value not known=>approx
  Control.addVariable("BilbaoWheelShaftHoleXYAngle",-1.0);  // value not known=>approx to put a hole
  
  Control.addVariable("BilbaoWheelShaftBaseDepth",33.8); // TSV32IS

  Control.addVariable("BilbaoWheelCatcherTopSteelThick",2.0);
  
  Control.addVariable("BilbaoWheelCatcherRadius",42.0);
  Control.addVariable("BilbaoWheelCatcherBaseHeight",8.0);  // TSV32IS
  Control.addVariable("BilbaoWheelCatcherBaseRadius",21.25);  // TSV32IS
  Control.addVariable("BilbaoWheelCatcherBaseAngle",45.0);  // TSV32IS + ESS-0038811
  Control.addVariable("BilbaoWheelCatcherNotchBaseThick",3.0);// TSV32IS
  Control.addVariable("BilbaoWheelCatcherNotchRadius",20.0);  // TSV32IS

  Control.addVariable("BilbaoWheelCirclePipesBigRad",30.0);
  Control.addVariable("BilbaoWheelCirclePipesRad",1.5);
  Control.addVariable("BilbaoWheelCirclePipesWallThick",0.2);
  
  Control.addVariable("BilbaoWheelXStep",0.0);
  Control.addVariable("BilbaoWheelYStep",112.2);
  Control.addVariable("BilbaoWheelZStep",0.0);
  Control.addVariable("BilbaoWheelXYAngle",0.0);
  Control.addVariable("BilbaoWheelZAngle",0.0);
  Control.addVariable("BilbaoWheelTargetHeight",8.0); // TSM141108V3000
  Control.addVariable("BilbaoWheelTargetInnerHeight",6.6); // TSM141108V3000
  // TSV31-TargetUpdate02
  // this sets the inner W radius to 77.2 cm
  // 77.2 = R750 + 2.2 in the upper sector plate
  // R750: TRGT-ESS-0106 page 2
  // 2.2: detail B in  TRGT-ESS-0106 page 1
  // In the lower sector plate:
  // 77.2 = R74.5 + 2.7

  // 71.8 = 76.7-4.9 (BilbaoWheelSecWallSegLength0)
  Control.addVariable("BilbaoWheelTargetInnerHeightRadius",71.8);

  Control.addVariable("BilbaoWheelVoidTungstenThick", 0.1);
  Control.addVariable("BilbaoWheelSteelTungstenThick", 0.2); // TSM141108V3000: upper 0.2, lower 0.3
  Control.addVariable("BilbaoWheelSteelTungstenInnerThick", 0.5); // TSM141108V3000: upper 0.2, lower 0.3
  Control.addVariable("BilbaoWheelTemp",600.0);
  Control.addVariable("BilbaoWheelCoolantThick",0.5);
  Control.addVariable("BilbaoWheelCaseThick",1.0);
  Control.addVariable("BilbaoWheelVoidThick",2.0);

  Control.addVariable("BilbaoWheelInnerRadius",45); // TSM141108V3000
  Control.addVariable("BilbaoWheelInnerHoleHeight",4.5); // TSM141108V3000
  Control.addVariable("BilbaoWheelInnerHoleSize",0.25); // value not known=>approx
  Control.addVariable("BilbaoWheelInnerHoleXYAngle",-1.0);  // value not known=>approx to put a hole on the Y-axis (0,65,0) to increase high-energy noise for conservative reasons

  Control.addVariable("BilbaoWheelCoolantRadiusIn",64.575); // TSM141108V3000
  Control.addVariable("BilbaoWheelCoolantRadiusOut",130.8); // Bilbao-MCNP-geometry.inp (received from LZ 3 Aug 2017)
  Control.addVariable("BilbaoWheelCaseRadius",131); // Bilbao-MCNP-geometry.inp (received from LZ 3 Aug 2017)
  Control.addVariable("BilbaoWheelVoidRadius",131.2); // Bilbao-MCNP-geometry.inp (received from LZ 3 Aug 2017)
  Control.addVariable("BilbaoWheelAspectRatio", 0.00138);
  Control.addVariable("BilbaoWheelNSectors", nSectors);
  Control.addVariable("BilbaoWheelSectorSepThick", 1.0);
  Control.addVariable("BilbaoWheelSectorSepMat", "SS316L");
  Control.addVariable("BilbaoWheelTemperature", 600);

  Control.addVariable("BilbaoWheelHomoWMat","Tungsten_15.3g"); // email from LZ 25 Oct 2017
  // calculated to have the same density fraction with respect to
  // SS316L as Tungsten/Tungsten_15.3g = 1.26131
  Control.addVariable("BilbaoWheelHomoSteelMat","SS316L_6.22g");
  Control.addVariable("BilbaoWheelSteelMat",ss316l);
  Control.addVariable("BilbaoWheelHeMat","Void"); // TSM141108V3000
  Control.addVariable("BilbaoWheelSS316LVoidMat","M2644"); // !!! use appropriate name
  Control.addVariable("BilbaoWheelInnerMat","Void");

  Control.addVariable("BilbaoWheelNLayers",3);

  Control.addVariable("BilbaoWheelRadius1",48);
  Control.addVariable("BilbaoWheelMatTYPE1",1); // SS316L

  Control.addVariable("BilbaoWheelRadius2",77.0); // TSM141108V3000
  Control.addVariable("BilbaoWheelMatTYPE2",0); // TSM141108V3000

  Control.addVariable("BilbaoWheelRadius3",126.2); // TSV31-TargetUpdate02 (email from AT 22 Nov 2017)
  Control.addVariable("BilbaoWheelMatTYPE3",3);

  // Sectors
  Control.addVariable("BilbaoWheelSecWallMat","SS316L");
  Control.addVariable("BilbaoWheelSecWallThick",0.0);

  // The sector variables are based on the
  // ESS-Bilbao drawing TRGT-ESS-0106.01.03 rev.4
  // The same drawings in CHESS: ESS-0102065
  // 3D project: ESS-0017676
  // https://plone.esss.lu.se/docs/neutronics/engineering/drawings/target/cassette-side-steel/view

  const std::vector<size_t> nBricks({0,9,10,11,10,11,12,11,12,13,14,13,14,15,16,15});
  const std::vector<double> segLen({4.9,-3.2,-3.4,3.0,-3.2,-3.4,3.0,-3.2,-3.2,-3.4,3.0,-3.2,-3.2,-3.4,3.0,-4-1.2});
  const size_t NB(nBricks.size());
  Control.addVariable("BilbaoWheelSecNWallSeg",NB); // 15 layers of bricks + 1
  if (segLen.size() != NB)
    ELog::EM << "Different lengths of vectors segLen and nBricks" << ELog::endCrit;

  for (size_t i=0; i<NB; i++)
    {
      const std::string si(std::to_string(i));
      Control.addVariable("BilbaoWheelSecWallSegLength"+si,segLen[i]);
      Control.addVariable("BilbaoWheelSecWallSegNBricks"+si,nBricks[i]);
    }

  Control.addVariable("BilbaoWheelSecWallSegThick",1.6); // 1st segment only - thickness of the others depends on total number of bircks and gaps
  Control.addVariable("BilbaoWheelSecBrickWidth",1.0);
  Control.addVariable("BilbaoWheelSecBrickLength",3.0);
  Control.addVariable("BilbaoWheelSecBrickGap",0.2);
  Control.addVariable("BilbaoWheelSecBrickSteelMat","SS316L");
  Control.addVariable("BilbaoWheelSecBrickWMat","Tungsten");
  Control.addVariable("BilbaoWheelSecNSteelRows",2);
  Control.addVariable("BilbaoWheelSecPipeCellThick", 1.2);
  Control.addVariable("BilbaoWheelSecPipeCellMat", "SS316LBilbaoWheelPipeCellMat");

  Control.addVariable("WheelShaftNLayers",3);
  Control.addVariable("WheelShaftHeight",435.0);
  Control.addVariable("WheelShaftRadius",32.0);
  Control.addVariable("WheelShaftCoolThick",1.0);
  Control.addVariable("WheelShaftCladThick",0.5);
  Control.addVariable("WheelShaftVoidThick",0.8);

  Control.addVariable("WheelCladShaftMat","Iron");
  Control.addVariable("WheelMainShaftMat","Iron");

  Control.addVariable("WheelXStep",0.0);  
  Control.addVariable("WheelYStep",113.0);  
  Control.addVariable("WheelZStep",0.0);
  Control.addVariable("WheelXYAngle",0.0); 
  Control.addVariable("WheelZAngle",0.0);
  Control.addVariable("WheelTargetHeight",8.0);
  Control.addVariable("WheelTemp",600.0);
  Control.addVariable("WheelCoolantThickIn",0.65);
  Control.addVariable("WheelCoolantThickOut",0.15);
  Control.addVariable("WheelCaseThick",0.5);
  Control.addVariable("WheelVoidThick",1.0);

  Control.addVariable("WheelInnerRadius",84.27);
  Control.addVariable("WheelCoolantRadiusIn",114.5);
  Control.addVariable("WheelCoolantRadiusOut",124.8);
  Control.addVariable("WheelCaseRadius",125.0);
  Control.addVariable("WheelVoidRadius",126.0);

  Control.addVariable("WheelWMat","Tungsten");
  Control.addVariable("WheelSteelMat","SS316L");
  Control.addVariable("WheelHeMat",0);
  Control.addVariable("WheelInnerMat","SS316L");

  Control.addVariable("WheelNLayers",24);

  Control.addVariable("WheelRadius1",85.65);
  Control.addVariable("WheelMatTYPE1",2);

  Control.addVariable("WheelRadius2",97.02);
  Control.addVariable("WheelMatTYPE2",3);

  Control.addVariable("WheelRadius3",97.52);
  Control.addVariable("WheelMatTYPE3",2);

  Control.addVariable("WheelRadius4",102.37);
  Control.addVariable("WheelMatTYPE4",3);

  Control.addVariable("WheelRadius5",102.77);    // WRONG
  Control.addVariable("WheelMatTYPE5",2);

  Control.addVariable("WheelRadius6",106.97);
  Control.addVariable("WheelMatTYPE6",3);

  Control.addVariable("WheelRadius7",107.29);
  Control.addVariable("WheelMatTYPE7",2);

  Control.addVariable("WheelRadius8",110.49);
  Control.addVariable("WheelMatTYPE8",3);

  Control.addVariable("WheelRadius9",110.78);
  Control.addVariable("WheelMatTYPE9",2);

  Control.addVariable("WheelRadius10",112.78);
  Control.addVariable("WheelMatTYPE10",3);

  Control.addVariable("WheelRadius11",113.05);
  Control.addVariable("WheelMatTYPE11",2);

  Control.addVariable("WheelRadius12",114.65);
  Control.addVariable("WheelMatTYPE12",3); 

  Control.addVariable("WheelRadius13",114.9);
  Control.addVariable("WheelMatTYPE13",2); 

  Control.addVariable("WheelRadius14",116.5);
  Control.addVariable("WheelMatTYPE14",3); 

  Control.addVariable("WheelRadius15",116.75);
  Control.addVariable("WheelMatTYPE15",2); 

  Control.addVariable("WheelRadius16",118.35);
  Control.addVariable("WheelMatTYPE16",3); 

  Control.addVariable("WheelRadius17",118.6);
  Control.addVariable("WheelMatTYPE17",2); 

  Control.addVariable("WheelRadius18",120);
  Control.addVariable("WheelMatTYPE18",3); 

  Control.addVariable("WheelRadius19",120.25);
  Control.addVariable("WheelMatTYPE19",2); 

  Control.addVariable("WheelRadius20",121.65);
  Control.addVariable("WheelMatTYPE20",3); 

  Control.addVariable("WheelRadius21",121.9);
  Control.addVariable("WheelMatTYPE21",2); 

  Control.addVariable("WheelRadius22",123.1);
  Control.addVariable("WheelMatTYPE22",3); 

  Control.addVariable("WheelRadius23",123.35);
  Control.addVariable("WheelMatTYPE23",2); 

  Control.addVariable("WheelRadius24",124.55);
  Control.addVariable("WheelMatTYPE24",3); 

  
  return;
}

void
EssProtonBeam(FuncDataBase& Control)
  /*!
    Set proton beam variables
    \param Control :: DataBase for variables
  */
{
  Control.addVariable("ProtonTubePipeXStep",0.0);  
  Control.addVariable("ProtonTubePipeYStep",0.0);  
  Control.addVariable("ProtonTubePipeZStep",0.0);
  Control.addVariable("ProtonTubePipeXYAngle",0.0); 
  Control.addVariable("ProtonTubePipeZAngle",0.0);

  Control.addVariable("ProtonTubePipeNSection",4);

  Control.addVariable("ProtonTubePipeRadius1",20.0); // MK170207
  Control.addVariable("ProtonTubePipeLength1",37.0); // MK170207
  Control.addVariable("ProtonTubePipeZcut1",0.0);  // to have the same height as BeRef::targetVoid
  Control.addVariable("ProtonTubePipeWallThick1",1.0);
  Control.addVariable("ProtonTubePipeInnerMat1","Helium");  // mat : 2000
  Control.addVariable("ProtonTubePipeWallMat1","SS316L"); // mat : 26316

  Control.addVariable("ProtonTubePipeRadius2",32.5/2.0); // MK170207
  Control.addVariable("ProtonTubePipeLength2",35.0); // MK170207
  Control.addVariable("ProtonTubePipeZcut2",0.0); 
  Control.addVariable("ProtonTubePipeWallThick2",1.0);
  Control.addVariable("ProtonTubePipeInnerMat2","Helium");  // mat : 2000
  Control.addVariable("ProtonTubePipeWallMat2","SS316L");  // differs from TSV30: M2636

  Control.addVariable("ProtonTubePipeRadius3",21.0/2.0); // MK170207
  Control.addVariable("ProtonTubePipeLength3",127.5);
  Control.addVariable("ProtonTubePipeZcut3",0.0);
  Control.addVariable("ProtonTubePipeWallThick3",4.0);
  Control.addVariable("ProtonTubePipeInnerMat3","Helium");   // mat : 2000
  Control.addVariable("ProtonTubePipeWallMat3","SS316L");  // differs from TSV30: M2636

  Control.addVariable("ProtonTubePipeRadius4",10.5);  // same as in TSV30
  // Control.addVariable("ProtonTubePipeLength4",147.5);
  Control.addVariable("ProtonTubePipeLength4",152.5);
  Control.addVariable("ProtonTubePipeZcut4",0.0);
  Control.addVariable("ProtonTubePipeWallThick4",4.0);
  Control.addVariable("ProtonTubePipeInnerMat4","Void");
  Control.addVariable("ProtonTubePipeWallMat4","SS316L");  // differs from TSV30: M2636


  Control.addVariable("BeamMonitorXStep",0.0);
  Control.addVariable("BeamMonitorYStep",450.0);
  Control.addVariable("BeamMonitorZStep",0.0);
  Control.addVariable("BeamMonitorXYAngle",0.0);
  Control.addVariable("BeamMonitorZAngle",0.0);
  
  Control.addVariable("BeamMonitorBoxSide",70.0);
  Control.addVariable("BeamMonitorBoxNSections",5); //other are symmetric

  Control.addVariable("BeamMonitorBoxRadius1",11.5);
  Control.addVariable("BeamMonitorBoxThick1",2.5);
  Control.addVariable("BeamMonitorBoxMat1","helium");  // mat:2000
  Control.addVariable("BeamMonitorBoxRadius2",32.5);
  Control.addVariable("BeamMonitorBoxThick2",10.0);
  Control.addVariable("BeamMonitorBoxMat2","CastIron");  // mat: 26000
  Control.addVariable("BeamMonitorBoxRadius3",20.0);
  Control.addVariable("BeamMonitorBoxThick3",2.5);
  Control.addVariable("BeamMonitorBoxMat3","CastIron");  // mat 26316
  Control.addVariable("BeamMonitorBoxRadius4",17.5);
  Control.addVariable("BeamMonitorBoxThick4",17.5);
  Control.addVariable("BeamMonitorBoxMat4","CastIron");  // mat 26316
  Control.addVariable("BeamMonitorBoxRadius5",25.0);
  Control.addVariable("BeamMonitorBoxThick5",5.0);
  Control.addVariable("BeamMonitorBoxMat5","Aluminium");  // mat 13060

  Control.addVariable("BeamMonitorBoxSide5",50.0);
  Control.addVariable("BeamMonitorBoxHeightA5",14.60);
  Control.addVariable("BeamMonitorBoxHeightB5",8.60);
  Control.addVariable("BeamMonitorBoxHeightC5",7.5);
  Control.addVariable("BeamMonitorBoxHeightD5",7.0);

  Control.addVariable("BeamMonitorBoxWidthA5",21.40);
  Control.addVariable("BeamMonitorBoxWidthB5",20.30);
  Control.addVariable("BeamMonitorBoxWidthC5",19.80);

  Control.addVariable("BeamMonitorBoxThickA5",3.2);
  Control.addVariable("BeamMonitorBoxThickB5",0.5);
  Control.addVariable("BeamMonitorBoxThickC5",0.2);

  Control.addVariable("BeamMonitorBoxTubeN",33);
  Control.addVariable("BeamMonitorBoxTubeRadius",0.27);
  Control.addVariable("BeamMonitorBoxTubeThick",0.03);

  // Control.addVariable("BeamMonitorBoxTubeThick",0.03);
  Control.addVariable("BeamMonitorBoxTubeHeMat","helium");
  Control.addVariable("BeamMonitorBoxTubeAlMat","Aluminium");
  Control.addVariable("BeamMonitorBoxExtHeMat","helium");

  Control.addVariable("ProtonBeamViewRadius",4.0);

  // Proton beam window
  Control.addVariable("ProtonTubePBWPlugMat","SS316L");
  Control.addVariable("ProtonTubePBWPlugLength1",45.0); // Drawing received from AT (https://plone.esss.lu.se/docs/neutronics/engineering/drawings/monolith/pbw-drawings/view)
  Control.addVariable("ProtonTubePBWPlugLength2",43.0);
  Control.addVariable("ProtonTubePBWPlugWidth1",76.3);
  Control.addVariable("ProtonTubePBWPlugWidth2",93.0);
  Control.addVariable("ProtonTubePBWPlugHeight",42.5);
  Control.addVariable("ProtonTubePBWPlugDepth",54.5);

  Control.addVariable("ProtonTubePBWPlugVoidLength",41.5);
  Control.addVariable("ProtonTubePBWPlugVoidWidth",50.0);
  Control.addVariable("ProtonTubePBWPlugVoidDepth",25.0);
  Control.Parse("ProtonTubePBWPlugHeight");
  Control.addVariable("ProtonTubePBWPlugVoidHeight");

  Control.addVariable("ProtonTubePBWPlugAlLength", 9.695); // ESS-0066872.1 page 4
  Control.addVariable("ProtonTubePBWPlugAlGrooveRadius", 16.0); // ESS-0066872.1 page 4
  Control.addVariable("ProtonTubePBWPlugAlGrooveDepth", 0.5); // ESS-0066872.1 page 4
  Control.addVariable("ProtonTubePBWPlugAlGapHeight", 20.75); // ESS-0066872.1 page 3 and 4
  Control.addVariable("ProtonTubePBWPlugAlGapWidth", 19.8); // ESS-0058437.3 and ESS-0066872.1 page 3 (there is a frame which is inserted into gap, but in this (and Alan's) geometries it is not modelled, just gap width reduced)

  Control.Parse("ProtonTubePipeRadius4");
  Control.addVariable("ProtonTubePBWPipeRadius");
  Control.addVariable("ProtonTubePBWPipeMatBefore", "Void");
  Control.addVariable("ProtonTubePBWPipeMatAfter", "Helium");

  Control.addVariable("ProtonTubePBWFlangeRadius",29.0/2); // ESS-0066872.1 page 5
  Control.addVariable("ProtonTubePBWFlangeThick",(45.0-29.0)/2.0); // ESS-0066872.1 page 5
  Control.addVariable("ProtonTubePBWFlangeWaterRingRadiusIn",31.0/2.0); // ESS-0066872.1 page 5
  Control.addVariable("ProtonTubePBWFlangeWaterRingRadiusOut",42.0/2.0); // ESS-0066872.1 page 5
  Control.addVariable("ProtonTubePBWFlangeWaterRingThick",5.4); // ESS-0066872.1 page 5
  Control.addVariable("ProtonTubePBWFlangeWaterRingOffset",4.5); // TSV30
  Control.addVariable("ProtonTubePBWFlangeNotchDepth",3.5); // TSV30
  Control.addVariable("ProtonTubePBWFlangeNotchThick",4.0); // TSV30
  Control.addVariable("ProtonTubePBWFlangeNotchOffset",10.9); // TSV30

  
  Control.addVariable("ProtonTubePBWCoolingMat","H2O");
  Control.addVariable("ProtonTubePBWMat","Aluminium");
  Control.addVariable("ProtonTubePBWYStep",-375);
  
  Control.addVariable("ProtonTubePBWFoilThick",0.4); // ESS-0058437.3
  Control.addVariable("ProtonTubePBWFoilRadius",8.9); // ESS-0058437.3
  Control.addVariable("ProtonTubePBWFoilOffset",1.0); // ESS-0066872.1 page 3
  Control.addVariable("ProtonTubePBWFoilCylOffset",7.0); // to make 2.0 cm top/bottom distance as in ESS-0058437.3
  Control.addVariable("ProtonTubePBWFoilWaterThick",0.2); // ESS-0058437.3
  Control.addVariable("ProtonTubePBWFoilWaterLength",9.2); // ESS-0058437.3
  
  Control.addVariable("ProtonTubePBWShieldXStep",0);
  Control.addVariable("ProtonTubePBWShieldYStep",0);
  Control.addVariable("ProtonTubePBWShieldZStep",0);
  Control.addVariable("ProtonTubePBWShieldXYAngle",0);
  Control.addVariable("ProtonTubePBWShieldZAngle",0);

  Control.addVariable("ProtonTubePBWShieldNSection",5);
  Control.addVariable("ProtonTubePBWShieldLength1",21.5); // void
  Control.addVariable("ProtonTubePBWShieldLength2",40.0); // m650
  Control.addVariable("ProtonTubePBWShieldLength3",152.5); // m2634, cell 10008
  Control.addVariable("ProtonTubePBWShieldLength4",98.5); // m2634, c10007
  Control.addVariable("ProtonTubePBWShieldLength5",50.0); // m2634, c10006
  Control.addVariable("ProtonTubePBWShieldLength6",50.0); // !!! remove me
  Control.addVariable("ProtonTubePBWShieldRadius1",30.0); // TSV32IS
  Control.addVariable("ProtonTubePBWShieldRadius2",63.0/2.0); // ESS-0066872.1 page 7
  Control.addVariable("ProtonTubePBWShieldRadius3",73.0/2.0); // ESS-0066872.1 page 7
  Control.addVariable("ProtonTubePBWShieldRadius4",83.0/2.0); // ESS-0066872.1 page 7
  Control.addVariable("ProtonTubePBWShieldRadius5",83.0/2.0); // ESS-0066872.1 page 7
  Control.addVariable("ProtonTubePBWShieldRadius6",83.0/2.0); // ESS-0066872.1 page 7

  for (int i=1; i<=6; i++)
    {
      Control.addVariable("ProtonTubePBWShieldZcut" + std::to_string(i),0);
      Control.addVariable("ProtonTubePBWShieldWallThick" + std::to_string(i),1.5); // TSV32IS
      Control.addVariable("ProtonTubePBWShieldInnerMat" + std::to_string(i),"CastIron");
      Control.addVariable("ProtonTubePBWShieldWallMat" + std::to_string(i),"Void");
    }

  // Beam instrumentation plug (dummy variables)
  Control.addVariable("PBIPYStep",-177.2); // MK170207

  Control.addVariable("PBIPLength",2.1*20/0.85); // measured from MK170207
  Control.addVariable("PBIPWidth",2.6*20/0.85); // measured from MK170207
  Control.addVariable("PBIPHeight",20.0); // a dummy value
  Control.addVariable("PBIPWallThick",1.0);
  Control.addVariable("PBIPMainMat","Helium");
  Control.addVariable("PBIPWallMat","SS316L");

  Control.addVariable("PBIPPipeBeforeHeight",7.0); // MK170207
  Control.addVariable("PBIPPipeBeforeWidthLeft",20.0); // MK170207
  Control.addVariable("PBIPPipeBeforeAngleLeft",4.14); // MK170207
  Control.addVariable("PBIPPipeBeforeWidthRight",9.0); // MK170207
  
  Control.addVariable("PBIPPipeAfterHeight",8.0); // MK170207
  Control.addVariable("PBIPPipeAfterWidthLeft",9.0); // MK170207
  Control.addVariable("PBIPPipeAfterWidthRight",20.0); // MK170207
  Control.addVariable("PBIPPipeAfterAngleRight",4.07); // MK170207

  Control.addVariable("PBIPFoilOffset",5.0); // a dummy value
  ELog::EM << "PBIP Foil offset??? + references for PBIP foil" << ELog::endDebug;
  Control.addVariable("PBIPFoilThick",0.05); // YJL
  Control.addVariable("PBIPFoilMat", "Tungsten_BeamMonitor"); // YJL
  

  
  return;
}
  
}  // NAMESPACE setVariable

// References:
// MK170207: https://plone.esss.lu.se/docs/neutronics/engineering/drawings/monolith/pbip-drawings/view
