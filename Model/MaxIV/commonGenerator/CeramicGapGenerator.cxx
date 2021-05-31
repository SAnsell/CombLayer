/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CeramicGapGenerator.cxx
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

#include "CeramicGapGenerator.h"

namespace setVariable
{

CeramicGapGenerator::CeramicGapGenerator() :
  radius(1.05),length(14.0),
  ceramicALen(0.5),ceramicWideLen(4.0),
  ceramicGapLen(2.0),
  ceramicBLen(1.2),ceramicThick(0.3),
  ceramicWideThick(0.8),
  pipeLen(2.0),pipeThick(0.15),
  bellowLen(3.5),bellowThick(0.85),
  flangeARadius(CF40::flangeRadius),flangeALength(CF40::flangeLength),
  flangeBRadius(CF40::flangeRadius),flangeBLength(CF40::flangeLength),
  voidMat("Void"),pipeMat("Stainless304L"),
  ceramicMat("Al2O3"),bellowMat("Stainless304L%Void%10.0"),
  flangeMat("Stainless304L"),outerMat("Void")
  /*!
    Constructor and defaults
  */
{
}

CeramicGapGenerator::~CeramicGapGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
CeramicGapGenerator::setCF()
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
CeramicGapGenerator::setAFlangeCF()
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
CeramicGapGenerator::setBFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

void
CeramicGapGenerator::setBellowMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Percentage of material
  */
{
  bellowMat=M+"%Void%"+std::to_string(T);
  return;
}

void
CeramicGapGenerator::generateCeramicGap(FuncDataBase& Control,
					const std::string& keyName)  const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("CeramicGapGenerator","generateCeramicGap");


  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"CeramicALen",ceramicALen);
  Control.addVariable(keyName+"CeramicWideLen",ceramicWideLen);
  Control.addVariable(keyName+"ceramicGapLength",ceramicGapLen);
  Control.addVariable(keyName+"CeramicBLen",ceramicBLen);
  Control.addVariable(keyName+"CeramicThick",ceramicThick);
  Control.addVariable(keyName+"CeramicWideThick",ceramicWideThick);

  Control.addVariable(keyName+"PipeLen",pipeLen);
  Control.addVariable(keyName+"PipeThick",pipeThick);

  Control.addVariable(keyName+"BellowLen",bellowLen);
  Control.addVariable(keyName+"BellowThick",bellowThick);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
  Control.addVariable(keyName+"CeramicMat",ceramicMat);
  Control.addVariable(keyName+"BellowMat",bellowMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"OuterMat",voidMat);

  return;

}

///\cond TEMPLATE

template void CeramicGapGenerator::setCF<CF40_22>();
template void CeramicGapGenerator::setCF<CF40>();

template void CeramicGapGenerator::setAFlangeCF<CF40_22>();
template void CeramicGapGenerator::setAFlangeCF<CF40>();

template void CeramicGapGenerator::setBFlangeCF<CF40_22>();
template void CeramicGapGenerator::setBFlangeCF<CF40>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
