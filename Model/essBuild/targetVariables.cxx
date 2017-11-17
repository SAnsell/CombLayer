/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/targetVariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
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
  Control.addVariable("BilbaoWheelEngineeringActive", 1);
  Control.addVariable("BilbaoWheelNShaftLayers",6);
  Control.addVariable("BilbaoWheelShaftRadius1",5.0);
  Control.addVariable("BilbaoWheelShaftMat1","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius2",13.5);
  Control.addVariable("BilbaoWheelShaftMat2","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius3",14.0);
  Control.addVariable("BilbaoWheelShaftMat3","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius4",20.0);
  Control.addVariable("BilbaoWheelShaftMat4","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius5",23.0);
  Control.addVariable("BilbaoWheelShaftMat5","SS316L");
  Control.addVariable("BilbaoWheelShaftRadius6",25.0);
  Control.addVariable("BilbaoWheelShaftMat6","Void");

  Control.addVariable("BilbaoWheelShaft2StepHeight",15);  // TSM141108V3000
  Control.addVariable("BilbaoWheelShaft2StepConnectionHeight",4);  // TSM141108V3000
  Control.addVariable("BilbaoWheelShaft2StepConnectionDist", 5);  // TSM141108V3000
  Control.addVariable("BilbaoWheelShaft2StepConnectionRadius", 27.5);  // TSM141108V3000

  Control.addVariable("BilbaoWheelShaftHoleHeight",4.5); // TSM141108V3000
  Control.addVariable("BilbaoWheelShaftHoleSize",0.25); // value not known=>approx
  Control.addVariable("BilbaoWheelShaftHoleXYangle",-1.0);  // value not known=>approx to put a hole
  
  Control.addVariable("BilbaoWheelShaftBaseDepth",35.0); // TSM141108V3000

  Control.addVariable("BilbaoWheelCatcherTopSteelThick",2.0);
  
  Control.addVariable("BilbaoWheelCatcherHeight",10.0);
  Control.addVariable("BilbaoWheelCatcherRadius",42.0);
  Control.addVariable("BilbaoWheelCatcherMiddleHeight",10.0);
  Control.addVariable("BilbaoWheelCatcherMiddleRadius",32.0);
  Control.addVariable("BilbaoWheelCatcherNotchDepth",5.0);
  Control.addVariable("BilbaoWheelCatcherNotchRadius",22.0);
  Control.addVariable("BilbaoWheelCatcherRingRadius",34.0);
  Control.addVariable("BilbaoWheelCatcherRingDepth",24.0);
  Control.addVariable("BilbaoWheelCatcherRingThick",2.0);

  Control.addVariable("BilbaoWheelCirclePipesBigRad",30.0);
  Control.addVariable("BilbaoWheelCirclePipesRad",1.5);
  Control.addVariable("BilbaoWheelCirclePipesWallThick",0.2);

  Control.addVariable("BilbaoWheelXStep",0.0);
  Control.addVariable("BilbaoWheelYStep",112.2);
  Control.addVariable("BilbaoWheelZStep",0.0);
  Control.addVariable("BilbaoWheelXYangle",0.0);
  Control.addVariable("BilbaoWheelZangle",0.0);
  Control.addVariable("BilbaoWheelTargetHeight",8.0); // TSM141108V3000
  Control.addVariable("BilbaoWheelTargetInnerHeight",6.6); // TSM141108V3000
  Control.addVariable("BilbaoWheelTargetInnerHeightRadius",74.5); // TSM141108V3000
  
  Control.addVariable("BilbaoWheelVoidTungstenThick", 0.1);
  Control.addVariable("BilbaoWheelSteelTungstenThick", 0.2);
  Control.addVariable("BilbaoWheelSteelTungstenInnerThick", 0.5); // TSM141108V3000: upper 0.2, lower 0.3
  Control.addVariable("BilbaoWheelTemp",600.0);
  Control.addVariable("BilbaoWheelCoolantThick",0.5);
  Control.addVariable("BilbaoWheelCaseThick",1.0);
  Control.addVariable("BilbaoWheelCaseThickIn",3.0);
  Control.addVariable("BilbaoWheelVoidThick",2.0);

  Control.addVariable("BilbaoWheelInnerRadius",45);
  Control.addVariable("BilbaoWheelInnerHoleHeight",4.5); // TSM141108V3000
  Control.addVariable("BilbaoWheelInnerHoleSize",0.25); // value not known=>approx

  // value not known=>
  //  approx to put a hole on the Y-axis (0,65,0) to increase high-energy noise for conservative reasons
  Control.addVariable("BilbaoWheelInnerHoleXYangle",-1.0);
											       
  Control.addVariable("BilbaoWheelCoolantRadiusIn",64.07);
  Control.addVariable("BilbaoWheelCoolantRadiusOut",128.95);
  Control.addVariable("BilbaoWheelCaseRadius",129.15);
  Control.addVariable("BilbaoWheelVoidRadius",131.15);
  Control.addVariable("BilbaoWheelAspectRatio", 0.00138);
  Control.addVariable("BilbaoWheelNSectors", 36);
  Control.addVariable("BilbaoWheelSectorSepThick", 1.0);
  Control.addVariable("BilbaoWheelSectorSepMat", "SS316L785");
  Control.addVariable("BilbaoWheelTemperature", 600);

  Control.addVariable("BilbaoWheelWMat","Tungsten151");
  Control.addVariable("BilbaoWheelSteelMat","SS316L785");
  Control.addVariable("BilbaoWheelHeMat","Helium");
  Control.addVariable("BilbaoWheelSS316LVoidMat","M2644"); // !!! use appropriate name
  Control.addVariable("BilbaoWheelInnerMat","SS316L785");

  Control.addVariable("BilbaoWheelNLayers",3);

  Control.addVariable("BilbaoWheelRadius1",48);
  Control.addVariable("BilbaoWheelMatTYPE1",1); // SS316L

  Control.addVariable("BilbaoWheelRadius2",85.0);
  Control.addVariable("BilbaoWheelMatTYPE2",1);

  Control.addVariable("BilbaoWheelRadius3",125.0);
  Control.addVariable("BilbaoWheelMatTYPE3",3);

  // Inner structure - engineering details
  Control.addVariable("BilbaoWheelInnerStructureXYangle",0.0);
  Control.addVariable("BilbaoWheelInnerStructureBrickLength",3);
  Control.addVariable("BilbaoWheelInnerStructureBrickWidth",1);
  Control.addVariable("BilbaoWheelInnerStructureBrickMat","Tungsten600K");

  Control.addVariable("BilbaoWheelInnerStructureBrickGapLength",0.2);
  Control.addVariable("BilbaoWheelInnerStructureBrickGapWidth",0.2);
  Control.addVariable("BilbaoWheelInnerStructureBrickGapMat","Helium"); // shoud be the same as BilbaoWheelHeMat

  Control.addVariable("BilbaoWheelInnerStructureNSectors", 36);
  Control.addVariable("BilbaoWheelInnerStructureNBrickSectors", 0);
  Control.addVariable("BilbaoWheelInnerStructureSectorSepThick", 1.0);
  Control.addVariable("BilbaoWheelInnerStructureSectorSepMat", "SS316L785");

  Control.addVariable("BilbaoWheelInnerStructureNSteelLayers", 3);
  Control.addVariable("BilbaoWheelInnerStructureBrickSteelMat", "SS316L");

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
  Control.addVariable("WheelXYangle",0.0); 
  Control.addVariable("WheelZangle",0.0);
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
  Control.addVariable("ProtonTubeXStep",0.0);  
  Control.addVariable("ProtonTubeYStep",0.0);  
  Control.addVariable("ProtonTubeZStep",0.0);
  Control.addVariable("ProtonTubeXYangle",0.0); 
  Control.addVariable("ProtonTubeZangle",0.0);

  Control.addVariable("ProtonTubeNSection",4);

  Control.addVariable("ProtonTubeRadius1",11.5);
  Control.addVariable("ProtonTubeLength1",148.25); //from mod centre leftside
  Control.addVariable("ProtonTubeZcut1",3.7);  // to have the same height as BeRef::targetVoid
  Control.addVariable("ProtonTubeWallThick1",0.0);
  Control.addVariable("ProtonTubeInnerMat1","helium");  // mat : 2000
  Control.addVariable("ProtonTubeWallMat1","CastIron"); // mat : 26316

  Control.addVariable("ProtonTubeRadius2",15.0);  // as in TSM141108V2003
  Control.addVariable("ProtonTubeLength2",200.0);
  Control.addVariable("ProtonTubeZcut2",0.0); 
  Control.addVariable("ProtonTubeWallThick2",1.0);
  Control.addVariable("ProtonTubeInnerMat2","helium");  // mat : 2000
  Control.addVariable("ProtonTubeWallMat2","CastIron"); // mat : 26316

  Control.addVariable("ProtonTubeRadius3",15.0);  // as in TSM141108V2003
  Control.addVariable("ProtonTubeLength3",127.5);
  Control.addVariable("ProtonTubeZcut3",0.0);
  Control.addVariable("ProtonTubeWallThick3",1.0);
  Control.addVariable("ProtonTubeInnerMat3","helium");   // mat : 2000
  Control.addVariable("ProtonTubeWallMat3","CastIron");  // mat : 26316

  Control.addVariable("ProtonTubeRadius4",15.0); // as in TSM141108V2003
  // Control.addVariable("ProtonTubeLength4",147.5);
  Control.addVariable("ProtonTubeLength4",152.5);
  Control.addVariable("ProtonTubeZcut4",0.0);
  Control.addVariable("ProtonTubeWallThick4",1.0);
  Control.addVariable("ProtonTubeInnerMat4","Void");
  Control.addVariable("ProtonTubeWallMat4","CastIron");  // mat: 26316


  Control.addVariable("BeamMonitorXStep",0.0);
  Control.addVariable("BeamMonitorYStep",450.0);
  Control.addVariable("BeamMonitorZStep",0.0);
  Control.addVariable("BeamMonitorXYangle",0.0);
  Control.addVariable("BeamMonitorZangle",0.0);
  
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
  Control.addVariable("PBIPFoilMat", "Tungsten#0.04"); // YJL

  Control.addVariable("ProtonBeamViewRadius",4.0);
  return;
}
  
}  // NAMESPACE setVariable
