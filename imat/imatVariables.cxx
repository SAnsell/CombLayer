/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imat/imatVariables.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
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
IMatVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("imatVariables(File)","IMatVariables");

  Control.addVariable("imatShutterXStep",0.0);
  Control.addVariable("imatShutterZStep",0.0);
  Control.addVariable("imatShutterXYAngle",1.0);
  Control.addVariable("imatShutterZAngle",0.0); 
  Control.addVariable("imatShutterHeight",10.0);
  Control.addVariable("imatShutterWidth",10.0); 

  Control.addVariable("imatShutterInnerThick",0.6); 
  Control.addVariable("imatShutterSupportThick",1.2); 
  Control.addVariable("imatShutterGapThick",0.1);
  Control.addVariable("imatShutterMaskHeight",11.5);  // GUESS
  Control.addVariable("imatShutterMaskWidth",11.5);  // Guess
  Control.addVariable("imatShutterMaskThick",0.6);  // 6mm GUESS

  Control.addVariable("imatShutterInnerMat","Glass");   // Float glass
  Control.addVariable("imatShutterSupportMat","Stainless304"); 
  Control.addVariable("imatShutterMaskMat","Boron"); 

  
  Control.addVariable("imatInsertXStep",0.0);
  Control.addVariable("imatInsertYStep",0.0);
  Control.addVariable("imatInsertZStep",0.0);
  Control.addVariable("imatInsertXYAngle",0.0);
  Control.addVariable("imatInsertZAngle",0.0); 
  Control.addVariable("imatInsertFrontGap",0.0);
  Control.addVariable("imatInsertHeight",9.60);
  Control.addVariable("imatInsertWidth",9.60); 
  Control.addVariable("imatInsertDefMat",3.0); 

  // GUIDE:
  Control.addVariable("imatGuideXStep",0.0);
  Control.addVariable("imatGuideYStep",0.0);
  Control.addVariable("imatGuideZStep",0.0);
  Control.addVariable("imatGuideXYAngle",0.0);
  Control.addVariable("imatGuideZAngle",0.0); 

  Control.addVariable("imatGuideLength",554.3);  // 
  Control.addVariable("imatGuideHeight",10.0);
  Control.addVariable("imatGuideWidth",10.00); 

  Control.addVariable("imatGuideGlassThick",0.3);
  Control.addVariable("imatGuideBoxThick",0.7);

  Control.addVariable("imatGuideVoidSide",10.75);   // 325mm full gap
  Control.addVariable("imatGuideVoidTop",10.75);   
  Control.addVariable("imatGuideVoidBase",10.75);  

  Control.addVariable("imatGuideFeCut",30.0);   
  Control.addVariable("imatGuideFeCutLen",90.0);   
  Control.addVariable("imatGuideFeSide",60.0);   
  Control.addVariable("imatGuideFeTop",60.0);   
  Control.addVariable("imatGuideFeBase",60.0);  

  Control.addVariable("imatGuideWallCutLen",554.3);   
  Control.addVariable("imatGuideWallCut",30.0);   
  Control.addVariable("imatGuideWallSide",60.0);   
  Control.addVariable("imatGuideWallTop",60.0);   
  Control.addVariable("imatGuideWallBase",60.0);  

  Control.addVariable("imatGuideGlassMat","Glass"); 
  Control.addVariable("imatGuideBoxMat","Aluminium"); 
  Control.addVariable("imatGuideFeMat","CastIron"); 
  Control.addVariable("imatGuideWallMat","Poly");  // Poly/wax
  
  return;
}

}  // NAMESPACE setVariable
