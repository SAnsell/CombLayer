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
  Control.addVariable("dreamFAXStep",-2.0);        // Centre of thermal
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
  Control.addVariable("dreamFA0WidthStart",8.0); // NOT Centred
  Control.addVariable("dreamFA0WidthEnd",2.0);
  Control.addVariable("dreamFA0Length",350.0);

  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("dreamVacAYStep",40.0);
    
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
  Control.addVariable("dreamDBladeYStep",11.0);
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
  Control.addVariable("dreamT0DiskBYStep",-2.0);
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
  Control.addVariable("dreamVacBYStep",40.0);
    
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


  // Band Pass/C : for the band pass chopper
  Control.addVariable("dreamBandADiskXStep",0.0);
  Control.addVariable("dreamBandADiskYStep",0.5);
  Control.addVariable("dreamBandADiskZStep",0.0);
  Control.addVariable("dreamBandADiskXYangle",0.0);
  Control.addVariable("dreamBandADiskZangle",0.0);

  Control.addVariable("dreamBandADiskInnerRadius",22.0);
  Control.addVariable("dreamBandADiskOuterRadius",30.0);
  Control.addVariable("dreamBandADiskNDisk",1);

  Control.addVariable("dreamBandADisk0Thick",0.5);  // to include B4C
  Control.addVariable("dreamBandADiskInnerMat","Inconnel");
  Control.addVariable("dreamBandADiskOuterMat","Tungsten");
  
  Control.addVariable("dreamBandADiskNBlades",2);
  Control.addVariable("dreamBandADisk0PhaseAngle0",95.0);
  Control.addVariable("dreamBandADisk0OpenAngle0",35.0);
  Control.addVariable("dreamBandADisk0PhaseAngle1",275.0);
  Control.addVariable("dreamBandADisk0OpenAngle1",25.0);

  Control.addVariable("dreamVacCYStep",213.0);    
  Control.addVariable("dreamVacCVoidHeight",20.0);
  Control.addVariable("dreamVacCVoidDepth",19.0);
  Control.addVariable("dreamVacCVoidWidth",87.0);
  Control.addVariable("dreamVacCVoidLength",10.7);
  
  Control.addVariable("dreamVacCFeHeight",0.5);
  Control.addVariable("dreamVacCFeDepth",0.5);
  Control.addVariable("dreamVacCFeWidth",0.5);
  Control.addVariable("dreamVacCFeFront",0.5);
  Control.addVariable("dreamVacCFeBack",0.5);
  Control.addVariable("dreamVacCFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacCFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacCFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacCFeMat","Stainless304");

  Control.addVariable("dreamBandAHouseYStep",0.0);
  Control.addVariable("dreamBandAHouseVoidHeight",38.0);
  Control.addVariable("dreamBandAHouseVoidDepth",20.0);
  Control.addVariable("dreamBandAHouseVoidThick",3.0);
  Control.addVariable("dreamBandAHouseVoidWidth",80.0);
  Control.addVariable("dreamBandAHouseWallThick",1.0);
  Control.addVariable("dreamBandAHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("dreamPipeCRadius",8.0);
  Control.addVariable("dreamPipeCLength",300.0);
  Control.addVariable("dreamPipeCFeThick",1.0);
  Control.addVariable("dreamPipeCFlangeRadius",12.0);
  Control.addVariable("dreamPipeCFlangeLength",1.0);
  Control.addVariable("dreamPipeCFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("dreamFDXStep",0.0);       
  Control.addVariable("dreamFDYStep",0.8);       
  Control.addVariable("dreamFDZStep",0.0);       
  Control.addVariable("dreamFDXYAngle",0.0);       
  Control.addVariable("dreamFDZAngle",0.0);
  Control.addVariable("dreamFDLength",210.0);       
  
  Control.addVariable("dreamFDBeamYStep",1.0);
 
  Control.addVariable("dreamFDNShapes",1);       
  Control.addVariable("dreamFDNShapeLayers",3);
  Control.addVariable("dreamFDActiveShield",0);

  Control.addVariable("dreamFDLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFDLayerThick2",1.5);

  Control.addVariable("dreamFDLayerMat0","Void");
  Control.addVariable("dreamFDLayerMat1","Glass");
  Control.addVariable("dreamFDLayerMat2","Void");       
  
  Control.addVariable("dreamFD0TypeID","Tapper");
  Control.addVariable("dreamFD0HeightStart",4.5);
  Control.addVariable("dreamFD0HeightEnd",4.97);
  Control.addVariable("dreamFD0WidthStart",2.24);
  Control.addVariable("dreamFD0WidthEnd",3.05);
  Control.addVariable("dreamFD0Length",210.0);

  // ------------------------------------------
  // Band Pass/C : for the band pass chopper
  // -------------------------------------------
  Control.addVariable("dreamVacDYStep",450.0);    
  Control.addVariable("dreamVacDVoidHeight",20.0);
  Control.addVariable("dreamVacDVoidDepth",19.0);
  Control.addVariable("dreamVacDVoidWidth",87.0);
  Control.addVariable("dreamVacDVoidLength",10.7);
  
  Control.addVariable("dreamVacDFeHeight",0.5);
  Control.addVariable("dreamVacDFeDepth",0.5);
  Control.addVariable("dreamVacDFeWidth",0.5);
  Control.addVariable("dreamVacDFeFront",0.5);
  Control.addVariable("dreamVacDFeBack",0.5);
  Control.addVariable("dreamVacDFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacDFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacDFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacDFeMat","Stainless304");

  Control.addVariable("dreamBandBDiskXStep",0.0);
  Control.addVariable("dreamBandBDiskYStep",0.5);
  Control.addVariable("dreamBandBDiskZStep",0.0);
  Control.addVariable("dreamBandBDiskXYangle",0.0);
  Control.addVariable("dreamBandBDiskZangle",0.0);

  Control.addVariable("dreamBandBDiskInnerRadius",22.0);
  Control.addVariable("dreamBandBDiskOuterRadius",30.0);
  Control.addVariable("dreamBandBDiskNDisk",1);

  Control.addVariable("dreamBandBDisk0Thick",0.5);  // to include B4C
  Control.addVariable("dreamBandBDiskInnerMat","Inconnel");
  Control.addVariable("dreamBandBDiskOuterMat","Tungsten");
  
  Control.addVariable("dreamBandBDiskNBlades",2);
  Control.addVariable("dreamBandBDisk0PhaseAngle0",95.0);
  Control.addVariable("dreamBandBDisk0OpenAngle0",35.0);
  Control.addVariable("dreamBandBDisk0PhaseAngle1",275.0);
  Control.addVariable("dreamBandBDisk0OpenAngle1",25.0);


  Control.addVariable("dreamBandBHouseYStep",0.0);
  Control.addVariable("dreamBandBHouseVoidHeight",38.0);
  Control.addVariable("dreamBandBHouseVoidDepth",22.0);
  Control.addVariable("dreamBandBHouseVoidThick",3.0);
  Control.addVariable("dreamBandBHouseVoidWidth",80.0);
  Control.addVariable("dreamBandBHouseWallThick",1.0);
  Control.addVariable("dreamBandBHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("dreamPipeDRadius",8.0);
  Control.addVariable("dreamPipeDLength",300.0);
  Control.addVariable("dreamPipeDFeThick",1.0);
  Control.addVariable("dreamPipeDFlangeRadius",12.0);
  Control.addVariable("dreamPipeDFlangeLength",1.0);
  Control.addVariable("dreamPipeDFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("dreamFEXStep",0.0);       
  Control.addVariable("dreamFEYStep",0.8);       
  Control.addVariable("dreamFEZStep",0.0);       
  Control.addVariable("dreamFEXYAngle",0.0);       
  Control.addVariable("dreamFEZAngle",0.0);
  Control.addVariable("dreamFELength",440.0);       
  
  Control.addVariable("dreamFEBeamYStep",1.0);
 
  Control.addVariable("dreamFENShapes",1);       
  Control.addVariable("dreamFENShapeLayers",3);
  Control.addVariable("dreamFEActiveShield",0);

  Control.addVariable("dreamFELayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFELayerThick2",1.5);

  Control.addVariable("dreamFELayerMat0","Void");
  Control.addVariable("dreamFELayerMat1","Glass");
  Control.addVariable("dreamFELayerMat2","Void");       
  
  Control.addVariable("dreamFE0TypeID","Tapper");
  Control.addVariable("dreamFE0HeightStart",4.5);
  Control.addVariable("dreamFE0HeightEnd",4.97);
  Control.addVariable("dreamFE0WidthStart",2.24);
  Control.addVariable("dreamFE0WidthEnd",3.05);
  Control.addVariable("dreamFE0Length",440.0);

  // ------------------------------------------
  // T0DiskC : for the second T0 pair
  // -------------------------------------------
  Control.addVariable("dreamVacEYStep",50.0);    
  Control.addVariable("dreamVacEVoidHeight",20.0);
  Control.addVariable("dreamVacEVoidDepth",19.0);
  Control.addVariable("dreamVacEVoidWidth",87.0);
  Control.addVariable("dreamVacEVoidLength",10.7);
  
  Control.addVariable("dreamVacEFeHeight",0.5);
  Control.addVariable("dreamVacEFeDepth",0.5);
  Control.addVariable("dreamVacEFeWidth",0.5);
  Control.addVariable("dreamVacEFeFront",0.5);
  Control.addVariable("dreamVacEFeBack",0.5);
  Control.addVariable("dreamVacEFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacEFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacEFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacEFeMat","Stainless304");


    // T0 Chopper disk A
  Control.addVariable("dreamT0DiskCXStep",0.0);
  Control.addVariable("dreamT0DiskCYStep",-2.0);
  Control.addVariable("dreamT0DiskCZStep",0.0);
  Control.addVariable("dreamT0DiskCXYangle",0.0);
  Control.addVariable("dreamT0DiskCZangle",0.0);

  Control.addVariable("dreamT0DiskCInnerRadius",28.0);
  Control.addVariable("dreamT0DiskCOuterRadius",38.0);
  Control.addVariable("dreamT0DiskCNDisk",1);

  Control.addVariable("dreamT0DiskC0Thick",5.4);  // to include B4C
  Control.addVariable("dreamT0DiskCInnerMat","Inconnel");
  Control.addVariable("dreamT0DiskCOuterMat","Tungsten");
  
  Control.addVariable("dreamT0DiskCNBlades",2);
  Control.addVariable("dreamT0DiskC0PhaseAngle0",95.0);
  Control.addVariable("dreamT0DiskC0OpenAngle0",35.0);
  Control.addVariable("dreamT0DiskC0PhaseAngle1",275.0);
  Control.addVariable("dreamT0DiskC0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("dreamT0DiskCHouseYStep",2.6);
  Control.addVariable("dreamT0DiskCHouseZStep",2.0);
  Control.addVariable("dreamT0DiskCHouseVoidHeight",38.0);
  Control.addVariable("dreamT0DiskCHouseVoidDepth",28.0);
  Control.addVariable("dreamT0DiskCHouseVoidThick",8.0);
  Control.addVariable("dreamT0DiskCHouseVoidWidth",80.0);
  Control.addVariable("dreamT0DiskCHouseWallThick",1.0);
  Control.addVariable("dreamT0DiskCHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("dreamPipeERadius",8.0);
  Control.addVariable("dreamPipeELength",300.0);
  Control.addVariable("dreamPipeEFeThick",1.0);
  Control.addVariable("dreamPipeEFlangeRadius",12.0);
  Control.addVariable("dreamPipeEFlangeLength",1.0);
  Control.addVariable("dreamPipeEFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("dreamFFXStep",0.0);       
  Control.addVariable("dreamFFYStep",0.8);       
  Control.addVariable("dreamFFZStep",0.0);       
  Control.addVariable("dreamFFXYAngle",0.0);       
  Control.addVariable("dreamFFZAngle",0.0);
  Control.addVariable("dreamFFLength",40.0);       
  
  Control.addVariable("dreamFFBeamYStep",1.0);
 
  Control.addVariable("dreamFFNShapes",1);       
  Control.addVariable("dreamFFNShapeLayers",3);
  Control.addVariable("dreamFFActiveShield",0);

  Control.addVariable("dreamFFLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFFLayerThick2",1.5);

  Control.addVariable("dreamFFLayerMat0","Void");
  Control.addVariable("dreamFFLayerMat1","Glass");
  Control.addVariable("dreamFFLayerMat2","Void");       
  
  Control.addVariable("dreamFF0TypeID","Tapper");
  Control.addVariable("dreamFF0HeightStart",4.5);
  Control.addVariable("dreamFF0HeightEnd",4.97);
  Control.addVariable("dreamFF0WidthStart",2.24);
  Control.addVariable("dreamFF0WidthEnd",3.05);
  Control.addVariable("dreamFF0Length",40.0);



  // ---------------------------------
  // T0 Chopper disk D (second Part B)
  //----------------------------------

  Control.addVariable("dreamVacFYStep",50.0);    
  Control.addVariable("dreamVacFVoidHeight",20.0);
  Control.addVariable("dreamVacFVoidDepth",19.0);
  Control.addVariable("dreamVacFVoidWidth",87.0);
  Control.addVariable("dreamVacFVoidLength",10.7);
  
  Control.addVariable("dreamVacFFeHeight",0.5);
  Control.addVariable("dreamVacFFeDepth",0.5);
  Control.addVariable("dreamVacFFeWidth",0.5);
  Control.addVariable("dreamVacFFeFront",0.5);
  Control.addVariable("dreamVacFFeBack",0.5);
  Control.addVariable("dreamVacFFlangeRadius",8.0);    // GUESS
  Control.addVariable("dreamVacFFlangeWall",1.0);      // GUESS
  Control.addVariable("dreamVacFFlangeLength",10.0);   // GUESS
  Control.addVariable("dreamVacFFeMat","Stainless304");
  
  Control.addVariable("dreamT0DiskDXStep",0.0);
  Control.addVariable("dreamT0DiskDYStep",-2.0);
  Control.addVariable("dreamT0DiskDZStep",0.0);
  Control.addVariable("dreamT0DiskDXYangle",0.0);
  Control.addVariable("dreamT0DiskDZangle",0.0);

  Control.addVariable("dreamT0DiskDInnerRadius",28.0);
  Control.addVariable("dreamT0DiskDOuterRadius",38.0);
  Control.addVariable("dreamT0DiskDNDisk",1);

  Control.addVariable("dreamT0DiskD0Thick",5.4);  // to include B4C
  Control.addVariable("dreamT0DiskDInnerMat","Inconnel");
  Control.addVariable("dreamT0DiskDOuterMat","Tungsten");
  
  Control.addVariable("dreamT0DiskDNBlades",2);
  Control.addVariable("dreamT0DiskD0PhaseAngle0",95.0);
  Control.addVariable("dreamT0DiskD0OpenAngle0",35.0);
  Control.addVariable("dreamT0DiskD0PhaseAngle1",275.0);
  Control.addVariable("dreamT0DiskD0OpenAngle1",25.0);

  // T0 Blade chopper house
  Control.addVariable("dreamT0DiskDHouseYStep",2.6);
  Control.addVariable("dreamT0DiskDHouseZStep",2.0);
  Control.addVariable("dreamT0DiskDHouseVoidHeight",38.0);
  Control.addVariable("dreamT0DiskDHouseVoidDepth",28.0);
  Control.addVariable("dreamT0DiskDHouseVoidThick",8.0);
  Control.addVariable("dreamT0DiskDHouseVoidWidth",80.0);
  Control.addVariable("dreamT0DiskDHouseWallThick",1.0);
  Control.addVariable("dreamT0DiskDHouseWallMat","Stainless304");

  // VACUUM PIPES / T0/B and Band Choppers
  Control.addVariable("dreamPipeFRadius",8.0);
  Control.addVariable("dreamPipeFLength",300.0);
  Control.addVariable("dreamPipeFFeThick",1.0);
  Control.addVariable("dreamPipeFFlangeRadius",12.0);
  Control.addVariable("dreamPipeFFlangeLength",1.0);
  Control.addVariable("dreamPipeFFeMat","Stainless304");


  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("dreamFGXStep",0.0);       
  Control.addVariable("dreamFGYStep",0.8);       
  Control.addVariable("dreamFGZStep",0.0);       
  Control.addVariable("dreamFGXYAngle",0.0);       
  Control.addVariable("dreamFGZAngle",0.0);
  Control.addVariable("dreamFGLength",40.0);       
  
  Control.addVariable("dreamFGBeamYStep",1.0);
 
  Control.addVariable("dreamFGNShapes",1);       
  Control.addVariable("dreamFGNShapeLayers",3);
  Control.addVariable("dreamFGActiveShield",0);

  Control.addVariable("dreamFGLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFGLayerThick2",1.5);

  Control.addVariable("dreamFGLayerMat0","Void");
  Control.addVariable("dreamFGLayerMat1","Glass");
  Control.addVariable("dreamFGLayerMat2","Void");       
  
  Control.addVariable("dreamFG0TypeID","Tapper");
  Control.addVariable("dreamFG0HeightStart",4.5);
  Control.addVariable("dreamFG0HeightEnd",4.97);
  Control.addVariable("dreamFG0WidthStart",2.24);
  Control.addVariable("dreamFG0WidthEnd",3.05);
  Control.addVariable("dreamFG0Length",40.0);

  
  // --------------------
  // PIPE to Bunker Wall:
  // --------------------
  Control.addVariable("dreamPipeFinalRadius",8.0);
  Control.addVariable("dreamPipeFinalLength",200.0);
  Control.addVariable("dreamPipeFinalFeThick",1.0);
  Control.addVariable("dreamPipeFinalFlangeRadius",12.0);
  Control.addVariable("dreamPipeFinalFlangeLength",1.0);
  Control.addVariable("dreamPipeFinalFeMat","Stainless304");

  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("dreamFFinalXStep",0.0);       
  Control.addVariable("dreamFFinalYStep",0.8);       
  Control.addVariable("dreamFFinalZStep",0.0);       
  Control.addVariable("dreamFFinalXYAngle",0.0);       
  Control.addVariable("dreamFFinalZAngle",0.0);
  Control.addVariable("dreamFFinalLength",440.0);       
  
  Control.addVariable("dreamFFinalBeamYStep",1.0);
 
  Control.addVariable("dreamFFinalNShapes",1);       
  Control.addVariable("dreamFFinalNShapeLayers",3);
  Control.addVariable("dreamFFinalActiveShield",0);

  Control.addVariable("dreamFFinalLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFFinalLayerThick2",1.5);

  Control.addVariable("dreamFFinalLayerMat0","Void");
  Control.addVariable("dreamFFinalLayerMat1","Glass");
  Control.addVariable("dreamFFinalLayerMat2","Void");       
  
  Control.addVariable("dreamFFinal0TypeID","Tapper");
  Control.addVariable("dreamFFinal0HeightStart",4.5);
  Control.addVariable("dreamFFinal0HeightEnd",4.97);
  Control.addVariable("dreamFFinal0WidthStart",2.24);
  Control.addVariable("dreamFFinal0WidthEnd",3.05);
  Control.addVariable("dreamFFinal0Length",440.0);

  // BEAM INSERT:
  Control.addVariable("dreamBInsertHeight",20.0);
  Control.addVariable("dreamBInsertWidth",28.0);
  Control.addVariable("dreamBInsertTopWall",1.0);
  Control.addVariable("dreamBInsertLowWall",1.0);
  Control.addVariable("dreamBInsertLeftWall",1.0);
  Control.addVariable("dreamBInsertRightWall",1.0);
  Control.addVariable("dreamBInsertWallMat","Stainless304");       

  // Guide in wall
  Control.addVariable("dreamFWallXStep",0.0);       
  Control.addVariable("dreamFWallYStep",0.8);       
  Control.addVariable("dreamFWallZStep",0.0);       
  Control.addVariable("dreamFWallXYAngle",0.0);       
  Control.addVariable("dreamFWallZAngle",0.0);
  Control.addVariable("dreamFWallLength",318.0);       
  
  Control.addVariable("dreamFWallBeamYStep",1.0);
 
  Control.addVariable("dreamFWallNShapes",1);       
  Control.addVariable("dreamFWallNShapeLayers",3);
  Control.addVariable("dreamFWallActiveShield",0);

  Control.addVariable("dreamFWallLayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFWallLayerThick2",1.5);

  Control.addVariable("dreamFWallLayerMat0","Void");
  Control.addVariable("dreamFWallLayerMat1","Glass");
  Control.addVariable("dreamFWallLayerMat2","Void");       
  
  Control.addVariable("dreamFWall0TypeID","Rectangle");
  Control.addVariable("dreamFWall0Height",6.0);
  Control.addVariable("dreamFWall0Width",6.0);
  Control.addVariable("dreamFWall0Length",318.0);

  // Guide after wall [17.5m - 3.20] for wall
  Control.addVariable("dreamFOutAXStep",0.0);       
  Control.addVariable("dreamFOutAYStep",0.8);       
  Control.addVariable("dreamFOutAZStep",0.0);       
  Control.addVariable("dreamFOutAXYAngle",0.0);       
  Control.addVariable("dreamFOutAZAngle",0.0);
  Control.addVariable("dreamFOutALength",1750.0-320.0);       
  
  Control.addVariable("dreamFOutABeamYStep",1.0);
 
  Control.addVariable("dreamFOutANShapes",1);       
  Control.addVariable("dreamFOutANShapeLayers",3);
  Control.addVariable("dreamFOutAActiveShield",0);

  Control.addVariable("dreamFOutALayerThick1",0.4);  // glass thick
  Control.addVariable("dreamFOutALayerThick2",1.5);

  Control.addVariable("dreamFOutALayerMat0","Void");
  Control.addVariable("dreamFOutALayerMat1","Glass");
  Control.addVariable("dreamFOutALayerMat2","Void");       
  
  Control.addVariable("dreamFOutA0TypeID","Tapper");
  Control.addVariable("dreamFOutA0HeightStart",4.5);
  Control.addVariable("dreamFOutA0HeightEnd",4.97);
  Control.addVariable("dreamFOutA0WidthStart",2.24);
  Control.addVariable("dreamFOutA0WidthEnd",3.05);
  Control.addVariable("dreamFOutA0Length",1750.0-320.0);

  Control.addVariable("dreamPipeOutARadius",8.0);
  Control.addVariable("dreamPipeOutALength",1750.0-320);
  Control.addVariable("dreamPipeOutAFeThick",1.0);
  Control.addVariable("dreamPipeOutAFlangeRadius",12.0);
  Control.addVariable("dreamPipeOutAFlangeLength",1.0);
  Control.addVariable("dreamPipeOutAFeMat","Stainless304");


  Control.addVariable("dreamShieldALength",1750.0-320);
  Control.addVariable("dreamShieldALeft",32.0);
  Control.addVariable("dreamShieldARight",32.0);
  Control.addVariable("dreamShieldAHeight",32.0);
  Control.addVariable("dreamShieldADepth",32.0);
  Control.addVariable("dreamShieldADefMat","Stainless304");
  Control.addVariable("dreamShieldANSeg",8);
  Control.addVariable("dreamShieldANWallLayers",8);
  Control.addVariable("dreamShieldANFloorLayers",3);
  Control.addVariable("dreamShieldANRoofLayers",8);
  Control.addVariable("dreamShieldAWallLen1",8.0);
  Control.addVariable("dreamShieldAWallMat1","CastIron");
  Control.addVariable("dreamShieldAWallMat5","Concrete");
  return;
}
 
}  // NAMESPACE setVariable
