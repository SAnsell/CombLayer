/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vor/DREAMvariables.cxx
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
DREAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("DREAMvariables[F]","DREAMvariables");

  // Bender in section so use cut system
  Control.addVariable("dreamFAXStep",0.0);       
  Control.addVariable("dreamFAYStep",0.0);       
  Control.addVariable("dreamFAZStep",0.0);       
  Control.addVariable("dreamFAXYAngle",0.0);
  Control.addVariable("dreamFAZAngle",0.0);
  Control.addVariable("dreamFABeamXYAngle",0.0);       

  Control.addVariable("dreamFALength",350.0);       
  Control.addVariable("dreamFANShapes",1);       
  Control.addVariable("dreamFANShapeLayers",3);
  Control.addVariable("dreamFAActiveShield",0);

  Control.addVariable("dreamFALayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFALayerThick2",1.5);

  Control.addVariable("dreamFALayerMat0","Void");
  Control.addVariable("dreamFALayerMat1","Glass");
  Control.addVariable("dreamFALayerMat2","Void");       

  Control.addVariable("dreamFA0TypeID","Tapper");
  Control.addVariable("dreamFA0HeightStart",3.5); // guess
  Control.addVariable("dreamFA0HeightEnd",4.5);
  Control.addVariable("dreamFA0WidthStart",0.7);
  Control.addVariable("dreamFA0WidthEnd",1.89);
  Control.addVariable("dreamFA0Length",350.0);

  // VACBOX A : A[2.08m from Bunker wall]
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("dreamVacAYStep",30.0);
    
  Control.addVariable("dreamVacAVoidHeight",20.0);
  Control.addVariable("dreamVacAVoidDepth",19.0);
  Control.addVariable("dreamVacAVoidWidth",87.0);
  Control.addVariable("dreamVacAVoidLength",25.7);
  
  Control.addVariable("dreamVacAFeHeight",0.5);
  Control.addVariable("dreamVacAFeDepth",0.5);
  Control.addVariable("dreamVacAFeWidth",0.5);
  Control.addVariable("dreamVacAFeFront",0.5);
  Control.addVariable("dreamVacAFeBack",0.5);
  Control.addVariable("dreamVacAFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacAFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacAFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacAFeMat","Stainless304");

  // VACUUM PIPES:
  Control.addVariable("dreamPipeARadius",8.0);
  Control.addVariable("dreamPipeALength",300.0);
  Control.addVariable("dreamPipeAFeThick",1.0);
  Control.addVariable("dreamPipeAFlangeRadius",12.0);
  Control.addVariable("dreamPipeAFlangeLength",1.0);
  Control.addVariable("dreamPipeAFeMat","Stainless304");

  // 5.5m to the first chopper:
  Control.addVariable("dreamFBXStep",0.0);       
  Control.addVariable("dreamFBYStep",0.2);       
  Control.addVariable("dreamFBZStep",0.0);       
  Control.addVariable("dreamFBXYAngle",0.0);       
  Control.addVariable("dreamFBZAngle",0.0);
  Control.addVariable("dreamFBLength",50.0);       
  
  Control.addVariable("dreamFBBeamYStep",1.0);
 
  Control.addVariable("dreamFBNShapes",1);       
  Control.addVariable("dreamFBNShapeLayers",3);
  Control.addVariable("dreamFBActiveShield",0);

  Control.addVariable("dreamFBLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFBLayerThick2",1.5);

  Control.addVariable("dreamFBLayerMat0","Void");
  Control.addVariable("dreamFBLayerMat1","Glass");
  Control.addVariable("dreamFBLayerMat2","Void");       
  
  Control.addVariable("dreamFB0TypeID","Tapper");
  Control.addVariable("dreamFB0HeightStart",4.5);
  Control.addVariable("dreamFB0HeightEnd",4.6);
  Control.addVariable("dreamFB0WidthStart",1.88);
  Control.addVariable("dreamFB0WidthEnd",2.06);
  Control.addVariable("dreamFB0Length",50.0);

  
  // Double Blade chopper
  Control.addVariable("dreamDBladeXStep",0.0);
  Control.addVariable("dreamDBladeYStep",1.0);
  Control.addVariable("dreamDBladeZStep",0.0);
  Control.addVariable("dreamDBladeXYangle",0.0);
  Control.addVariable("dreamDBladeZangle",0.0);

  Control.addVariable("dreamDBladeGap",1.0);
  Control.addVariable("dreamDBladeInnerRadius",30.0);
  Control.addVariable("dreamDBladeOuterRadius",37.5);
  Control.addVariable("dreamDBladeNDisk",2);

  Control.addVariable("dreamDBlade0Thick",0.2);
  Control.addVariable("dreamDBlade1Thick",0.2);
  Control.addVariable("dreamDBladeInnerMat","Inconnel");
  Control.addVariable("dreamDBladeOuterMat","B4C");
  
  Control.addVariable("dreamDBladeNBlades",2);
  Control.addVariable("dreamDBlade0PhaseAngle0",95.0);
  Control.addVariable("dreamDBlade0OpenAngle0",30.0);
  Control.addVariable("dreamDBlade1PhaseAngle0",95.0);
  Control.addVariable("dreamDBlade1OpenAngle0",30.0);

  Control.addVariable("dreamDBlade0PhaseAngle1",275.0);
  Control.addVariable("dreamDBlade0OpenAngle1",30.0);
  Control.addVariable("dreamDBlade1PhaseAngle1",275.0);
  Control.addVariable("dreamDBlade1OpenAngle1",30.0);

  // Single Blade chopper
  Control.addVariable("dreamSBladeXStep",0.0);
  Control.addVariable("dreamSBladeYStep",2.0);
  Control.addVariable("dreamSBladeZStep",0.0);
  Control.addVariable("dreamSBladeXYangle",0.0);
  Control.addVariable("dreamSBladeZangle",0.0);

  Control.addVariable("dreamSBladeInnerRadius",28.0);
  Control.addVariable("dreamSBladeOuterRadius",33.0);
  Control.addVariable("dreamSBladeNDisk",1);

  Control.addVariable("dreamSBlade0Thick",0.2);
  Control.addVariable("dreamSBladeInnerMat","Inconnel");
  Control.addVariable("dreamSBladeOuterMat","B4C");
  
  Control.addVariable("dreamSBladeNBlades",2);
  Control.addVariable("dreamSBlade0PhaseAngle0",95.0);
  Control.addVariable("dreamSBlade0OpenAngle0",30.0);
  Control.addVariable("dreamSBlade0PhaseAngle1",275.0);
  Control.addVariable("dreamSBlade0OpenAngle1",30.0);


  // Double Blade chopper
  Control.addVariable("dreamDBladeHouseYStep",-2.2);
  Control.addVariable("dreamDBladeHouseVoidHeight",38.0);
  Control.addVariable("dreamDBladeHouseVoidDepth",28.0);
  Control.addVariable("dreamDBladeHouseVoidThick",8.0);
  Control.addVariable("dreamDBladeHouseVoidWidth",80.0);
  Control.addVariable("dreamDBladeHouseWallThick",1.0);
  Control.addVariable("dreamDBladeHouseWallMat","Stainless304");

  // Single Blade chopper [Reversed]
  Control.addVariable("dreamSBladeHouseZAngle",180.0);
  Control.addVariable("dreamSBladeHouseVoidHeight",35.0);
  Control.addVariable("dreamSBladeHouseVoidDepth",22.0);
  Control.addVariable("dreamSBladeHouseVoidThick",4.0);
  Control.addVariable("dreamSBladeHouseVoidWidth",80.0);
  Control.addVariable("dreamSBladeHouseWallThick",1.0);
  Control.addVariable("dreamSBladeHouseWallMat","Stainless304");


  // T0 Chopper disk A
  Control.addVariable("dreamT0DiskAXStep",0.0);
  Control.addVariable("dreamT0DiskAYStep",2.0);
  Control.addVariable("dreamT0DiskAZStep",0.0);
  Control.addVariable("dreamT0DiskAXYangle",0.0);
  Control.addVariable("dreamT0DiskAZangle",0.0);

  Control.addVariable("dreamT0DiskAInnerRadius",28.0);
  Control.addVariable("dreamT0DiskAOuterRadius",38.0);
  Control.addVariable("dreamT0DiskANDisk",1);

  Control.addVariable("dreamT0DiskA0Thick",5.4);  // to include B4C
  Control.addVariable("dreamT0DiskAInnerMat","Inconnel");
  Control.addVariable("dreamT0DiskAOuterMat","Tungsten");
  
  Control.addVariable("dreamT0DiskANBlades",2);
  Control.addVariable("dreamT0DiskA0PhaseAngle0",95.0);
  Control.addVariable("dreamT0DiskA0OpenAngle0",35.0);
  Control.addVariable("dreamT0DiskA0PhaseAngle1",275.0);
  Control.addVariable("dreamT0DiskA0OpenAngle1",25.0);

  // T0 Chopper disk B
  Control.addVariable("dreamT0DiskBXStep",0.0);
  Control.addVariable("dreamT0DiskBYStep",47.0);
  Control.addVariable("dreamT0DiskBZStep",0.0);
  Control.addVariable("dreamT0DiskBXYangle",0.0);
  Control.addVariable("dreamT0DiskBZangle",0.0);

  Control.addVariable("dreamT0DiskBInnerRadius",28.0);
  Control.addVariable("dreamT0DiskBOuterRadius",38.0);
  Control.addVariable("dreamT0DiskBNDisk",1);

  Control.addVariable("dreamT0DiskB0Thick",5.4);  // to include B4C
  Control.addVariable("dreamT0DiskBInnerMat","Inconnel");
  Control.addVariable("dreamT0DiskBOuterMat","Tungsten");
  
  Control.addVariable("dreamT0DiskBNBlades",2);
  Control.addVariable("dreamT0DiskB0PhaseAngle0",95.0);
  Control.addVariable("dreamT0DiskB0OpenAngle0",35.0);
  Control.addVariable("dreamT0DiskB0PhaseAngle1",275.0);
  Control.addVariable("dreamT0DiskB0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("dreamT0DiskAHouseYStep",2.6);
  Control.addVariable("dreamT0DiskAHouseZStep",2.0);
  Control.addVariable("dreamT0DiskAHouseVoidHeight",38.0);
  Control.addVariable("dreamT0DiskAHouseVoidDepth",28.0);
  Control.addVariable("dreamT0DiskAHouseVoidThick",8.0);
  Control.addVariable("dreamT0DiskAHouseVoidWidth",80.0);
  Control.addVariable("dreamT0DiskAHouseWallThick",1.0);
  Control.addVariable("dreamT0DiskAHouseWallMat","Stainless304");

  Control.addVariable("dreamT0DiskBHouseYStep",2.6);
  Control.addVariable("dreamT0DiskBHouseZStep",2.0);
  Control.addVariable("dreamT0DiskBHouseVoidHeight",38.0);
  Control.addVariable("dreamT0DiskBHouseVoidDepth",28.0);
  Control.addVariable("dreamT0DiskBHouseVoidThick",8.0);
  Control.addVariable("dreamT0DiskBHouseVoidWidth",80.0);
  Control.addVariable("dreamT0DiskBHouseWallThick",1.0);
  Control.addVariable("dreamT0DiskBHouseWallMat","Stainless304");

  // VACUUM PIPES:
  Control.addVariable("dreamPipeBRadius",8.0);
  Control.addVariable("dreamPipeBLength",10.0);
  Control.addVariable("dreamPipeBFeThick",1.0);
  Control.addVariable("dreamPipeBFlangeRadius",12.0);
  Control.addVariable("dreamPipeBFlangeLength",1.0);
  Control.addVariable("dreamPipeBFeMat","Stainless304");

  // VACBOX B : for the second chopper T0 unit
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("dreamVacBYStep",27.0);
    
  Control.addVariable("dreamVacBVoidHeight",20.0);
  Control.addVariable("dreamVacBVoidDepth",19.0);
  Control.addVariable("dreamVacBVoidWidth",87.0);
  Control.addVariable("dreamVacBVoidLength",10.7);
  
  Control.addVariable("dreamVacBFeHeight",0.5);
  Control.addVariable("dreamVacBFeDepth",0.5);
  Control.addVariable("dreamVacBFeWidth",0.5);
  Control.addVariable("dreamVacBFeFront",0.5);
  Control.addVariable("dreamVacBFeBack",0.5);
  Control.addVariable("dreamVacBFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacBFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacBFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacBFeMat","Stainless304");

  // 5.5m to the first chopper:
  Control.addVariable("dreamFCXStep",0.0);       
  Control.addVariable("dreamFCYStep",0.2);       
  Control.addVariable("dreamFCZStep",0.0);       
  Control.addVariable("dreamFCXYAngle",0.0);       
  Control.addVariable("dreamFCZAngle",0.0);
  Control.addVariable("dreamFCLength",37.0);       
  
  Control.addVariable("dreamFCBeamYStep",1.0);
 
  Control.addVariable("dreamFCNShapes",1);       
  Control.addVariable("dreamFCNShapeLayers",3);
  Control.addVariable("dreamFCActiveShield",0);

  Control.addVariable("dreamFCLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFCLayerThick2",1.5);

  Control.addVariable("dreamFCLayerMat0","Void");
  Control.addVariable("dreamFCLayerMat1","Glass");
  Control.addVariable("dreamFCLayerMat2","Void");       
  
  Control.addVariable("dreamFC0TypeID","Tapper");
  Control.addVariable("dreamFC0HeightStart",4.5);
  Control.addVariable("dreamFC0HeightEnd",4.6);
  Control.addVariable("dreamFC0WidthStart",1.88);
  Control.addVariable("dreamFC0WidthEnd",2.06);
  Control.addVariable("dreamFC0Length",37.0);

  
  return;
}
 
}  // NAMESPACE setVariable
