/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BiPortGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "BiPortGenerator.h"

namespace setVariable
{

BiPortGenerator::BiPortGenerator() :
  radius(CF200::innerRadius),
  wallThick(CF200::wallThick),
  length(36.0),
  flangeRadius(CF200::flangeRadius),
  flangeLength(CF200::flangeLength),
  outLength(5.0),

  inPortRadius(CF50::innerRadius),
  inWallThick(CF50::wallThick),
  inFlangeRadius(CF50::flangeRadius),
  inFlangeLength(CF50::flangeLength),

  outPortRadius(CF50::innerRadius),
  outWallThick(CF50::wallThick),
  outFlangeRadius(CF50::flangeRadius),
  outFlangeLength(CF50::flangeLength),
  
  voidMat("Void"),wallMat("Stainless304"),
  capMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

  
BiPortGenerator::~BiPortGenerator() 
 /*!
   Destructor
 */
{}

  
template<typename CF>
void
BiPortGenerator::setCF(const double L)
  /*!
    Set pipe/flange to CF format
  */
{
  length=L;
  radius=CF::innerRadius;
  wallThick=CF::wallThick;
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}

template<typename CF>
void
BiPortGenerator::setFrontCF()
  /*!
    Set front ports to CF format
  */
{
  inPortRadius=CF::innerRadius;
  inWallThick=CF::wallThick;
  inFlangeRadius=CF::flangeRadius;
  inFlangeLength=CF::flangeLength;
  return;
}

template<typename CF>
void
BiPortGenerator::setBackCF(const double L)
  /*!
    Set front ports to CF format
  */
{
  outLength=L;
  outPortRadius=CF::outnerRadius;
  outWallThick=CF::wallThick;
  outFlangeRadius=CF::flangeRadius;
  outFlangeLength=CF::flangeLength;
  return;
}

void
BiPortGenerator::generateBPort(FuncDataBase& Control,
			       const std::string& keyName,
			       const double yStep) const
/*!
    Primary funciton for setting the variables
    It is expected that BiPort is places a set distance from
    a base object. The left/right link points will need to be 
    set in the control program before it can be built.

    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Distance from main fixed poit
  */
{
  ELog::RegMethod RegA("BiPortGenerator","generateColl");
  
  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"CapThick",capThick);
  
  Control.addVariable(keyName+"OutLength",outLength);
  Control.addVariable(keyName+"InPortRadius",inPortRadius);
  Control.addVariable(keyName+"InWallThick",inWallThick);
  Control.addVariable(keyName+"InFlangeRadius",inFlangeRadius);
  Control.addVariable(keyName+"InFlangeLength",inFlangeLength);

  Control.addVariable(keyName+"OutPortRadius",outPortRadius);
  Control.addVariable(keyName+"OutWallThick",outWallThick);
  Control.addVariable(keyName+"OutFlangeRadius",outFlangeRadius);
  Control.addVariable(keyName+"OutFlangeLength",outFlangeLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"CapMat",capMat);
  
  return;
}

template void BiPortGenerator::setCF<CF120>(const double);
template void BiPortGenerator::setCF<CF150>(const double);
template void BiPortGenerator::setCF<CF200>(const double);

  
}  // NAMESPACE setVariable
