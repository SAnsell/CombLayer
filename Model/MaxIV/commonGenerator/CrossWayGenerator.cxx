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

#include "CrossWayGenerator.h"

namespace setVariable
{


CrossWayGenerator::CrossWayGenerator() :
  radius(CF63::innerRadius),
  xRadius(CF40::innerRadius),
  yRadius(CF40::innerRadius),
  wallThick(CF63::wallThick),
  height(12.5),depth(8.3),
  frontLength(6.8),backLength(6.8),sideLength(6.8),
  flangeXRadius(CF40::flangeRadius),
  flangeYRadius(CF40::flangeRadius),
  flangeZRadius(CF63::flangeRadius),
  flangeXLength(CF40::flangeLength),
  flangeYLength(CF40::flangeLength),
  flangeZLength(CF63::flangeLength),
  plateThick(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304"),
  plateMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

CrossWayGenerator::~CrossWayGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
CrossWayGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius=CF::innerRadius;
  xRadius=CF::innerRadius;
  yRadius=CF::innerRadius;
  wallThick=CF::wallThick;
  
  setFlangeCF<CF>();
  return;
}

template<typename CF>
void
CrossWayGenerator::setFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeXRadius=CF::flangeRadius;
  flangeYLength=CF::flangeLength;
  flangeZRadius=CF::flangeRadius;
  flangeXLength=CF::flangeLength;
  flangeYRadius=CF::flangeRadius;
  flangeZLength=CF::flangeLength;
  return;
}

void
CrossWayGenerator::setMainLength(const double F,const double B)
  /*!
    Setter for front / back port length
    \param F :: Front length
    \param B :: Back length
  */
{
  frontLength=F;
  backLength=B;
  return;
}
  
void
CrossWayGenerator::generateCrossWay(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("CrossWayGenerator","generateCrossWay");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"XRadius",xRadius);
  Control.addVariable(keyName+"YRadius",yRadius);
  Control.addVariable(keyName+"WallThick",wallThick);
  
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"SideLength",sideLength);

  Control.addVariable(keyName+"FlangeXRadius",flangeXRadius);
  Control.addVariable(keyName+"FlangeYRadius",flangeYRadius);
  Control.addVariable(keyName+"FlangeZRadius",flangeZRadius);
  
  Control.addVariable(keyName+"FlangeXLength",flangeXLength);
  Control.addVariable(keyName+"FlangeYLength",flangeYLength);
  Control.addVariable(keyName+"FlangeZLength",flangeZLength);

  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void CrossWayGenerator::setCF<CF63>();
template void CrossWayGenerator::setCF<CF100>();
template void CrossWayGenerator::setCF<CF120>();
template void CrossWayGenerator::setCF<CF150>();

template void CrossWayGenerator::setFlangeCF<CF100>();
template void CrossWayGenerator::setFlangeCF<CF120>();
template void CrossWayGenerator::setFlangeCF<CF150>();


///\endcond TEPLATE
  
}  // NAMESPACE setVariable
