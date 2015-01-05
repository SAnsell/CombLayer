/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuild/EPBVariables.cxx
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
EPBVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for ESS ()
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("LineVoidXStep",0.0);  
  Control.addVariable("LineVoidYStep",0.0);  
  Control.addVariable("LineVoidZStep",0.0);  
  Control.addVariable("LineVoidXYangle",0.0);  
  Control.addVariable("LineVoidZangle",0.0);  
  Control.addVariable("LineVoidInnerRad",4.0); 
  Control.addVariable("LineVoidWallThick",1.0); 
  Control.addVariable("LineVoidWallMat",3); 
  Control.addVariable("LineVoidNSeg",8);  

  Control.addVariable("LineVoidPoint1",Geometry::Vec3D(0,0,0)); 
  Control.addVariable("LineVoidPoint2",Geometry::Vec3D(0,240.0,0)); 
  Control.addVariable("LineVoidPoint3",Geometry::Vec3D(0,475.5,46.0)); 
  Control.addVariable("LineVoidPoint4",Geometry::Vec3D(0,698,136)); 
  Control.addVariable("LineVoidPoint5",Geometry::Vec3D(0,920,225.9)); 
  Control.addVariable("LineVoidPoint6",Geometry::Vec3D(0,1143,315.8)); 
  Control.addVariable("LineVoidPoint7",Geometry::Vec3D(0,1365,405.7)); 
  Control.addVariable("LineVoidPoint8",Geometry::Vec3D(0,1601,451.5)); 
  Control.addVariable("LineVoidPoint9",Geometry::Vec3D(0,1840,451.5)); 

  Control.addVariable("EPBNMagnets",4); 
  Control.addVariable("EPBNFocus",3); 

  Control.addVariable("MagnetXStep",0.0); 
  Control.addVariable("MagnetXStep",0.0); 
  Control.addVariable("MagnetYStep",0.0); 
  Control.addVariable("MagnetZStep",0.0); 
  Control.addVariable("MagnetXYangle",0.0); 
  Control.addVariable("MagnetZangle",0.0); 
  Control.addVariable("MagnetWidth",90.0); 
  Control.addVariable("MagnetHeight",90.0); 
  Control.addVariable("MagnetLength",180.0); 
  Control.addVariable("MagnetFeMat",5); 
  Control.addVariable("Magnet1SegIndex",3); 
  Control.addVariable("Magnet1SegLen",2); 

  Control.addVariable("Magnet2SegIndex",2); 
  Control.addVariable("Magnet2SegLen",2); 

  Control.addVariable("Magnet3SegIndex",7); 
  Control.addVariable("Magnet3SegLen",2); 
  Control.addVariable("Magnet4SegIndex",8); 
  Control.addVariable("Magnet4SegLen",2); 

  Control.addVariable("FocusXStep",0.0); 
  Control.addVariable("FocusXStep",0.0); 
  Control.addVariable("FocusYStep",0.0); 
  Control.addVariable("FocusZStep",0.0); 
  Control.addVariable("FocusXYangle",0.0); 
  Control.addVariable("FocusZangle",0.0); 
  Control.addVariable("FocusWidth",50.0); 
  Control.addVariable("FocusHeight",50.0); 
  Control.addVariable("FocusLength",100.0); 
  Control.addVariable("FocusFeMat",3); 

  Control.addVariable("Focus1SegIndex",4); 
  Control.addVariable("Focus1SegLen",2); 

  Control.addVariable("Focus2SegIndex",5); 
  Control.addVariable("Focus2SegLen",2); 

  Control.addVariable("Focus3SegIndex",6); 
  Control.addVariable("Focus3SegLen",2); 


  Control.addVariable("HallXStep",0.0); 
  Control.addVariable("HallYStep",0.0); 
  Control.addVariable("HallZStep",0.0); 
  Control.addVariable("HallXYangle",0.0); 
  Control.addVariable("HallZangle",0.0); 
  Control.addVariable("HallLength",2000.0); 
  Control.addVariable("HallWidth",400.0); 
  Control.addVariable("HallDepth",100.0); 
  Control.addVariable("HallHeight",600.0); 
  Control.addVariable("HallFloorThick",100.0); 
  Control.addVariable("HallWallThick",100.0); 
  Control.addVariable("HallRoofThick",100.0); 
  Control.addVariable("HallConcMat",67); 


  Control.addVariable("sdefEnergy",2500.0);  

  return;
}

}  // NAMESPACE setVariable
