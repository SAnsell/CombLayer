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

#include "ConnectorGenerator.h"

namespace setVariable
{


ConnectorGenerator::ConnectorGenerator() :
  radius(CF40::innerRadius),
  wallThick(CF40::wallThick),
  innerRadius(CF16::innerRadius),
  innerLength(0.75),
  outerRadius(CF63::innerRadius),
  outerLength(0.65),
  flangeRadius(CF40::flangeRadius),
  flangeLength(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

ConnectorGenerator::~ConnectorGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
ConnectorGenerator::setCF(const double L)
  /*!
    Setter for flange beam direction flanges
    \param L :: fractional length of total length
  */
{
  innerRadius=CF::innerRadius;
  innerLength=L;
  return;
}


void
ConnectorGenerator::setOuter(const double R,const double L)
  /*!
    Setter for flange beam direction flanges
    \param L :: fractional length of total length
  */
{
  outerRadius=R;
  outerLength=L;
  return;
}

template<typename CF>
void
ConnectorGenerator::setPortCF()
  /*!
    Setter for flange beam direction flanges
  */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}


void
ConnectorGenerator::setPort(const double R,const double L)
  /*!
    Setter for flange beam direction flanges
    \param R :: Radius
    \param l :: length
  */
{
  flangeRadius=R;
  flangeLength=L;
  return;
}

void
ConnectorGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName,
				 const double length) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("ConnectorGenerator","generatePipe");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"InnerLength",innerLength*length);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"OuterLength",outerLength*length);
  
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  
  return;
  
}

///\cond TEMPLATE

template void ConnectorGenerator::setPortCF<CF40>();



///\endcond TEPLATE

}  // NAMESPACE setVariable
