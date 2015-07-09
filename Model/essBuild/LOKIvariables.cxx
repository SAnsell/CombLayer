/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LOKIvariables.cxx
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
LOKIvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for loki
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("LOKIvariables[F]","LOKIvariables");

  // Bender in section so use cut system
  Control.addVariable("G1BLine2Filled",1);
  
  Control.addVariable("lokiBAXStep",0.0);       
  Control.addVariable("lokiBAYStep",0.0);       
  Control.addVariable("lokiBAZStep",0.0);       
  Control.addVariable("lokiBAXYAngle",0.0);
  Control.addVariable("lokiBAZAngle",0.0);
  Control.addVariable("lokiBABeamXYAngle",0.0);       

  Control.addVariable("lokiBALength",400.0);       
  Control.addVariable("lokiBANShapes",1);       
  Control.addVariable("lokiBANShapeLayers",3);
  Control.addVariable("lokiBAActiveShield",0);

  Control.addVariable("lokiBALayerThick1",0.4);  // glass thick
  Control.addVariable("lokiBALayerThick2",1.5);

  Control.addVariable("lokiBALayerMat0","Void");
  Control.addVariable("lokiBALayerMat1","Glass");
  Control.addVariable("lokiBALayerMat2","Void");       

  Control.addVariable("lokiBA0TypeID","Bend");
  Control.addVariable("lokiBA0AHeight",3.0);
  Control.addVariable("lokiBA0BHeight",3.0);
  Control.addVariable("lokiBA0AWidth",3.0);
  Control.addVariable("lokiBA0BWidth",3.0);
  Control.addVariable("lokiBA0Length",400.0);
  Control.addVariable("lokiBA0AngDir",180.0);
  Control.addVariable("lokiBA0Radius",6600.0);

  // VACBOX A : A[1.08m from Bunker wall]
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("lokiVacAYStep",0.5);
    
  Control.addVariable("lokiVacAVoidHeight",20.0);
  Control.addVariable("lokiVacAVoidDepth",19.0);
  Control.addVariable("lokiVacAVoidWidth",87.0);
  Control.addVariable("lokiVacAVoidLength",100.7);
  
  Control.addVariable("lokiVacAFeHeight",0.5);
  Control.addVariable("lokiVacAFeDepth",0.5);
  Control.addVariable("lokiVacAFeWidth",0.5);
  Control.addVariable("lokiVacAFeFront",0.5);
  Control.addVariable("lokiVacAFeBack",0.5);
  Control.addVariable("lokiVacAFlangeRadius",16.0);  // GUESS
  Control.addVariable("lokiVacAFlangeWall",1.0);  // GUESS
  Control.addVariable("lokiVacAFlangeLength",10.0);  // GUESS
  Control.addVariable("lokiVacAFeMat","Stainless304");

  
  // GUIDE EXITING BEND [Straight A]
  Control.addVariable("lokiGAXStep",0.0);       
  Control.addVariable("lokiGAYStep",0.0);       
  Control.addVariable("lokiGAZStep",0.0);       
  Control.addVariable("lokiGAXYAngle",0.0);       
  Control.addVariable("lokiGAZAngle",0.0);       
  Control.addVariable("lokiGALength",50.0);       
  
  Control.addVariable("lokiGANShapes",1);       
  Control.addVariable("lokiGANShapeLayers",3);
  Control.addVariable("lokiGAActiveShield",0);

  Control.addVariable("lokiGALayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGALayerThick2",1.5);

  Control.addVariable("lokiGALayerMat0","Void");
  Control.addVariable("lokiGALayerMat1","Glass");
  Control.addVariable("lokiGALayerMat2","Void");       

  Control.addVariable("lokiGA0TypeID","Rectangle");
  Control.addVariable("lokiGA0Height",3.0);
  Control.addVariable("lokiGA0Width",3.0);
  Control.addVariable("lokiGA0Length",50.0);
  Control.addVariable("lokiGA0ZAngle",0.0);


  // Double Blade chopper
  Control.addVariable("lokiDBladeXStep",0.0);
  Control.addVariable("lokiDBladeYStep",2.0);
  Control.addVariable("lokiDBladeZStep",0.0);
  Control.addVariable("lokiDBladeXYangle",0.0);
  Control.addVariable("lokiDBladeZangle",0.0);

  Control.addVariable("lokiDBladeGap",3.0);
  Control.addVariable("lokiDBladeInnerRadius",10.0);
  Control.addVariable("lokiDBladeOuterRadius",22.50);
  Control.addVariable("lokiDBladeNDisk",2);

  Control.addVariable("lokiDBlade0Thick",1.0);
  Control.addVariable("lokiDBlade1Thick",1.0);
  Control.addVariable("lokiDBladeInnerMat","Inconnel");
  Control.addVariable("lokiDBladeOuterMat","Aluminium");
  
  Control.addVariable("lokiDBladeNBlades",2);
  Control.addVariable("lokiDBlade0PhaseAngle0",95.0);
  Control.addVariable("lokiDBlade0OpenAngle0",30.0);
  Control.addVariable("lokiDBlade1PhaseAngle0",95.0);
  Control.addVariable("lokiDBlade1OpenAngle0",30.0);

  Control.addVariable("lokiDBlade0PhaseAngle1",275.0);
  Control.addVariable("lokiDBlade0OpenAngle1",30.0);
  Control.addVariable("lokiDBlade1PhaseAngle1",275.0);
  Control.addVariable("lokiDBlade1OpenAngle1",30.0);

  // GUIDE BETWEEN BENDERS
  Control.addVariable("lokiGInnerXStep",0.0);       
  Control.addVariable("lokiGInnerYStep",2.0);       
  Control.addVariable("lokiGInnerZStep",0.0);       
  Control.addVariable("lokiGInnerXYAngle",0.0);       
  Control.addVariable("lokiGInnerZAngle",0.0);       
  Control.addVariable("lokiGInnerLength",40.0);       

  Control.addVariable("lokiGInnerBeamYStep",4.0);
  
  Control.addVariable("lokiGInnerNShapes",1);       
  Control.addVariable("lokiGInnerNShapeLayers",3);
  Control.addVariable("lokiGInnerActiveShield",0);

  Control.addVariable("lokiGInnerLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGInnerLayerThick2",1.5);

  Control.addVariable("lokiGInnerLayerMat0","Void");
  Control.addVariable("lokiGInnerLayerMat1","Glass");
  Control.addVariable("lokiGInnerLayerMat2","Void");       

  Control.addVariable("lokiGInner0TypeID","Rectangle");
  Control.addVariable("lokiGInner0Height",3.0);
  Control.addVariable("lokiGInner0Width",3.0);
  Control.addVariable("lokiGInner0Length",40.0);
  Control.addVariable("lokiGInner0ZAngle",0.0);

  // Single Blade chopper
  Control.addVariable("lokiSBladeXStep",0.0);
  Control.addVariable("lokiSBladeYStep",2.0);
  Control.addVariable("lokiSBladeZStep",0.0);
  Control.addVariable("lokiSBladeXYangle",0.0);
  Control.addVariable("lokiSBladeZangle",0.0);

  Control.addVariable("lokiSBladeInnerRadius",10.0);
  Control.addVariable("lokiSBladeOuterRadius",22.50);
  Control.addVariable("lokiSBladeNDisk",1);

  Control.addVariable("lokiSBlade0Thick",1.0);
  Control.addVariable("lokiSBladeInnerMat","Inconnel");
  Control.addVariable("lokiSBladeOuterMat","B4C");
  
  Control.addVariable("lokiSBladeNBlades",2);
  Control.addVariable("lokiSBlade0PhaseAngle0",95.0);
  Control.addVariable("lokiSBlade0OpenAngle0",30.0);

  Control.addVariable("lokiSBlade0PhaseAngle1",275.0);
  Control.addVariable("lokiSBlade0OpenAngle1",30.0);

  // GUIDE EXITING Single Chopper [Straight A]
  Control.addVariable("lokiGBXStep",0.0);       
  Control.addVariable("lokiGBYStep",2.0);
  Control.addVariable("lokiGBBeamYStep",3.0);
  Control.addVariable("lokiGBZStep",0.0);       
  Control.addVariable("lokiGBXYAngle",0.0);       
  Control.addVariable("lokiGBZAngle",0.0);       
  Control.addVariable("lokiGBLength",45.0);       
  
  Control.addVariable("lokiGBNShapes",1);       
  Control.addVariable("lokiGBNShapeLayers",3);
  Control.addVariable("lokiGBActiveShield",0);

  Control.addVariable("lokiGBLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGBLayerThick2",1.5);

  Control.addVariable("lokiGBLayerMat0","Void");
  Control.addVariable("lokiGBLayerMat1","Glass");
  Control.addVariable("lokiGBLayerMat2","Void");       

  Control.addVariable("lokiGB0TypeID","Rectangle");
  Control.addVariable("lokiGB0Height",3.0);
  Control.addVariable("lokiGB0Width",3.0);
  Control.addVariable("lokiGB0Length",45.0);
  Control.addVariable("lokiGB0ZAngle",0.0);

  // Bender B : Straighten the guide
  Control.addVariable("lokiBBXStep",0.0);       
  Control.addVariable("lokiBBYStep",0.0);       
  Control.addVariable("lokiBBZStep",0.0);       
  Control.addVariable("lokiBBXYAngle",0.0);
  Control.addVariable("lokiBBZAngle",0.0);
  Control.addVariable("lokiBBBeamXYAngle",0.0);       

  Control.addVariable("lokiBBLength",220.0);       
  Control.addVariable("lokiBBNShapes",1);       
  Control.addVariable("lokiBBNShapeLayers",3);
  Control.addVariable("lokiBBActiveShield",0);

  Control.addVariable("lokiBBLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiBBLayerThick2",1.5);

  Control.addVariable("lokiBBLayerMat0","Void");
  Control.addVariable("lokiBBLayerMat1","Glass");
  Control.addVariable("lokiBBLayerMat2","Void");       

  Control.addVariable("lokiBB0TypeID","Bend");
  Control.addVariable("lokiBB0AHeight",3.0);
  Control.addVariable("lokiBB0BHeight",3.0);
  Control.addVariable("lokiBB0AWidth",3.0);
  Control.addVariable("lokiBB0BWidth",3.0);
  Control.addVariable("lokiBB0Length",220.0);
  Control.addVariable("lokiBB0AngDir",0.0);
  Control.addVariable("lokiBB0Radius",3680.0);

  // VACUUM For single chopper at 10m
  Control.addVariable("lokiVacBYStep",-8.0);  // GUESS
    
  Control.addVariable("lokiVacBVoidHeight",20.0);
  Control.addVariable("lokiVacBVoidDepth",19.0);
  Control.addVariable("lokiVacBVoidWidth",87.0);
  Control.addVariable("lokiVacBVoidLength",55.0);
  
  Control.addVariable("lokiVacBFeHeight",0.5);
  Control.addVariable("lokiVacBFeDepth",0.5);
  Control.addVariable("lokiVacBFeWidth",0.5);
  Control.addVariable("lokiVacBFeFront",0.5);
  Control.addVariable("lokiVacBFeBack",0.5);
  Control.addVariable("lokiVacBFlangeRadius",16.0);  // GUESS
  Control.addVariable("lokiVacBFlangeWall",1.0);  // GUESS
  Control.addVariable("lokiVacBFlangeLength",10.0);  // GUESS
  Control.addVariable("lokiVacBFeMat","Stainless304");

  
  // STRAIGHT SECTION TO Chopper 10m
  Control.addVariable("lokiGCXStep",0.0);       
  Control.addVariable("lokiGCYStep",0.0);       
  Control.addVariable("lokiGCZStep",0.0);       
  Control.addVariable("lokiGCXYAngle",0.0);       
  Control.addVariable("lokiGCZAngle",0.0);       
  Control.addVariable("lokiGCLength",30.0);        // GUESS
  
  Control.addVariable("lokiGCNShapes",1);       
  Control.addVariable("lokiGCNShapeLayers",3);
  Control.addVariable("lokiGCActiveShield",0);

  Control.addVariable("lokiGCLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGCLayerThick2",1.5);

  Control.addVariable("lokiGCLayerMat0","Void");
  Control.addVariable("lokiGCLayerMat1","Glass");
  Control.addVariable("lokiGCLayerMat2","Void");       

  Control.addVariable("lokiGC0TypeID","Rectangle");
  Control.addVariable("lokiGC0Height",3.0);
  Control.addVariable("lokiGC0Width",3.0);
  Control.addVariable("lokiGC0Length",30.0);   // GUESS
  Control.addVariable("lokiGC0ZAngle",0.0);


  // Single Blade chopper at 10m
  Control.addVariable("loki10mBladeXStep",0.0);
  Control.addVariable("loki10mBladeYStep",2.0);
  Control.addVariable("loki10mBladeZStep",0.0);
  Control.addVariable("loki10mBladeXYangle",0.0);
  Control.addVariable("loki10mBladeZangle",0.0);

  Control.addVariable("loki10mBladeInnerRadius",10.0);
  Control.addVariable("loki10mBladeOuterRadius",22.50);
  Control.addVariable("loki10mBladeNDisk",1);

  Control.addVariable("loki10mBlade0Thick",1.0);
  Control.addVariable("loki10mBladeInnerMat","Inconnel");
  Control.addVariable("loki10mBladeOuterMat","B4C");
  
  Control.addVariable("loki10mBladeNBlades",2);
  Control.addVariable("loki10mBlade0PhaseAngle0",95.0);
  Control.addVariable("loki10mBlade0OpenAngle0",30.0);

  Control.addVariable("loki10mBlade0PhaseAngle1",275.0);
  Control.addVariable("loki10mBlade0OpenAngle1",30.0);


  // STRAIGHT SECTION TO Chopper 10m
  Control.addVariable("lokiGDXStep",0.0);       
  Control.addVariable("lokiGDYStep",2.0);       
  Control.addVariable("lokiGDZStep",0.0);       
  Control.addVariable("lokiGDXYAngle",0.0);       
  Control.addVariable("lokiGDZAngle",0.0);       
  Control.addVariable("lokiGDLength",143.0);        // GUESS
  
  Control.addVariable("lokiGDNShapes",1);       
  Control.addVariable("lokiGDNShapeLayers",3);
  Control.addVariable("lokiGDActiveShield",0);

  Control.addVariable("lokiGDLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGDLayerThick2",1.5);

  Control.addVariable("lokiGDLayerMat0","Void");
  Control.addVariable("lokiGDLayerMat1","Glass");
  Control.addVariable("lokiGDLayerMat2","Void");       

  Control.addVariable("lokiGD0TypeID","Rectangle");
  Control.addVariable("lokiGD0Height",3.0);
  Control.addVariable("lokiGD0Width",3.0);
  Control.addVariable("lokiGD0Length",143.0);   // GUESS
  Control.addVariable("lokiGD0ZAngle",0.0);


  // VACUUM For doule chopper at 12m
  Control.addVariable("lokiVacCYStep",-28.0);  // GUESS
    
  Control.addVariable("lokiVacCVoidHeight",20.0);
  Control.addVariable("lokiVacCVoidDepth",19.0);
  Control.addVariable("lokiVacCVoidWidth",87.0);
  Control.addVariable("lokiVacCVoidLength",100.7);
  
  Control.addVariable("lokiVacCFeHeight",0.5);
  Control.addVariable("lokiVacCFeDepth",0.5);
  Control.addVariable("lokiVacCFeWidth",0.5);
  Control.addVariable("lokiVacCFeFront",0.5);
  Control.addVariable("lokiVacCFeBack",0.5);
  Control.addVariable("lokiVacCFlangeRadius",16.0);  // GUESS
  Control.addVariable("lokiVacCFlangeWall",1.0);  // GUESS
  Control.addVariable("lokiVacCFlangeLength",10.0);  // GUESS
  Control.addVariable("lokiVacCFeMat","Stainless304");

  // Double Blade chopper
  Control.addVariable("lokiDDiskXStep",0.0);
  Control.addVariable("lokiDDiskYStep",2.0);
  Control.addVariable("lokiDDiskZStep",0.0);
  Control.addVariable("lokiDDiskXYangle",0.0);
  Control.addVariable("lokiDDiskZangle",0.0);

  Control.addVariable("lokiDDiskGap",3.0);
  Control.addVariable("lokiDDiskInnerRadius",10.0);
  Control.addVariable("lokiDDiskOuterRadius",22.50);
  Control.addVariable("lokiDDiskNDisk",2);

  Control.addVariable("lokiDDisk0Thick",1.0);
  Control.addVariable("lokiDDisk1Thick",1.0);
  Control.addVariable("lokiDDiskInnerMat","Inconnel");
  Control.addVariable("lokiDDiskOuterMat","Aluminium");
  
  Control.addVariable("lokiDDiskNBlades",2);
  Control.addVariable("lokiDDisk0PhaseAngle0",95.0);
  Control.addVariable("lokiDDisk0OpenAngle0",30.0);
  Control.addVariable("lokiDDisk1PhaseAngle0",95.0);
  Control.addVariable("lokiDDisk1OpenAngle0",30.0);

  Control.addVariable("lokiDDisk0PhaseAngle1",275.0);
  Control.addVariable("lokiDDisk0OpenAngle1",30.0);
  Control.addVariable("lokiDDisk1PhaseAngle1",275.0);
  Control.addVariable("lokiDDisk1OpenAngle1",30.0);


  // STRAIGHT SECTION TO Chopper 10m
  Control.addVariable("lokiG12mIXStep",0.0);       
  Control.addVariable("lokiG12mIYStep",2.0);       
  Control.addVariable("lokiG12mIZStep",0.0);       
  Control.addVariable("lokiG12mIXYAngle",0.0);       
  Control.addVariable("lokiG12mIZAngle",0.0);       
  Control.addVariable("lokiG12mILength",43.0);        // GUESS
  
  Control.addVariable("lokiG12mINShapes",1);       
  Control.addVariable("lokiG12mINShapeLayers",3);
  Control.addVariable("lokiG12mIActiveShield",0);

  Control.addVariable("lokiG12mILayerThick1",0.4);  // glass thick
  Control.addVariable("lokiG12mILayerThick2",1.5);

  Control.addVariable("lokiG12mILayerMat0","Void");
  Control.addVariable("lokiG12mILayerMat1","Glass");
  Control.addVariable("lokiG12mILayerMat2","Void");       

  Control.addVariable("lokiG12mI0TypeID","Rectangle");
  Control.addVariable("lokiG12mI0Height",3.0);
  Control.addVariable("lokiG12mI0Width",3.0);
  Control.addVariable("lokiG12mI0Length",43.0);   // GUESS
  Control.addVariable("lokiG12mI0ZAngle",0.0);

    // Single 12m Blade chopper
  Control.addVariable("lokiSDiskXStep",0.0);
  Control.addVariable("lokiSDiskYStep",2.0);
  Control.addVariable("lokiSDiskZStep",0.0);
  Control.addVariable("lokiSDiskXYangle",0.0);
  Control.addVariable("lokiSDiskZangle",0.0);

  Control.addVariable("lokiSDiskInnerRadius",10.0);
  Control.addVariable("lokiSDiskOuterRadius",22.50);
  Control.addVariable("lokiSDiskNDisk",1);

  Control.addVariable("lokiSDisk0Thick",1.0);
  Control.addVariable("lokiSDiskInnerMat","Inconnel");
  Control.addVariable("lokiSDiskOuterMat","Aluminium");
  
  Control.addVariable("lokiSDiskNBlades",1);
  Control.addVariable("lokiSDisk0PhaseAngle0",95.0);
  Control.addVariable("lokiSDisk0OpenAngle0",30.0);

  Control.addVariable("lokiSDisk0PhaseAngle1",275.0);
  Control.addVariable("lokiSDisk0OpenAngle1",30.0);

  // STRAIGHT SECTION from chopper pit 3 to collimator
  Control.addVariable("lokiGEXStep",0.0);       
  Control.addVariable("lokiGEYStep",2.0);       
  Control.addVariable("lokiGEZStep",0.0);       
  Control.addVariable("lokiGEXYAngle",0.0);       
  Control.addVariable("lokiGEZAngle",0.0);       
  Control.addVariable("lokiGELength",48.0);        // GUESS
  
  Control.addVariable("lokiGENShapes",1);       
  Control.addVariable("lokiGENShapeLayers",3);
  Control.addVariable("lokiGEActiveShield",0);

  Control.addVariable("lokiGELayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGELayerThick2",1.5);

  Control.addVariable("lokiGELayerMat0","Void");
  Control.addVariable("lokiGELayerMat1","Glass");
  Control.addVariable("lokiGELayerMat2","Void");       

  Control.addVariable("lokiGE0TypeID","Rectangle");
  Control.addVariable("lokiGE0Height",3.0);
  Control.addVariable("lokiGE0Width",3.0);
  Control.addVariable("lokiGE0Length",48.0);   // GUESS
  Control.addVariable("lokiGE0ZAngle",0.0);


  // PIN COLLIMATOR SYSTEM
  Control.addVariable("odinPinLength",50.0);
  Control.addVariable("odinPinRadius",60.0);
  // Collimator A:
  Control.addVariable("lokiCollAYStep",1.0);
  Control.addVariable("lokiCollARotDepth",0.0);
  Control.addVariable("lokiCollARadius",30.0);
  Control.addVariable("lokiCollAThick",10.0);

  Control.addVariable("lokiCollAInnerWall",1.0);
  Control.addVariable("lokiCollAInnerWallMat","Void");

  Control.addVariable("lokiCollANLayers",0);
  Control.addVariable("lokiCollAHoleIndex",0);
  Control.addVariable("lokiCollAHoleAngOff",0.0);

  Control.addVariable("lokiCollADefMat","Stainless304");
  // collimator holes:
  Control.addVariable("lokiCollANHole",0);

  return;
}

}  // NAMESPACE setVariable
