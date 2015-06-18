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

    // GUIDE IN the beam insert
  Control.addVariable("odinGDXStep",0.0);       
  Control.addVariable("odinGDYStep",0.0);       
  Control.addVariable("odinGDZStep",0.0);       
  Control.addVariable("odinGDXYAngle",0.0);       
  Control.addVariable("odinGDZAngle",0.0);       
  Control.addVariable("odinGDLength",220.0);       
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
  Control.addVariable("odinGD0Length",220.0);
  Control.addVariable("odinGD0ZAngle",0.0);

  Control.addVariable("odinPitAVoidHeight",20.0);
  Control.addVariable("odinPitAVoidDepth",20.0);
  Control.addVariable("odinPitAVoidWidth",50.0);
  Control.addVariable("odinPitAVoidLength",50.0);
  
  Control.addVariable("odinPitAVoidHeight",20.0);
  Control.addVariable("odinPitAVoidDepth",20.0);
  Control.addVariable("odinPitAVoidWidth",50.0);
  Control.addVariable("odinPitAVoidLength",50.0);

  
  return;
}

}  // NAMESPACE setVariable
