/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/WallLeadGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#include "WallLeadGenerator.h"

namespace setVariable
{

WallLeadGenerator::WallLeadGenerator() :
  xStep(0.0),frontLength(10.0),backLength(10.0),
  frontWidth(80.0),frontHeight(100.0),
  backWidth(40.0),backHeight(60.0),
  steelOutWidth(145.0),steelRingWidth(140.0),
  steelHeight(70.0),steelDepth(60.0),
  steelThick(5.0),steelXCut(90.0),steelZCut(70.0),
  extraLeadOutWidth(140.0),extraLeadRingWidth(140.0),
  extraLeadHeight(72.0),extraLeadDepth(8.0),extraLeadXCut(50.0),
  voidMat("Void"),midMat("Concrete"),wallMat("Lead"),
  steelMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

WallLeadGenerator::~WallLeadGenerator() 
 /*!
   Destructor
 */
{}

void
WallLeadGenerator::setXOffset(const double XS)
  /*!
    Set the wall offset 
    \param XS :: Wall step
   */
{
  xStep=XS;
  return;
}
  
void
WallLeadGenerator::setWidth(const double OW,const double RW)
  /*!
    Set the widths of the first two objects
    \param OW :: Outer Width
    \param RW :: Ring Width
   */
{
  extraLeadOutWidth=OW;
  steelOutWidth=OW;

  extraLeadRingWidth=RW;
  steelRingWidth=RW;
  return;
}
  
void
WallLeadGenerator::generateWall(FuncDataBase& Control,
				const std::string& keyName,
				const double VR) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param VR :: void radius 						
  */
{
  ELog::RegMethod RegA("WallLeadGenerator","generateWall");
  
  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"FrontWidth",frontWidth);
  Control.addVariable(keyName+"FrontHeight",frontHeight);
  Control.addVariable(keyName+"BackWidth",backWidth);
  Control.addVariable(keyName+"BackHeight",backHeight);
  Control.addVariable(keyName+"SteelOutWidth",steelOutWidth+xStep);
  Control.addVariable(keyName+"SteelRingWidth",steelRingWidth-xStep);
  Control.addVariable(keyName+"SteelHeight",steelHeight);
  Control.addVariable(keyName+"SteelDepth",steelDepth);
  Control.addVariable(keyName+"SteelThick",steelThick);
  Control.addVariable(keyName+"SteelXCut",steelXCut);
  Control.addVariable(keyName+"SteelZCut",steelZCut);
  Control.addVariable(keyName+"ExtraLeadOutWidth",extraLeadOutWidth+xStep);
  Control.addVariable(keyName+"ExtraLeadRingWidth",extraLeadRingWidth-xStep);
  Control.addVariable(keyName+"ExtraLeadHeight",extraLeadHeight);
  Control.addVariable(keyName+"ExtraLeadDepth",extraLeadDepth);
  Control.addVariable(keyName+"ExtraLeadXCut",extraLeadXCut);
  Control.addVariable(keyName+"VoidRadius",VR);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MidMat",midMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"SteelMat",steelMat);
 
  return;

}

  
}  // NAMESPACE setVariable
