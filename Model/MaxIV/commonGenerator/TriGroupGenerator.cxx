/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/TriGroupGenerator.cxx
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

#include "TriGroupGenerator.h"

namespace setVariable
{

TriGroupGenerator::TriGroupGenerator() :
  mainWidth(0.86), mainHeight(1.8),
  mainLength(107.5), // No_44_00
  mainSideAngle(4.7),wallThick(0.10),flangeRadius(CF40::flangeRadius),
  flangeLength(CF40::flangeLength),

  topRadius(CF8_TDC::innerRadius),
  topLength(35.5), // No_44_00
  topWallThick(CF8_TDC::wallThick),topFlangeRadius(CF40::flangeRadius),
  topFlangeLength(CF40::flangeLength),

  midZAngle(2.8), midOpeningAngle(1.2),
  midLength(69.287),midHeight(5.7),
  midWidth(0.68),midThick(0.15),midFlangeRadius(CF63::flangeRadius),
  midFlangeLength(CF63::flangeLength),

  bendZAngle(4.6),bendZDrop(8.2),
  bendArcRadius(267.75),bendArcLength(168.0),
  bendStrLength(26.75),
  bendHeight(2.0),bendWidth(0.68),bendThick(0.1),
  bendFlangeRadius(CF63::flangeRadius),
  bendFlangeLength(CF63::flangeLength),

  voidMat("Void"),wallMat("Stainless316L"),
  flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

TriGroupGenerator::~TriGroupGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
TriGroupGenerator::setFlangeCF()
  /*!
    Set the front flange
  */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}

template<typename CF>
void
TriGroupGenerator::setTopFlangeCF()
  /*!
    Set the back flange
   */
{
  topFlangeRadius=CF::flangeRadius;
  topFlangeLength=CF::flangeLength;
  return;
}

template<typename CF>
void
TriGroupGenerator::setMidFlangeCF()
  /*!
    Set the back flange
   */
{
  midFlangeRadius=CF::flangeRadius;
  midFlangeLength=CF::flangeLength;
  return;
}

template<typename CF>
void
TriGroupGenerator::setBendFlangeCF()
  /*!
    Set the back flange
   */
{
  bendFlangeRadius=CF::flangeRadius;
  bendFlangeLength=CF::flangeLength;
  return;
}



void
TriGroupGenerator::generateTri(FuncDataBase& Control,
			      const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("TriGroupGenerator","generateFlat");

  Control.addVariable(keyName+"MainWidth",mainWidth);
  Control.addVariable(keyName+"MainHeight",mainHeight);
  Control.addVariable(keyName+"MainLength",mainLength);
  Control.addVariable(keyName+"MainSideAngle",mainSideAngle);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"TopRadius",topRadius);
  Control.addVariable(keyName+"TopLength",topLength);
  Control.addVariable(keyName+"TopWallThick",topWallThick);
  Control.addVariable(keyName+"TopFlangeRadius",topFlangeRadius);
  Control.addVariable(keyName+"TopFlangeLength",topFlangeLength);

  Control.addVariable(keyName+"MidZAngle",midZAngle);
  Control.addVariable(keyName+"MidOpeningAngle",midOpeningAngle);
  Control.addVariable(keyName+"MidLength",midLength);
  Control.addVariable(keyName+"MidHeight",midHeight);
  Control.addVariable(keyName+"MidWidth",midWidth);
  Control.addVariable(keyName+"MidThick",midThick);
  Control.addVariable(keyName+"MidFlangeRadius",midFlangeRadius);
  Control.addVariable(keyName+"MidFlangeLength",midFlangeLength);

  Control.addVariable(keyName+"BendZAngle",bendZAngle);
  Control.addVariable(keyName+"BendZDrop",bendZDrop);
  Control.addVariable(keyName+"BendArcRadius",bendArcRadius);
  Control.addVariable(keyName+"BendArcLength",bendArcLength);
  Control.addVariable(keyName+"BendStrLength",bendStrLength);
  Control.addVariable(keyName+"BendHeight",bendHeight);
  Control.addVariable(keyName+"BendWidth",bendWidth);
  Control.addVariable(keyName+"BendThick",bendThick);
  Control.addVariable(keyName+"BendFlangeRadius",bendFlangeRadius);
  Control.addVariable(keyName+"BendFlangeLength",bendFlangeLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;

}

///\cond TEMPLATE

template void TriGroupGenerator::setFlangeCF<CF100>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
