/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   snsBuild/SNSVariables.cxx
*
 * Copyright (c) 2004-2014 by Stuart Ansell
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
SNSVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for SNS ()
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("tarFrontXOffset",0.0);  
  Control.addVariable("tarFrontYOffset",-0.3175);  
  Control.addVariable("tarFrontZOffset",0.0);  
  Control.addVariable("tarFrontTotalLength",100.0);

  Control.addVariable("tarFrontMainLength",69.215+0.3175);
  Control.addVariable("tarFrontMainJoin",83.82+0.3175);
  Control.addVariable("tarFrontHeLength",67.6148+0.3175);     //2320
  Control.addVariable("tarFrontHeJoin",83.0707+0.3175);
  Control.addVariable("tarFrontPressLength",66.3829+0.3175);     //2301
  Control.addVariable("tarFrontPressJoin",74.1807+0.3175);     //2321
  Control.addVariable("tarFrontWaterLength",66.3829+0.3175);     //2301
  Control.addVariable("tarFrontWaterJoin",74.1807+0.3175);     //2321

  Control.addVariable("tarFrontOuterLength",66.3829+0.3175);     //NOT CORRECT
  Control.addVariable("tarFrontOuterJoin",74.1807+0.3175);     //NOT CORRECT
  Control.addVariable("tarFrontOuterLift",2.5);            //NOT CORRECT

  Control.addVariable("tarFrontHgCutAngle",6.2524); // [sur 1454]
  Control.addVariable("tarFrontInnerCutAngle",15.318); // [sur 1502]
  Control.addVariable("tarFrontHeCutAngle",14.512); // [sur 2326
  Control.addVariable("tarFrontPressCutAngle",26.505);  //[sur 2332]
  Control.addVariable("tarFrontWaterCutAngle",28.473);  //[sur 2380]
  Control.addVariable("tarFrontOuterCutAngle",12.390);  //[sur 2380]

  Control.addVariable("tarFrontMainHeight",9.8044);  
  Control.addVariable("tarFrontMainWidth",32.004);  
  Control.addVariable("tarFrontMainCorner",32.004);  

  Control.addVariable("tarFrontInnerWallXStep",0.0);     // 16.002
  Control.addVariable("tarFrontInnerWallStep",0.0);     // -0.3175
  Control.addVariable("tarFrontInnerWallThick",0.3048);  // 5.207  

  Control.addVariable("tarFrontHeliumXStep",0.0);      // 16.002
  Control.addVariable("tarFrontHeliumStep",0.0127);    // -0.3048
  Control.addVariable("tarFrontHeliumThick",0.3175);   // 5.5245

  Control.addVariable("tarFrontPressXStep",0.2921);  // 16.2941  
  Control.addVariable("tarFrontPressStep",0.3556);   // 0.0508
  Control.addVariable("tarFrontPressThick",0.5334);  // 6.0579

  Control.addVariable("tarFrontWaterXStep",0.2794);      // 16.5735
  Control.addVariable("tarFrontWaterStep",0.0);       // 0.0508  
  Control.addVariable("tarFrontWaterThick",0.3556);   // 6.4135

  Control.addVariable("tarFrontOuterXStep",-0.0889);  // 16.4846 
  Control.addVariable("tarFrontOuterStep",0.2667);   // 0.3175
  Control.addVariable("tarFrontOuterThick",0.4191);  // 6.8326

  Control.addVariable("tarFrontMercuryMat","Mercury");  
  Control.addVariable("tarFrontInnerWallMat","Stainless304");  
  Control.addVariable("tarFrontPressMat","Stainless304");  
  Control.addVariable("tarFrontHeMat","helium");  
  Control.addVariable("tarFrontWaterMat","D2O");  
  Control.addVariable("tarFrontOuterMat","Stainless304");  
  Control.addVariable("tarFrontMercuryTemp",600.0);  

  Control.addVariable("sdefEnergy",1400.0);  

  Control.addVariable("RefPlugXStep",0.0);  
  Control.addVariable("RefPlugYStep",0.0);  
  Control.addVariable("RefPlugZStep",0.0);
  Control.addVariable("RefPlugXYangle",0.0); 
  Control.addVariable("RefPlugZangle",0.0);

  Control.addVariable("RefPlugRadius",32.004);
  Control.addVariable("RefPlugHeight",28.0);
  Control.addVariable("RefPlugDepth",28.0);
  Control.addVariable("RefPlugMat","Be300K");
  Control.addVariable("RefPlugTemp",20.0);
  Control.addVariable("RefPlugNLayers",3);
  // Water
  Control.addVariable("RefPlugHGap1",1.143);
  Control.addVariable("RefPlugRadGap1",1.143);
  Control.addVariable("RefPlugMaterial1","H2O");  
  Control.addVariable("RefPlugTemp1",20.0);  
  // Be next
  Control.addVariable("RefPlugHGap2",13.5382);
  Control.addVariable("RefPlugRadGap2",13.5382);
  Control.addVariable("RefPlugMaterial2","Be300K"); 
  // Water
  Control.addVariable("RefPlugHGap3",0.9398);
  Control.addVariable("RefPlugRadGap3",0.9398);
  Control.addVariable("RefPlugMaterial3","H2O"); 

  return;
}

}  // NAMESPACE setVariable
