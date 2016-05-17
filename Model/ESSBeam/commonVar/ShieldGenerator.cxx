/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/ShieldGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "ShieldGenerator.h"

namespace essSystem
{

ShieldGenerator::ShieldGenerator() 
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

ShieldGenerator::~ShieldGenerator() 
 /*!
   Destructor
 */
{}

  /*
void
ShieldGenerator::generatorShield
  (FuncDataBase& Control,const double length,
   const double side,const double height,
   const size_t NLayer,const size_t 
 )
{

    Control.addVariable("dreamShieldALength",1750.0-320);
  Control.addVariable("dreamShieldALeft",40.0);
  Control.addVariable("dreamShieldARight",40.0);
  Control.addVariable("dreamShieldAHeight",40.0);
  Control.addVariable("dreamShieldADepth",40.0);
  Control.addVariable("dreamShieldADefMat","Stainless304");
  Control.addVariable("dreamShieldANSeg",8);
  Control.addVariable("dreamShieldANWallLayers",8);
  Control.addVariable("dreamShieldANFloorLayers",3);
  Control.addVariable("dreamShieldANRoofLayers",8);
  Control.addVariable("dreamShieldAWallLen1",20.0);
  Control.addVariable("dreamShieldAWallMat1","CastIron");
  Control.addVariable("dreamShieldAWallMat5","Concrete");
  Control.addVariable("dreamShieldARoofLen1",20.0);
  Control.addVariable("dreamShieldAFloorLen1",20.0);

}
  */  
}  // NAMESPACE essSystem
