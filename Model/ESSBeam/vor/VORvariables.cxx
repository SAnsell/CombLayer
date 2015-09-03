/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vor/VORvariables.cxx
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

  // SECOND SECTION [EXTRA]
  Control.addVariable("vorFBextraXStep",0.0);       
  Control.addVariable("vorFBextraYStep",0.0);       
  Control.addVariable("vorFBextraZStep",0.0);       
  Control.addVariable("vorFBextraXYAngle",0.0);       
  Control.addVariable("vorFBextraZAngle",0.0);
  Control.addVariable("vorFBextraLength",300.0);       
  Control.addVariable("vorFBextraHeight",110.0);       
  Control.addVariable("vorFBextraDepth",110.0);       
  Control.addVariable("vorFBextraLeftWidth",90.0);       
  Control.addVariable("vorFBextraRightWidth",90.0);       
  Control.addVariable("vorFBextraFeMat","Concrete");       

  Control.addVariable("vorFBextraNShapes",1);       
  Control.addVariable("vorFBextraNShapeLayers",4);
  Control.addVariable("vorFBextraActiveShield",1);

  Control.addVariable("vorFBextraLayerThick1",0.4);  // glass thick
  Control.addVariable("vorFBextraLayerThick2",1.5);
  Control.addVariable("vorFBextraLayerThick3",50.0);
  
  Control.addVariable("vorFBextraLayerMat0","Void");
  Control.addVariable("vorFBextraLayerMat1","Glass");
  Control.addVariable("vorFBextraLayerMat2","Void");
  Control.addVariable("vorFBextraLayerMat3","Stainless304");       

  // This values are pro-rated relative to the FocusB guide 
  Control.addVariable("vorFBextra0TypeID","Tapper");
  Control.addVariable("vorFBextra0HeightStart",6.1226);
  Control.addVariable("vorFBextra0HeightEnd",6.224);
  Control.addVariable("vorFBextra0WidthStart",4.0233);
  Control.addVariable("vorFBextra0WidthEnd",4.10);
  Control.addVariable("vorFBextra0Length",300.0);

   // PIT A : [1.08m from Bunker wall]
  // Centre position is GuideD Length + Radius [12m] + void Depth/2 + front
  Control.addVariable("vorPitAVoidHeight",167.0);
  Control.addVariable("vorPitAVoidDepth",36.0);
  Control.addVariable("vorPitAVoidWidth",246.0);
  Control.addVariable("vorPitAVoidLength",22.0);
  
  Control.addVariable("vorPitAFeHeight",70.0);
  Control.addVariable("vorPitAFeDepth",60.0);
  Control.addVariable("vorPitAFeWidth",60.0);
  Control.addVariable("vorPitAFeFront",45.0);
  Control.addVariable("vorPitAFeBack",70.0);
  Control.addVariable("vorPitAFeMat","Stainless304");

  
  Control.addVariable("vorPitAConcHeight",50.0);
  Control.addVariable("vorPitAConcDepth",50.0);
  Control.addVariable("vorPitAConcWidth",50.0);
  Control.addVariable("vorPitAConcFront",50.0);
  Control.addVariable("vorPitAConcBack",50.0);
  Control.addVariable("vorPitAConcMat","Concrete");

  Control.addVariable("vorPitAColletHeight",15.0);
  Control.addVariable("vorPitAColletDepth",15.0);
  Control.addVariable("vorPitAColletWidth",40.0);
  Control.addVariable("vorPitAColletLength",5.0);
  Control.addVariable("vorPitAColletMat","Tungsten");

  // Guide from Wall to PitA
  Control.addVariable("vorGPitAFrontLength",220.0);       
  Control.addVariable("vorGPitAFrontHeight",6.5);       
  Control.addVariable("vorGPitAFrontDepth",6.5);       
  Control.addVariable("vorGPitAFrontLeftWidth",7.0);       
  Control.addVariable("vorGPitAFrontRightWidth",7.0);       
  Control.addVariable("vorGPitAFrontFeMat","Void");       
  Control.addVariable("vorGPitAFrontNShapes",1);       
  Control.addVariable("vorGPitAFrontNShapeLayers",3);

  Control.addVariable("vorGPitAFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("vorGPitAFrontLayerThick2",1.5);

  Control.addVariable("vorGPitAFrontLayerMat0","Void");
  Control.addVariable("vorGPitAFrontLayerMat1","Glass");
  Control.addVariable("vorGPitAFrontLayerMat2","Void");       

  Control.addVariable("vorGPitAFront0TypeID","Rectangle");
  Control.addVariable("vorGPitAFront0Height",6.224);
  Control.addVariable("vorGPitAFront0Width",4.10);
  Control.addVariable("vorGPitAFront0Length",220.0);
  Control.addVariable("vorGPitAFront0ZAngle",0.0);

  // GUIDE IN the beam insert [sample side]
  Control.addVariable("vorGPitABackXStep",0.0);       
  Control.addVariable("vorGPitABackYStep",0.0);       
  Control.addVariable("vorGPitABackZStep",0.0);       
  Control.addVariable("vorGPitABackXYAngle",0.0);       
  Control.addVariable("vorGPitABackZAngle",0.0);       
  Control.addVariable("vorGPitABackLength",220.0);       
  Control.addVariable("vorGPitABackHeight",6.0);       
  Control.addVariable("vorGPitABackDepth",6.0);       
  Control.addVariable("vorGPitABackLeftWidth",7.0);       
  Control.addVariable("vorGPitABackRightWidth",7.0);       
  Control.addVariable("vorGPitABackFeMat","Void");       
  Control.addVariable("vorGPitABackNShapes",1);       
  Control.addVariable("vorGPitABackNShapeLayers",3);

  Control.addVariable("vorGPitABackLayerThick1",0.4);  // glass thick
  Control.addVariable("vorGPitABackLayerThick2",1.5);

  Control.addVariable("vorGPitABackLayerMat0","Void");
  Control.addVariable("vorGPitABackLayerMat1","Glass");
  Control.addVariable("vorGPitABackLayerMat2","Void");       

  Control.addVariable("vorGPitABack0TypeID","Rectangle");
  Control.addVariable("vorGPitABack0Height",6.2158);
  Control.addVariable("vorGPitABack0Width",4.0980);
  Control.addVariable("vorGPitABack0Length",220.0);
  Control.addVariable("vorGPitABack0ZAngle",0.0);

  // Chopper A : Single 1.2m disk chopper [6 phases]
  Control.addVariable("vorChopperAXStep",0.0);
  Control.addVariable("vorChopperAYStep",0.0);
  Control.addVariable("vorChopperAZStep",0.0);
  Control.addVariable("vorChopperAXYangle",0.0);
  Control.addVariable("vorChopperAZangle",0.0);

  Control.addVariable("vorChopperAGap",3.0);
  Control.addVariable("vorChopperAInnerRadius",45.0);
  Control.addVariable("vorChopperAOuterRadius",63.0);
  Control.addVariable("vorChopperANDisk",1);

  Control.addVariable("vorChopperA0Thick",2.0);
  Control.addVariable("vorChopperAInnerMat","Aluminium");
  Control.addVariable("vorChopperAOuterMat","Inconnel");

  Control.addVariable("vorChopperANBlades",2);
  Control.addVariable("vorChopperA0PhaseAngle0",-20.0);
  Control.addVariable("vorChopperA0OpenAngle0",160.0);

  Control.addVariable("vorChopperA0PhaseAngle1",160.0);
  Control.addVariable("vorChopperA0OpenAngle1",160.0);

  // SECTION THREE:

  Control.addVariable("vorFCXStep",0.0);       
  Control.addVariable("vorFCYStep",0.0);       
  Control.addVariable("vorFCZStep",0.0);       
  Control.addVariable("vorFCXYAngle",0.0);       
  Control.addVariable("vorFCZAngle",0.0);
  Control.addVariable("vorFCLength",739.0);
  Control.addVariable("vorFCHeight",110.0);
  Control.addVariable("vorFCDepth",110.0);       
  Control.addVariable("vorFCLeftWidth",65.0);       
  Control.addVariable("vorFCRightWidth",65.0);       
  Control.addVariable("vorFCFeMat","Concrete");       

  Control.addVariable("vorFCBeamYStep",0.0);
 
  Control.addVariable("vorFCNShapes",1);       
  Control.addVariable("vorFCNShapeLayers",4);
  Control.addVariable("vorFCActiveShield",1);

  Control.addVariable("vorFCLayerThick1",0.4);  // glass thick
  Control.addVariable("vorFCLayerThick2",1.5);
  Control.addVariable("vorFCLayerThick3",25.0);
  
  Control.addVariable("vorFCLayerMat0","Void");
  Control.addVariable("vorFCLayerMat1","Glass");
  Control.addVariable("vorFCLayerMat2","Void");       
  Control.addVariable("vorFCLayerMat3","Stainless304");
  
  Control.addVariable("vorFC0TypeID","Tapper");
  Control.addVariable("vorFC0HeightStart",6.216);
  Control.addVariable("vorFC0HeightEnd",4.2255);
  Control.addVariable("vorFC0WidthStart",4.098);
  Control.addVariable("vorFC0WidthEnd",2.9014);
  Control.addVariable("vorFC0Length",739.0);   // 7.89m -50cm of pitA

  
  // PIT B : 
  // Centre position is
  Control.addVariable("vorPitBVoidHeight",167.0);
  Control.addVariable("vorPitBVoidDepth",36.0);
  Control.addVariable("vorPitBVoidWidth",246.0);
  Control.addVariable("vorPitBVoidLength",22.0);
  
  Control.addVariable("vorPitBFeHeight",40.0);
  Control.addVariable("vorPitBFeDepth",30.0);
  Control.addVariable("vorPitBFeWidth",30.0);
  Control.addVariable("vorPitBFeFront",20.0);
  Control.addVariable("vorPitBFeBack",30.0);
  Control.addVariable("vorPitBFeMat","Stainless304");

  
  Control.addVariable("vorPitBConcHeight",50.0);
  Control.addVariable("vorPitBConcDepth",50.0);
  Control.addVariable("vorPitBConcWidth",50.0);
  Control.addVariable("vorPitBConcFront",50.0);
  Control.addVariable("vorPitBConcBack",50.0);
  Control.addVariable("vorPitBConcMat","Concrete");

  Control.addVariable("vorPitBColletHeight",15.0);
  Control.addVariable("vorPitBColletDepth",15.0);
  Control.addVariable("vorPitBColletWidth",40.0);
  Control.addVariable("vorPitBColletLength",5.0);
  Control.addVariable("vorPitBColletMat","Tungsten");

  // Guide from Wall to PitB
  Control.addVariable("vorGPitBFrontLength",220.0);       
  Control.addVariable("vorGPitBFrontHeight",6.5);       
  Control.addVariable("vorGPitBFrontDepth",6.5);       
  Control.addVariable("vorGPitBFrontLeftWidth",7.0);       
  Control.addVariable("vorGPitBFrontRightWidth",7.0);       
  Control.addVariable("vorGPitBFrontFeMat","Void");       
  Control.addVariable("vorGPitBFrontNShapes",1);       
  Control.addVariable("vorGPitBFrontNShapeLayers",3);

  Control.addVariable("vorGPitBFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("vorGPitBFrontLayerThick2",1.5);

  Control.addVariable("vorGPitBFrontLayerMat0","Void");
  Control.addVariable("vorGPitBFrontLayerMat1","Glass");
  Control.addVariable("vorGPitBFrontLayerMat2","Void");       

  Control.addVariable("vorGPitBFront0TypeID","Rectangle");
  Control.addVariable("vorGPitBFront0Height",4.2255);
  Control.addVariable("vorGPitBFront0Width",2.9013);
  Control.addVariable("vorGPitBFront0Length",220.0);
  Control.addVariable("vorGPitBFront0ZAngle",0.0);

    // Chopper A : Single 1.2m disk chopper [6 phases]
  Control.addVariable("vorChopperBXStep",0.0);
  Control.addVariable("vorChopperBYStep",0.0);
  Control.addVariable("vorChopperBZStep",0.0);
  Control.addVariable("vorChopperBXYangle",0.0);
  Control.addVariable("vorChopperBZangle",0.0);

  Control.addVariable("vorChopperBGap",3.0);
  Control.addVariable("vorChopperBInnerRadius",45.0);
  Control.addVariable("vorChopperBOuterRadius",63.0);
  Control.addVariable("vorChopperBNDisk",1);

  Control.addVariable("vorChopperB0Thick",2.0);
  Control.addVariable("vorChopperBInnerMat","Aluminium");
  Control.addVariable("vorChopperBOuterMat","Inconnel");

  Control.addVariable("vorChopperBNBlades",2);
  Control.addVariable("vorChopperB0PhaseAngle0",-20.0);
  Control.addVariable("vorChopperB0OpenAngle0",160.0);

  Control.addVariable("vorChopperB0PhaseAngle1",160.0);
  Control.addVariable("vorChopperB0OpenAngle1",160.0);

  Control.addVariable("vorFDXStep",0.0);       
  Control.addVariable("vorFDYStep",0.0);       
  Control.addVariable("vorFDZStep",0.0);       
  Control.addVariable("vorFDXYAngle",0.0);       
  Control.addVariable("vorFDZAngle",0.0);
  Control.addVariable("vorFDLength",101.0);  // 1.51m - 50cm
  Control.addVariable("vorFDHeight",110.0);
  Control.addVariable("vorFDDepth",110.0);       
  Control.addVariable("vorFDLeftWidth",60.0);       
  Control.addVariable("vorFDRightWidth",60.0);       
  Control.addVariable("vorFDFeMat","Concrete");       

  Control.addVariable("vorFDBeamYStep",0.0);
 
  Control.addVariable("vorFDNShapes",1);       
  Control.addVariable("vorFDNShapeLayers",4);
  Control.addVariable("vorFDActiveShield",1);

  Control.addVariable("vorFDLayerThick1",0.4);  // glass thick
  Control.addVariable("vorFDLayerThick2",1.5);
  Control.addVariable("vorFDLayerThick3",30.0);
  
  Control.addVariable("vorFDLayerMat0","Void");
  Control.addVariable("vorFDLayerMat1","Glass");
  Control.addVariable("vorFDLayerMat2","Void");       
  Control.addVariable("vorFDLayerMat3","Stainless304");
  
  Control.addVariable("vorFD0TypeID","Tapper");
  Control.addVariable("vorFD0WidthStart",2.9013);
  Control.addVariable("vorFD0WidthEnd",2.4061);
  Control.addVariable("vorFD0HeightStart",4.22);
  Control.addVariable("vorFD0HeightEnd",3.379);
  Control.addVariable("vorFD0Length",101.0);   // 1.5m -50cm of pitA

  // GUIDE IN the beam insert [sample side]
  Control.addVariable("vorGPitBBackXStep",0.0);       
  Control.addVariable("vorGPitBBackYStep",0.0);       
  Control.addVariable("vorGPitBBackZStep",0.0);       
  Control.addVariable("vorGPitBBackXYAngle",0.0);       
  Control.addVariable("vorGPitBBackZAngle",0.0);       
  Control.addVariable("vorGPitBBackLength",220.0);       
  Control.addVariable("vorGPitBBackHeight",6.0);       
  Control.addVariable("vorGPitBBackDepth",6.0);       
  Control.addVariable("vorGPitBBackLeftWidth",7.0);       
  Control.addVariable("vorGPitBBackRightWidth",7.0);       
  Control.addVariable("vorGPitBBackFeMat","Void");       
  Control.addVariable("vorGPitBBackNShapes",1);       
  Control.addVariable("vorGPitBBackNShapeLayers",3);

  Control.addVariable("vorGPitBBackLayerThick1",0.4);  // glass thick
  Control.addVariable("vorGPitBBackLayerThick2",1.5);

  Control.addVariable("vorGPitBBackLayerMat0","Void");
  Control.addVariable("vorGPitBBackLayerMat1","Glass");
  Control.addVariable("vorGPitBBackLayerMat2","Void");       

  Control.addVariable("vorGPitBBack0TypeID","Rectangle");
  Control.addVariable("vorGPitBBack0Height",6.2158);
  Control.addVariable("vorGPitBBack0Width",4.0980);
  Control.addVariable("vorGPitBBack0Length",220.0);
  Control.addVariable("vorGPitBBack0ZAngle",0.0);


  Control.addVariable("vorFEXStep",0.0);       
  Control.addVariable("vorFEYStep",0.0);       
  Control.addVariable("vorFEZStep",0.0);       
  Control.addVariable("vorFEXYAngle",0.0);       
  Control.addVariable("vorFEZAngle",0.0);
  Control.addVariable("vorFELength",101.0);  // 1.51m - 50cm
  Control.addVariable("vorFEHeight",110.0);
  Control.addVariable("vorFEDepth",110.0);       
  Control.addVariable("vorFELeftWidth",60.0);       
  Control.addVariable("vorFERightWidth",60.0);       
  Control.addVariable("vorFEFeMat","Concrete");       

  Control.addVariable("vorFEBeamYStep",0.0);
 
  Control.addVariable("vorFENShapes",1);       
  Control.addVariable("vorFENShapeLayers",4);
  Control.addVariable("vorFEActiveShield",1);

  Control.addVariable("vorFELayerThick1",0.4);  // glass thick
  Control.addVariable("vorFELayerThick2",1.5);
  Control.addVariable("vorFELayerThick3",30.0);
  
  Control.addVariable("vorFELayerMat0","Void");
  Control.addVariable("vorFELayerMat1","Glass");
  Control.addVariable("vorFELayerMat2","Void");       
  Control.addVariable("vorFELayerMat3","Stainless304");
  
  Control.addVariable("vorFE0TypeID","Tapper");
  Control.addVariable("vorFE0WidthStart",2.3672);
  Control.addVariable("vorFE0WidthEnd",1.6829);
  Control.addVariable("vorFE0HeightStart",3.311);
  Control.addVariable("vorFE0HeightEnd",2.0493);
  Control.addVariable("vorFE0Length",101.0);   

    // HUT:
  Control.addVariable("vorCaveXStep",80.0);
  Control.addVariable("vorCaveYStep",0.0);
  Control.addVariable("vorCaveVoidHeight",200.0);
  Control.addVariable("vorCaveVoidDepth",183.0);
  Control.addVariable("vorCaveVoidWidth",640.0);
  Control.addVariable("vorCaveVoidLength",600.0);
  Control.addVariable("vorCaveVoidFrontCut",160.0);
  Control.addVariable("vorCaveVoidBackCut",90.0);
  Control.addVariable("vorCaveVoidFrontStep",60.0);
  Control.addVariable("vorCaveVoidBackStep",60.0);

  Control.addVariable("vorCaveFeThick",25.0);
  Control.addVariable("vorCaveConcThick",35.0);

  Control.addVariable("vorCaveFeMat","Stainless304");
  Control.addVariable("vorCaveConcMat","Concrete");


  Control.addVariable("vorFFXStep",0.0);       
  Control.addVariable("vorFFYStep",0.0);       
  Control.addVariable("vorFFZStep",0.0);       
  Control.addVariable("vorFFXYAngle",0.0);       
  Control.addVariable("vorFFZAngle",0.0);
  Control.addVariable("vorFFLength",251.0);  // 1.51m - 50cm
  Control.addVariable("vorFFBeamYStep",0.0);
 
  Control.addVariable("vorFFNShapes",1);       
  Control.addVariable("vorFFNShapeLayers",3);
  Control.addVariable("vorFFActiveShield",0);

  Control.addVariable("vorFFLayerThick1",0.4);  // glass thick
  Control.addVariable("vorFFLayerThick2",1.5);
  
  Control.addVariable("vorFFLayerMat0","Void");
  Control.addVariable("vorFFLayerMat1","Glass");
  Control.addVariable("vorFFLayerMat2","Void");       
  
  Control.addVariable("vorFF0TypeID","Rectangle");
  Control.addVariable("vorFF0Width",1.6829);
  Control.addVariable("vorFF0Height",2.0493);
  Control.addVariable("vorFF0Length",251.0);

  // DetectorTank
  Control.addVariable("vorTankXStep",0.0);
  Control.addVariable("vorTankYStep",35.0);
  Control.addVariable("vorTankZStep",0.0);
  Control.addVariable("vorTankXYAngle",0.0);
  Control.addVariable("vorTankZAngle",0.0);

  Control.addVariable("vorTankNLayers",2.0);
  Control.addVariable("vorTankInnerRadius",10.0);
  Control.addVariable("vorTankOuterRadius",300.0);
  Control.addVariable("vorTankHeight",100.0);
  Control.addVariable("vorTankMidAngle",-40.0);
  Control.addVariable("vorTankFrontThick",2.0);
  Control.addVariable("vorTankInnerThick",2.0);
  Control.addVariable("vorTankRoofThick",1.0);
  Control.addVariable("vorTankBackThick",3.0);
  Control.addVariable("vorTankWallMat","Stainless304");

  
  // SAMPLE
  Control.addVariable("vorSampleXStep",0.0);
  Control.addVariable("vorSampleYStep",0.0);
  Control.addVariable("vorSampleZStep",0.0);
  Control.addVariable("vorSampleXYangle",0.0);
  Control.addVariable("vorSampleZangle",0.0);
  Control.addVariable("vorSampleNLayers",2.0);
  Control.addVariable("vorSampleRadius1",0.5);
  Control.addVariable("vorSampleRadius2",0.6);
  Control.addVariable("vorSampleHeight1",2.0);
  Control.addVariable("vorSampleHeight2",2.1);
  Control.addVariable("vorSampleMaterial1","H2O");
  Control.addVariable("vorSampleMaterial2","Aluminium");

  return;
}
 
}  // NAMESPACE setVariable
