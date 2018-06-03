/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/JawFlangeGenerator.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "JawFlangeGenerator.h"

namespace setVariable
{

JawFlangeGenerator::JawFlangeGenerator() :
  radius(3.0),length(5.0)
  /*!
    Constructor and defaults
  */
{}

JawFlangeGenerator::~JawFlangeGenerator() 
 /*!
   Destructor
 */
{}


void
JawFlangeGenerator::setSlits(const double W,const double H,
			     const double T,const std::string& Mat)
  /*!
    Set the jaw valve generator
    \param W :: Width					       
    \param H :: Height
    \param T :: thickness
    \param Mat :: Material
   */
{
  jawWidth=W;
  jawHeight=H;
  jawThick=T;
  jawMat=Mat;
  return;
}

				  
void
JawFlangeGenerator::generateFlange(FuncDataBase& Control,
				   const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("JawFlangeGenerator","generateFlange");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);

  // stuff for jaws:
  const std::string jawName=keyName+"Jaw";
  Control.addVariable(jawName+"Gap",jawGap);
  Control.addVariable(jawName+"XOpen",xOpen);
  Control.addVariable(jawName+"XOpen",xOpen);
  Control.addVariable(jawName+"XOffset",0.0);
  Control.addVariable(jawName+"XThick",jawThick);
  Control.addVariable(jawName+"XHeight",jawWidth);  // note reverse
  Control.addVariable(jawName+"XWidth",jawHeight);  // note reverse

  Control.addVariable(jawName+"ZOpen",zOpen);
  Control.addVariable(jawName+"ZOffset",0.0);
  Control.addVariable(jawName+"ZThick",jawThick);
  Control.addVariable(jawName+"ZHeight",jawHeight); 
  Control.addVariable(jawName+"ZWidth",jawWidth);  
  
  
  Control.addVariable(jawName+"XJawMat",jawMat);
  Control.addVariable(jawName+"ZJawMat",jawMat);

       
  return;

}
  
}  // NAMESPACE setVariable
