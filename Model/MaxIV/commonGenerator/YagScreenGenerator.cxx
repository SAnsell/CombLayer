/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/YagScreenGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "YagScreenGenerator.h"

namespace setVariable
{

YagScreenGenerator::YagScreenGenerator() :
  jbLength(11.5),jbWidth(8.0),jbHeight(8.0),
  jbWallThick(0.3),
  jbWallMat("Aluminium"),
  jbMat("StbTCABL%Void%50"), // guess
  ffLength(19.5),
  ffInnerRadius(0.95),
  ffWallThick(0.95),
  ffFlangeLen(1.2),
  ffFlangeRadius(3.5),
  ffWallMat("Stainless304L"),
  threadLift(7.0),
  threadRad(ffInnerRadius*0.7), // guess
  threadMat("Aluminium"),
  mirrorRadius(1.75),
  mirrorAngle(-45.0),
  mirrorThick(0.5),
  mirrorMat("SiO2"),
  screenRadius(0.9), // yag-drawings.pdf, drawing #201375,
  screenHolderRadius(1.3),
  screenHolderThick(0.2), // measured,
  screenHolderMat("Stainless304L"),
  voidMat("Void"),
  closed(false)
  /*!
    Constructor and defaults
  */
{}

YagScreenGenerator::~YagScreenGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
YagScreenGenerator::setCF()
  /*!
    Set pipe and flange to CF-X format
  */
{
  ffInnerRadius=CF::innerRadius;
  ffWallThick=CF::wallThick;
  setFlangeCF<CF>();

  return;
}

template<typename CF>
void
YagScreenGenerator::setFlangeCF()
  /*!
    Setter for flange
   */
{
  ffFlangeRadius=CF::flangeRadius;
  ffFlangeLen=CF::flangeLength;

  return;
}


void
YagScreenGenerator::generateScreen(FuncDataBase& Control,
				   const std::string& keyName,
				   const bool inBeam) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param inBeam :: is item in beam
  */
{
  ELog::RegMethod RegA("YagScreenGenerator","generate");

  
  Control.addVariable(keyName+"InBeam",static_cast<int>(inBeam));
  
  Control.addVariable(keyName+"JBLength",jbLength);
  Control.addVariable(keyName+"JBWidth",jbWidth);
  Control.addVariable(keyName+"JBHeight",jbHeight);
  Control.addVariable(keyName+"JBWallThick",jbWallThick);
  Control.addVariable(keyName+"FFLength",ffLength);
  Control.addVariable(keyName+"FFInnerRadius",ffInnerRadius);
  Control.addVariable(keyName+"FFWallThick",ffWallThick);
  Control.addVariable(keyName+"FFFlangeLength",ffFlangeLen);
  Control.addVariable(keyName+"FFFlangeRadius",ffFlangeRadius);
  Control.addVariable(keyName+"FFWallMat",ffWallMat);
  Control.addVariable(keyName+"ThreadLift",threadLift);
  Control.addVariable(keyName+"ThreadRadius",threadRad);
  Control.addVariable(keyName+"ThreadMat",threadMat);
  Control.addVariable(keyName+"MirrorRadius",mirrorRadius);
  Control.addVariable(keyName+"MirrorAngle",mirrorAngle);
  Control.addVariable(keyName+"MirrorThick",mirrorThick);
  Control.addVariable(keyName+"MirrorMat",mirrorMat);
  Control.addVariable(keyName+"ScreenRadius",screenRadius);
  Control.addVariable(keyName+"ScreenHolderRadius",screenHolderRadius);
  Control.addVariable(keyName+"ScreenHolderThick",screenHolderThick);
  Control.addVariable(keyName+"ScreenHolderMat",screenHolderMat);
  Control.addVariable(keyName+"JBWallMat",jbWallMat);
  Control.addVariable(keyName+"JBMat",jbMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}

///\cond TEMPLATE

  template void YagScreenGenerator::setCF<CF40_22>();
  template void YagScreenGenerator::setFlangeCF<CF40_22>();

  ///\endcond TEMPLATE

}  // namespace setVariable
