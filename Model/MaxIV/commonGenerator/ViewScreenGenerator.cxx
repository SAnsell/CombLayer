/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/ViewScreenGenerator.cxx
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

#include "ViewScreenGenerator.h"

namespace setVariable
{

ViewScreenGenerator::ViewScreenGenerator() :
  radius(CF100::innerRadius),height(13.2),
  depth(8.8),wallThick(CF63::wallThick),
  plateThick(CF63::flangeLength),
  flangeRadius(CF100::flangeRadius),
  flangeLength(CF100::flangeLength),
  portARadius(CF40::innerRadius),
  portBRadius(CF100::innerRadius),
  portALength(CF100::outerRadius+7.0),
  portBLength(CF100::outerRadius+10.0),
  flangeARadius(CF40::flangeRadius),
  flangeALength(CF40::flangeLength),
  flangeBRadius(CF100::flangeRadius),
  flangeBLength(CF100::flangeLength),

  viewRadius(CF40::innerRadius),
  viewLength(CF100::outerRadius+4.5),
  viewFlangeRadius(CF40::flangeRadius),
  viewFlangeLength(CF40::flangeLength),
  viewPlateThick(CF40::flangeLength),

  voidMat("Void"),wallMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

ViewScreenGenerator::~ViewScreenGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
ViewScreenGenerator::setCF()
  /*!
    Setter for flange A
   */
{  
  return;
}

template<typename CF>
void
ViewScreenGenerator::setPortBCF()
  /*!
    Setter for port B
   */
{
  portBRadius=CF::innerRadius;
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

void
ViewScreenGenerator::generateView(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param flip    :: flag to flip front/back
  */
{
  ELog::RegMethod RegA("ViewScreenGenerator","generateView");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"PlateThick",plateThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"PortARadius",portARadius);
  Control.addVariable(keyName+"PortBRadius",portBRadius);
  Control.addVariable(keyName+"PortALength",portALength);
  Control.addVariable(keyName+"PortBLength",portBLength);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"ViewRadius",viewRadius);
  Control.addVariable(keyName+"ViewLength",viewLength);
  Control.addVariable(keyName+"ViewFlangeRadius",viewFlangeRadius);
  Control.addVariable(keyName+"ViewFlangeLength",viewFlangeLength);
  Control.addVariable(keyName+"ViewPlateThick",viewPlateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}

///\cond TEMPLATE

template void ViewScreenGenerator::setPortBCF<CF40>();

///\endcond TEPLATE

}  // NAMESPACE setVariable
