/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/ScreenGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "ScreenGenerator.h"

namespace setVariable
{

ScreenGenerator::ScreenGenerator() :
  juncBoxLength(11.5),juncBoxWidth(8.0),
  juncBoxHeight(8.0),juncBoxWallThick(0.3),
  
  feedLength(19.5),feedInnerRadius(0.95),
  feedWallThick(0.95),feedFlangeLen(1.2),
  feedFlangeRadius(3.5),

  threadLift(7.0),threadRadius(feedInnerRadius*0.7), // guess
  
  voidMat("Void"),
  juncBoxMat("StbTCABL%Void%50"), // guess
  juncBoxWallMat("Aluminium"),
  threadMat("Aluminium"),
  feedWallMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

ScreenGenerator::~ScreenGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
ScreenGenerator::setCF()
  /*!
    Set pipe and flange to CF-X format
  */
{
  feedInnerRadius=CF::innerRadius;
  feedWallThick=CF::wallThick;
  setFlangeCF<CF>();

  return;
}

template<typename CF>
void
ScreenGenerator::setFlangeCF()
  /*!
    Setter for flange
   */
{
  feedFlangeRadius=CF::flangeRadius;
  feedFlangeLen=CF::flangeLength;

  return;
}


void
ScreenGenerator::generateScreen(FuncDataBase& Control,
				const std::string& keyName,
				const bool inBeam) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param inBeam :: is item in beam
  */
{
  ELog::RegMethod RegA("ScreenGenerator","generate");

  Control.addVariable(keyName+"InBeam",static_cast<int>(inBeam));

  Control.addVariable(keyName+"JuncBoxLength",juncBoxLength);
  Control.addVariable(keyName+"JuncBoxWidth",juncBoxWidth);
  Control.addVariable(keyName+"JuncBoxHeight",juncBoxHeight);
  Control.addVariable(keyName+"JuncBoxWallThick",juncBoxWallThick);

  Control.addVariable(keyName+"FeedLength",feedLength);
  Control.addVariable(keyName+"FeedInnerRadius",feedInnerRadius);
  Control.addVariable(keyName+"FeedWallThick",feedWallThick);
  Control.addVariable(keyName+"FeedFlangeLen",feedFlangeLen);
  Control.addVariable(keyName+"FeedFlangeRadius",feedFlangeRadius);

  Control.addVariable(keyName+"ThreadLift",threadLift);
  Control.addVariable(keyName+"ThreadRadius",threadRadius);
    
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"JuncBoxMat",juncBoxMat);
  Control.addVariable(keyName+"JuncBoxWallMat",juncBoxWallMat);
  Control.addVariable(keyName+"ThreadMat",threadMat);
    
  Control.addVariable(keyName+"FeedWallMat",feedWallMat);
 
 return;

}

///\cond TEMPLATE

template void ScreenGenerator::setCF<CF40_22>();
template void ScreenGenerator::setFlangeCF<CF40_22>();

///\endcond TEMPLATE

}  // namespace setVariable
