/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutterGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "MonoShutterGenerator.h"

namespace setVariable
{

MonoShutterGenerator::MonoShutterGenerator() :
  height(5.0),width(5.0),
  thick(5.0),lift(3.0),liftScrewRadius(1.0),
  threadLength(30.0),
  topInnerRadius(CF100::innerRadius),
  topFlangeRadius(CF100::flangeRadius),
  topFlangeLength(CF100::flangeLength),
  bellowLength(2.0),bellowThick(CF100::bellowThick),
  outRadius(CF100::flangeRadius),
  outLength(CF100::flangeLength),
  mat("Tungsten"),threadMat("Copper"),
  flangeMat("Stainless304"),
  bellowMat("Stainless304%Void%10.0")
  /*!
    Constructor and defaults
  */
{}


  
MonoShutterGenerator::~MonoShutterGenerator() 
 /*!
   Destructor
 */
{}



void
MonoShutterGenerator::setMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Percentage of material
  */
{
  flangeMat=M;
  bellowMat=M+"%Void%"+std::to_string(T);
  return;
}
  
template<typename CF>
void
MonoShutterGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  topInnerRadius=CF::innerRadius;
  bellowThick=CF::bellowThick;
  setMat(flangeMat,20.0*CF::wallThick/CF::bellowThick);
  
  setTopCF<CF>();
  setOutCF<CF>();
  return;
}

template<typename CF>
void
MonoShutterGenerator::setOutCF()
  /*!
    Set the Out [highest] flange 
  */
{
  outRadius=CF::flangeRadius;
  outLength=CF::flangeLength;
  return;
}

template<typename CF>
void
MonoShutterGenerator::setTopCF()
  /*!
    Set the top [flange] level
  */
{
  topFlangeRadius=CF::flangeRadius;
  topFlangeLength=CF::flangeLength;
  return;
}

void
MonoShutterGenerator::generateHD(FuncDataBase& Control,
				 const std::string& keyName,
				 const bool upFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param upFlag :: Shutter Dum up
  */
{
  ELog::RegMethod RegA("MonoShutterGenerator","generateColl");
  
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"Lift",lift);
  Control.addVariable(keyName+"LiftScrewRadius",liftScrewRadius);
  Control.addVariable(keyName+"ThreadLength",threadLength);

  Control.addVariable(keyName+"UpFlag",static_cast<int>(upFlag));
  
  Control.addVariable(keyName+"TopInnerRadius",topInnerRadius);
  Control.addVariable(keyName+"TopFlangeRadius",topFlangeRadius);
  Control.addVariable(keyName+"TopFlangeLength",topFlangeLength);

  Control.addVariable(keyName+"BellowLength",bellowLength);
  Control.addVariable(keyName+"BellowThick",bellowThick);

  Control.addVariable(keyName+"OutLength",outLength);
  Control.addVariable(keyName+"OutRadius",outRadius);

  
  Control.addVariable(keyName+"BlockMat",mat);
  Control.addVariable(keyName+"ThreadMat",threadMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"BellowMat",bellowMat);
       
  return;
}

template void MonoShutterGenerator::setCF<CF40>();
template void MonoShutterGenerator::setCF<CF63>();
template void MonoShutterGenerator::setCF<CF100>();
template void MonoShutterGenerator::setCF<CF120>();
template void MonoShutterGenerator::setCF<CF150>();

template void MonoShutterGenerator::setOutCF<CF40>();
template void MonoShutterGenerator::setOutCF<CF63>();
template void MonoShutterGenerator::setOutCF<CF100>();
template void MonoShutterGenerator::setOutCF<CF120>();
template void MonoShutterGenerator::setOutCF<CF150>();

template void MonoShutterGenerator::setTopCF<CF40>();
template void MonoShutterGenerator::setTopCF<CF63>();
template void MonoShutterGenerator::setTopCF<CF100>();
template void MonoShutterGenerator::setTopCF<CF120>();
template void MonoShutterGenerator::setTopCF<CF150>();

  
}  // NAMESPACE setVariable
