/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/nmx/NMXvariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
NMXvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for nmx
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("NMXvariables[F]","NMXvariables");

  
  Control.addVariable("nmxAxisXStep",0.0);
  Control.addVariable("nmxAxisYStep",0.0);
  Control.addVariable("nmxAxisZStep",0.0);
  Control.addVariable("nmxAxisXYAngle",0.0);   // rotation 
  Control.addVariable("nmxAxisZAngle",0.0);

  
  // FIRST SECTION
  Control.addVariable("nmxGAXStep",0.0);       
  Control.addVariable("nmxGAYStep",0.0);       
  Control.addVariable("nmxGAZStep",0.0);       
  Control.addVariable("nmxGAXYAngle",0.0);
  Control.addVariable("nmxGAZAngle",0.0);
  Control.addVariable("nmxGABeamXYAngle",-1.0);       

  Control.addVariable("nmxGALength",350.0);       
  Control.addVariable("nmxGANShapes",1);       
  Control.addVariable("nmxGANShapeLayers",3);
  Control.addVariable("nmxGAActiveShield",0);

  Control.addVariable("nmxGALayerThick1",0.4);  // glass thick
  Control.addVariable("nmxGALayerThick2",1.5);

  Control.addVariable("nmxGALayerMat0","Void");
  Control.addVariable("nmxGALayerMat1","Glass");
  Control.addVariable("nmxGALayerMat2","Void");       

  Control.addVariable("nmxGA0TypeID","Tapper");

  Control.addVariable("nmxGA0HeightStart",3.0);
  Control.addVariable("nmxGA0HeightEnd",4.5);
  Control.addVariable("nmxGA0WidthStart",3.0);
  Control.addVariable("nmxGA0WidthEnd",3.0);
  Control.addVariable("nmxGA0Length",350.0);
  Control.addVariable("nmxGA0ZAngle",0.0);
    
  Control.addVariable("nmxBAXStep",0.0);       
  Control.addVariable("nmxBAYStep",50.0);       
  Control.addVariable("nmxBAZStep",0.0);       
  Control.addVariable("nmxBAXYAngle",0.0);
  Control.addVariable("nmxBAZAngle",0.0);
  Control.addVariable("nmxBABeamXYAngle",0.0);       

  Control.addVariable("nmxBALength",1849.0);       
  Control.addVariable("nmxBANShapes",1);       
  Control.addVariable("nmxBANShapeLayers",3);
  Control.addVariable("nmxBAActiveShield",0);

  Control.addVariable("nmxBALayerThick1",0.4);  // glass thick
  Control.addVariable("nmxBALayerThick2",1.5);

  Control.addVariable("nmxBALayerMat0","Void");
  Control.addVariable("nmxBALayerMat1","Glass");
  Control.addVariable("nmxBALayerMat2","Void");       

  Control.addVariable("nmxBA0TypeID","Bend");
  Control.addVariable("nmxBA0AHeight",4.50);
  Control.addVariable("nmxBA0BHeight",4.50);
  Control.addVariable("nmxBA0AWidth",3.0);
  Control.addVariable("nmxBA0BWidth",3.0);
  Control.addVariable("nmxBA0AngDir",0.0);
  Control.addVariable("nmxBA0Radius",60000.0);
  Control.addParse<double>("nmxBA0Length","nmxBALength");

  // VACUUM PIPES:
  Control.addVariable("nmxPipeAYStep",50.0);
  Control.addVariable("nmxPipeARadius",12.0);
  Control.addVariable("nmxPipeALength",700.0);
  Control.addVariable("nmxPipeAFeThick",1.0);
  Control.addVariable("nmxPipeAFlangeRadius",14.0);
  Control.addVariable("nmxPipeAFlangeLength",1.0);
  Control.addVariable("nmxPipeAFeMat","Aluminium");
  Control.addVariable("nmxPipeAVoidMat","Void");
  
  // VACUUM PIPES:
  Control.addVariable("nmxPipeBXYAngle",-1.2);   // from -0.8
  Control.addVariable("nmxPipeBRadius",12.0);
  Control.addVariable("nmxPipeBLength",500.0);
  Control.addVariable("nmxPipeBFeThick",1.0);
  Control.addVariable("nmxPipeBFlangeRadius",16.0);
  Control.addVariable("nmxPipeBFlangeLength",1.0);
  Control.addVariable("nmxPipeBFeMat","Aluminium");
  Control.addVariable("nmxPipeBVoidMat","Void");

  // BEAM INSERT:
  Control.addVariable("nmxBInsertHeight",20.0);
  Control.addVariable("nmxBInsertWidth",28.0);
  Control.addVariable("nmxBInsertTopWall",1.0);
  Control.addVariable("nmxBInsertLowWall",1.0);
  Control.addVariable("nmxBInsertLeftWall",1.0);
  Control.addVariable("nmxBInsertRightWall",1.0);
  Control.addVariable("nmxBInsertWallMat","Stainless304");       
  
    // Guide in wall
  Control.addVariable("nmxFWallXStep",0.0);       
  Control.addVariable("nmxFWallYStep",0.0);       
  Control.addVariable("nmxFWallZStep",0.0);       
  Control.addVariable("nmxFWallXYAngle",0.0);       
  Control.addVariable("nmxFWallZAngle",0.0);
  Control.addVariable("nmxFWallLength",318.0);       
  
  Control.addVariable("nmxFWallBeamYStep",1.5);
 
  Control.addVariable("nmxFWallNShapes",1);       
  Control.addVariable("nmxFWallNShapeLayers",3);
  Control.addVariable("nmxFWallActiveShield",0);

  Control.addVariable("nmxFWallLayerThick1",0.4);  // glass thick
  Control.addVariable("nmxFWallLayerThick2",1.5);

  Control.addVariable("nmxFWallLayerMat0","Void");
  Control.addVariable("nmxFWallLayerMat1","Glass");
  Control.addVariable("nmxFWallLayerMat2","Void");       
  
  Control.addVariable("nmxFWall0TypeID","Rectangle");
  Control.addVariable("nmxFWall0Height",6.0);
  Control.addVariable("nmxFWall0Width",6.0);
  Control.addVariable("nmxFWall0Length",318.0);


  Control.addVariable("nmxShieldALength",1750.0);
  Control.addVariable("nmxShieldALeft",40.0);
  Control.addVariable("nmxShieldARight",40.0);
  Control.addVariable("nmxShieldAHeight",40.0);
  Control.addVariable("nmxShieldADepth",40.0);
  Control.addVariable("nmxShieldADefMat","Stainless304");
  Control.addVariable("nmxShieldANSeg",8);
  Control.addVariable("nmxShieldANWallLayers",8);
  Control.addVariable("nmxShieldANFloorLayers",3);
  Control.addVariable("nmxShieldANRoofLayers",8);
  Control.addVariable("nmxShieldAWallLen1",10.0);
  Control.addVariable("nmxShieldAWallMat1","CastIron");
  Control.addVariable("nmxShieldAWallMat5","Concrete");
  Control.addVariable("nmxShieldARoofLen1",10.0);
  Control.addVariable("nmxShieldAFloorLen1",10.0);

  return;
}

}  // NAMESPACE setVariable
