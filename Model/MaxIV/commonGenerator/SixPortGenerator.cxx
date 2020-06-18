/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/SixPortGenerator.cxx
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

#include "SixPortGenerator.h"

namespace setVariable
{

SixPortGenerator::SixPortGenerator() :
  radius(CF100::innerRadius),linkRadius(CF100::innerRadius),
  wallThick(CF100::wallThick),
  frontLength(22.0),backLength(22.0),
  sideLength(22.0),
  flangeARadius(CF100::flangeRadius),
  flangeBRadius(CF100::flangeRadius),
  flangeSRadius(CF100::flangeRadius),
  flangeALength(CF100::flangeLength),
  flangeBLength(CF100::flangeLength),
  flangeSLength(CF100::flangeLength),
  plateThick(CF100::flangeLength),
  voidMat("Void"),mainMat("Stainless304"),
  flangeMat("Stainless304"),plateMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
SixPortGenerator::~SixPortGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
SixPortGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius=CF::innerRadius;
  linkRadius=CF::innerRadius;
  wallThick=CF::wallThick;
  
  setFlangeCF<CF>();
  return;
}

template<typename CF>
void
SixPortGenerator::setFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  flangeSRadius=CF::flangeRadius;
  flangeSLength=CF::flangeLength;
  return;
}

void
SixPortGenerator::generateSixPort(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("SixPortGenerator","generateSixPort");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"LinkRadius",linkRadius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"SideLength",sideLength);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeSRadius",flangeSRadius);
  
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);
  Control.addVariable(keyName+"FlangeSLength",flangeSLength);

  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void SixPortGenerator::setCF<CF100>();
template void SixPortGenerator::setCF<CF120>();
template void SixPortGenerator::setCF<CF150>();

template void SixPortGenerator::setFlangeCF<CF100>();
template void SixPortGenerator::setFlangeCF<CF120>();
template void SixPortGenerator::setFlangeCF<CF150>();


///\endcond TEPLATE
  
}  // NAMESPACE setVariable
