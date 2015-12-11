/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/shortODINvariables.cxx
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
#include "essVariables.h"

namespace setVariable
{

void
shortODINvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("shortODINvariables[F]","shortODINvariables");

  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("shortOdinVacAYStep",85.0);
    
  Control.addVariable("shortOdinVacAVoidHeight",20.0);
  Control.addVariable("shortOdinVacAVoidDepth",19.0);
  Control.addVariable("shortOdinVacAVoidWidth",87.0);
  Control.addVariable("shortOdinVacAVoidLength",25.7);
  
  Control.addVariable("shortOdinVacAFeHeight",0.5);
  Control.addVariable("shortOdinVacAFeDepth",0.5);
  Control.addVariable("shortOdinVacAFeWidth",0.5);
  Control.addVariable("shortOdinVacAFeFront",0.5);
  Control.addVariable("shortOdinVacAFeBack",0.5);
  Control.addVariable("shortOdinVacAFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortOdinVacAFlangeWall",1.0);      // GUESS
  Control.addVariable("shortOdinVacAFlangeLength",10.0);   // GUESS
  Control.addVariable("shortOdinVacAFeMat","Stainless304");
  Control.addVariable("shortOdinVacAVoidMat","Void");

  // VACUUM PIPES:
  Control.addVariable("shortOdinPipeARadius",8.0);
  Control.addVariable("shortOdinPipeALength",300.0);
  Control.addVariable("shortOdinPipeAFeThick",1.0);
  Control.addVariable("shortOdinPipeAFlangeRadius",12.0);
  Control.addVariable("shortOdinPipeAFlangeLength",1.0);
  Control.addVariable("shortOdinPipeAFeMat","Stainless304");

  // 5.5m to the first chopper:
  Control.addVariable("shortOdinFBXStep",0.0);       
  Control.addVariable("shortOdinFBYStep",0.2);       
  Control.addVariable("shortOdinFBZStep",0.0);       
  Control.addVariable("shortOdinFBXYAngle",0.0);       
  Control.addVariable("shortOdinFBZAngle",0.0);
  Control.addVariable("shortOdinFBLength",50.0);       
  
  Control.addVariable("shortOdinFBBeamYStep",1.0);
 
  Control.addVariable("shortOdinFBNShapes",1);       
  Control.addVariable("shortOdinFBNShapeLayers",3);
  Control.addVariable("shortOdinFBActiveShield",0);

  Control.addVariable("shortOdinFBLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinFBLayerThick2",1.5);

  Control.addVariable("shortOdinFBLayerMat0","Void");
  Control.addVariable("shortOdinFBLayerMat1","Aluminium");
  Control.addVariable("shortOdinFBLayerMat2","Void");       
  
  Control.addVariable("shortOdinFB0TypeID","Tapper");
  Control.addVariable("shortOdinFB0HeightStart",4.5);
  Control.addVariable("shortOdinFB0HeightEnd",4.6);
  Control.addVariable("shortOdinFB0WidthStart",1.88);
  Control.addVariable("shortOdinFB0WidthEnd",2.06);
  Control.addVariable("shortOdinFB0Length",50.0);


  // Double Blade chopper
  Control.addVariable("shortOdinT0HouseYStep",20.0);
  Control.addVariable("shortOdinT0HouseVoidHeight",38.0);
  Control.addVariable("shortOdinT0HouseVoidDepth",15.0);
  Control.addVariable("shortOdinT0HouseVoidThick",65.0);
  Control.addVariable("shortOdinT0HouseVoidWidth",80.0);
  Control.addVariable("shortOdinT0HouseWallThick",1.0);
  Control.addVariable("shortOdinT0HouseWallMat","Stainless304");

  // Quad Blade chopper
  Control.addVariable("shortOdinBladeXStep",0.0);
  Control.addVariable("shortOdinBladeYStep",52.0);
  Control.addVariable("shortOdinBladeZStep",0.0);
  Control.addVariable("shortOdinBladeXYangle",0.0);
  Control.addVariable("shortOdinBladeZangle",0.0);

  Control.addVariable("shortOdinBladeGap",3.0);
  Control.addVariable("shortOdinBladeInnerRadius",10.0);
  Control.addVariable("shortOdinBladeOuterRadius",22.50);
  Control.addVariable("shortOdinBladeNDisk",4);

  Control.addVariable("shortOdinBlade0Thick",1.0);
  Control.addVariable("shortOdinBlade1Thick",1.0);
  Control.addVariable("shortOdinBlade2Thick",1.0);
  Control.addVariable("shortOdinBlade3Thick",1.0);
  Control.addVariable("shortOdinBladeInnerMat","Inconnel");
  Control.addVariable("shortOdinBladeOuterMat","Aluminium");
  
  Control.addVariable("shortOdinBladeNBlades",2);
  Control.addVariable("shortOdinBlade0PhaseAngle0",95.0);
  Control.addVariable("shortOdinBlade0OpenAngle0",30.0);
  Control.addVariable("shortOdinBlade1PhaseAngle0",95.0);
  Control.addVariable("shortOdinBlade1OpenAngle0",30.0);
  Control.addVariable("shortOdinBlade2PhaseAngle0",95.0);
  Control.addVariable("shortOdinBlade2OpenAngle0",30.0);
  Control.addVariable("shortOdinBlade3PhaseAngle0",95.0);
  Control.addVariable("shortOdinBlade3OpenAngle0",30.0);

  Control.addVariable("shortOdinBlade0PhaseAngle1",275.0);
  Control.addVariable("shortOdinBlade0OpenAngle1",30.0);
  Control.addVariable("shortOdinBlade1PhaseAngle1",275.0);
  Control.addVariable("shortOdinBlade1OpenAngle1",30.0);
  Control.addVariable("shortOdinBlade2PhaseAngle1",275.0);
  Control.addVariable("shortOdinBlade2OpenAngle1",30.0);
  Control.addVariable("shortOdinBlade3PhaseAngle1",275.0);
  Control.addVariable("shortOdinBlade3OpenAngle1",30.0);


  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  Control.addVariable("shortOdinVacBYStep",85.0);
    
  Control.addVariable("shortOdinVacBVoidHeight",20.0);
  Control.addVariable("shortOdinVacBVoidDepth",19.0);
  Control.addVariable("shortOdinVacBVoidWidth",87.0);
  Control.addVariable("shortOdinVacBVoidLength",85.7);
  
  Control.addVariable("shortOdinVacBFeHeight",0.5);
  Control.addVariable("shortOdinVacBFeDepth",0.5);
  Control.addVariable("shortOdinVacBFeWidth",0.5);
  Control.addVariable("shortOdinVacBFeFront",0.5);
  Control.addVariable("shortOdinVacBFeBack",0.5);
  Control.addVariable("shortOdinVacBFlangeRadius",8.0);    // GUESS
  Control.addVariable("shortOdinVacBFlangeWall",1.0);      // GUESS
  Control.addVariable("shortOdinVacBFlangeLength",10.0);   // GUESS
  Control.addVariable("shortOdinVacBFeMat","Stainless304");
  Control.addVariable("shortOdinVacBVoidMat","Void");

  // VACUUM PIPES:
  Control.addVariable("shortOdinPipeBRadius",8.0);
  Control.addVariable("shortOdinPipeBLength",300.0);
  Control.addVariable("shortOdinPipeBFeThick",1.0);
  Control.addVariable("shortOdinPipeBFlangeRadius",12.0);
  Control.addVariable("shortOdinPipeBFlangeLength",1.0);
  Control.addVariable("shortOdinPipeBFeMat","Stainless304");

  //First  chopper to T0
  Control.addVariable("shortOdinFCXStep",0.0);       
  Control.addVariable("shortOdinFCYStep",0.2);       
  Control.addVariable("shortOdinFCZStep",0.0);       
  Control.addVariable("shortOdinFCXYAngle",0.0);       
  Control.addVariable("shortOdinFCZAngle",0.0);
  Control.addVariable("shortOdinFCLength",105.0);       
  
  Control.addVariable("shortOdinFCBeamYStep",1.0);
 
  Control.addVariable("shortOdinFCNShapes",1);       
  Control.addVariable("shortOdinFCNShapeLayers",3);
  Control.addVariable("shortOdinFCActiveShield",0);

  Control.addVariable("shortOdinFCLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinFCLayerThick2",1.5);

  Control.addVariable("shortOdinFCLayerMat0","Void");
  Control.addVariable("shortOdinFCLayerMat1","Aluminium");
  Control.addVariable("shortOdinFCLayerMat2","Void");       
  
  Control.addVariable("shortOdinFC0TypeID","Tapper");
  Control.addVariable("shortOdinFC0HeightStart",4.5);
  Control.addVariable("shortOdinFC0HeightEnd",4.6);
  Control.addVariable("shortOdinFC0WidthStart",1.88);
  Control.addVariable("shortOdinFC0WidthEnd",2.06);
  Control.addParse<double>("shortOdinFC0Length","shortOdinFCLength");


  // --------------------
  // PIPE to Bunker Wall:
  // --------------------
  Control.addVariable("shortOdinPipeFinalRadius",8.0);
  Control.addVariable("shortOdinPipeFinalLength",200.0);
  Control.addVariable("shortOdinPipeFinalFeThick",1.0);
  Control.addVariable("shortOdinPipeFinalFlangeRadius",12.0);
  Control.addVariable("shortOdinPipeFinalFlangeLength",1.0);
  Control.addVariable("shortOdinPipeFinalFeMat","Stainless304");

  // 6.6m to 8.5m between T0 and Band chopper
  Control.addVariable("shortOdinFFinalXStep",0.0);       
  Control.addVariable("shortOdinFFinalYStep",0.8);       
  Control.addVariable("shortOdinFFinalZStep",0.0);       
  Control.addVariable("shortOdinFFinalXYAngle",0.0);       
  Control.addVariable("shortOdinFFinalZAngle",0.0);
  Control.addVariable("shortOdinFFinalLength",440.0);       
  
  Control.addVariable("shortOdinFFinalBeamYStep",1.0);
 
  Control.addVariable("shortOdinFFinalNShapes",1);       
  Control.addVariable("shortOdinFFinalNShapeLayers",3);
  Control.addVariable("shortOdinFFinalActiveShield",0);

  Control.addVariable("shortOdinFFinalLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinFFinalLayerThick2",1.5);

  Control.addVariable("shortOdinFFinalLayerMat0","Void");
  Control.addVariable("shortOdinFFinalLayerMat1","Glass");
  Control.addVariable("shortOdinFFinalLayerMat2","Void");       
  
  Control.addVariable("shortOdinFFinal0TypeID","Tapper");
  Control.addVariable("shortOdinFFinal0HeightStart",4.5);
  Control.addVariable("shortOdinFFinal0HeightEnd",4.97);
  Control.addVariable("shortOdinFFinal0WidthStart",2.24);
  Control.addVariable("shortOdinFFinal0WidthEnd",3.05);
  Control.addParse<double>("shortOdinFFinal0Length","shortOdinFFinalLength");



  

  
  
  Control.addVariable("shortOdinGAXStep",0.0);       
  Control.addVariable("shortOdinGAYStep",0.0);       
  Control.addVariable("shortOdinGAZStep",0.0);       
  Control.addVariable("shortOdinGAXYAngle",0.0);       
  Control.addVariable("shortOdinGAZAngle",0.0);       
  Control.addVariable("shortOdinGALength",220.0);       
  Control.addVariable("shortOdinGAHeight",10.0);       
  Control.addVariable("shortOdinGADepth",30.0);       
  Control.addVariable("shortOdinGALeftWidth",20.0);       
  Control.addVariable("shortOdinGARightWidth",20.0);       
  Control.addVariable("shortOdinGAFeMat","Void");       
  Control.addVariable("shortOdinGANShapes",1);       
  Control.addVariable("shortOdinGANShapeLayers",3);

  Control.addVariable("shortOdinGALayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGALayerThick2",1.5);

  Control.addVariable("shortOdinGALayerMat0","Void");
  Control.addVariable("shortOdinGALayerMat1","Glass");
  Control.addVariable("shortOdinGALayerMat2","Void");       

  Control.addVariable("shortOdinGA0TypeID","Tapper");
  Control.addVariable("shortOdinGA0HeightStart",3.0);
  Control.addVariable("shortOdinGA0HeightEnd",3.0);
  Control.addVariable("shortOdinGA0WidthStart",7.0);
  Control.addVariable("shortOdinGA0WidthEnd",10.0);
  Control.addVariable("shortOdinGA0Length",220.0);
  Control.addVariable("shortOdinGA0ZAngle",0.0);       

  // T0 Chopper 
  Control.addVariable("shortOdinTZeroXStep",0.0);
  Control.addVariable("shortOdinTZeroYStep",-20.0);
  Control.addVariable("shortOdinTZeroZStep",0.0);
  Control.addVariable("shortOdinTZeroXYangle",0.0);
  Control.addVariable("shortOdinTZeroZangle",0.0);

  Control.addVariable("shortOdinTZeroGap",3.0);
  Control.addVariable("shortOdinTZeroInnerRadius",10.0);
  Control.addVariable("shortOdinTZeroOuterRadius",25.0);
  Control.addVariable("shortOdinTZeroNDisk",1);

  Control.addVariable("shortOdinTZero0Thick",40.0);
  Control.addVariable("shortOdinTZeroInnerMat","Aluminium");
  Control.addVariable("shortOdinTZeroOuterMat","Inconnel");

  Control.addVariable("shortOdinTZeroNBlades",2);
  Control.addVariable("shortOdinTZero0PhaseAngle0",-20.0);
  Control.addVariable("shortOdinTZero0OpenAngle0",160.0);

  Control.addVariable("shortOdinTZero0PhaseAngle1",160.0);
  Control.addVariable("shortOdinTZero0OpenAngle1",160.0);

  // SECOND GUIDE SEGMENT
  
  Control.addVariable("shortOdinGBXStep",0.0);       
  Control.addVariable("shortOdinGBYStep",0.0);       
  Control.addVariable("shortOdinGBZStep",0.0);       
  Control.addVariable("shortOdinGBXYAngle",0.0);       
  Control.addVariable("shortOdinGBZAngle",0.0);       
  Control.addVariable("shortOdinGBLength",475.0);       
  Control.addVariable("shortOdinGBHeight",50.0);       
  Control.addVariable("shortOdinGBDepth",50.0);       
  Control.addVariable("shortOdinGBLeftWidth",20.0);       
  Control.addVariable("shortOdinGBRightWidth",20.0);       
  Control.addVariable("shortOdinGBFeMat","Void");       
  Control.addVariable("shortOdinGBNShapes",1);       
  Control.addVariable("shortOdinGBNShapeLayers",3);

  Control.addVariable("shortOdinGBLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGBLayerThick2",1.5);

  Control.addVariable("shortOdinGBLayerMat0","Void");
  Control.addVariable("shortOdinGBLayerMat1","Glass");
  Control.addVariable("shortOdinGBLayerMat2","Void");       

  Control.addVariable("shortOdinGB0TypeID","Tapper");
  Control.addVariable("shortOdinGB0HeightStart",3.0);
  Control.addVariable("shortOdinGB0HeightEnd",3.0);
  Control.addVariable("shortOdinGB0WidthStart",7.0);
  Control.addVariable("shortOdinGB0WidthEnd",10.0);
  Control.addVariable("shortOdinGB0Length",475.0);
  Control.addVariable("shortOdinGB0ZAngle",0.0);

  // BEAM INSERT:
  Control.addVariable("shortOdinBInsertHeight",20.0);
  Control.addVariable("shortOdinBInsertWidth",28.0);
  Control.addVariable("shortOdinBInsertTopWall",1.0);
  Control.addVariable("shortOdinBInsertLowWall",1.0);
  Control.addVariable("shortOdinBInsertLeftWall",1.0);
  Control.addVariable("shortOdinBInsertRightWall",1.0);
  Control.addVariable("shortOdinBInsertWallMat","Stainless304");       

  // GUDE IN the beam insert
  Control.addVariable("shortOdinGCXStep",0.0);       
  Control.addVariable("shortOdinGCYStep",0.0);       
  Control.addVariable("shortOdinGCZStep",0.0);       
  Control.addVariable("shortOdinGCXYAngle",0.0);       
  Control.addVariable("shortOdinGCZAngle",0.0);       
  Control.addVariable("shortOdinGCLength",220.0);       
  Control.addVariable("shortOdinGCHeight",5.0);       
  Control.addVariable("shortOdinGCDepth",5.0);       
  Control.addVariable("shortOdinGCLeftWidth",7.0);       
  Control.addVariable("shortOdinGCRightWidth",7.0);       
  Control.addVariable("shortOdinGCFeMat","Void");       
  Control.addVariable("shortOdinGCNShapes",1);       
  Control.addVariable("shortOdinGCNShapeLayers",3);
  Control.addVariable("shortOdinGCActiveShield",0);

  Control.addVariable("shortOdinGCLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGCLayerThick2",1.5);

  Control.addVariable("shortOdinGCLayerMat0","Void");
  Control.addVariable("shortOdinGCLayerMat1","Glass");
  Control.addVariable("shortOdinGCLayerMat2","Void");       

  Control.addVariable("shortOdinGC0TypeID","Rectangle");
  Control.addVariable("shortOdinGC0Height",5.0);
  Control.addVariable("shortOdinGC0Width",9.0);
  Control.addVariable("shortOdinGC0Length",220.0);
  Control.addVariable("shortOdinGC0ZAngle",0.0);


  // GUIDE  Bunker wall to PIT A
  Control.addVariable("shortOdinGDXStep",0.0);       
  Control.addVariable("shortOdinGDYStep",0.0);       
  Control.addVariable("shortOdinGDZStep",0.0);       
  Control.addVariable("shortOdinGDXYAngle",0.0);       
  Control.addVariable("shortOdinGDZAngle",0.0);       
  Control.addVariable("shortOdinGDLength",141.0);       
  Control.addVariable("shortOdinGDHeight",110.0);       
  Control.addVariable("shortOdinGDDepth",110.0);       
  Control.addVariable("shortOdinGDLeftWidth",90.0);       
  Control.addVariable("shortOdinGDRightWidth",90.0);       
  Control.addVariable("shortOdinGDFeMat","Concrete");       
  Control.addVariable("shortOdinGDNShapes",1);       
  Control.addVariable("shortOdinGDNShapeLayers",4);

  Control.addVariable("shortOdinGDLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGDLayerThick2",1.5);
  Control.addVariable("shortOdinGDLayerThick3",50.0);

  Control.addVariable("shortOdinGDLayerMat0","Void");
  Control.addVariable("shortOdinGDLayerMat1","Glass");
  Control.addVariable("shortOdinGDLayerMat2","Void");
  Control.addVariable("shortOdinGDLayerMat3","Stainless304");       

  Control.addVariable("shortOdinGD0TypeID","Rectangle");
  Control.addVariable("shortOdinGD0Height",5.0);
  Control.addVariable("shortOdinGD0Width",9.0);
  Control.addVariable("shortOdinGD0Length",141.0);
  Control.addVariable("shortOdinGD0ZAngle",0.0);

 
  // PIT A : At 3520 pixels (13.08m) [1.08m from Bunker wall]
  // Centre position is GuideD Length + Radius [12m] + void Depth/2 + front
  Control.addVariable("shortOdinPitAVoidHeight",167.0);
  Control.addVariable("shortOdinPitAVoidDepth",36.0);
  Control.addVariable("shortOdinPitAVoidWidth",246.0);
  Control.addVariable("shortOdinPitAVoidLength",105.0);
  
  Control.addVariable("shortOdinPitAFeHeight",70.0);
  Control.addVariable("shortOdinPitAFeDepth",60.0);
  Control.addVariable("shortOdinPitAFeWidth",60.0);
  Control.addVariable("shortOdinPitAFeFront",45.0);
  Control.addVariable("shortOdinPitAFeBack",70.0);
  Control.addVariable("shortOdinPitAFeMat","Stainless304");

  
  Control.addVariable("shortOdinPitAConcHeight",50.0);
  Control.addVariable("shortOdinPitAConcDepth",50.0);
  Control.addVariable("shortOdinPitAConcWidth",50.0);
  Control.addVariable("shortOdinPitAConcFront",50.0);
  Control.addVariable("shortOdinPitAConcBack",50.0);
  Control.addVariable("shortOdinPitAConcMat","Concrete");

  Control.addVariable("shortOdinPitAColletHeight",15.0);
  Control.addVariable("shortOdinPitAColletDepth",15.0);
  Control.addVariable("shortOdinPitAColletWidth",40.0);
  Control.addVariable("shortOdinPitAColletLength",5.0);
  Control.addVariable("shortOdinPitAColletMat","Tungsten");
  
  // Guide from Wall to PitA
  Control.addVariable("shortOdinGPitAFrontXStep",0.0);       
  Control.addVariable("shortOdinGPitAFrontYStep",0.0);       
  Control.addVariable("shortOdinGPitAFrontZStep",0.0);       
  Control.addVariable("shortOdinGPitAFrontXYAngle",0.0);       
  Control.addVariable("shortOdinGPitAFrontZAngle",0.0);       
  Control.addVariable("shortOdinGPitAFrontLength",220.0);       
  Control.addVariable("shortOdinGPitAFrontHeight",5.0);       
  Control.addVariable("shortOdinGPitAFrontDepth",5.0);       
  Control.addVariable("shortOdinGPitAFrontLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitAFrontRightWidth",7.0);       
  Control.addVariable("shortOdinGPitAFrontFeMat","Void");       
  Control.addVariable("shortOdinGPitAFrontNShapes",1);       
  Control.addVariable("shortOdinGPitAFrontNShapeLayers",3);

  Control.addVariable("shortOdinGPitAFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitAFrontLayerThick2",1.5);

  Control.addVariable("shortOdinGPitAFrontLayerMat0","Void");
  Control.addVariable("shortOdinGPitAFrontLayerMat1","Glass");
  Control.addVariable("shortOdinGPitAFrontLayerMat2","Void");       

  Control.addVariable("shortOdinGPitAFront0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitAFront0Height",5.0);
  Control.addVariable("shortOdinGPitAFront0Width",9.0);
  Control.addVariable("shortOdinGPitAFront0Length",220.0);
  Control.addVariable("shortOdinGPitAFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("shortOdinGPitABackXStep",0.0);       
  Control.addVariable("shortOdinGPitABackYStep",0.0);       
  Control.addVariable("shortOdinGPitABackZStep",0.0);       
  Control.addVariable("shortOdinGPitABackXYAngle",0.0);       
  Control.addVariable("shortOdinGPitABackZAngle",0.0);       
  Control.addVariable("shortOdinGPitABackLength",220.0);       
  Control.addVariable("shortOdinGPitABackHeight",5.0);       
  Control.addVariable("shortOdinGPitABackDepth",5.0);       
  Control.addVariable("shortOdinGPitABackLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitABackRightWidth",7.0);       
  Control.addVariable("shortOdinGPitABackFeMat","Void");       
  Control.addVariable("shortOdinGPitABackNShapes",1);       
  Control.addVariable("shortOdinGPitABackNShapeLayers",3);

  Control.addVariable("shortOdinGPitABackLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitABackLayerThick2",1.5);

  Control.addVariable("shortOdinGPitABackLayerMat0","Void");
  Control.addVariable("shortOdinGPitABackLayerMat1","Glass");
  Control.addVariable("shortOdinGPitABackLayerMat2","Void");       

  Control.addVariable("shortOdinGPitABack0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitABack0Height",5.0);
  Control.addVariable("shortOdinGPitABack0Width",9.0);
  Control.addVariable("shortOdinGPitABack0Length",220.0);
  Control.addVariable("shortOdinGPitABack0ZAngle",0.0);

  // Chopper A : Single 1.2m disk chopper [6 phases]
  Control.addVariable("shortOdinChopperAXStep",0.0);
  Control.addVariable("shortOdinChopperAYStep",0.0);
  Control.addVariable("shortOdinChopperAZStep",0.0);
  Control.addVariable("shortOdinChopperAXYangle",0.0);
  Control.addVariable("shortOdinChopperAZangle",0.0);

  Control.addVariable("shortOdinChopperAGap",3.0);
  Control.addVariable("shortOdinChopperAInnerRadius",45.0);
  Control.addVariable("shortOdinChopperAOuterRadius",63.0);
  Control.addVariable("shortOdinChopperANDisk",1);

  Control.addVariable("shortOdinChopperA0Thick",2.0);
  Control.addVariable("shortOdinChopperAInnerMat","Aluminium");
  Control.addVariable("shortOdinChopperAOuterMat","Inconnel");

  Control.addVariable("shortOdinChopperANBlades",2);
  Control.addVariable("shortOdinChopperA0PhaseAngle0",-20.0);
  Control.addVariable("shortOdinChopperA0OpenAngle0",160.0);

  Control.addVariable("shortOdinChopperA0PhaseAngle1",160.0);
  Control.addVariable("shortOdinChopperA0OpenAngle1",160.0);

  
  // GUIDE EXITING PIT
  Control.addVariable("shortOdinGEXStep",0.0);       
  Control.addVariable("shortOdinGEYStep",0.0);       
  Control.addVariable("shortOdinGEZStep",0.0);       
  Control.addVariable("shortOdinGEXYAngle",0.0);       
  Control.addVariable("shortOdinGEZAngle",0.0);       
  Control.addVariable("shortOdinGELength",787.0);       
  Control.addVariable("shortOdinGEHeight",81.0);       
  Control.addVariable("shortOdinGEDepth",81.0);       
  Control.addVariable("shortOdinGELeftWidth",81.0);       
  Control.addVariable("shortOdinGERightWidth",81.0);       
  Control.addVariable("shortOdinGEFeMat","Concrete");       
  Control.addVariable("shortOdinGENShapes",1);       
  Control.addVariable("shortOdinGENShapeLayers",4);

  Control.addVariable("shortOdinGELayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGELayerThick2",1.5);
  Control.addVariable("shortOdinGELayerThick3",36.0);

  Control.addVariable("shortOdinGELayerMat0","Void");
  Control.addVariable("shortOdinGELayerMat1","Glass");
  Control.addVariable("shortOdinGELayerMat2","Void");
  Control.addVariable("shortOdinGELayerMat3","Stainless304");       


  Control.addVariable("shortOdinGE0TypeID","Rectangle");
  Control.addVariable("shortOdinGE0Height",5.0);
  Control.addVariable("shortOdinGE0Width",9.0);
  Control.addVariable("shortOdinGE0Length",787.0);
  Control.addVariable("shortOdinGE0ZAngle",0.0);

  // 
  // PIT B ::
  // 
  Control.addVariable("shortOdinPitBVoidHeight",251.1);
  Control.addVariable("shortOdinPitBVoidDepth",36.1);
  Control.addVariable("shortOdinPitBVoidWidth",246.0);
  Control.addVariable("shortOdinPitBVoidLength",105.0);
  
  Control.addVariable("shortOdinPitBFeHeight",50.0);
  Control.addVariable("shortOdinPitBFeDepth",50.0);
  Control.addVariable("shortOdinPitBFeWidth",50.0);
  Control.addVariable("shortOdinPitBFeFront",50.0);
  Control.addVariable("shortOdinPitBFeBack",50.0);
  Control.addVariable("shortOdinPitBFeMat","Stainless304");

  Control.addVariable("shortOdinPitBConcHeight",50.0);
  Control.addVariable("shortOdinPitBConcDepth",50.0);
  Control.addVariable("shortOdinPitBConcWidth",50.0);
  Control.addVariable("shortOdinPitBConcFront",50.0);
  Control.addVariable("shortOdinPitBConcBack",50.0);
  Control.addVariable("shortOdinPitBConcMat","Concrete");

  // Chopper A : Single 1.2m disk chopper [6 phases]
  Control.addVariable("shortOdinChopperBXStep",0.0);
  Control.addVariable("shortOdinChopperBYStep",0.0);
  Control.addVariable("shortOdinChopperBZStep",0.0);
  Control.addVariable("shortOdinChopperBXYangle",0.0);
  Control.addVariable("shortOdinChopperBZangle",0.0);

  Control.addVariable("shortOdinChopperBGap",3.0);
  Control.addVariable("shortOdinChopperBInnerRadius",45.0);
  Control.addVariable("shortOdinChopperBOuterRadius",63.0);
  Control.addVariable("shortOdinChopperBNDisk",1);

  Control.addVariable("shortOdinChopperB0Thick",2.0);
  Control.addVariable("shortOdinChopperBInnerMat","Aluminium");
  Control.addVariable("shortOdinChopperBOuterMat","Inconnel");

  Control.addVariable("shortOdinChopperBNBlades",2);
  Control.addVariable("shortOdinChopperB0PhaseAngle0",-20.0);
  Control.addVariable("shortOdinChopperB0OpenAngle0",160.0);

  Control.addVariable("shortOdinChopperB0PhaseAngle1",160.0);
  Control.addVariable("shortOdinChopperB0OpenAngle1",160.0);

  // GUDE IN the beam insert
  Control.addVariable("shortOdinGPitBFrontXStep",0.0);       
  Control.addVariable("shortOdinGPitBFrontYStep",0.0);       
  Control.addVariable("shortOdinGPitBFrontZStep",0.0);       
  Control.addVariable("shortOdinGPitBFrontXYAngle",0.0);       
  Control.addVariable("shortOdinGPitBFrontZAngle",0.0);       
  Control.addVariable("shortOdinGPitBFrontLength",220.0);       
  Control.addVariable("shortOdinGPitBFrontHeight",5.0);       
  Control.addVariable("shortOdinGPitBFrontDepth",5.0);       
  Control.addVariable("shortOdinGPitBFrontLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitBFrontRightWidth",7.0);       
  Control.addVariable("shortOdinGPitBFrontFeMat","Void");       
  Control.addVariable("shortOdinGPitBFrontNShapes",1);       
  Control.addVariable("shortOdinGPitBFrontNShapeLayers",3);

  Control.addVariable("shortOdinGPitBFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitBFrontLayerThick2",1.5);

  Control.addVariable("shortOdinGPitBFrontLayerMat0","Void");
  Control.addVariable("shortOdinGPitBFrontLayerMat1","Glass");
  Control.addVariable("shortOdinGPitBFrontLayerMat2","Void");       

  Control.addVariable("shortOdinGPitBFront0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitBFront0Height",5.0);
  Control.addVariable("shortOdinGPitBFront0Width",9.0);
  Control.addVariable("shortOdinGPitBFront0Length",220.0);
  Control.addVariable("shortOdinGPitBFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("shortOdinGPitBBackXStep",0.0);       
  Control.addVariable("shortOdinGPitBBackYStep",0.0);       
  Control.addVariable("shortOdinGPitBBackZStep",0.0);       
  Control.addVariable("shortOdinGPitBBackXYAngle",0.0);       
  Control.addVariable("shortOdinGPitBBackZAngle",0.0);       
  Control.addVariable("shortOdinGPitBBackLength",220.0);       
  Control.addVariable("shortOdinGPitBBackHeight",5.0);       
  Control.addVariable("shortOdinGPitBBackDepth",5.0);       
  Control.addVariable("shortOdinGPitBBackLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitBBackRightWidth",7.0);       
  Control.addVariable("shortOdinGPitBBackFeMat","Void");       
  Control.addVariable("shortOdinGPitBBackNShapes",1);       
  Control.addVariable("shortOdinGPitBBackNShapeLayers",3);

  Control.addVariable("shortOdinGPitBBackLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitBBackLayerThick2",1.5);

  Control.addVariable("shortOdinGPitBBackLayerMat0","Void");
  Control.addVariable("shortOdinGPitBBackLayerMat1","Glass");
  Control.addVariable("shortOdinGPitBBackLayerMat2","Void");       

  Control.addVariable("shortOdinGPitBBack0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitBBack0Height",5.0);
  Control.addVariable("shortOdinGPitBBack0Width",9.0);
  Control.addVariable("shortOdinGPitBBack0Length",220.0);
  Control.addVariable("shortOdinGPitBBack0ZAngle",0.0);

  // GUIDE EXITING PIT B
  Control.addVariable("shortOdinGFXStep",0.0);       
  Control.addVariable("shortOdinGFYStep",0.0);       
  Control.addVariable("shortOdinGFZStep",0.0);       
  Control.addVariable("shortOdinGFXYAngle",0.0);       
  Control.addVariable("shortOdinGFZAngle",0.0);       
  Control.addVariable("shortOdinGFLength",220.0);       
  Control.addVariable("shortOdinGFHeight",85.0);       
  Control.addVariable("shortOdinGFDepth",85.0);       
  Control.addVariable("shortOdinGFLeftWidth",85.0);       
  Control.addVariable("shortOdinGFRightWidth",85.0);       
  Control.addVariable("shortOdinGFFeMat","Concrete");       
  Control.addVariable("shortOdinGFNShapes",1);       
  Control.addVariable("shortOdinGFNShapeLayers",4);

  Control.addVariable("shortOdinGFLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGFLayerThick2",1.5);
  Control.addVariable("shortOdinGFLayerThick3",36.0);

  Control.addVariable("shortOdinGFLayerMat0","Void");
  Control.addVariable("shortOdinGFLayerMat1","Glass");
  Control.addVariable("shortOdinGFLayerMat2","Void");
  Control.addVariable("shortOdinGFLayerMat3","Stainless304");       

  Control.addVariable("shortOdinGF0TypeID","Rectangle");
  Control.addVariable("shortOdinGF0Height",5.0);
  Control.addVariable("shortOdinGF0Width",9.0);
  Control.addVariable("shortOdinGF0Length",220.0);
  Control.addVariable("shortOdinGF0ZAngle",0.0);
  // 
  // PIT C ::
  // 
  Control.addVariable("shortOdinPitCVoidHeight",251.0);
  Control.addVariable("shortOdinPitCVoidDepth",36.0);
  Control.addVariable("shortOdinPitCVoidWidth",246.0);
  Control.addVariable("shortOdinPitCVoidLength",105.0);
  
  Control.addVariable("shortOdinPitCFeHeight",45.0);
  Control.addVariable("shortOdinPitCFeDepth",45.0);
  Control.addVariable("shortOdinPitCFeWidth",45.0);
  Control.addVariable("shortOdinPitCFeFront",45.0);
  Control.addVariable("shortOdinPitCFeBack",45.0);
  Control.addVariable("shortOdinPitCFeMat","Stainless304");

  Control.addVariable("shortOdinPitCConcHeight",50.0);
  Control.addVariable("shortOdinPitCConcDepth",50.0);
  Control.addVariable("shortOdinPitCConcWidth",50.0);
  Control.addVariable("shortOdinPitCConcFront",50.0);
  Control.addVariable("shortOdinPitCConcBack",50.0);
  Control.addVariable("shortOdinPitCConcMat","Concrete");
  
  // GUDE IN the beam insert
  Control.addVariable("shortOdinGPitCFrontXStep",0.0);       
  Control.addVariable("shortOdinGPitCFrontYStep",0.0);       
  Control.addVariable("shortOdinGPitCFrontZStep",0.0);       
  Control.addVariable("shortOdinGPitCFrontXYAngle",0.0);       
  Control.addVariable("shortOdinGPitCFrontZAngle",0.0);       
  Control.addVariable("shortOdinGPitCFrontLength",220.0);       
  Control.addVariable("shortOdinGPitCFrontHeight",5.0);       
  Control.addVariable("shortOdinGPitCFrontDepth",5.0);       
  Control.addVariable("shortOdinGPitCFrontLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitCFrontRightWidth",7.0);       
  Control.addVariable("shortOdinGPitCFrontFeMat","Void");       
  Control.addVariable("shortOdinGPitCFrontNShapes",1);       
  Control.addVariable("shortOdinGPitCFrontNShapeLayers",3);

  Control.addVariable("shortOdinGPitCFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitCFrontLayerThick2",1.5);

  Control.addVariable("shortOdinGPitCFrontLayerMat0","Void");
  Control.addVariable("shortOdinGPitCFrontLayerMat1","Glass");
  Control.addVariable("shortOdinGPitCFrontLayerMat2","Void");       

  Control.addVariable("shortOdinGPitCFront0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitCFront0Height",5.0);
  Control.addVariable("shortOdinGPitCFront0Width",9.0);
  Control.addVariable("shortOdinGPitCFront0Length",220.0);
  Control.addVariable("shortOdinGPitCFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("shortOdinGPitCBackXStep",0.0);       
  Control.addVariable("shortOdinGPitCBackYStep",0.0);       
  Control.addVariable("shortOdinGPitCBackZStep",0.0);       
  Control.addVariable("shortOdinGPitCBackXYAngle",0.0);       
  Control.addVariable("shortOdinGPitCBackZAngle",0.0);       
  Control.addVariable("shortOdinGPitCBackLength",220.0);       
  Control.addVariable("shortOdinGPitCBackHeight",5.0);       
  Control.addVariable("shortOdinGPitCBackDepth",5.0);       
  Control.addVariable("shortOdinGPitCBackLeftWidth",7.0);       
  Control.addVariable("shortOdinGPitCBackRightWidth",7.0);       
  Control.addVariable("shortOdinGPitCBackFeMat","Void");       
  Control.addVariable("shortOdinGPitCBackNShapes",1);       
  Control.addVariable("shortOdinGPitCBackNShapeLayers",3);

  Control.addVariable("shortOdinGPitCBackLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGPitCBackLayerThick2",1.5);

  Control.addVariable("shortOdinGPitCBackLayerMat0","Void");
  Control.addVariable("shortOdinGPitCBackLayerMat1","Glass");
  Control.addVariable("shortOdinGPitCBackLayerMat2","Void");       

  Control.addVariable("shortOdinGPitCBack0TypeID","Rectangle");
  Control.addVariable("shortOdinGPitCBack0Height",5.0);
  Control.addVariable("shortOdinGPitCBack0Width",9.0);
  Control.addVariable("shortOdinGPitCBack0Length",220.0);
  Control.addVariable("shortOdinGPitCBack0ZAngle",0.0);

  // GUIDE EXITING PIT C
  Control.addVariable("shortOdinGGXStep",0.0);       
  Control.addVariable("shortOdinGGYStep",0.0);       
  Control.addVariable("shortOdinGGZStep",0.0);       
  Control.addVariable("shortOdinGGXYAngle",0.0);       
  Control.addVariable("shortOdinGGZAngle",0.0);       
  Control.addVariable("shortOdinGGLength",1530.0);       
  Control.addVariable("shortOdinGGHeight",90.0);       
  Control.addVariable("shortOdinGGDepth",90.0);       
  Control.addVariable("shortOdinGGLeftWidth",90.0);       
  Control.addVariable("shortOdinGGRightWidth",90.0);       
  Control.addVariable("shortOdinGGFeMat","Concrete");       
  Control.addVariable("shortOdinGGNShapes",1);       
  Control.addVariable("shortOdinGGNShapeLayers",4);

  Control.addVariable("shortOdinGGLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGGLayerThick2",1.5);
  Control.addVariable("shortOdinGGLayerThick3",30.0);

  Control.addVariable("shortOdinGGLayerMat0","Void");
  Control.addVariable("shortOdinGGLayerMat1","Glass");
  Control.addVariable("shortOdinGGLayerMat2","Void");
  Control.addVariable("shortOdinGGLayerMat3","Stainless304");       

  Control.addVariable("shortOdinGG0TypeID","Rectangle");
  Control.addVariable("shortOdinGG0Height",5.0);
  Control.addVariable("shortOdinGG0Width",9.0);
  Control.addVariable("shortOdinGG0Length",1530.0);
  Control.addVariable("shortOdinGG0ZAngle",0.0);

  // HUT:
  Control.addVariable("shortOdinCaveVoidHeight",300.0);
  Control.addVariable("shortOdinCaveVoidDepth",183.0);
  Control.addVariable("shortOdinCaveVoidWidth",600.0);
  Control.addVariable("shortOdinCaveVoidLength",1686.0);
  Control.addVariable("shortOdinCaveVoidNoseWidth",293.0);
  Control.addVariable("shortOdinCaveVoidNoseLen",183.0);

  Control.addVariable("shortOdinCaveFeLeftWall",15.0);
  Control.addVariable("shortOdinCaveFeRightWall",15.0);
  Control.addVariable("shortOdinCaveFeRoof",15.0);
  Control.addVariable("shortOdinCaveFeFloor",15.0);
  Control.addVariable("shortOdinCaveFeNoseFront",25.0);
  Control.addVariable("shortOdinCaveFeNoseSide",25.0);
  Control.addVariable("shortOdinCaveFeBack",15.0);

  Control.addVariable("shortOdinCaveConcLeftWall",35.0);
  Control.addVariable("shortOdinCaveConcRightWall",35.0);
  Control.addVariable("shortOdinCaveConcRoof",35.0);
  Control.addVariable("shortOdinCaveConcFloor",50.0);
  Control.addVariable("shortOdinCaveConcNoseFront",35.0);
  Control.addVariable("shortOdinCaveConcNoseSide",35.0);
  Control.addVariable("shortOdinCaveConcBack",35.0);

  Control.addVariable("shortOdinCaveFeMat","Stainless304");
  Control.addVariable("shortOdinCaveConcMat","Concrete");
   
  // WALL for Hut
  Control.addVariable("shortOdinCaveWallYStep",6.0);
  Control.addVariable("shortOdinCaveWallThick",45.0);
  Control.addVariable("shortOdinCaveWallXGap",6.0);
  Control.addVariable("shortOdinCaveWallZGap",6.0);
  Control.addVariable("shortOdinCaveWallMat","Stainless304");

  // GUIDE in Hutch nose cone
  Control.addVariable("shortOdinGHXStep",0.0);       
  Control.addVariable("shortOdinGHYStep",0.0);       
  Control.addVariable("shortOdinGHZStep",0.0);       
  Control.addVariable("shortOdinGHXYAngle",0.0);       
  Control.addVariable("shortOdinGHZAngle",0.0);       
  Control.addVariable("shortOdinGHLength",150.0);       
  Control.addVariable("shortOdinGHHeight",9.0);       
  Control.addVariable("shortOdinGHDepth",9.0);       
  Control.addVariable("shortOdinGHLeftWidth",9.0);       
  Control.addVariable("shortOdinGHRightWidth",9.0);       
  Control.addVariable("shortOdinGHFeMat","Void");       
  Control.addVariable("shortOdinGHNShapes",1);       
  Control.addVariable("shortOdinGHNShapeLayers",3);

  Control.addVariable("shortOdinGHLayerThick1",0.4);  // glass thick
  Control.addVariable("shortOdinGHLayerThick2",1.5);

  Control.addVariable("shortOdinGHLayerMat0","Void");
  Control.addVariable("shortOdinGHLayerMat1","Glass");
  Control.addVariable("shortOdinGHLayerMat2","Void");

  Control.addVariable("shortOdinGH0TypeID","Rectangle");
  Control.addVariable("shortOdinGH0Height",5.0);
  Control.addVariable("shortOdinGH0Width",9.0);
  Control.addVariable("shortOdinGH0Length",150.0);
  Control.addVariable("shortOdinGH0ZAngle",0.0);

  // PIN COLLIMATOR SYSTEM
  Control.addVariable("shortOdinPinLength",50.0);
  Control.addVariable("shortOdinPinRadius",60.0);
  // Collimator A:
  Control.addVariable("shortOdinPinCollAYStep",1.0);
  Control.addVariable("shortOdinPinCollARotDepth",20.0);
  Control.addVariable("shortOdinPinCollARadius",30.0);
  Control.addVariable("shortOdinPinCollAThick",10.0);

  Control.addVariable("shortOdinPinCollAInnerWall",1.0);
  Control.addVariable("shortOdinPinCollAInnerWallMat","Void");

  Control.addVariable("shortOdinPinCollANLayers",0);
  Control.addVariable("shortOdinPinCollAHoleIndex",0);
  Control.addVariable("shortOdinPinCollAHoleAngOff",0.0);

  Control.addVariable("shortOdinPinCollADefMat","Tungsten");
  // collimator holes:
  Control.addVariable("shortOdinPinCollANHole",3);
  Control.addVariable("shortOdinPinCollAHole0Shape",1);
  Control.addVariable("shortOdinPinCollAHole0Radius",3.0);

  Control.addVariable("shortOdinPinCollAHole0AngleCentre",0.0);
  Control.addVariable("shortOdinPinCollAHole0AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollAHole0RadialStep",20.0);

  Control.addVariable("shortOdinPinCollAHole1Shape",3);
  Control.addVariable("shortOdinPinCollAHole1Radius",5.0);
  Control.addVariable("shortOdinPinCollAHole1AngleCentre",120.0);
  Control.addVariable("shortOdinPinCollAHole1AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollAHole1RadialStep",15.0);
  
  Control.addVariable("shortOdinPinCollAHole2Shape",1);
  Control.addVariable("shortOdinPinCollAHole2Radius",4.0);
  Control.addVariable("shortOdinPinCollAHole2AngleCentre",240.0);
  Control.addVariable("shortOdinPinCollAHole2AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollAHole2RadialStep",20.0);

  // Collimator B:
  Control.addVariable("shortOdinPinCollBYStep",40.0);
  Control.addVariable("shortOdinPinCollBRotDepth",20.0);
  Control.addVariable("shortOdinPinCollBRadius",30.0);
  Control.addVariable("shortOdinPinCollBThick",10.0);

  Control.addVariable("shortOdinPinCollBInnerWall",1.0);
  Control.addVariable("shortOdinPinCollBInnerWallMat","Void");

  Control.addVariable("shortOdinPinCollBNLayers",0);
  Control.addVariable("shortOdinPinCollBHoleIndex",0);
  Control.addVariable("shortOdinPinCollBHoleAngOff",0.0);

  Control.addVariable("shortOdinPinCollBDefMat","Tungsten");
  // collimator holes:
  Control.addVariable("shortOdinPinCollBNHole",3);
  Control.addVariable("shortOdinPinCollBHole0Shape",1);
  Control.addVariable("shortOdinPinCollBHole0Radius",3.0);

  Control.addVariable("shortOdinPinCollBHole0AngleCentre",0.0);
  Control.addVariable("shortOdinPinCollBHole0AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollBHole0RadialStep",20.0);

  Control.addVariable("shortOdinPinCollBHole1Shape",3);
  Control.addVariable("shortOdinPinCollBHole1Radius",5.0);
  Control.addVariable("shortOdinPinCollBHole1AngleCentre",120.0);
  Control.addVariable("shortOdinPinCollBHole1AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollBHole1RadialStep",15.0);
  
  Control.addVariable("shortOdinPinCollBHole2Shape",1);
  Control.addVariable("shortOdinPinCollBHole2Radius",4.0);
  Control.addVariable("shortOdinPinCollBHole2AngleCentre",240.0);
  Control.addVariable("shortOdinPinCollBHole2AngleOffset",0.0);
  Control.addVariable("shortOdinPinCollBHole2RadialStep",20.0);

  //
  // JAWS
  //
  // Jaw X
  Control.addVariable("shortOdinPinJawVertXStep",0.0);
  Control.addVariable("shortOdinPinJawVertYStep",17.0);
  Control.addVariable("shortOdinPinJawVertZStep",0.0);
  Control.addVariable("shortOdinPinJawVertXYangle",180.0);
  Control.addVariable("shortOdinPinJawVertZangle",0.0);

  Control.addVariable("shortOdinPinJawVertZOpen",1.5);
  Control.addVariable("shortOdinPinJawVertZThick",4.0);
  Control.addVariable("shortOdinPinJawVertZCross",15.0);
  Control.addVariable("shortOdinPinJawVertZLen",8.0);

  Control.addVariable("shortOdinPinJawVertGap",0.5);  
  
  Control.addVariable("shortOdinPinJawVertXOpen",1.5);
  Control.addVariable("shortOdinPinJawVertXThick",4.0);
  Control.addVariable("shortOdinPinJawVertXCross",15.0);
  Control.addVariable("shortOdinPinJawVertXLen",8.0);  

  Control.addVariable("shortOdinPinJawVertXHeight",28.0);
  Control.addVariable("shortOdinPinJawVertYHeight",9.0);
  Control.addVariable("shortOdinPinJawVertZHeight",28.0);
  Control.addVariable("shortOdinPinJawVertWallThick",2.0);

  Control.addVariable("shortOdinPinJawVertxJawMat","Tungsten");
  Control.addVariable("shortOdinPinJawVertzJawMat","Tungsten");
  Control.addVariable("shortOdinPinJawVertWallMat","Aluminium");

  // Jaw XZ
  Control.addVariable("shortOdinPinJawDiagXStep",0.0);
  Control.addVariable("shortOdinPinJawDiagYStep",32.0);
  Control.addVariable("shortOdinPinJawDiagZStep",0.0);
  Control.addVariable("shortOdinPinJawDiagXAngle",0.0);
  Control.addVariable("shortOdinPinJawDiagYAngle",45.0);
  Control.addVariable("shortOdinPinJawDiagZAngle",0.0);

  Control.addVariable("shortOdinPinJawDiagZOpen",1.5);
  Control.addVariable("shortOdinPinJawDiagZThick",4.0);
  Control.addVariable("shortOdinPinJawDiagZCross",15.0);
  Control.addVariable("shortOdinPinJawDiagZLen",8.0);

  Control.addVariable("shortOdinPinJawDiagGap",0.5);  
  
  Control.addVariable("shortOdinPinJawDiagXOpen",1.5);
  Control.addVariable("shortOdinPinJawDiagXThick",4.0);
  Control.addVariable("shortOdinPinJawDiagXCross",15.0);
  Control.addVariable("shortOdinPinJawDiagXLen",8.0);  

  Control.addVariable("shortOdinPinJawDiagXHeight",28.0);
  Control.addVariable("shortOdinPinJawDiagYHeight",9.0);
  Control.addVariable("shortOdinPinJawDiagZHeight",28.0);
  Control.addVariable("shortOdinPinJawDiagWallThick",2.0);


  Control.addVariable("shortOdinPinJawDiagxJawMat","Tungsten");
  Control.addVariable("shortOdinPinJawDiagzJawMat","Tungsten");
  Control.addVariable("shortOdinPinJawDiagWallMat","Aluminium");

  Control.addVariable("shortOdinBeamStopYStep",1200.0);
  
  Control.addVariable("shortOdinBeamStopWidth",360.0);
  Control.addVariable("shortOdinBeamStopDepth",180.0);
  Control.addVariable("shortOdinBeamStopHeight",180.0);
  Control.addVariable("shortOdinBeamStopLength",350.0);

  Control.addVariable("shortOdinBeamStopFeRadius",25.0);
  Control.addVariable("shortOdinBeamStopFeLength",72.0);
  
  Control.addVariable("shortOdinBeamStopOuterRadius",75.0);
  Control.addVariable("shortOdinBeamStopOuterFeRadius",20.0);
  Control.addVariable("shortOdinBeamStopOuterFeStep",25.0);
  Control.addVariable("shortOdinBeamStopOuterCut",50.0);

  Control.addVariable("shortOdinBeamStopInnerRadius",50.0);
  Control.addVariable("shortOdinBeamStopInnerFeRadius",20.0);
  Control.addVariable("shortOdinBeamStopInnerFeStep",35.0);
  Control.addVariable("shortOdinBeamStopInnerCut",70.0);
  
  Control.addVariable("shortOdinBeamStopFeMat","Stainless304");
  Control.addVariable("shortOdinBeamStopConcMat","Concrete");

  return;
}

}  // NAMESPACE setVariable
