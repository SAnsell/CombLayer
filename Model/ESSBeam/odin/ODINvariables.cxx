/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODINvariables.cxx
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
ODINvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("ODINvariables[F]","ODINvariables");


  // Quad Blade chopper
  Control.addVariable("odinBladeXStep",0.0);
  Control.addVariable("odinBladeYStep",2.0);
  Control.addVariable("odinBladeZStep",0.0);
  Control.addVariable("odinBladeXYangle",0.0);
  Control.addVariable("odinBladeZangle",0.0);

  Control.addVariable("odinBladeGap",3.0);
  Control.addVariable("odinBladeInnerRadius",10.0);
  Control.addVariable("odinBladeOuterRadius",22.50);
  Control.addVariable("odinBladeNDisk",4);

  Control.addVariable("odinBlade0Thick",1.0);
  Control.addVariable("odinBlade1Thick",1.0);
  Control.addVariable("odinBlade2Thick",1.0);
  Control.addVariable("odinBlade3Thick",1.0);
  Control.addVariable("odinBladeInnerMat","Inconnel");
  Control.addVariable("odinBladeOuterMat","Aluminium");
  
  Control.addVariable("odinBladeNBlades",2);
  Control.addVariable("odinBlade0PhaseAngle0",95.0);
  Control.addVariable("odinBlade0OpenAngle0",30.0);
  Control.addVariable("odinBlade1PhaseAngle0",95.0);
  Control.addVariable("odinBlade1OpenAngle0",30.0);
  Control.addVariable("odinBlade2PhaseAngle0",95.0);
  Control.addVariable("odinBlade2OpenAngle0",30.0);
  Control.addVariable("odinBlade3PhaseAngle0",95.0);
  Control.addVariable("odinBlade3OpenAngle0",30.0);

  Control.addVariable("odinBlade0PhaseAngle1",275.0);
  Control.addVariable("odinBlade0OpenAngle1",30.0);
  Control.addVariable("odinBlade1PhaseAngle1",275.0);
  Control.addVariable("odinBlade1OpenAngle1",30.0);
  Control.addVariable("odinBlade2PhaseAngle1",275.0);
  Control.addVariable("odinBlade2OpenAngle1",30.0);
  Control.addVariable("odinBlade3PhaseAngle1",275.0);
  Control.addVariable("odinBlade3OpenAngle1",30.0);

  Control.addVariable("odinGAXStep",0.0);       
  Control.addVariable("odinGAYStep",0.0);       
  Control.addVariable("odinGAZStep",0.0);       
  Control.addVariable("odinGAXYAngle",0.0);       
  Control.addVariable("odinGAZAngle",0.0);       
  Control.addVariable("odinGALength",220.0);       
  Control.addVariable("odinGAHeight",10.0);       
  Control.addVariable("odinGADepth",30.0);       
  Control.addVariable("odinGALeftWidth",20.0);       
  Control.addVariable("odinGARightWidth",20.0);       
  Control.addVariable("odinGAFeMat","Void");       
  Control.addVariable("odinGANShapes",1);       
  Control.addVariable("odinGANShapeLayers",3);

  Control.addVariable("odinGALayerThick1",0.4);  // glass thick
  Control.addVariable("odinGALayerThick2",1.5);

  Control.addVariable("odinGALayerMat0","Void");
  Control.addVariable("odinGALayerMat1","Glass");
  Control.addVariable("odinGALayerMat2","Void");       

  Control.addVariable("odinGA0TypeID","Tapper");
  Control.addVariable("odinGA0HeightStart",3.0);
  Control.addVariable("odinGA0HeightEnd",3.0);
  Control.addVariable("odinGA0WidthStart",7.0);
  Control.addVariable("odinGA0WidthEnd",10.0);
  Control.addVariable("odinGA0Length",220.0);
  Control.addVariable("odinGA0ZAngle",0.0);       

  // T0 Chopper 
  Control.addVariable("odinTZeroXStep",0.0);
  Control.addVariable("odinTZeroYStep",2.0);
  Control.addVariable("odinTZeroZStep",0.0);
  Control.addVariable("odinTZeroXYangle",0.0);
  Control.addVariable("odinTZeroZangle",0.0);

  Control.addVariable("odinTZeroGap",3.0);
  Control.addVariable("odinTZeroInnerRadius",10.0);
  Control.addVariable("odinTZeroOuterRadius",25.0);
  Control.addVariable("odinTZeroNDisk",1);

  Control.addVariable("odinTZero0Thick",40.0);
  Control.addVariable("odinTZeroInnerMat","Aluminium");
  Control.addVariable("odinTZeroOuterMat","Inconnel");

  Control.addVariable("odinTZeroNBlades",2);
  Control.addVariable("odinTZero0PhaseAngle0",-20.0);
  Control.addVariable("odinTZero0OpenAngle0",160.0);

  Control.addVariable("odinTZero0PhaseAngle1",160.0);
  Control.addVariable("odinTZero0OpenAngle1",160.0);

  // SECOND GUIDE SEGMENT
  
  Control.addVariable("odinGBXStep",0.0);       
  Control.addVariable("odinGBYStep",0.0);       
  Control.addVariable("odinGBZStep",0.0);       
  Control.addVariable("odinGBXYAngle",0.0);       
  Control.addVariable("odinGBZAngle",0.0);       
  Control.addVariable("odinGBLength",475.0);       
  Control.addVariable("odinGBHeight",50.0);       
  Control.addVariable("odinGBDepth",50.0);       
  Control.addVariable("odinGBLeftWidth",20.0);       
  Control.addVariable("odinGBRightWidth",20.0);       
  Control.addVariable("odinGBFeMat","Void");       
  Control.addVariable("odinGBNShapes",1);       
  Control.addVariable("odinGBNShapeLayers",3);

  Control.addVariable("odinGBLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGBLayerThick2",1.5);

  Control.addVariable("odinGBLayerMat0","Void");
  Control.addVariable("odinGBLayerMat1","Glass");
  Control.addVariable("odinGBLayerMat2","Void");       

  Control.addVariable("odinGB0TypeID","Tapper");
  Control.addVariable("odinGB0HeightStart",3.0);
  Control.addVariable("odinGB0HeightEnd",3.0);
  Control.addVariable("odinGB0WidthStart",7.0);
  Control.addVariable("odinGB0WidthEnd",10.0);
  Control.addVariable("odinGB0Length",475.0);
  Control.addVariable("odinGB0ZAngle",0.0);

  // BEAM INSERT:
  Control.addVariable("odinBInsertHeight",20.0);
  Control.addVariable("odinBInsertWidth",28.0);
  Control.addVariable("odinBInsertTopWall",1.0);
  Control.addVariable("odinBInsertLowWall",1.0);
  Control.addVariable("odinBInsertLeftWall",1.0);
  Control.addVariable("odinBInsertRightWall",1.0);
  Control.addVariable("odinBInsertWallMat","Stainless304");       

  // GUDE IN the beam insert
  Control.addVariable("odinGCXStep",0.0);       
  Control.addVariable("odinGCYStep",0.0);       
  Control.addVariable("odinGCZStep",0.0);       
  Control.addVariable("odinGCXYAngle",0.0);       
  Control.addVariable("odinGCZAngle",0.0);       
  Control.addVariable("odinGCLength",220.0);       
  Control.addVariable("odinGCHeight",5.0);       
  Control.addVariable("odinGCDepth",5.0);       
  Control.addVariable("odinGCLeftWidth",7.0);       
  Control.addVariable("odinGCRightWidth",7.0);       
  Control.addVariable("odinGCFeMat","Void");       
  Control.addVariable("odinGCNShapes",1);       
  Control.addVariable("odinGCNShapeLayers",3);
  Control.addVariable("odinGCActiveShield",0);

  Control.addVariable("odinGCLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGCLayerThick2",1.5);

  Control.addVariable("odinGCLayerMat0","Void");
  Control.addVariable("odinGCLayerMat1","Glass");
  Control.addVariable("odinGCLayerMat2","Void");       

  Control.addVariable("odinGC0TypeID","Rectangle");
  Control.addVariable("odinGC0Height",5.0);
  Control.addVariable("odinGC0Width",9.0);
  Control.addVariable("odinGC0Length",220.0);
  Control.addVariable("odinGC0ZAngle",0.0);


  // GUIDE  Bunker wall to PIT A
  Control.addVariable("odinGDXStep",0.0);       
  Control.addVariable("odinGDYStep",0.0);       
  Control.addVariable("odinGDZStep",0.0);       
  Control.addVariable("odinGDXYAngle",0.0);       
  Control.addVariable("odinGDZAngle",0.0);       
  Control.addVariable("odinGDLength",141.0);       
  Control.addVariable("odinGDHeight",110.0);       
  Control.addVariable("odinGDDepth",110.0);       
  Control.addVariable("odinGDLeftWidth",90.0);       
  Control.addVariable("odinGDRightWidth",90.0);       
  Control.addVariable("odinGDFeMat","Concrete");       
  Control.addVariable("odinGDNShapes",1);       
  Control.addVariable("odinGDNShapeLayers",4);

  Control.addVariable("odinGDLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGDLayerThick2",1.5);
  Control.addVariable("odinGDLayerThick3",50.0);

  Control.addVariable("odinGDLayerMat0","Void");
  Control.addVariable("odinGDLayerMat1","Glass");
  Control.addVariable("odinGDLayerMat2","Void");
  Control.addVariable("odinGDLayerMat3","Stainless304");       

  Control.addVariable("odinGD0TypeID","Rectangle");
  Control.addVariable("odinGD0Height",5.0);
  Control.addVariable("odinGD0Width",9.0);
  Control.addVariable("odinGD0Length",141.0);
  Control.addVariable("odinGD0ZAngle",0.0);

 
  // PIT A : At 3520 pixels (13.08m) [1.08m from Bunker wall]
  // Centre position is GuideD Length + Radius [12m] + void Depth/2 + front
  Control.addVariable("odinPitAVoidHeight",167.0);
  Control.addVariable("odinPitAVoidDepth",36.0);
  Control.addVariable("odinPitAVoidWidth",246.0);
  Control.addVariable("odinPitAVoidLength",105.0);
  
  Control.addVariable("odinPitAFeHeight",70.0);
  Control.addVariable("odinPitAFeDepth",60.0);
  Control.addVariable("odinPitAFeWidth",60.0);
  Control.addVariable("odinPitAFeFront",45.0);
  Control.addVariable("odinPitAFeBack",70.0);
  Control.addVariable("odinPitAFeMat","Stainless304");

  
  Control.addVariable("odinPitAConcHeight",50.0);
  Control.addVariable("odinPitAConcDepth",50.0);
  Control.addVariable("odinPitAConcWidth",50.0);
  Control.addVariable("odinPitAConcFront",50.0);
  Control.addVariable("odinPitAConcBack",50.0);
  Control.addVariable("odinPitAConcMat","Concrete");

  Control.addVariable("odinPitAColletHeight",15.0);
  Control.addVariable("odinPitAColletDepth",15.0);
  Control.addVariable("odinPitAColletWidth",40.0);
  Control.addVariable("odinPitAColletLength",5.0);
  Control.addVariable("odinPitAColletMat","Tungsten");
  
  // Guide from Wall to PitA
  Control.addVariable("odinGPitAFrontXStep",0.0);       
  Control.addVariable("odinGPitAFrontYStep",0.0);       
  Control.addVariable("odinGPitAFrontZStep",0.0);       
  Control.addVariable("odinGPitAFrontXYAngle",0.0);       
  Control.addVariable("odinGPitAFrontZAngle",0.0);       
  Control.addVariable("odinGPitAFrontLength",220.0);       
  Control.addVariable("odinGPitAFrontHeight",5.0);       
  Control.addVariable("odinGPitAFrontDepth",5.0);       
  Control.addVariable("odinGPitAFrontLeftWidth",7.0);       
  Control.addVariable("odinGPitAFrontRightWidth",7.0);       
  Control.addVariable("odinGPitAFrontFeMat","Void");       
  Control.addVariable("odinGPitAFrontNShapes",1);       
  Control.addVariable("odinGPitAFrontNShapeLayers",3);

  Control.addVariable("odinGPitAFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitAFrontLayerThick2",1.5);

  Control.addVariable("odinGPitAFrontLayerMat0","Void");
  Control.addVariable("odinGPitAFrontLayerMat1","Glass");
  Control.addVariable("odinGPitAFrontLayerMat2","Void");       

  Control.addVariable("odinGPitAFront0TypeID","Rectangle");
  Control.addVariable("odinGPitAFront0Height",5.0);
  Control.addVariable("odinGPitAFront0Width",9.0);
  Control.addVariable("odinGPitAFront0Length",220.0);
  Control.addVariable("odinGPitAFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("odinGPitABackXStep",0.0);       
  Control.addVariable("odinGPitABackYStep",0.0);       
  Control.addVariable("odinGPitABackZStep",0.0);       
  Control.addVariable("odinGPitABackXYAngle",0.0);       
  Control.addVariable("odinGPitABackZAngle",0.0);       
  Control.addVariable("odinGPitABackLength",220.0);       
  Control.addVariable("odinGPitABackHeight",5.0);       
  Control.addVariable("odinGPitABackDepth",5.0);       
  Control.addVariable("odinGPitABackLeftWidth",7.0);       
  Control.addVariable("odinGPitABackRightWidth",7.0);       
  Control.addVariable("odinGPitABackFeMat","Void");       
  Control.addVariable("odinGPitABackNShapes",1);       
  Control.addVariable("odinGPitABackNShapeLayers",3);

  Control.addVariable("odinGPitABackLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitABackLayerThick2",1.5);

  Control.addVariable("odinGPitABackLayerMat0","Void");
  Control.addVariable("odinGPitABackLayerMat1","Glass");
  Control.addVariable("odinGPitABackLayerMat2","Void");       

  Control.addVariable("odinGPitABack0TypeID","Rectangle");
  Control.addVariable("odinGPitABack0Height",5.0);
  Control.addVariable("odinGPitABack0Width",9.0);
  Control.addVariable("odinGPitABack0Length",220.0);
  Control.addVariable("odinGPitABack0ZAngle",0.0);

  // Chopper A : Single 1.2m disk chopper [6 phases]
  Control.addVariable("odinChopperAXStep",0.0);
  Control.addVariable("odinChopperAYStep",0.0);
  Control.addVariable("odinChopperAZStep",0.0);
  Control.addVariable("odinChopperAXYangle",0.0);
  Control.addVariable("odinChopperAZangle",0.0);

  Control.addVariable("odinChopperAGap",3.0);
  Control.addVariable("odinChopperAInnerRadius",45.0);
  Control.addVariable("odinChopperAOuterRadius",63.0);
  Control.addVariable("odinChopperANDisk",1);

  Control.addVariable("odinChopperA0Thick",2.0);
  Control.addVariable("odinChopperAInnerMat","Aluminium");
  Control.addVariable("odinChopperAOuterMat","Inconnel");

  Control.addVariable("odinChopperANBlades",2);
  Control.addVariable("odinChopperA0PhaseAngle0",-20.0);
  Control.addVariable("odinChopperA0OpenAngle0",160.0);

  Control.addVariable("odinChopperA0PhaseAngle1",160.0);
  Control.addVariable("odinChopperA0OpenAngle1",160.0);

  
  // GUIDE EXITING PIT
  Control.addVariable("odinGEXStep",0.0);       
  Control.addVariable("odinGEYStep",0.0);       
  Control.addVariable("odinGEZStep",0.0);       
  Control.addVariable("odinGEXYAngle",0.0);       
  Control.addVariable("odinGEZAngle",0.0);       
  Control.addVariable("odinGELength",787.0);       
  Control.addVariable("odinGEHeight",81.0);       
  Control.addVariable("odinGEDepth",81.0);       
  Control.addVariable("odinGELeftWidth",81.0);       
  Control.addVariable("odinGERightWidth",81.0);       
  Control.addVariable("odinGEFeMat","Concrete");       
  Control.addVariable("odinGENShapes",1);       
  Control.addVariable("odinGENShapeLayers",4);

  Control.addVariable("odinGELayerThick1",0.4);  // glass thick
  Control.addVariable("odinGELayerThick2",1.5);
  Control.addVariable("odinGELayerThick3",36.0);

  Control.addVariable("odinGELayerMat0","Void");
  Control.addVariable("odinGELayerMat1","Glass");
  Control.addVariable("odinGELayerMat2","Void");
  Control.addVariable("odinGELayerMat3","Stainless304");       


  Control.addVariable("odinGE0TypeID","Rectangle");
  Control.addVariable("odinGE0Height",5.0);
  Control.addVariable("odinGE0Width",9.0);
  Control.addVariable("odinGE0Length",787.0);
  Control.addVariable("odinGE0ZAngle",0.0);

  // 
  // PIT B ::
  // 
  Control.addVariable("odinPitBVoidHeight",251.1);
  Control.addVariable("odinPitBVoidDepth",36.1);
  Control.addVariable("odinPitBVoidWidth",246.0);
  Control.addVariable("odinPitBVoidLength",105.0);
  
  Control.addVariable("odinPitBFeHeight",50.0);
  Control.addVariable("odinPitBFeDepth",50.0);
  Control.addVariable("odinPitBFeWidth",50.0);
  Control.addVariable("odinPitBFeFront",50.0);
  Control.addVariable("odinPitBFeBack",50.0);
  Control.addVariable("odinPitBFeMat","Stainless304");

  Control.addVariable("odinPitBConcHeight",50.0);
  Control.addVariable("odinPitBConcDepth",50.0);
  Control.addVariable("odinPitBConcWidth",50.0);
  Control.addVariable("odinPitBConcFront",50.0);
  Control.addVariable("odinPitBConcBack",50.0);
  Control.addVariable("odinPitBConcMat","Concrete");
  
  // GUDE IN the beam insert
  Control.addVariable("odinGPitBFrontXStep",0.0);       
  Control.addVariable("odinGPitBFrontYStep",0.0);       
  Control.addVariable("odinGPitBFrontZStep",0.0);       
  Control.addVariable("odinGPitBFrontXYAngle",0.0);       
  Control.addVariable("odinGPitBFrontZAngle",0.0);       
  Control.addVariable("odinGPitBFrontLength",220.0);       
  Control.addVariable("odinGPitBFrontHeight",5.0);       
  Control.addVariable("odinGPitBFrontDepth",5.0);       
  Control.addVariable("odinGPitBFrontLeftWidth",7.0);       
  Control.addVariable("odinGPitBFrontRightWidth",7.0);       
  Control.addVariable("odinGPitBFrontFeMat","Void");       
  Control.addVariable("odinGPitBFrontNShapes",1);       
  Control.addVariable("odinGPitBFrontNShapeLayers",3);

  Control.addVariable("odinGPitBFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitBFrontLayerThick2",1.5);

  Control.addVariable("odinGPitBFrontLayerMat0","Void");
  Control.addVariable("odinGPitBFrontLayerMat1","Glass");
  Control.addVariable("odinGPitBFrontLayerMat2","Void");       

  Control.addVariable("odinGPitBFront0TypeID","Rectangle");
  Control.addVariable("odinGPitBFront0Height",5.0);
  Control.addVariable("odinGPitBFront0Width",9.0);
  Control.addVariable("odinGPitBFront0Length",220.0);
  Control.addVariable("odinGPitBFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("odinGPitBBackXStep",0.0);       
  Control.addVariable("odinGPitBBackYStep",0.0);       
  Control.addVariable("odinGPitBBackZStep",0.0);       
  Control.addVariable("odinGPitBBackXYAngle",0.0);       
  Control.addVariable("odinGPitBBackZAngle",0.0);       
  Control.addVariable("odinGPitBBackLength",220.0);       
  Control.addVariable("odinGPitBBackHeight",5.0);       
  Control.addVariable("odinGPitBBackDepth",5.0);       
  Control.addVariable("odinGPitBBackLeftWidth",7.0);       
  Control.addVariable("odinGPitBBackRightWidth",7.0);       
  Control.addVariable("odinGPitBBackFeMat","Void");       
  Control.addVariable("odinGPitBBackNShapes",1);       
  Control.addVariable("odinGPitBBackNShapeLayers",3);

  Control.addVariable("odinGPitBBackLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitBBackLayerThick2",1.5);

  Control.addVariable("odinGPitBBackLayerMat0","Void");
  Control.addVariable("odinGPitBBackLayerMat1","Glass");
  Control.addVariable("odinGPitBBackLayerMat2","Void");       

  Control.addVariable("odinGPitBBack0TypeID","Rectangle");
  Control.addVariable("odinGPitBBack0Height",5.0);
  Control.addVariable("odinGPitBBack0Width",9.0);
  Control.addVariable("odinGPitBBack0Length",220.0);
  Control.addVariable("odinGPitBBack0ZAngle",0.0);

  // GUIDE EXITING PIT B
  Control.addVariable("odinGFXStep",0.0);       
  Control.addVariable("odinGFYStep",0.0);       
  Control.addVariable("odinGFZStep",0.0);       
  Control.addVariable("odinGFXYAngle",0.0);       
  Control.addVariable("odinGFZAngle",0.0);       
  Control.addVariable("odinGFLength",220.0);       
  Control.addVariable("odinGFHeight",85.0);       
  Control.addVariable("odinGFDepth",85.0);       
  Control.addVariable("odinGFLeftWidth",85.0);       
  Control.addVariable("odinGFRightWidth",85.0);       
  Control.addVariable("odinGFFeMat","Concrete");       
  Control.addVariable("odinGFNShapes",1);       
  Control.addVariable("odinGFNShapeLayers",4);

  Control.addVariable("odinGFLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGFLayerThick2",1.5);
  Control.addVariable("odinGFLayerThick3",36.0);

  Control.addVariable("odinGFLayerMat0","Void");
  Control.addVariable("odinGFLayerMat1","Glass");
  Control.addVariable("odinGFLayerMat2","Void");
  Control.addVariable("odinGFLayerMat3","Stainless304");       

  Control.addVariable("odinGF0TypeID","Rectangle");
  Control.addVariable("odinGF0Height",5.0);
  Control.addVariable("odinGF0Width",9.0);
  Control.addVariable("odinGF0Length",220.0);
  Control.addVariable("odinGF0ZAngle",0.0);
  // 
  // PIT C ::
  // 
  Control.addVariable("odinPitCVoidHeight",251.0);
  Control.addVariable("odinPitCVoidDepth",36.0);
  Control.addVariable("odinPitCVoidWidth",246.0);
  Control.addVariable("odinPitCVoidLength",105.0);
  
  Control.addVariable("odinPitCFeHeight",45.0);
  Control.addVariable("odinPitCFeDepth",45.0);
  Control.addVariable("odinPitCFeWidth",45.0);
  Control.addVariable("odinPitCFeFront",45.0);
  Control.addVariable("odinPitCFeBack",45.0);
  Control.addVariable("odinPitCFeMat","Stainless304");

  Control.addVariable("odinPitCConcHeight",50.0);
  Control.addVariable("odinPitCConcDepth",50.0);
  Control.addVariable("odinPitCConcWidth",50.0);
  Control.addVariable("odinPitCConcFront",50.0);
  Control.addVariable("odinPitCConcBack",50.0);
  Control.addVariable("odinPitCConcMat","Concrete");
  
  // GUDE IN the beam insert
  Control.addVariable("odinGPitCFrontXStep",0.0);       
  Control.addVariable("odinGPitCFrontYStep",0.0);       
  Control.addVariable("odinGPitCFrontZStep",0.0);       
  Control.addVariable("odinGPitCFrontXYAngle",0.0);       
  Control.addVariable("odinGPitCFrontZAngle",0.0);       
  Control.addVariable("odinGPitCFrontLength",220.0);       
  Control.addVariable("odinGPitCFrontHeight",5.0);       
  Control.addVariable("odinGPitCFrontDepth",5.0);       
  Control.addVariable("odinGPitCFrontLeftWidth",7.0);       
  Control.addVariable("odinGPitCFrontRightWidth",7.0);       
  Control.addVariable("odinGPitCFrontFeMat","Void");       
  Control.addVariable("odinGPitCFrontNShapes",1);       
  Control.addVariable("odinGPitCFrontNShapeLayers",3);

  Control.addVariable("odinGPitCFrontLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitCFrontLayerThick2",1.5);

  Control.addVariable("odinGPitCFrontLayerMat0","Void");
  Control.addVariable("odinGPitCFrontLayerMat1","Glass");
  Control.addVariable("odinGPitCFrontLayerMat2","Void");       

  Control.addVariable("odinGPitCFront0TypeID","Rectangle");
  Control.addVariable("odinGPitCFront0Height",5.0);
  Control.addVariable("odinGPitCFront0Width",9.0);
  Control.addVariable("odinGPitCFront0Length",220.0);
  Control.addVariable("odinGPitCFront0ZAngle",0.0);

  // GUDE IN the beam insert
  Control.addVariable("odinGPitCBackXStep",0.0);       
  Control.addVariable("odinGPitCBackYStep",0.0);       
  Control.addVariable("odinGPitCBackZStep",0.0);       
  Control.addVariable("odinGPitCBackXYAngle",0.0);       
  Control.addVariable("odinGPitCBackZAngle",0.0);       
  Control.addVariable("odinGPitCBackLength",220.0);       
  Control.addVariable("odinGPitCBackHeight",5.0);       
  Control.addVariable("odinGPitCBackDepth",5.0);       
  Control.addVariable("odinGPitCBackLeftWidth",7.0);       
  Control.addVariable("odinGPitCBackRightWidth",7.0);       
  Control.addVariable("odinGPitCBackFeMat","Void");       
  Control.addVariable("odinGPitCBackNShapes",1);       
  Control.addVariable("odinGPitCBackNShapeLayers",3);

  Control.addVariable("odinGPitCBackLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGPitCBackLayerThick2",1.5);

  Control.addVariable("odinGPitCBackLayerMat0","Void");
  Control.addVariable("odinGPitCBackLayerMat1","Glass");
  Control.addVariable("odinGPitCBackLayerMat2","Void");       

  Control.addVariable("odinGPitCBack0TypeID","Rectangle");
  Control.addVariable("odinGPitCBack0Height",5.0);
  Control.addVariable("odinGPitCBack0Width",9.0);
  Control.addVariable("odinGPitCBack0Length",220.0);
  Control.addVariable("odinGPitCBack0ZAngle",0.0);

  // GUIDE EXITING PIT C
  Control.addVariable("odinGGXStep",0.0);       
  Control.addVariable("odinGGYStep",0.0);       
  Control.addVariable("odinGGZStep",0.0);       
  Control.addVariable("odinGGXYAngle",0.0);       
  Control.addVariable("odinGGZAngle",0.0);       
  Control.addVariable("odinGGLength",1530.0);       
  Control.addVariable("odinGGHeight",90.0);       
  Control.addVariable("odinGGDepth",90.0);       
  Control.addVariable("odinGGLeftWidth",90.0);       
  Control.addVariable("odinGGRightWidth",90.0);       
  Control.addVariable("odinGGFeMat","Concrete");       
  Control.addVariable("odinGGNShapes",1);       
  Control.addVariable("odinGGNShapeLayers",4);

  Control.addVariable("odinGGLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGGLayerThick2",1.5);
  Control.addVariable("odinGGLayerThick3",30.0);

  Control.addVariable("odinGGLayerMat0","Void");
  Control.addVariable("odinGGLayerMat1","Glass");
  Control.addVariable("odinGGLayerMat2","Void");
  Control.addVariable("odinGGLayerMat3","Stainless304");       

  Control.addVariable("odinGG0TypeID","Rectangle");
  Control.addVariable("odinGG0Height",5.0);
  Control.addVariable("odinGG0Width",9.0);
  Control.addVariable("odinGG0Length",1530.0);
  Control.addVariable("odinGG0ZAngle",0.0);

  // HUT:
  Control.addVariable("odinCaveVoidHeight",300.0);
  Control.addVariable("odinCaveVoidDepth",183.0);
  Control.addVariable("odinCaveVoidWidth",600.0);
  Control.addVariable("odinCaveVoidLength",1686.0);
  Control.addVariable("odinCaveVoidNoseWidth",293.0);
  Control.addVariable("odinCaveVoidNoseLen",183.0);

  Control.addVariable("odinCaveFeLeftWall",15.0);
  Control.addVariable("odinCaveFeRightWall",15.0);
  Control.addVariable("odinCaveFeRoof",15.0);
  Control.addVariable("odinCaveFeFloor",15.0);
  Control.addVariable("odinCaveFeNoseFront",25.0);
  Control.addVariable("odinCaveFeNoseSide",25.0);
  Control.addVariable("odinCaveFeBack",15.0);

  Control.addVariable("odinCaveConcLeftWall",35.0);
  Control.addVariable("odinCaveConcRightWall",35.0);
  Control.addVariable("odinCaveConcRoof",35.0);
  Control.addVariable("odinCaveConcFloor",50.0);
  Control.addVariable("odinCaveConcNoseFront",35.0);
  Control.addVariable("odinCaveConcNoseSide",35.0);
  Control.addVariable("odinCaveConcBack",35.0);

  Control.addVariable("odinCaveFeMat","Stainless304");
  Control.addVariable("odinCaveConcMat","Concrete");
   
  // WALL for Hut
  Control.addVariable("odinCaveWallYStep",6.0);
  Control.addVariable("odinCaveWallThick",45.0);
  Control.addVariable("odinCaveWallXGap",6.0);
  Control.addVariable("odinCaveWallZGap",6.0);
  Control.addVariable("odinCaveWallMat","Stainless304");

  // GUIDE in Hutch nose cone
  Control.addVariable("odinGHXStep",0.0);       
  Control.addVariable("odinGHYStep",0.0);       
  Control.addVariable("odinGHZStep",0.0);       
  Control.addVariable("odinGHXYAngle",0.0);       
  Control.addVariable("odinGHZAngle",0.0);       
  Control.addVariable("odinGHLength",150.0);       
  Control.addVariable("odinGHHeight",9.0);       
  Control.addVariable("odinGHDepth",9.0);       
  Control.addVariable("odinGHLeftWidth",9.0);       
  Control.addVariable("odinGHRightWidth",9.0);       
  Control.addVariable("odinGHFeMat","Void");       
  Control.addVariable("odinGHNShapes",1);       
  Control.addVariable("odinGHNShapeLayers",3);

  Control.addVariable("odinGHLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGHLayerThick2",1.5);

  Control.addVariable("odinGHLayerMat0","Void");
  Control.addVariable("odinGHLayerMat1","Glass");
  Control.addVariable("odinGHLayerMat2","Void");

  Control.addVariable("odinGH0TypeID","Rectangle");
  Control.addVariable("odinGH0Height",5.0);
  Control.addVariable("odinGH0Width",9.0);
  Control.addVariable("odinGH0Length",150.0);
  Control.addVariable("odinGH0ZAngle",0.0);

  // PIN COLLIMATOR SYSTEM
  Control.addVariable("odinPinLength",50.0);
  Control.addVariable("odinPinRadius",60.0);
  // Collimator A:
  Control.addVariable("odinPinCollAYStep",1.0);
  Control.addVariable("odinPinCollARotDepth",20.0);
  Control.addVariable("odinPinCollARadius",30.0);
  Control.addVariable("odinPinCollAThick",10.0);

  Control.addVariable("odinPinCollAInnerWall",1.0);
  Control.addVariable("odinPinCollAInnerWallMat","Void");

  Control.addVariable("odinPinCollANLayers",0);
  Control.addVariable("odinPinCollAHoleIndex",0);
  Control.addVariable("odinPinCollAHoleAngOff",0.0);

  Control.addVariable("odinPinCollADefMat","Tungsten");
  // collimator holes:
  Control.addVariable("odinPinCollANHole",3);
  Control.addVariable("odinPinCollAHole0Shape",1);
  Control.addVariable("odinPinCollAHole0Radius",3.0);

  Control.addVariable("odinPinCollAHole0AngleCentre",0.0);
  Control.addVariable("odinPinCollAHole0AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole0RadialStep",20.0);

  Control.addVariable("odinPinCollAHole1Shape",3);
  Control.addVariable("odinPinCollAHole1Radius",5.0);
  Control.addVariable("odinPinCollAHole1AngleCentre",120.0);
  Control.addVariable("odinPinCollAHole1AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole1RadialStep",15.0);
  
  Control.addVariable("odinPinCollAHole2Shape",1);
  Control.addVariable("odinPinCollAHole2Radius",4.0);
  Control.addVariable("odinPinCollAHole2AngleCentre",240.0);
  Control.addVariable("odinPinCollAHole2AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole2RadialStep",20.0);

  // Collimator B:
  Control.addVariable("odinPinCollBYStep",40.0);
  Control.addVariable("odinPinCollBRotDepth",20.0);
  Control.addVariable("odinPinCollBRadius",30.0);
  Control.addVariable("odinPinCollBThick",10.0);

  Control.addVariable("odinPinCollBInnerWall",1.0);
  Control.addVariable("odinPinCollBInnerWallMat","Void");

  Control.addVariable("odinPinCollBNLayers",0);
  Control.addVariable("odinPinCollBHoleIndex",0);
  Control.addVariable("odinPinCollBHoleAngOff",0.0);

  Control.addVariable("odinPinCollBDefMat","Tungsten");
  // collimator holes:
  Control.addVariable("odinPinCollBNHole",3);
  Control.addVariable("odinPinCollBHole0Shape",1);
  Control.addVariable("odinPinCollBHole0Radius",3.0);

  Control.addVariable("odinPinCollBHole0AngleCentre",0.0);
  Control.addVariable("odinPinCollBHole0AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole0RadialStep",20.0);

  Control.addVariable("odinPinCollBHole1Shape",3);
  Control.addVariable("odinPinCollBHole1Radius",5.0);
  Control.addVariable("odinPinCollBHole1AngleCentre",120.0);
  Control.addVariable("odinPinCollBHole1AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole1RadialStep",15.0);
  
  Control.addVariable("odinPinCollBHole2Shape",1);
  Control.addVariable("odinPinCollBHole2Radius",4.0);
  Control.addVariable("odinPinCollBHole2AngleCentre",240.0);
  Control.addVariable("odinPinCollBHole2AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole2RadialStep",20.0);

  //
  // JAWS
  //
  // Jaw X
  Control.addVariable("odinPinJawVertXStep",0.0);
  Control.addVariable("odinPinJawVertYStep",17.0);
  Control.addVariable("odinPinJawVertZStep",0.0);
  Control.addVariable("odinPinJawVertXYangle",180.0);
  Control.addVariable("odinPinJawVertZangle",0.0);

  Control.addVariable("odinPinJawVertZOpen",1.5);
  Control.addVariable("odinPinJawVertZThick",4.0);
  Control.addVariable("odinPinJawVertZCross",15.0);
  Control.addVariable("odinPinJawVertZLen",8.0);

  Control.addVariable("odinPinJawVertGap",0.5);  
  
  Control.addVariable("odinPinJawVertXOpen",1.5);
  Control.addVariable("odinPinJawVertXThick",4.0);
  Control.addVariable("odinPinJawVertXCross",15.0);
  Control.addVariable("odinPinJawVertXLen",8.0);  

  Control.addVariable("odinPinJawVertXHeight",28.0);
  Control.addVariable("odinPinJawVertYHeight",9.0);
  Control.addVariable("odinPinJawVertZHeight",28.0);
  Control.addVariable("odinPinJawVertWallThick",2.0);

  Control.addVariable("odinPinJawVertxJawMat","Tungsten");
  Control.addVariable("odinPinJawVertzJawMat","Tungsten");
  Control.addVariable("odinPinJawVertWallMat","Aluminium");

  // Jaw XZ
  Control.addVariable("odinPinJawDiagXStep",0.0);
  Control.addVariable("odinPinJawDiagYStep",32.0);
  Control.addVariable("odinPinJawDiagZStep",0.0);
  Control.addVariable("odinPinJawDiagXAngle",0.0);
  Control.addVariable("odinPinJawDiagYAngle",45.0);
  Control.addVariable("odinPinJawDiagZAngle",0.0);

  Control.addVariable("odinPinJawDiagZOpen",1.5);
  Control.addVariable("odinPinJawDiagZThick",4.0);
  Control.addVariable("odinPinJawDiagZCross",15.0);
  Control.addVariable("odinPinJawDiagZLen",8.0);

  Control.addVariable("odinPinJawDiagGap",0.5);  
  
  Control.addVariable("odinPinJawDiagXOpen",1.5);
  Control.addVariable("odinPinJawDiagXThick",4.0);
  Control.addVariable("odinPinJawDiagXCross",15.0);
  Control.addVariable("odinPinJawDiagXLen",8.0);  

  Control.addVariable("odinPinJawDiagXHeight",28.0);
  Control.addVariable("odinPinJawDiagYHeight",9.0);
  Control.addVariable("odinPinJawDiagZHeight",28.0);
  Control.addVariable("odinPinJawDiagWallThick",2.0);


  Control.addVariable("odinPinJawDiagxJawMat","Tungsten");
  Control.addVariable("odinPinJawDiagzJawMat","Tungsten");
  Control.addVariable("odinPinJawDiagWallMat","Aluminium");

  Control.addVariable("odinBeamStopYStep",1200.0);
  
  Control.addVariable("odinBeamStopWidth",360.0);
  Control.addVariable("odinBeamStopDepth",180.0);
  Control.addVariable("odinBeamStopHeight",180.0);
  Control.addVariable("odinBeamStopLength",350.0);

  Control.addVariable("odinBeamStopFeRadius",25.0);
  Control.addVariable("odinBeamStopFeLength",72.0);
  
  Control.addVariable("odinBeamStopOuterRadius",75.0);
  Control.addVariable("odinBeamStopOuterFeRadius",20.0);
  Control.addVariable("odinBeamStopOuterFeStep",25.0);
  Control.addVariable("odinBeamStopOuterCut",50.0);

  Control.addVariable("odinBeamStopInnerRadius",50.0);
  Control.addVariable("odinBeamStopInnerFeRadius",20.0);
  Control.addVariable("odinBeamStopInnerFeStep",35.0);
  Control.addVariable("odinBeamStopInnerCut",70.0);
  
  Control.addVariable("odinBeamStopFeMat","Stainless304");
  Control.addVariable("odinBeamStopConcMat","Concrete");

  return;
}

}  // NAMESPACE setVariable
