/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   ts3Model/ts3Variables.cxx
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
TS3model(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("CentH2XStep",0.0);  
  Control.addVariable("CentH2YStep",0.0);  
  Control.addVariable("CentH2ZStep",0.0);
  Control.addVariable("CentH2XYangle",0.0); 
  Control.addVariable("CentH2Zangle",0.0);
  Control.addVariable("CentH2Radius",4.0);
  Control.addVariable("CentH2Height",10.0);
  Control.addVariable("CentH2Mat","ParaH2");
  Control.addVariable("CentH2Temp",20.0);
  Control.addVariable("CentH2NLayers",12);   // Inner counts as 1 
  // al layer
  Control.addVariable("CentH2HGap1",0.3);
  Control.addVariable("CentH2RadGap1",0.3);
  Control.addVariable("CentH2Material1","Aluminium");  // Al materk
  Control.addVariable("CentH2Temp1",20.0);  
  // Vac gap
  Control.addVariable("CentH2HGap2",0.5);
  Control.addVariable("CentH2RadGap2",0.5);
  Control.addVariable("CentH2Material2","Void"); 

  // Al layer
  Control.addVariable("CentH2HGap3",0.2);
  Control.addVariable("CentH2RadGap3",0.2);
  Control.addVariable("CentH2Material3","Aluminium"); 
  Control.addVariable("CentH2Temp3",4.0);  
  // He Layer
  Control.addVariable("CentH2HGap4",2.0);
  Control.addVariable("CentH2RadGap4",5.0);
  Control.addVariable("CentH2Material4","LiqHelium"); 
  Control.addVariable("CentH2Temp4",4.0);

  // Carbon Layer
  Control.addVariable("CentH2HGap5",1.0);
  Control.addVariable("CentH2RadGap5",1.0);
  Control.addVariable("CentH2Material5","Graphite"); 
  Control.addVariable("CentH2Temp5",4.0);
 
  // He Layer
  Control.addVariable("CentH2HGap6",0.5);
  Control.addVariable("CentH2RadGap6",0.5);
  Control.addVariable("CentH2Material6","LiqHelium"); 
  Control.addVariable("CentH2Temp6",4.0);

  // Al wall
  Control.addVariable("CentH2HGap7",0.3);
  Control.addVariable("CentH2RadGap7",0.3);
  Control.addVariable("CentH2Material7","Aluminium"); 
  Control.addVariable("CentH2Temp7",4.0);

    // Al wall
  Control.addVariable("CentH2HGap8",0.3);
  Control.addVariable("CentH2RadGap8",0.3);
  Control.addVariable("CentH2Material8","Void"); 

  // Al wall
  Control.addVariable("CentH2HGap9",0.2);
  Control.addVariable("CentH2RadGap9",0.2);
  Control.addVariable("CentH2Material9","Aluminium"); 
  Control.addVariable("CentH2Temp9",300.0);

  // clearance gap
  Control.addVariable("CentH2HGap10",0.6);
  Control.addVariable("CentH2RadGap10",0.6);
  Control.addVariable("CentH2Material10","H2O"); 

  Control.addVariable("CentH2Temp10",300.0); 
  // Reflector
  Control.addVariable("CentH2HGap11",8.0);
  Control.addVariable("CentH2RadGap11",10.0);
  Control.addVariable("CentH2Material11","Be300K");  
  Control.addVariable("CentH2Temp11",300.0); 

  Control.addVariable("CentH2NConic",0);
  Control.addVariable("CentH2NWedge",1);

  Control.addVariable("CentH2Wedge1Cent",Geometry::Vec3D(13,0,0));
  Control.addVariable("CentH2Wedge1XYangle",-90.0);
  Control.addVariable("CentH2Wedge1Zangle",0.0);
  Control.addVariable("CentH2Wedge1ViewWidth",6.0);
  Control.addVariable("CentH2Wedge1ViewHeight",3.0);
  Control.addVariable("CentH2Wedge1ViewXY",30.0);
  Control.addVariable("CentH2Wedge1ViewZ",0.0);
  Control.addVariable("CentH2Wedge1Mat","Void");
  Control.addVariable("CentH2Wedge1WallMat","Aluminium");
  Control.addVariable("CentH2Wedge1Wall",0.2);
  Control.addVariable("CentH2Wedge1Temp",20.0);


  Control.addVariable("WCut0Centre",Geometry::Vec3D(10,0,0));
  Control.addVariable("WCut0RotXYangle",90.0);
  Control.addVariable("WCut0Length",-1.0);
  Control.addVariable("WCut0Width",3.0);
  Control.addVariable("WCut0Height",3.0);
  Control.addVariable("WCutMat","LiqHelium");
  Control.addVariable("WCutMatTemp",4.0);

  
  return;
}

}  // NAMESPACE setVariable
