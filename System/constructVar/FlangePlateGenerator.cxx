/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/FlangePlateGenerator.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "FlangePlateGenerator.h"

namespace setVariable
{

FlangePlateGenerator::FlangePlateGenerator() :
  innerRadius(-1.0),
  flangeRadius(CF40::flangeRadius),
  flangeLength(CF40::flangeLength),
  innerMat("Void"),flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

FlangePlateGenerator::~FlangePlateGenerator()
 /*!
   Destructor
 */
{}


void
FlangePlateGenerator::setFlange(const double R,const double T)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
   */
{
  flangeRadius=R;
  flangeLength=T;
  return;
}


template<typename CF>
void
FlangePlateGenerator::setCF(const double R)
  /*!
    Set pipe/flange to CF-X format
    \param R :: inner radius
  */
{
  innerRadius=R;
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;

  return;
}

template<typename CF>
void
FlangePlateGenerator::setFlangeCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;

  return;
}

void
FlangePlateGenerator::setFlangeLen(const double L)
  /*!
    Set flange length alone
  */
{
  flangeLength=L;
  return;
}

void
FlangePlateGenerator::generateFlangePlate
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("FlangePlateGenerator","generatorFlangePlate");

  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;
}

///\cond TEMPLATE
  template void FlangePlateGenerator::setCF<CF16_TDC>(const double);
  template void FlangePlateGenerator::setCF<CF16>(const double);
  template void FlangePlateGenerator::setCF<CF18_TDC>(const double);
  template void FlangePlateGenerator::setCF<CF25>(const double);
  template void FlangePlateGenerator::setCF<CF40_22>(const double);
  template void FlangePlateGenerator::setCF<CF40>(const double);
  template void FlangePlateGenerator::setCF<CF50>(const double);
  template void FlangePlateGenerator::setCF<CF63>(const double);
  template void FlangePlateGenerator::setCF<CF66_TDC>(const double);
  template void FlangePlateGenerator::setCF<CF100>(const double);
  template void FlangePlateGenerator::setCF<CF120>(const double);
  template void FlangePlateGenerator::setCF<CF150>(const double);
  template void FlangePlateGenerator::setFlangeCF<CF25>();
  template void FlangePlateGenerator::setFlangeCF<CF40_22>();
  template void FlangePlateGenerator::setFlangeCF<CF63>();
  template void FlangePlateGenerator::setFlangeCF<CF66_TDC>();
  template void FlangePlateGenerator::setFlangeCF<CF100>();
  template void FlangePlateGenerator::setFlangeCF<CF120>();
  template void FlangePlateGenerator::setFlangeCF<CF150>();

///\endcond TEMPLATE


}  // NAMESPACE setVariable
