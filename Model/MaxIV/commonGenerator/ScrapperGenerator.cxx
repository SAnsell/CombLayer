/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/ScrapperGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "CFFlanges.h"

#include "ScrapperGenerator.h"

namespace setVariable
{

ScrapperGenerator::ScrapperGenerator() :
  radius(CF63::innerRadius),length(27.0),
  wallThick(CF63::wallThick),
  flangeRadius(CF63::flangeRadius),
  flangeLength(CF63::flangeLength),
  tubeAYStep(-2.0),tubeBYStep(2.0),
  tubeRadius(CF63::innerRadius),tubeLength(10.3),
  tubeThick(CF63::wallThick),
  tubeFlangeRadius(CF63::flangeRadius),
  tubeFlangeLength(CF63::flangeLength),
  scrapperRadius(CF63::innerRadius-0.2),
  scrapperHeight(6.0),driveRadius(0.4),
  driveFlangeRadius(CF40::flangeRadius),
  driveFlangeLength(CF40::flangeLength),
  supportRadius(1.8),supportThick(0.1),
  supportHeight(17.0), topBoxWidth(5.5),topBoxHeight(1.5),
  voidMat("Void"),tubeMat("Stainless304L"),
  flangeMat("Stainless304L"),scrapperMat("Tungsten"),
  driveMat("Nickel"),topMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

ScrapperGenerator::~ScrapperGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
ScrapperGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius=CF::innerRadius;
  wallThick=CF::wallThick;
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
}

void
ScrapperGenerator::generateScrapper (FuncDataBase& Control,
				     const std::string& keyName,
				     const double ZLift) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param ZLift :: lift of main scrapper from beam centre line
  */
{
  ELog::RegMethod RegA("ScrapperGenerator","generateScrapper");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"TubeAYStep",tubeAYStep);
  Control.addVariable(keyName+"TubeBYStep",tubeBYStep);
  Control.addVariable(keyName+"TubeRadius",tubeRadius);
  Control.addVariable(keyName+"TubeLength",tubeLength);
  Control.addVariable(keyName+"TubeThick",tubeThick);
  Control.addVariable(keyName+"TubeFlangeRadius",tubeFlangeRadius);
  Control.addVariable(keyName+"TubeFlangeLength",tubeFlangeLength);

  Control.addVariable(keyName+"ScrapperRadius",scrapperRadius);
  Control.addVariable(keyName+"ScrapperHeight",scrapperHeight);
  Control.addVariable(keyName+"ScrapperZLift",ZLift);

  Control.addVariable(keyName+"DriveRadius",driveRadius);
  Control.addVariable(keyName+"DriveFlangeRadius",driveFlangeRadius);
  Control.addVariable(keyName+"DriveFlangeLength",driveFlangeLength);  

  Control.addVariable(keyName+"SupportRadius",supportRadius);
  Control.addVariable(keyName+"SupportThick",supportThick);
  Control.addVariable(keyName+"SupportHeight",supportHeight);

  Control.addVariable(keyName+"TopBoxWidth",topBoxWidth);
  Control.addVariable(keyName+"TopBoxHeight",topBoxHeight);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"TubeMat",tubeMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"ScrapperMat",scrapperMat);
  Control.addVariable(keyName+"DriveMat",driveMat);
  Control.addVariable(keyName+"TopMat",topMat);


  return;

}

///\cond TEMPLATE

template void ScrapperGenerator::setCF<CF40_22>();
template void ScrapperGenerator::setCF<CF40>();


///\endcond TEMPLATE

}  // NAMESPACE setVariable
