/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VORvariables.cxx
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
VORvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VORvariables[F]","VORvariables");

  // Bender in section so use cut system
  Control.addVariable("G1BLine7Filled",1);
  
  Control.addVariable("vorFAXStep",0.0);       
  Control.addVariable("vorFAYStep",0.0);       
  Control.addVariable("vorFAZStep",0.0);       
  Control.addVariable("vorFAXYAngle",0.0);
  Control.addVariable("vorFAZAngle",0.0);
  Control.addVariable("vorFABeamXYAngle",0.0);       

  Control.addVariable("vorFALength",745.0);       
  Control.addVariable("vorFANShapes",1);       
  Control.addVariable("vorFANShapeLayers",3);
  Control.addVariable("vorFAActiveShield",0);

  Control.addVariable("vorFALayerThick1",0.4);  // glass thick
  Control.addVariable("vorFALayerThick2",1.5);

  Control.addVariable("vorFALayerMat0","Void");
  Control.addVariable("vorFALayerMat1","Glass");
  Control.addVariable("vorFALayerMat2","Void");       

  Control.addVariable("vorFA0TypeID","Tapper");
  Control.addVariable("vorFA0HeightStart",3.16);
  Control.addVariable("vorFA0HeightEnd",5.905);
  Control.addVariable("vorFA0WidthStart",2.114);
  Control.addVariable("vorFA0WidthEnd",3.87);
  Control.addVariable("vorFA0Length",745.0);

  // VACBOX A : A[2.08m from Bunker wall]
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("vorVacAYStep",-15.0);
    
  Control.addVariable("vorVacAVoidHeight",20.0);
  Control.addVariable("vorVacAVoidDepth",19.0);
  Control.addVariable("vorVacAVoidWidth",87.0);
  Control.addVariable("vorVacAVoidLength",25.7);
  
  Control.addVariable("vorVacAFeHeight",0.5);
  Control.addVariable("vorVacAFeDepth",0.5);
  Control.addVariable("vorVacAFeWidth",0.5);
  Control.addVariable("vorVacAFeFront",0.5);
  Control.addVariable("vorVacAFeBack",0.5);
  Control.addVariable("vorVacAFlangeRadius",8.0);    // GUESS
  Control.addVariable("vorVacAFlangeWall",1.0);      // GUESS
  Control.addVariable("vorVacAFlangeLength",10.0);   // GUESS
  Control.addVariable("vorVacAFeMat","Stainless304");

  // Double Blade chopper
  Control.addVariable("vorDBladeXStep",0.0);
  Control.addVariable("vorDBladeYStep",1.0);
  Control.addVariable("vorDBladeZStep",0.0);
  Control.addVariable("vorDBladeXYangle",0.0);
  Control.addVariable("vorDBladeZangle",0.0);

  Control.addVariable("vorDBladeGap",1.0);
  Control.addVariable("vorDBladeInnerRadius",10.0);
  Control.addVariable("vorDBladeOuterRadius",22.50);
  Control.addVariable("vorDBladeNDisk",2);

  Control.addVariable("vorDBlade0Thick",1.0);
  Control.addVariable("vorDBlade1Thick",1.0);
  Control.addVariable("vorDBladeInnerMat","Inconnel");
  Control.addVariable("vorDBladeOuterMat","Aluminium");
  
  Control.addVariable("vorDBladeNBlades",2);
  Control.addVariable("vorDBlade0PhaseAngle0",95.0);
  Control.addVariable("vorDBlade0OpenAngle0",30.0);
  Control.addVariable("vorDBlade1PhaseAngle0",95.0);
  Control.addVariable("vorDBlade1OpenAngle0",30.0);

  Control.addVariable("vorDBlade0PhaseAngle1",275.0);
  Control.addVariable("vorDBlade0OpenAngle1",30.0);
  Control.addVariable("vorDBlade1PhaseAngle1",275.0);
  Control.addVariable("vorDBlade1OpenAngle1",30.0);

  // Double Blade chopper
  Control.addVariable("vorDBladeHouseVoidHeight",40.0);
  Control.addVariable("vorDBladeHouseVoidDepth",12.0);
  Control.addVariable("vorDBladeHouseVoidThick",8.0);
  Control.addVariable("vorDBladeHouseVoidWidth",60.0);
  Control.addVariable("vorDBladeHouseWallThick",1.0);
  Control.addVariable("vorDBladeHouseWallMat","Stainless304");

    // VACUUM PIPES:
  Control.addVariable("vorPipeBRadius",8.0);
  Control.addVariable("vorPipeBLength",300.0);
  Control.addVariable("vorPipeBFeThick",1.0);
  Control.addVariable("vorPipeBFlangeRadius",12.0);
  Control.addVariable("vorPipeBFlangeLength",1.0);
  Control.addVariable("vorPipeBFeMat","Stainless304");

  // SECOND SECTION
  Control.addVariable("vorFBXStep",0.0);       
  Control.addVariable("vorFBYStep",1.0);       
  Control.addVariable("vorFBZStep",0.0);       
  Control.addVariable("vorFBXYAngle",0.0);       
  Control.addVariable("vorFBZAngle",0.0);
  Control.addVariable("vorFBLength",940.0);       
  
  Control.addVariable("vorFBBeamYStep",1.0);
 
  Control.addVariable("vorFBNShapes",1);       
  Control.addVariable("vorFBNShapeLayers",3);
  Control.addVariable("vorFBActiveShield",0);

  Control.addVariable("vorFBLayerThick1",0.4);  // glass thick
  Control.addVariable("vorFBLayerThick2",1.5);

  Control.addVariable("vorFBLayerMat0","Void");
  Control.addVariable("vorFBLayerMat1","Glass");
  Control.addVariable("vorFBLayerMat2","Void");       
  
  Control.addVariable("vorFB0TypeID","Tapper");
  Control.addVariable("vorFB0HeightStart",5.92);
  Control.addVariable("vorFB0HeightEnd",6.224);
  Control.addVariable("vorFB0WidthStart",3.87);
  Control.addVariable("vorFB0WidthEnd",4.10);
  Control.addVariable("vorFB0Length",940.0);

  // BEAM INSERT:
  Control.addVariable("vorBInsertHeight",20.0);
  Control.addVariable("vorBInsertWidth",28.0);
  Control.addVariable("vorBInsertTopWall",1.0);
  Control.addVariable("vorBInsertLowWall",1.0);
  Control.addVariable("vorBInsertLeftWall",1.0);
  Control.addVariable("vorBInsertRightWall",1.0);
  Control.addVariable("vorBInsertWallMat","Stainless304");       
    
  return;
}
 
}  // NAMESPACE setVariable
