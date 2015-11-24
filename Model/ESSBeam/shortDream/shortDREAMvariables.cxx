/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/dream/DREAMvariables.cxx
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
shortDREAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("SHORTDREAMvariables[F]","SHORTDREAMvariables");

  // Bender in section so use cut system
  Control.addVariable("shortDreamFAXStep",0.0);        // Centre of thermal [-2]
  Control.addVariable("shortDreamFAYStep",0.0);       
  Control.addVariable("shortDreamFAZStep",0.0);       
  Control.addVariable("shortDreamFAXYAngle",0.0);
  Control.addVariable("shortDreamFAZAngle",0.0);
  Control.addVariable("shortDreamFABeamXYAngle",0.0);       

  Control.addVariable("shortDreamFALength",350.0);       
  Control.addVariable("shortDreamFANShapes",1);       
  Control.addVariable("shortDreamFANShapeLayers",3);
  Control.addVariable("shortDreamFAActiveShield",0);

  Control.addVariable("shortDreamFALayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFALayerThick2",1.5);

  Control.addVariable("shortDreamFALayerMat0","Void");
  Control.addVariable("shortDreamFALayerMat1","Aluminium");
  Control.addVariable("shortDreamFALayerMat2","Void");       

  Control.addVariable("shortDreamFA0TypeID","Tapper");
  Control.addVariable("shortDreamFA0HeightStart",3.5); // guess
  Control.addVariable("shortDreamFA0HeightEnd",4.5);
  Control.addVariable("shortDreamFA0WidthStart",8.0); // NOT Centred
  Control.addVariable("shortDreamFA0WidthEnd",2.0);
  Control.addVariable("shortDreamFA0Length",350.0);

  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("shortDreamVacAYStep",40.0);
    
  Control.addVariable("shortDreamVacAVoidHeight",20.0);
  Control.addVariable("shortDreamVacAVoidDepth",19.0);
  Control.addVariable("shortDreamVacAVoidWidth",87.0);
  Control.addVariable("shortDreamVacAVoidLength",25.7);
  
  Control.addVariable("shortDreamVacAFeHeight",0.5);
  Control.addVariable("shortDreamVacAFeDepth",0.5);
  Control.addVariable("shortDreamVacAFeWidth",0.5);
  Control.addVariable("shortDreamVacAFeFront",0.5);
  Control.addVariable("shortDreamVacAFeBack",0.5);
  Control.addVariable("shortDreamVacAFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacAFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacAFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacAFeMat","Stainless304");
  Control.addVariable("shortDreamVacAVoidMat","Void");

  // VACUUM PIPES:
  Control.addVariable("shortDreamPipeARadius",8.0);
  Control.addVariable("shortDreamPipeALength",300.0);
  Control.addVariable("shortDreamPipeAFeThick",1.0);
  Control.addVariable("shortDreamPipeAFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeAFlangeLength",1.0);
  Control.addVariable("shortDreamPipeAFeMat","Stainless304");

  // 5.5m to the first chopper:
  Control.addVariable("shortDreamFBXStep",0.0);       
  Control.addVariable("shortDreamFBYStep",0.2);       
  Control.addVariable("shortDreamFBZStep",0.0);       
  Control.addVariable("shortDreamFBXYAngle",0.0);       
  Control.addVariable("shortDreamFBZAngle",0.0);
  Control.addVariable("shortDreamFBLength",50.0);       
  
  Control.addVariable("shortDreamFBBeamYStep",1.0);
 
  Control.addVariable("shortDreamFBNShapes",1);       
  Control.addVariable("shortDreamFBNShapeLayers",3);
  Control.addVariable("shortDreamFBActiveShield",0);

  Control.addVariable("shortDreamFBLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFBLayerThick2",1.5);

  Control.addVariable("shortDreamFBLayerMat0","Void");
  Control.addVariable("shortDreamFBLayerMat1","Aluminium");
  Control.addVariable("shortDreamFBLayerMat2","Void");       
  
  Control.addVariable("shortDreamFB0TypeID","Tapper");
  Control.addVariable("shortDreamFB0HeightStart",4.5);
  Control.addVariable("shortDreamFB0HeightEnd",4.6);
  Control.addVariable("shortDreamFB0WidthStart",1.88);
  Control.addVariable("shortDreamFB0WidthEnd",2.06);
  Control.addVariable("shortDreamFB0Length",50.0);

  
  // Double Blade chopper
  Control.addVariable("shortDreamDBladeXStep",0.0);
  Control.addVariable("shortDreamDBladeYStep",11.0);
  Control.addVariable("shortDreamDBladeZStep",0.0);
  Control.addVariable("shortDreamDBladeXYangle",0.0);
  Control.addVariable("shortDreamDBladeZangle",0.0);

  Control.addVariable("shortDreamDBladeGap",1.0);
  Control.addVariable("shortDreamDBladeInnerRadius",30.0);
  Control.addVariable("shortDreamDBladeOuterRadius",37.5);
  Control.addVariable("shortDreamDBladeNDisk",2);

  Control.addVariable("shortDreamDBlade0Thick",0.2);
  Control.addVariable("shortDreamDBlade1Thick",0.2);
  Control.addVariable("shortDreamDBladeInnerMat","Inconnel");
  Control.addVariable("shortDreamDBladeOuterMat","B4C");
  
  Control.addVariable("shortDreamDBladeNBlades",2);
  Control.addVariable("shortDreamDBlade0PhaseAngle0",95.0);
  Control.addVariable("shortDreamDBlade0OpenAngle0",30.0);
  Control.addVariable("shortDreamDBlade1PhaseAngle0",95.0);
  Control.addVariable("shortDreamDBlade1OpenAngle0",30.0);

  Control.addVariable("shortDreamDBlade0PhaseAngle1",275.0);
  Control.addVariable("shortDreamDBlade0OpenAngle1",30.0);
  Control.addVariable("shortDreamDBlade1PhaseAngle1",275.0);
  Control.addVariable("shortDreamDBlade1OpenAngle1",30.0);

  // Single Blade chopper
  Control.addVariable("shortDreamSBladeXStep",0.0);
  Control.addVariable("shortDreamSBladeYStep",2.0);
  Control.addVariable("shortDreamSBladeZStep",0.0);
  Control.addVariable("shortDreamSBladeXYangle",0.0);
  Control.addVariable("shortDreamSBladeZangle",0.0);

  Control.addVariable("shortDreamSBladeInnerRadius",28.0);
  Control.addVariable("shortDreamSBladeOuterRadius",33.0);
  Control.addVariable("shortDreamSBladeNDisk",1);

  Control.addVariable("shortDreamSBlade0Thick",0.2);
  Control.addVariable("shortDreamSBladeInnerMat","Inconnel");
  Control.addVariable("shortDreamSBladeOuterMat","B4C");
  
  Control.addVariable("shortDreamSBladeNBlades",2);
  Control.addVariable("shortDreamSBlade0PhaseAngle0",95.0);
  Control.addVariable("shortDreamSBlade0OpenAngle0",30.0);
  Control.addVariable("shortDreamSBlade0PhaseAngle1",275.0);
  Control.addVariable("shortDreamSBlade0OpenAngle1",30.0);


  // Double Blade chopper
  Control.addVariable("shortDreamDBladeHouseYStep",-2.2);
  Control.addVariable("shortDreamDBladeHouseVoidHeight",38.0);
  Control.addVariable("shortDreamDBladeHouseVoidDepth",28.0);
  Control.addVariable("shortDreamDBladeHouseVoidThick",8.0);
  Control.addVariable("shortDreamDBladeHouseVoidWidth",80.0);
  Control.addVariable("shortDreamDBladeHouseWallThick",1.0);
  Control.addVariable("shortDreamDBladeHouseWallMat","Stainless304");

  // Single Blade chopper [Reversed]
  Control.addVariable("shortDreamSBladeHouseZAngle",180.0);
  Control.addVariable("shortDreamSBladeHouseVoidHeight",35.0);
  Control.addVariable("shortDreamSBladeHouseVoidDepth",22.0);
  Control.addVariable("shortDreamSBladeHouseVoidThick",4.0);
  Control.addVariable("shortDreamSBladeHouseVoidWidth",80.0);
  Control.addVariable("shortDreamSBladeHouseWallThick",1.0);
  Control.addVariable("shortDreamSBladeHouseWallMat","Stainless304");


  // T0 Chopper disk A
  Control.addVariable("shortDreamT0DiskAXStep",0.0);
  Control.addVariable("shortDreamT0DiskAYStep",2.0);
  Control.addVariable("shortDreamT0DiskAZStep",0.0);
  Control.addVariable("shortDreamT0DiskAXYangle",0.0);
  Control.addVariable("shortDreamT0DiskAZangle",0.0);

  Control.addVariable("shortDreamT0DiskAInnerRadius",28.0);
  Control.addVariable("shortDreamT0DiskAOuterRadius",38.0);
  Control.addVariable("shortDreamT0DiskANDisk",1);

  Control.addVariable("shortDreamT0DiskA0Thick",5.4);  // to include B4C
  Control.addVariable("shortDreamT0DiskAInnerMat","Inconnel");
  Control.addVariable("shortDreamT0DiskAOuterMat","Tungsten");
  
  Control.addVariable("shortDreamT0DiskANBlades",2);
  Control.addVariable("shortDreamT0DiskA0PhaseAngle0",95.0);
  Control.addVariable("shortDreamT0DiskA0OpenAngle0",35.0);
  Control.addVariable("shortDreamT0DiskA0PhaseAngle1",275.0);
  Control.addVariable("shortDreamT0DiskA0OpenAngle1",25.0);

  // T0 Chopper disk B
  Control.addVariable("shortDreamT0DiskBXStep",0.0);
  Control.addVariable("shortDreamT0DiskBYStep",-2.0);
  Control.addVariable("shortDreamT0DiskBZStep",0.0);
  Control.addVariable("shortDreamT0DiskBXYangle",0.0);
  Control.addVariable("shortDreamT0DiskBZangle",0.0);

  Control.addVariable("shortDreamT0DiskBInnerRadius",28.0);
  Control.addVariable("shortDreamT0DiskBOuterRadius",38.0);
  Control.addVariable("shortDreamT0DiskBNDisk",1);

  Control.addVariable("shortDreamT0DiskB0Thick",5.4);  // to include B4C
  Control.addVariable("shortDreamT0DiskBInnerMat","Inconnel");
  Control.addVariable("shortDreamT0DiskBOuterMat","Void");
  
  Control.addVariable("shortDreamT0DiskBNBlades",2);
  Control.addVariable("shortDreamT0DiskB0PhaseAngle0",95.0);
  Control.addVariable("shortDreamT0DiskB0OpenAngle0",35.0);
  Control.addVariable("shortDreamT0DiskB0PhaseAngle1",275.0);
  Control.addVariable("shortDreamT0DiskB0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("shortDreamT0DiskAHouseYStep",2.6);
  Control.addVariable("shortDreamT0DiskAHouseZStep",2.0);
  Control.addVariable("shortDreamT0DiskAHouseVoidHeight",38.0);
  Control.addVariable("shortDreamT0DiskAHouseVoidDepth",28.0);
  Control.addVariable("shortDreamT0DiskAHouseVoidThick",8.0);
  Control.addVariable("shortDreamT0DiskAHouseVoidWidth",80.0);
  Control.addVariable("shortDreamT0DiskAHouseWallThick",1.0);
  Control.addVariable("shortDreamT0DiskAHouseWallMat","Stainless304");

  Control.addVariable("shortDreamT0DiskBHouseYStep",2.6);
  Control.addVariable("shortDreamT0DiskBHouseZStep",2.0);
  Control.addVariable("shortDreamT0DiskBHouseVoidHeight",38.0);
  Control.addVariable("shortDreamT0DiskBHouseVoidDepth",28.0);
  Control.addVariable("shortDreamT0DiskBHouseVoidThick",8.0);
  Control.addVariable("shortDreamT0DiskBHouseVoidWidth",80.0);
  Control.addVariable("shortDreamT0DiskBHouseWallThick",1.0);
  Control.addVariable("shortDreamT0DiskBHouseWallMat","Stainless304");

  // VACUUM PIPES:
  Control.addVariable("shortDreamPipeBRadius",8.0);
  Control.addVariable("shortDreamPipeBLength",10.0);
  Control.addVariable("shortDreamPipeBFeThick",1.0);
  Control.addVariable("shortDreamPipeBFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeBFlangeLength",1.0);
  Control.addVariable("shortDreamPipeBFeMat","Stainless304");

  // VACBOX B : for the second chopper T0 unit
  Control.addVariable("shortDreamVacBYStep",40.0);
    
  Control.addVariable("shortDreamVacBVoidHeight",20.0);
  Control.addVariable("shortDreamVacBVoidDepth",19.0);
  Control.addVariable("shortDreamVacBVoidWidth",87.0);
  Control.addVariable("shortDreamVacBVoidLength",10.7);
  
  Control.addVariable("shortDreamVacBFeHeight",0.5);
  Control.addVariable("shortDreamVacBFeDepth",0.5);
  Control.addVariable("shortDreamVacBFeWidth",0.5);
  Control.addVariable("shortDreamVacBFeFront",0.5);
  Control.addVariable("shortDreamVacBFeBack",0.5);
  Control.addVariable("shortDreamVacBFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacBFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacBFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacBFeMat","Stainless304");

  // 5.5m to the first chopper:
  Control.addVariable("shortDreamFCXStep",0.0);       
  Control.addVariable("shortDreamFCYStep",0.2);       
  Control.addVariable("shortDreamFCZStep",0.0);       
  Control.addVariable("shortDreamFCXYAngle",0.0);       
  Control.addVariable("shortDreamFCZAngle",0.0);
  Control.addVariable("shortDreamFCLength",37.0);       
  
  Control.addVariable("shortDreamFCBeamYStep",1.0);
 
  Control.addVariable("shortDreamFCNShapes",1);       
  Control.addVariable("shortDreamFCNShapeLayers",3);
  Control.addVariable("shortDreamFCActiveShield",0);

  Control.addVariable("shortDreamFCLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFCLayerThick2",1.5);

  Control.addVariable("shortDreamFCLayerMat0","Void");
  Control.addVariable("shortDreamFCLayerMat1","Aluminium");
  Control.addVariable("shortDreamFCLayerMat2","Void");       
  
  Control.addVariable("shortDreamFC0TypeID","Tapper");
  Control.addVariable("shortDreamFC0HeightStart",4.5);
  Control.addVariable("shortDreamFC0HeightEnd",4.6);
  Control.addVariable("shortDreamFC0WidthStart",1.88);
  Control.addVariable("shortDreamFC0WidthEnd",2.06);
  Control.addVariable("shortDreamFC0Length",37.0);


  // Band Pass/C : for the band pass chopper
  Control.addVariable("shortDreamBandADiskXStep",0.0);
  Control.addVariable("shortDreamBandADiskYStep",0.5);
  Control.addVariable("shortDreamBandADiskZStep",0.0);
  Control.addVariable("shortDreamBandADiskXYangle",0.0);
  Control.addVariable("shortDreamBandADiskZangle",0.0);

  Control.addVariable("shortDreamBandADiskInnerRadius",22.0);
  Control.addVariable("shortDreamBandADiskOuterRadius",30.0);
  Control.addVariable("shortDreamBandADiskNDisk",1);

  Control.addVariable("shortDreamBandADisk0Thick",0.5);  // to include B4C
  Control.addVariable("shortDreamBandADiskInnerMat","Inconnel");
  Control.addVariable("shortDreamBandADiskOuterMat","Void");
  
  Control.addVariable("shortDreamBandADiskNBlades",2);
  Control.addVariable("shortDreamBandADisk0PhaseAngle0",95.0);
  Control.addVariable("shortDreamBandADisk0OpenAngle0",35.0);
  Control.addVariable("shortDreamBandADisk0PhaseAngle1",275.0);
  Control.addVariable("shortDreamBandADisk0OpenAngle1",25.0);

  Control.addVariable("shortDreamVacCYStep",213.0);    
  Control.addVariable("shortDreamVacCVoidHeight",20.0);
  Control.addVariable("shortDreamVacCVoidDepth",19.0);
  Control.addVariable("shortDreamVacCVoidWidth",87.0);
  Control.addVariable("shortDreamVacCVoidLength",10.7);
  
  Control.addVariable("shortDreamVacCFeHeight",0.5);
  Control.addVariable("shortDreamVacCFeDepth",0.5);
  Control.addVariable("shortDreamVacCFeWidth",0.5);
  Control.addVariable("shortDreamVacCFeFront",0.5);
  Control.addVariable("shortDreamVacCFeBack",0.5);
  Control.addVariable("shortDreamVacCFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacCFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacCFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacCFeMat","Stainless304");

  Control.addVariable("shortDreamBandAHouseYStep",0.0);
  Control.addVariable("shortDreamBandAHouseVoidHeight",38.0);
  Control.addVariable("shortDreamBandAHouseVoidDepth",20.0);
  Control.addVariable("shortDreamBandAHouseVoidThick",3.0);
  Control.addVariable("shortDreamBandAHouseVoidWidth",80.0);
  Control.addVariable("shortDreamBandAHouseWallThick",1.0);
  Control.addVariable("shortDreamBandAHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("shortDreamPipeCRadius",8.0);
  Control.addVariable("shortDreamPipeCLength",300.0);
  Control.addVariable("shortDreamPipeCFeThick",1.0);
  Control.addVariable("shortDreamPipeCFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeCFlangeLength",1.0);
  Control.addVariable("shortDreamPipeCFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortDreamFDXStep",0.0);       
  Control.addVariable("shortDreamFDYStep",0.8);       
  Control.addVariable("shortDreamFDZStep",0.0);       
  Control.addVariable("shortDreamFDXYAngle",0.0);       
  Control.addVariable("shortDreamFDZAngle",0.0);
  Control.addVariable("shortDreamFDLength",210.0);       
  
  Control.addVariable("shortDreamFDBeamYStep",1.0);
 
  Control.addVariable("shortDreamFDNShapes",1);       
  Control.addVariable("shortDreamFDNShapeLayers",3);
  Control.addVariable("shortDreamFDActiveShield",0);

  Control.addVariable("shortDreamFDLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFDLayerThick2",1.5);

  Control.addVariable("shortDreamFDLayerMat0","Void");
  Control.addVariable("shortDreamFDLayerMat1","Aluminium");
  Control.addVariable("shortDreamFDLayerMat2","Void");       
  
  Control.addVariable("shortDreamFD0TypeID","Tapper");
  Control.addVariable("shortDreamFD0HeightStart",4.5);
  Control.addVariable("shortDreamFD0HeightEnd",4.97);
  Control.addVariable("shortDreamFD0WidthStart",2.24);
  Control.addVariable("shortDreamFD0WidthEnd",3.05);
  Control.addVariable("shortDreamFD0Length",210.0);

  // ------------------------------------------
  // Band Pass/C : for the band pass chopper
  // -------------------------------------------
  Control.addVariable("shortDreamVacDYStep",165.0);    
  Control.addVariable("shortDreamVacDVoidHeight",20.0);
  Control.addVariable("shortDreamVacDVoidDepth",19.0);
  Control.addVariable("shortDreamVacDVoidWidth",87.0);
  Control.addVariable("shortDreamVacDVoidLength",10.7);
  
  Control.addVariable("shortDreamVacDFeHeight",0.5);
  Control.addVariable("shortDreamVacDFeDepth",0.5);
  Control.addVariable("shortDreamVacDFeWidth",0.5);
  Control.addVariable("shortDreamVacDFeFront",0.5);
  Control.addVariable("shortDreamVacDFeBack",0.5);
  Control.addVariable("shortDreamVacDFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacDFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacDFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacDFeMat","Stainless304");

  Control.addVariable("shortDreamBandBDiskXStep",0.0);
  Control.addVariable("shortDreamBandBDiskYStep",0.5);
  Control.addVariable("shortDreamBandBDiskZStep",0.0);
  Control.addVariable("shortDreamBandBDiskXYangle",0.0);
  Control.addVariable("shortDreamBandBDiskZangle",0.0);

  Control.addVariable("shortDreamBandBDiskInnerRadius",22.0);
  Control.addVariable("shortDreamBandBDiskOuterRadius",30.0);
  Control.addVariable("shortDreamBandBDiskNDisk",1);

  Control.addVariable("shortDreamBandBDisk0Thick",0.5);  // to include B4C
  Control.addVariable("shortDreamBandBDiskInnerMat","Inconnel");
  Control.addVariable("shortDreamBandBDiskOuterMat","Void");
  
  Control.addVariable("shortDreamBandBDiskNBlades",2);
  Control.addVariable("shortDreamBandBDisk0PhaseAngle0",95.0);
  Control.addVariable("shortDreamBandBDisk0OpenAngle0",35.0);
  Control.addVariable("shortDreamBandBDisk0PhaseAngle1",275.0);
  Control.addVariable("shortDreamBandBDisk0OpenAngle1",25.0);


  Control.addVariable("shortDreamBandBHouseYStep",0.0);
  Control.addVariable("shortDreamBandBHouseVoidHeight",38.0);
  Control.addVariable("shortDreamBandBHouseVoidDepth",22.0);
  Control.addVariable("shortDreamBandBHouseVoidThick",3.0);
  Control.addVariable("shortDreamBandBHouseVoidWidth",80.0);
  Control.addVariable("shortDreamBandBHouseWallThick",1.0);
  Control.addVariable("shortDreamBandBHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("shortDreamPipeDRadius",8.0);
  Control.addVariable("shortDreamPipeDLength",40.0);
  Control.addVariable("shortDreamPipeDFeThick",1.0);
  Control.addVariable("shortDreamPipeDFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeDFlangeLength",1.0);
  Control.addVariable("shortDreamPipeDFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortDreamFEXStep",0.0);       
  Control.addVariable("shortDreamFEYStep",0.8);       
  Control.addVariable("shortDreamFEZStep",0.0);       
  Control.addVariable("shortDreamFEXYAngle",0.0);       
  Control.addVariable("shortDreamFEZAngle",0.0);
  Control.addVariable("shortDreamFELength",40.0);      //
  
  Control.addVariable("shortDreamFEBeamYStep",1.0);
 
  Control.addVariable("shortDreamFENShapes",1);       
  Control.addVariable("shortDreamFENShapeLayers",3);
  Control.addVariable("shortDreamFEActiveShield",0);

  Control.addVariable("shortDreamFELayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFELayerThick2",1.5);

  Control.addVariable("shortDreamFELayerMat0","Void");
  Control.addVariable("shortDreamFELayerMat1","Aluminium");
  Control.addVariable("shortDreamFELayerMat2","Void");       
  
  Control.addVariable("shortDreamFE0TypeID","Tapper");
  Control.addVariable("shortDreamFE0HeightStart",4.5);
  Control.addVariable("shortDreamFE0HeightEnd",4.97);
  Control.addVariable("shortDreamFE0WidthStart",2.24);
  Control.addVariable("shortDreamFE0WidthEnd",3.05);
  
  Control.addParse<double>("shortDreamFE0Length","shortDreamFELength");
  //  Control.addVariable("shortDreamFE0Length",80.0);

  // ------------------------------------------
  // T0DiskC : for the second T0 pair
  // -------------------------------------------
  Control.addVariable("shortDreamVacEYStep",50.0);    
  Control.addVariable("shortDreamVacEVoidHeight",20.0);
  Control.addVariable("shortDreamVacEVoidDepth",19.0);
  Control.addVariable("shortDreamVacEVoidWidth",87.0);
  Control.addVariable("shortDreamVacEVoidLength",10.7);
  
  Control.addVariable("shortDreamVacEFeHeight",0.5);
  Control.addVariable("shortDreamVacEFeDepth",0.5);
  Control.addVariable("shortDreamVacEFeWidth",0.5);
  Control.addVariable("shortDreamVacEFeFront",0.5);
  Control.addVariable("shortDreamVacEFeBack",0.5);
  Control.addVariable("shortDreamVacEFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacEFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacEFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacEFeMat","Stainless304");


    // T0 Chopper disk A
  Control.addVariable("shortDreamT0DiskCXStep",0.0);
  Control.addVariable("shortDreamT0DiskCYStep",-2.0);
  Control.addVariable("shortDreamT0DiskCZStep",0.0);
  Control.addVariable("shortDreamT0DiskCXYangle",0.0);
  Control.addVariable("shortDreamT0DiskCZangle",0.0);

  Control.addVariable("shortDreamT0DiskCInnerRadius",28.0);
  Control.addVariable("shortDreamT0DiskCOuterRadius",38.0);
  Control.addVariable("shortDreamT0DiskCNDisk",1);

  Control.addVariable("shortDreamT0DiskC0Thick",5.4);  // to include B4C
  Control.addVariable("shortDreamT0DiskCInnerMat","Inconnel");
  Control.addVariable("shortDreamT0DiskCOuterMat","Void");
  
  Control.addVariable("shortDreamT0DiskCNBlades",2);
  Control.addVariable("shortDreamT0DiskC0PhaseAngle0",95.0);
  Control.addVariable("shortDreamT0DiskC0OpenAngle0",35.0);
  Control.addVariable("shortDreamT0DiskC0PhaseAngle1",275.0);
  Control.addVariable("shortDreamT0DiskC0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("shortDreamT0DiskCHouseYStep",2.6);
  Control.addVariable("shortDreamT0DiskCHouseZStep",2.0);
  Control.addVariable("shortDreamT0DiskCHouseVoidHeight",38.0);
  Control.addVariable("shortDreamT0DiskCHouseVoidDepth",28.0);
  Control.addVariable("shortDreamT0DiskCHouseVoidThick",8.0);
  Control.addVariable("shortDreamT0DiskCHouseVoidWidth",80.0);
  Control.addVariable("shortDreamT0DiskCHouseWallThick",1.0);
  Control.addVariable("shortDreamT0DiskCHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("shortDreamPipeERadius",8.0);
  Control.addVariable("shortDreamPipeELength",300.0);
  Control.addVariable("shortDreamPipeEFeThick",1.0);
  Control.addVariable("shortDreamPipeEFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeEFlangeLength",1.0);
  Control.addVariable("shortDreamPipeEFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortDreamFFXStep",0.0);       
  Control.addVariable("shortDreamFFYStep",0.8);       
  Control.addVariable("shortDreamFFZStep",0.0);       
  Control.addVariable("shortDreamFFXYAngle",0.0);       
  Control.addVariable("shortDreamFFZAngle",0.0);
  Control.addVariable("shortDreamFFLength",40.0);       
  
  Control.addVariable("shortDreamFFBeamYStep",1.0);
 
  Control.addVariable("shortDreamFFNShapes",1);       
  Control.addVariable("shortDreamFFNShapeLayers",3);
  Control.addVariable("shortDreamFFActiveShield",0);

  Control.addVariable("shortDreamFFLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFFLayerThick2",1.5);

  Control.addVariable("shortDreamFFLayerMat0","Void");
  Control.addVariable("shortDreamFFLayerMat1","Aluminium");
  Control.addVariable("shortDreamFFLayerMat2","Void");       
  
  Control.addVariable("shortDreamFF0TypeID","Tapper");
  Control.addVariable("shortDreamFF0HeightStart",4.5);
  Control.addVariable("shortDreamFF0HeightEnd",4.97);
  Control.addVariable("shortDreamFF0WidthStart",2.24);
  Control.addVariable("shortDreamFF0WidthEnd",3.05);
  Control.addVariable("shortDreamFF0Length",40.0);



  // ---------------------------------
  // T0 Chopper disk D (second Part B)
  //----------------------------------

  Control.addVariable("shortDreamVacFYStep",50.0);    
  Control.addVariable("shortDreamVacFVoidHeight",20.0);
  Control.addVariable("shortDreamVacFVoidDepth",19.0);
  Control.addVariable("shortDreamVacFVoidWidth",87.0);
  Control.addVariable("shortDreamVacFVoidLength",10.7);
  
  Control.addVariable("shortDreamVacFFeHeight",0.5);
  Control.addVariable("shortDreamVacFFeDepth",0.5);
  Control.addVariable("shortDreamVacFFeWidth",0.5);
  Control.addVariable("shortDreamVacFFeFront",0.5);
  Control.addVariable("shortDreamVacFFeBack",0.5);
  Control.addVariable("shortDreamVacFFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortDreamVacFFlangeWall",1.0);      // GUESS
  Control.addVariable("shortDreamVacFFlangeLength",10.0);   // GUESS
  Control.addVariable("shortDreamVacFFeMat","Stainless304");
  
  Control.addVariable("shortDreamT0DiskDXStep",0.0);
  Control.addVariable("shortDreamT0DiskDYStep",-2.0);
  Control.addVariable("shortDreamT0DiskDZStep",0.0);
  Control.addVariable("shortDreamT0DiskDXYangle",0.0);
  Control.addVariable("shortDreamT0DiskDZangle",0.0);

  Control.addVariable("shortDreamT0DiskDInnerRadius",28.0);
  Control.addVariable("shortDreamT0DiskDOuterRadius",38.0);
  Control.addVariable("shortDreamT0DiskDNDisk",1);

  Control.addVariable("shortDreamT0DiskD0Thick",5.4);  // to include B4C
  Control.addVariable("shortDreamT0DiskDInnerMat","Inconnel");
  Control.addVariable("shortDreamT0DiskDOuterMat","Tungsten");
  
  Control.addVariable("shortDreamT0DiskDNBlades",2);
  Control.addVariable("shortDreamT0DiskD0PhaseAngle0",95.0);
  Control.addVariable("shortDreamT0DiskD0OpenAngle0",35.0);
  Control.addVariable("shortDreamT0DiskD0PhaseAngle1",275.0);
  Control.addVariable("shortDreamT0DiskD0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("shortDreamT0DiskDHouseYStep",2.6);
  Control.addVariable("shortDreamT0DiskDHouseZStep",2.0);
  Control.addVariable("shortDreamT0DiskDHouseVoidHeight",38.0);
  Control.addVariable("shortDreamT0DiskDHouseVoidDepth",28.0);
  Control.addVariable("shortDreamT0DiskDHouseVoidThick",8.0);
  Control.addVariable("shortDreamT0DiskDHouseVoidWidth",80.0);
  Control.addVariable("shortDreamT0DiskDHouseWallThick",1.0);
  Control.addVariable("shortDreamT0DiskDHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("shortDreamPipeFRadius",8.0);
  Control.addVariable("shortDreamPipeFLength",300.0);
  Control.addVariable("shortDreamPipeFFeThick",1.0);
  Control.addVariable("shortDreamPipeFFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeFFlangeLength",1.0);
  Control.addVariable("shortDreamPipeFFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortDreamFGXStep",0.0);       
  Control.addVariable("shortDreamFGYStep",0.8);       
  Control.addVariable("shortDreamFGZStep",0.0);       
  Control.addVariable("shortDreamFGXYAngle",0.0);       
  Control.addVariable("shortDreamFGZAngle",0.0);
  Control.addVariable("shortDreamFGLength",40.0);       
  
  Control.addVariable("shortDreamFGBeamYStep",1.0);
 
  Control.addVariable("shortDreamFGNShapes",1);       
  Control.addVariable("shortDreamFGNShapeLayers",3);
  Control.addVariable("shortDreamFGActiveShield",0);

  Control.addVariable("shortDreamFGLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFGLayerThick2",1.5);

  Control.addVariable("shortDreamFGLayerMat0","Void");
  Control.addVariable("shortDreamFGLayerMat1","Glass");
  Control.addVariable("shortDreamFGLayerMat2","Void");       
  
  Control.addVariable("shortDreamFG0TypeID","Tapper");
  Control.addVariable("shortDreamFG0HeightStart",4.5);
  Control.addVariable("shortDreamFG0HeightEnd",4.97);
  Control.addVariable("shortDreamFG0WidthStart",2.24);
  Control.addVariable("shortDreamFG0WidthEnd",3.05);
  Control.addVariable("shortDreamFG0Length",40.0);

  
  // --------------------
  // PIPE to Bunker Wall:
  // --------------------
  Control.addVariable("shortDreamPipeFinalRadius",8.0);
  Control.addVariable("shortDreamPipeFinalLength",200.0);
  Control.addVariable("shortDreamPipeFinalFeThick",1.0);
  Control.addVariable("shortDreamPipeFinalFlangeRadius",12.0);
  Control.addVariable("shortDreamPipeFinalFlangeLength",1.0);
  Control.addVariable("shortDreamPipeFinalFeMat","Stainless304");

  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortDreamFFinalXStep",0.0);       
  Control.addVariable("shortDreamFFinalYStep",0.8);       
  Control.addVariable("shortDreamFFinalZStep",0.0);       
  Control.addVariable("shortDreamFFinalXYAngle",0.0);       
  Control.addVariable("shortDreamFFinalZAngle",0.0);
  Control.addVariable("shortDreamFFinalLength",440.0);       
  
  Control.addVariable("shortDreamFFinalBeamYStep",1.0);
 
  Control.addVariable("shortDreamFFinalNShapes",1);       
  Control.addVariable("shortDreamFFinalNShapeLayers",3);
  Control.addVariable("shortDreamFFinalActiveShield",0);

  Control.addVariable("shortDreamFFinalLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFFinalLayerThick2",1.5);

  Control.addVariable("shortDreamFFinalLayerMat0","Void");
  Control.addVariable("shortDreamFFinalLayerMat1","Glass");
  Control.addVariable("shortDreamFFinalLayerMat2","Void");       
  
  Control.addVariable("shortDreamFFinal0TypeID","Tapper");
  Control.addVariable("shortDreamFFinal0HeightStart",4.5);
  Control.addVariable("shortDreamFFinal0HeightEnd",4.97);
  Control.addVariable("shortDreamFFinal0WidthStart",2.24);
  Control.addVariable("shortDreamFFinal0WidthEnd",3.05);
  Control.addVariable("shortDreamFFinal0Length",440.0);

  // BEAM INSERT:
  Control.addVariable("shortDreamBInsertHeight",20.0);
  Control.addVariable("shortDreamBInsertWidth",28.0);
  Control.addVariable("shortDreamBInsertTopWall",1.0);
  Control.addVariable("shortDreamBInsertLowWall",1.0);
  Control.addVariable("shortDreamBInsertLeftWall",1.0);
  Control.addVariable("shortDreamBInsertRightWall",1.0);
  Control.addVariable("shortDreamBInsertWallMat","Stainless304");       

  // Guide in wall
  Control.addVariable("shortDreamFWallXStep",0.0);       
  Control.addVariable("shortDreamFWallYStep",0.8);       
  Control.addVariable("shortDreamFWallZStep",0.0);       
  Control.addVariable("shortDreamFWallXYAngle",0.0);       
  Control.addVariable("shortDreamFWallZAngle",0.0);
  Control.addVariable("shortDreamFWallLength",318.0);       
  
  Control.addVariable("shortDreamFWallBeamYStep",1.0);
 
  Control.addVariable("shortDreamFWallNShapes",1);       
  Control.addVariable("shortDreamFWallNShapeLayers",3);
  Control.addVariable("shortDreamFWallActiveShield",0);

  Control.addVariable("shortDreamFWallLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFWallLayerThick2",1.5);

  Control.addVariable("shortDreamFWallLayerMat0","Void");
  Control.addVariable("shortDreamFWallLayerMat1","Glass");
  Control.addVariable("shortDreamFWallLayerMat2","Void");       
  
  Control.addVariable("shortDreamFWall0TypeID","Rectangle");
  Control.addVariable("shortDreamFWall0Height",6.0);
  Control.addVariable("shortDreamFWall0Width",6.0);
  Control.addVariable("shortDreamFWall0Length",318.0);

  // Guide after wall [17.5m - 3.20] for wall
  Control.addVariable("shortDreamFOutAXStep",0.0);       
  Control.addVariable("shortDreamFOutAYStep",1.6);
  Control.addVariable("shortDreamFOutAZStep",0.0);       
  Control.addVariable("shortDreamFOutAXYAngle",0.0);       
  Control.addVariable("shortDreamFOutAZAngle",0.0);
  Control.addVariable("shortDreamFOutALength",1750.0-320.8);       
  
  Control.addVariable("shortDreamFOutANShapes",1);       
  Control.addVariable("shortDreamFOutANShapeLayers",3);
  Control.addVariable("shortDreamFOutAActiveShield",0);

  Control.addVariable("shortDreamFOutALayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFOutALayerThick2",1.5);

  Control.addVariable("shortDreamFOutALayerMat0","Void");
  Control.addVariable("shortDreamFOutALayerMat1","Glass");
  Control.addVariable("shortDreamFOutALayerMat2","Void");       
  
  Control.addVariable("shortDreamFOutA0TypeID","Tapper");
  Control.addVariable("shortDreamFOutA0HeightStart",4.5);
  Control.addVariable("shortDreamFOutA0HeightEnd",4.97);
  Control.addVariable("shortDreamFOutA0WidthStart",2.24);
  Control.addVariable("shortDreamFOutA0WidthEnd",3.05);
  Control.addVariable("shortDreamFOutA0Length",1750.0-320.8);

  Control.addVariable("shortDreamPipeOutARadius",6.0);
  Control.addVariable("shortDreamPipeOutALength",1750.0-320);
  Control.addVariable("shortDreamPipeOutAFeThick",1.0);
  Control.addVariable("shortDreamPipeOutAFlangeRadius",9.0);
  Control.addVariable("shortDreamPipeOutAFlangeLength",1.0);
  Control.addVariable("shortDreamPipeOutAFeMat","Stainless304");


  Control.addVariable("shortDreamShieldALength",1750.0-320);
  Control.addVariable("shortDreamShieldALeft",40.0);
  Control.addVariable("shortDreamShieldARight",40.0);
  Control.addVariable("shortDreamShieldAHeight",40.0);
  Control.addVariable("shortDreamShieldADepth",40.0);
  Control.addVariable("shortDreamShieldADefMat","Stainless304");
  Control.addVariable("shortDreamShieldANSeg",8);
  Control.addVariable("shortDreamShieldANWallLayers",8);
  Control.addVariable("shortDreamShieldANFloorLayers",3);
  Control.addVariable("shortDreamShieldANRoofLayers",8);
  Control.addVariable("shortDreamShieldAWallLen1",10.0);
  Control.addVariable("shortDreamShieldAWallMat1","CastIron");
  Control.addVariable("shortDreamShieldAWallMat5","Concrete");

  Control.addVariable("shortDreamShieldARoofLen1",10.0);
  Control.addVariable("shortDreamShieldAFloorLen1",10.0);


  // Guide after wall [+17.5m] after section 1
  Control.addVariable("shortDreamFOutBXStep",0.0);       
  Control.addVariable("shortDreamFOutBYStep",0.8);       
  Control.addVariable("shortDreamFOutBZStep",0.0);       
  Control.addVariable("shortDreamFOutBXYAngle",0.0);       
  Control.addVariable("shortDreamFOutBZAngle",0.0);
  Control.addVariable("shortDreamFOutBLength",1748.2);       
  
  Control.addVariable("shortDreamFOutBBeamYStep",1.0);
 
  Control.addVariable("shortDreamFOutBNShapes",1);       
  Control.addVariable("shortDreamFOutBNShapeLayers",3);
  Control.addVariable("shortDreamFOutBActiveShield",0);

  Control.addVariable("shortDreamFOutBLayerThick1",0.4);  // glass thick
  Control.addVariable("shortDreamFOutBLayerThick2",1.5);

  Control.addVariable("shortDreamFOutBLayerMat0","Void");
  Control.addVariable("shortDreamFOutBLayerMat1","Glass");
  Control.addVariable("shortDreamFOutBLayerMat2","Void");       
  
  Control.addVariable("shortDreamFOutB0TypeID","Rectangle");
  Control.addVariable("shortDreamFOutB0Height",5.0);
  Control.addVariable("shortDreamFOutB0Width",4.0);
  Control.addVariable("shortDreamFOutB0Length",1748.2);

  Control.addVariable("shortDreamPipeOutBRadius",6.0);
  Control.addVariable("shortDreamPipeOutBLength",1750.0);
  Control.addVariable("shortDreamPipeOutBFeThick",1.0);
  Control.addVariable("shortDreamPipeOutBFlangeRadius",9.0);
  Control.addVariable("shortDreamPipeOutBFlangeLength",1.0);
  Control.addVariable("shortDreamPipeOutBFeMat","Stainless304");


  Control.addVariable("shortDreamShieldBLength",1750.0);
  Control.addVariable("shortDreamShieldBLeft",32.0);
  Control.addVariable("shortDreamShieldBRight",32.0);
  Control.addVariable("shortDreamShieldBHeight",32.0);
  Control.addVariable("shortDreamShieldBDepth",32.0);
  Control.addVariable("shortDreamShieldBDefMat","Stainless304");
  Control.addVariable("shortDreamShieldBNSeg",8);
  Control.addVariable("shortDreamShieldBNWallLayers",8);
  Control.addVariable("shortDreamShieldBNFloorLayers",3);
  Control.addVariable("shortDreamShieldBNRoofLayers",8);
  Control.addVariable("shortDreamShieldBWallLen1",10.0);
  Control.addVariable("shortDreamShieldBWallMat1","CastIron");
  Control.addVariable("shortDreamShieldBWallMat5","Concrete");

  Control.addVariable("shortDreamShieldBRoofLen1",10.0);
  Control.addVariable("shortDreamShieldBFloorLen1",10.0);

  return;
}
 
}  // NAMESPACE setVariable
