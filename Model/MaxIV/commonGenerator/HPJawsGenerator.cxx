/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/HRJawsGenerator.cxx
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

#include "HPJawsGenerator.h"

namespace setVariable
{


HPJawsGenerator::HPJawsGenerator() :
  radius(CF150::flangeRadius-1.5),
  length(10.5),sideThick(1.5),
  wallThick(1.5),
  flangeInnerRadius(CF40::innerRadius),
  flangeRadius(CF40::flangeRadius),
  flangeLength(CF40::flangeLength),
  dividerThick(1.5),
  dividerGap(0.7),
  jawFarLen(3.5),
  jawEdgeLen(4.5),
  jawThick(0.8),
  jawCornerEdge(2.5),
  jawCornerFar(1.0),
  voidMat("Void"),wallMat("Stainless304L"),
  jawMat("Tungsten")
  /*!
    Constructor and defaults
  */
{} 

HPJawsGenerator::~HPJawsGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
HPJawsGenerator::setPortCF()
  /*!
    Setter for flange beam direction flanges
  */
{
  flangeInnerRadius=CF::innerRadius;
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}

void
HPJawsGenerator::generateJaws(FuncDataBase& Control,
			      const std::string& keyName,
			      const double xGap,
			      const double zGap) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("HPJawsGenerator","generateHPJaws");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"SideThick",sideThick);
  Control.addVariable(keyName+"WallThick",wallThick);
  
  Control.addVariable(keyName+"FlangeInnerRadius",flangeInnerRadius);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"DividerThick",dividerThick);
  Control.addVariable(keyName+"DividerGap",dividerGap);

  Control.addVariable(keyName+"JawFarLen",jawFarLen);
  Control.addVariable(keyName+"JawEdgeLen",jawEdgeLen);
  Control.addVariable(keyName+"JawThick",jawThick);
  Control.addVariable(keyName+"JawCornerEdge",jawCornerEdge);
  Control.addVariable(keyName+"JawCornerFar",jawCornerFar);
  Control.addVariable(keyName+"JawXGap",xGap);
  Control.addVariable(keyName+"JawZGap",zGap);

  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"JawMat",jawMat);
  
  return;
  
}

///\cond TEMPLATE


template void HPJawsGenerator::setPortCF<CF40>();


///\endcond TEPLATE

}  // NAMESPACE setVariable
