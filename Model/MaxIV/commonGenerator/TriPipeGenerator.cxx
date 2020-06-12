/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/TriPipeGenerator.cxx
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

#include "TriPipeGenerator.h"

namespace setVariable
{

TriPipeGenerator::TriPipeGenerator() :
  axisXStep(0.0),axisZStep(0.0),
  axisXYAngle(0.0),axisZAngle(0.0),
  frontWidth(2.7),frontHeight(1.0),
  backWidth(2.7),backHeight(1.0),
  length(80.0),wallThick(0.75),
  flangeARadius(CF40::flangeRadius),
  flangeALength(CF40::flangeLength),
  flangeBRadius(CF40::flangeRadius),
  flangeBLength(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

TriPipeGenerator::~TriPipeGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
TriPipeGenerator::setAFlangeCF()
  /*!
    Set the front flange
  */
{
  flangeARadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  return;
}

template<typename CF>
void
TriPipeGenerator::setBFlangeCF()
  /*!
    Set the back flange
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

void
TriPipeGenerator::setXYWindow(const double LWA,const double RWA,
			      const double LWB,const double RWB)
  /*!
    Sets the width window based on +/- values of the width
    \param LWA :: left width front
    \param RWA :: right width front
    \param LWB :: left width back
    \param RWB :: right width back
  */
{
  ELog::RegMethod RegA("TriPipeGenerator","setXYWindow");

  frontWidth=LWA+RWA;
  backWidth=LWB+RWB;
  axisXStep=(RWA-LWA)/2.0;    // offset
  
  const double tanA=((RWB-RWA)/2.0-axisXStep)/length;
  axisXYAngle=atan(tanA);
  
  return;
}

void
TriPipeGenerator::generateTri(FuncDataBase& Control,
			      const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("TriPipeGenerator","generateFlat");

  Control.addVariable(keyName+"AxisXStep",axisXStep);
  Control.addVariable(keyName+"AxisZStep",axisZStep);   
  Control.addVariable(keyName+"AxisXYAngle",axisXYAngle);
  Control.addVariable(keyName+"AxisZAngle",axisZAngle);   

  Control.addVariable(keyName+"Length",length);   
  Control.addVariable(keyName+"FrontWidth",frontWidth);
  Control.addVariable(keyName+"FrontHeight",frontHeight);
  Control.addVariable(keyName+"BackWidth",backWidth);
  Control.addVariable(keyName+"BackHeight",backHeight);

  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  
  return;

}

///\cond TEMPLATE

template void TriPipeGenerator::setBFlangeCF<CF100>();
  
///\endcond TEMPLATE
  
}  // NAMESPACE setVariable
