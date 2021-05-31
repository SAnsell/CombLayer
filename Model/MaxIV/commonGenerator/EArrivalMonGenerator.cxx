/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/EArrivalMonGenerator.cxx
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

#include "EArrivalMonGenerator.h"

namespace setVariable
{

EArrivalMonGenerator::EArrivalMonGenerator() :
  radius(3.167), // No_2_00.pdf
  length(4.75),
  thick(1.0),faceThick(1.0),
  frontPipeILen(1.0),frontPipeLen(3.13),
  frontPipeRadius(1.0), // No_2_00.pdf
  frontPipeThick(0.8),backPipeILen(0.5),backPipeLen(3.13),
  backPipeRadius(1.0),backPipeThick(0.8),
  flangeRadius(CF40::flangeRadius),flangeLength(CF40::flangeLength),
  windowRotAngle(90.0),windowRadius(CF40_22::innerRadius),
  windowThick(0.7),voidMat("Void"),mainMat("Copper"),
  windowMat("SiO2"),flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

EArrivalMonGenerator::~EArrivalMonGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
EArrivalMonGenerator::setCF()
  /*!
    Setter for main radius / flange
  */
{
  radius=CF63::innerRadius;
  frontPipeRadius=CF::innerRadius;
  backPipeRadius=CF::innerRadius;
  setFlangeCF<CF>();
  return;
}

template<typename CF>
void
EArrivalMonGenerator::setFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}


void
EArrivalMonGenerator::generateMon(FuncDataBase& Control,
				  const std::string& keyName,
				  const double yStep)  const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("EArrivalMonGenerator","generateQuad");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"FaceThick",faceThick);

  Control.addVariable(keyName+"FrontPipeILen",frontPipeILen);
  Control.addVariable(keyName+"FrontPipeLen",frontPipeLen);
  Control.addVariable(keyName+"FrontPipeRadius",frontPipeRadius);
  Control.addVariable(keyName+"FrontPipeThick",frontPipeThick);

  Control.addVariable(keyName+"BackPipeILen",backPipeILen);
  Control.addVariable(keyName+"BackPipeLen",backPipeLen);
  Control.addVariable(keyName+"BackPipeRadius",backPipeRadius);
  Control.addVariable(keyName+"BackPipeThick",backPipeThick);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"WindowRotAngle",windowRotAngle);
  Control.addVariable(keyName+"WindowRadius",windowRadius);
  Control.addVariable(keyName+"WindowThick",windowThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"WindowMat",windowMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;

}

///\cond TEMPLATE

template void EArrivalMonGenerator::setCF<CF18_TDC>();
template void EArrivalMonGenerator::setCF<CF40_22>();
template void EArrivalMonGenerator::setCF<CF40>();

template void EArrivalMonGenerator::setFlangeCF<CF18_TDC>();
template void EArrivalMonGenerator::setFlangeCF<CF40_22>();
template void EArrivalMonGenerator::setFlangeCF<CF40>();

///\endcond TEPLATE

}  // NAMESPACE setVariable
