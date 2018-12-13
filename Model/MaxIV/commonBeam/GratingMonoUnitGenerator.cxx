/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/GratingMonoUnitGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "GratingMonoUnitGenerator.h"

namespace setVariable
{

GratingMonoUnitGenerator::GratingMonoUnitGenerator() :
  gWidth(4.0),gThick(2.0),gLength(12.0),
  mainGap(12.5),mainBarXLen(36.0),mainBarDepth(6.0),
  mainBarYWidth(2.0),
  slidePlateThick(1.0),slidePlateLength(16.5),
  slidePlateGapWidth(4.0),slidePlateGapLength(1.0),
  sideThick(2.0),sideHeight(3.0),endWidth(3.0),
  endHeight(10.0),endThick(3.0),
  xstalMat("Silicon80K"), mainMat("Stainless304"),
  slideMat("Aluminium")
 /*!
    Constructor and defaults
  */
{}

  
GratingMonoUnitGenerator::~GratingMonoUnitGenerator() 
 /*!
   Destructor
 */
{}
				  
void
GratingMonoUnitGenerator::generateGrating(FuncDataBase& Control,
				      const std::string& keyName,
				      const double yStep,
				      const double angle) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param angle :: theta angle
  */
{
  ELog::RegMethod RegA("GratingMonoUnitGenerator","generateBox");

  for(size_t i=0;i<3;i++)
    {
      const std::string GKey=keyName+"Grate"+std::to_string(i);


      Control.addVariable(GKey+"XStep",
			  static_cast<double>(i)*(sideThick*3.0+gWidth));
      Control.addVariable(GKey+"Width",gWidth);
      Control.addVariable(GKey+"Thick",gThick);
      Control.addVariable(GKey+"Length",gLength);
      
      Control.addVariable(GKey+"SideThick",sideThick);
      Control.addVariable(GKey+"SideHeight",sideHeight);

      Control.addVariable(GKey+"EndWidth",endWidth);
      Control.addVariable(GKey+"EndThick",endThick);
      Control.addVariable(GKey+"EndHeight",endHeight);
      Control.addVariable(GKey+"XstalMat",xstalMat);
      Control.addVariable(GKey+"MainMat",mainMat);
      Control.addVariable(GKey+"SideMat",slideMat);
    }
  
  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"GrateTheta",angle);
  
  Control.addVariable(keyName+"GrateWidth",gWidth);
  Control.addVariable(keyName+"GrateThick",gThick);
  Control.addVariable(keyName+"GrateLength",gLength);

  Control.addVariable(keyName+"MainGap",mainGap);
  Control.addVariable(keyName+"MainBarXLen",mainBarXLen);
  Control.addVariable(keyName+"MainBarDepth",mainBarDepth);
  Control.addVariable(keyName+"MainBarYWidth",mainBarYWidth);

  Control.addVariable(keyName+"SlidePlateThick",slidePlateThick);
  Control.addVariable(keyName+"SlidePlateLength",slidePlateLength);
  Control.addVariable(keyName+"SlidePlateGapWidth",slidePlateGapWidth);
  Control.addVariable(keyName+"SlidePlateGapLength",slidePlateGapLength);

  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"SlideMat",slideMat);
       
  return;

}

  
}  // NAMESPACE setVariable
