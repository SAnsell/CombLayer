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
  radius(CF63::innerRadius),height(13.2),
  depth(6.8),wallThick(CF63::wallThick),
  flangeRadius(CF63::flangeRadius),
  flangeLength(CF63::flangeLength),
  plateThick(CF63::flangeLength),
  viewZStep(3.2),viewRadius(CF63::innerRadius),
  viewLength(9.27),viewThick(CF63::wallThick),
  viewFlangeRadius(CF63::flangeRadius),
  viewFlangeLength(CF63::flangeLength),
  viewPlateThick(CF63::flangeLength),
  portRadius(CF40::innerRadius),portThick(CF40::wallThick),
  portFlangeRadius(CF40::flangeRadius),
  portFlangeLength(CF40::flangeLength),
  frontLength(9.0),backLength(11.2),
  outerRadius(CF63::flangeRadius*1.2),
  voidMat("Void"),mainMat("Stainless304")
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
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
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
  
  
  return;

}

///\cond TEMPLATE

template void SixPortGenerator::setCF<CF100>();
template void SixPortGenerator::setCF<CF120>();



///\endcond TEPLATE
  
}  // NAMESPACE setVariable
