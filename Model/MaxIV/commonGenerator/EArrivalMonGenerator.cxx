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

#include "EArrivalMonGenerator.h"

namespace setVariable
{

EArrivalMonGenerator::EArrivalMonGenerator() :
  radius(CF63::innerRadius),length(3.67),
  thick(1.0),faceThick(1.0),
  frontPipeILen(1.0),frontPipeLen(2.75),
  frontPipeRadius(CF40_22::innerRadius),
  frontPipeThick(0.8),backPipeILen(0.5),backPipeLen(3.5),
  backPipeRadius(CF40_22::innerRadius),
  backPipeThick(0.8)
  flangeRadius(CF40::flangeRadius),flangeLength(CF40::flangeLength),
  windowRotAngle(90.0),windowRadius(CF40_22::innerRadius),
  windowThick(0.8),voidMat("Void"),electrodeMat("Copper"),
  flangeMat("Stainless304"),outerMat("Stainless304")
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
    Setter for flange A
   */
{
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  return;
}

template<typename CF>
void
EArrivalMonGenerator::setAFlangeCF()
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
EArrivalMonGenerator::setBFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

    
void
EArrivalMonGenerator::generateEArrivalMon(FuncDataBase& Control,
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
  
    
  return;

}

///\cond TEMPLATE

template void EArrivalMonGenerator::setCF<CF40_22>();
template void EArrivalMonGenerator::setCF<CF40>();

template void EArrivalMonGenerator::setFlangeCF<CF40_22>();
template void EArrivalMonGenerator::setFlangeCF<CF40>();

///\endcond TEPLATE
  
}  // NAMESPACE setVariable
