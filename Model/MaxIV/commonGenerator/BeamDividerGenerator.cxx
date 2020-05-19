/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/BeamDividerGenerator.cxx
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

#include "BeamDividerGenerator.h"

namespace setVariable
{

BeamDividerGenerator::BeamDividerGenerator() :
  boxLength(60.0),wallThick(0.2),
  mainWidth(1.0),exitWidth(1.0),
  height(3.0),mainXStep(-0.675),exitXStep(0.0),
  exitAngle(2.86),mainLength(93.0),
  mainRadius(CF40_22::innerRadius),mainThick(CF40_22::wallThick),
  exitLength(14.5), exitRadius(CF16::innerRadius),
  exitThick(CF16::wallThick),
  flangeARadius(CF63::innerRadius),flangeALength(CF63::flangeLength),
  flangeBRadius(CF40::innerRadius),flangeBLength(CF40::flangeLength),
  flangeERadius(CF40::innerRadius),flangeELength(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
BeamDividerGenerator::~BeamDividerGenerator() 
 /*!
   Destructor
 */
{}


template<typename CF>
void
BeamDividerGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  return;
}

template<typename CF>
void
BeamDividerGenerator::setBFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

template<typename CF>
void
BeamDividerGenerator::setEFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeERadius=CF::flangeRadius;
  flangeELength=CF::flangeLength;
  return;
}

    
void
BeamDividerGenerator::generateDivider(FuncDataBase& Control,
				      const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("BeamDividerGenerator","generateDivider");

  Control.addVariable(keyName+"BoxLength",boxLength);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"MainWidth",mainWidth);
  Control.addVariable(keyName+"ExitWidth",exitWidth);
  Control.addVariable(keyName+"Height",height);

  Control.addVariable(keyName+"MainXStep",mainXStep);
  Control.addVariable(keyName+"ExitXStep",exitXStep);
  Control.addVariable(keyName+"ExitAngle",exitAngle);

  Control.addVariable(keyName+"MainLength",mainLength);
  Control.addVariable(keyName+"MainRadius",mainRadius);
  Control.addVariable(keyName+"MainThick",mainThick);

  Control.addVariable(keyName+"ExitLength",exitLength);
  Control.addVariable(keyName+"ExitRadius",exitRadius);
  Control.addVariable(keyName+"ExitThick",exitThick);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);
  Control.addVariable(keyName+"FlangeERadius",flangeERadius);
  Control.addVariable(keyName+"FlangeELength",flangeELength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;

}

///\cond TEMPLATE


template void BeamDividerGenerator::setAFlangeCF<CF63>();
template void BeamDividerGenerator::setAFlangeCF<CF40>();

template void BeamDividerGenerator::setBFlangeCF<CF40_22>();
template void BeamDividerGenerator::setBFlangeCF<CF40>();

template void BeamDividerGenerator::setEFlangeCF<CF40_22>();
template void BeamDividerGenerator::setEFlangeCF<CF40>();

///\endcond TEMPLATE
  
}  // NAMESPACE setVariable
