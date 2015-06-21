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

  Control.addVariable("ODINCollAXStep",0.0);
  Control.addVariable("ODINCollAYStep",50.0);
  Control.addVariable("ODINCollAZStep",0.0);
  Control.addVariable("ODINCollAXYangle",180.0);
  Control.addVariable("ODINCollAZangle",180.0);

  Control.addVariable("ODINCollAZOpen",1.5);
  Control.addVariable("ODINCollAZThick",4.0);
  Control.addVariable("ODINCollAZCross",15.0);
  Control.addVariable("ODINCollAZLen",8.0);

  Control.addVariable("ODINCollAGap",0.5);  
  
  Control.addVariable("ODINCollAXOpen",1.5);
  Control.addVariable("ODINCollAXThick",4.0);
  Control.addVariable("ODINCollAXCross",15.0);
  Control.addVariable("ODINCollAXLen",8.0);  

  Control.addVariable("ODINCollAXHeight",18.0);
  Control.addVariable("ODINCollAYHeight",9.0);
  Control.addVariable("ODINCollAZHeight",18.0);
  Control.addVariable("ODINCollAWallThick",2.0);

  Control.addVariable("ODINCollAzJawMat","Aluminium");
  Control.addVariable("ODINCollAxJawMat","Aluminium");
  Control.addVariable("ODINCollAWallMat","Aluminium");
  Control.addVariable("ODINCollAzJawMat","Aluminium");


  // Quad Blade chopper
  Control.addVariable("odinBladeXStep",0.0);
  Control.addVariable("odinBladeYStep",2.0);
  Control.addVariable("odinBladeZStep",15.0/2.0);
  Control.addVariable("odinBladeXYangle",0.0);
  Control.addVariable("odinBladeZangle",0.0);

  Control.addVariable("odinBladeGap",3.0);
  Control.addVariable("odinBladeInnerRadius",10.0);
  Control.addVariable("odinBladeOuterRadius",25.0);
  Control.addVariable("odinBladeNDisk",4);

  Control.addVariable("odinBlade0Thick",1.0);
  Control.addVariable("odinBlade1Thick",1.0);
  Control.addVariable("odinBlade2Thick",1.0);
  Control.addVariable("odinBlade3Thick",1.0);
  Control.addVariable("odinBladeInnerMat","Inconnel");
  Control.addVariable("odinBladeOuterMat","Aluminum");

  Control.addVariable("odinBlade0PhaseAngle",95.0);
  Control.addVariable("odinBlade0OpenAngle",30.0);
  Control.addVariable("odinBlade1PhaseAngle",95.0);
  Control.addVariable("odinBlade1OpenAngle",30.0);
  Control.addVariable("odinBlade2PhaseAngle",95.0);
  Control.addVariable("odinBlade2OpenAngle",30.0);
  Control.addVariable("odinBlade3PhaseAngle",95.0);
  Control.addVariable("odinBlade3OpenAngle",30.0);
  
  Control.addVariable("odinGAXStep",0.0);       
  Control.addVariable("odinGAYStep",0.0);       
  Control.addVariable("odinGAZStep",0.0);       
  Control.addVariable("odinGAXYAngle",0.0);       
  Control.addVariable("odinGAZAngle",0.0);       
  Control.addVariable("odinGALength",220.0);       
  Control.addVariable("odinGAHeight",20.0);       
  Control.addVariable("odinGADepth",20.0);       
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
  Control.addVariable("odinTZeroZStep",15.0/2.0);
  Control.addVariable("odinTZeroXYangle",0.0);
  Control.addVariable("odinTZeroZangle",0.0);

  Control.addVariable("odinTZeroGap",3.0);
  Control.addVariable("odinTZeroInnerRadius",10.0);
  Control.addVariable("odinTZeroOuterRadius",25.0);
  Control.addVariable("odinTZeroNDisk",1);

  Control.addVariable("odinTZero0Thick",40.0);
  Control.addVariable("odinTZeroInnerMat","Aluminium");
  Control.addVariable("odinTZeroOuterMat","Inconnel");

  Control.addVariable("odinTZero0PhaseAngle",95.0);
  Control.addVariable("odinTZero0OpenAngle",30.0);

  // SECOND GUIDE SEGMENT
  
  Control.addVariable("odinGBXStep",0.0);       
  Control.addVariable("odinGBYStep",0.0);       
  Control.addVariable("odinGBZStep",0.0);       
  Control.addVariable("odinGBXYAngle",0.0);       
  Control.addVariable("odinGBZAngle",0.0);       
  Control.addVariable("odinGBLength",220.0);       
  Control.addVariable("odinGBHeight",100.0);       
  Control.addVariable("odinGBDepth",100.0);       
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
  Control.addVariable("odinGB0Length",220.0);
  Control.addVariable("odinGB0ZAngle",0.0);

  // BEAM INSERT:
  Control.addVariable("odinBInsertHeight",14.0);
  Control.addVariable("odinBInsertWidth",14.0);
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

    // GUIDE Exiting Bunker wall
  Control.addVariable("odinGDXStep",0.0);       
  Control.addVariable("odinGDYStep",0.0);       
  Control.addVariable("odinGDZStep",0.0);       
  Control.addVariable("odinGDXYAngle",0.0);       
  Control.addVariable("odinGDZAngle",0.0);       
  Control.addVariable("odinGDLength",141.0);       
  Control.addVariable("odinGDHeight",90.0);       
  Control.addVariable("odinGDDepth",90.0);       
  Control.addVariable("odinGDLeftWidth",90.0);       
  Control.addVariable("odinGDRightWidth",90.0);       
  Control.addVariable("odinGDFeMat","Concrete");       
  Control.addVariable("odinGDNShapes",1);       
  Control.addVariable("odinGDNShapeLayers",4);

  Control.addVariable("odinGDLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGDLayerThick2",1.5);
  Control.addVariable("odinGDLayerThick3",33.0);

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
  
  Control.addVariable("odinPitAFeHeight",50.0);
  Control.addVariable("odinPitAFeDepth",50.0);
  Control.addVariable("odinPitAFeWidth",50.0);
  Control.addVariable("odinPitAFeFront",35.0);
  Control.addVariable("odinPitAFeBack",50.0);
  Control.addVariable("odinPitAFeMat","Stainless304");

  
  Control.addVariable("odinPitAConcHeight",50.0);
  Control.addVariable("odinPitAConcDepth",50.0);
  Control.addVariable("odinPitAConcWidth",50.0);
  Control.addVariable("odinPitAConcFront",50.0);
  Control.addVariable("odinPitAConcBack",50.0);
  Control.addVariable("odinPitAConcMat","Concrete");
  
  // GUDE IN the beam insert
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

  
  // GUIDE EXITING PIT
  Control.addVariable("odinGEXStep",0.0);       
  Control.addVariable("odinGEYStep",0.0);       
  Control.addVariable("odinGEZStep",0.0);       
  Control.addVariable("odinGEXYAngle",0.0);       
  Control.addVariable("odinGEZAngle",0.0);       
  Control.addVariable("odinGELength",787.0);       
  Control.addVariable("odinGEHeight",90.0);       
  Control.addVariable("odinGEDepth",90.0);       
  Control.addVariable("odinGELeftWidth",90.0);       
  Control.addVariable("odinGERightWidth",90.0);       
  Control.addVariable("odinGEFeMat","Concrete");       
  Control.addVariable("odinGENShapes",1);       
  Control.addVariable("odinGENShapeLayers",4);

  Control.addVariable("odinGELayerThick1",0.4);  // glass thick
  Control.addVariable("odinGELayerThick2",1.5);
  Control.addVariable("odinGELayerThick3",33.0);

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
  Control.addVariable("odinPitBVoidHeight",251.0);
  Control.addVariable("odinPitBVoidDepth",36.0);
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
  Control.addVariable("odinGFHeight",90.0);       
  Control.addVariable("odinGFDepth",90.0);       
  Control.addVariable("odinGFLeftWidth",90.0);       
  Control.addVariable("odinGFRightWidth",90.0);       
  Control.addVariable("odinGFFeMat","Concrete");       
  Control.addVariable("odinGFNShapes",1);       
  Control.addVariable("odinGFNShapeLayers",4);

  Control.addVariable("odinGFLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGFLayerThick2",1.5);
  Control.addVariable("odinGFLayerThick3",33.0);

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
  
  Control.addVariable("odinPitCFeHeight",50.0);
  Control.addVariable("odinPitCFeDepth",50.0);
  Control.addVariable("odinPitCFeWidth",50.0);
  Control.addVariable("odinPitCFeFront",50.0);
  Control.addVariable("odinPitCFeBack",50.0);
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

  // GUIDE EXITING PIT Ba
  Control.addVariable("odinGGXStep",0.0);       
  Control.addVariable("odinGGYStep",0.0);       
  Control.addVariable("odinGGZStep",0.0);       
  Control.addVariable("odinGGXYAngle",0.0);       
  Control.addVariable("odinGGZAngle",0.0);       
  Control.addVariable("odinGGLength",220.0);       
  Control.addVariable("odinGGHeight",90.0);       
  Control.addVariable("odinGGDepth",90.0);       
  Control.addVariable("odinGGLeftWidth",90.0);       
  Control.addVariable("odinGGRightWidth",90.0);       
  Control.addVariable("odinGGFeMat","Concrete");       
  Control.addVariable("odinGGNShapes",1);       
  Control.addVariable("odinGGNShapeLayers",4);

  Control.addVariable("odinGGLayerThick1",0.4);  // glass thick
  Control.addVariable("odinGGLayerThick2",1.5);
  Control.addVariable("odinGGLayerThick3",33.0);

  Control.addVariable("odinGGLayerMat0","Void");
  Control.addVariable("odinGGLayerMat1","Glass");
  Control.addVariable("odinGGLayerMat2","Void");
  Control.addVariable("odinGGLayerMat3","Stainless304");       

  Control.addVariable("odinGG0TypeID","Rectangle");
  Control.addVariable("odinGG0Height",5.0);
  Control.addVariable("odinGG0Width",9.0);
  Control.addVariable("odinGG0Length",220.0);
  Control.addVariable("odinGG0ZAngle",0.0);



  

  
  return;
}

}  // NAMESPACE setVariable
