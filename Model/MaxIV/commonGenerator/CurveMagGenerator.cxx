/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CurveMagGenerator.cxx
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

#include "CurveMagGenerator.h"

namespace setVariable
{

CurveMagGenerator::CurveMagGenerator() :

  poleGap(1.1),poleHeight(2.4),poleAngle(30.0),

  coilGap(3.65),coilArcRadius(555.127),
  coilArcLength(240.0),
  coilDepth(15.0),coilWidth(8.0),
  
  supportGap(20.4),supportRadius(267.75),
  supportDepth(25.0),supportBase(12.0),
  
  poleMat("Iron"),coilMat("Copper"),supportMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

CurveMagGenerator::CurveMagGenerator(const CurveMagGenerator& A) : 
  poleGap(A.poleGap),poleHeight(A.poleHeight),poleAngle(A.poleAngle),
  coilGap(A.coilGap),coilArcRadius(A.coilArcRadius),
  coilDepth(A.coilDepth),coilWidth(A.coilWidth),
  supportGap(A.supportGap),supportRadius(A.supportRadius),
  supportTop(A.supportTop),supportDepth(A.supportDepth),
  supportBase(A.supportBase),poleMat(A.poleMat),
  coilMat(A.coilMat),supportMat(A.supportMat)
  /*!
    Copy constructor
    \param A :: CurveMagGenerator to copy
  */
{}

CurveMagGenerator&
CurveMagGenerator::operator=(const CurveMagGenerator& A)
  /*!
    Assignment operator
    \param A :: CurveMagGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      poleGap=A.poleGap;
      poleHeight=A.poleHeight;
      poleAngle=A.poleAngle;
      coilGap=A.coilGap;
      coilArcRadius=A.coilArcRadius;
      coilDepth=A.coilDepth;
      coilWidth=A.coilWidth;
      supportGap=A.supportGap;
      supportRadius=A.supportRadius;
      supportTop=A.supportTop;
      supportDepth=A.supportDepth;
      supportBase=A.supportBase;
      poleMat=A.poleMat;
      coilMat=A.coilMat;
      supportMat=A.supportMat;
    }
  return *this;
}

CurveMagGenerator::~CurveMagGenerator()
 /*!
   Destructor
 */
{}

void
CurveMagGenerator::generateMag(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("CurveMagGenerator","generate");

  Control.addVariable(keyName+"PoleGap",poleGap);
  Control.addVariable(keyName+"PoleHeight",poleHeight);
  Control.addVariable(keyName+"PoleAngle",poleAngle);

  Control.addVariable(keyName+"CoilGap",coilGap);
  Control.addVariable(keyName+"CoilArcRadius",coilArcRadius);
  Control.addVariable(keyName+"CoilArcLength",coilArcLength);
  Control.addVariable(keyName+"CoilDepth",coilDepth);
  Control.addVariable(keyName+"CoilWidth",coilWidth);

  Control.addVariable(keyName+"SupportGap",supportGap);
  Control.addVariable(keyName+"SupportRadius",supportRadius);
  Control.addVariable(keyName+"SupportTop",supportTop);
  Control.addVariable(keyName+"SupportDepth",supportDepth);
  Control.addVariable(keyName+"SupportBase",supportBase);

  Control.addVariable(keyName+"PoleMat",poleMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"SupportMat",supportMat);
  
  return;

}


}  // xraySystem setVariable
