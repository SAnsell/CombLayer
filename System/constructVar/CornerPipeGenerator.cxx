/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/PipeGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "CornerPipeGenerator.h"

namespace setVariable
{

CornerPipeGenerator::CornerPipeGenerator() :
  cornerRadius(0.2),
  height(1.40),width(1.60),wallThick(0.5),
  flangeARadius(12.0),flangeALength(1.0),
  flangeBRadius(12.0),flangeBLength(1.0),
  voidMat("Void"),wallMat("Aluminium"),
  flangeMat("Aluminium"),outerVoid(0)
  /*!
    Constructor and defaults
  */
{}

CornerPipeGenerator::CornerPipeGenerator(const CornerPipeGenerator& A) : 
  cornerRadius(A.cornerRadius),height(A.height),
  width(A.width),length(A.length),wallThick(A.wallThick),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  voidMat(A.voidMat),wallMat(A.wallMat),flangeMat(A.flangeMat),
  outerVoid(A.outerVoid)
  /*!
    Copy constructor
    \param A :: CornerPipeGenerator to copy
  */
{}

CornerPipeGenerator&
CornerPipeGenerator::operator=(const CornerPipeGenerator& A)
  /*!
    Assignment operator
    \param A :: CornerPipeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cornerRadius=A.cornerRadius;
      height=A.height;
      width=A.width;
      length=A.length;
      wallThick=A.wallThick;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      flangeMat=A.flangeMat;
      outerVoid=A.outerVoid;
    }
  return *this;
}



CornerPipeGenerator::~CornerPipeGenerator()
 /*!
   Destructor
 */
{}

void
CornerPipeGenerator::setPipe(const double W,const double H,
			     const double R,const double T = -1.0)
  /*!
    Set all the pipe values
    \param W :: full width of main pipe
    \param H :: full height of main pipe
    \param T :: Thickness
   */
{
  cornerRadius=R;
  width=W;
  height=H;
  if (T>=0.0)
    wallThick=T;
  return;
}


void
CornerPipeGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  setAFlange(R,L);
  setBFlange(R,L);
  return;
}

void
CornerPipeGenerator::setAFlange(const double R,const double L)
  /*!
    Set all the A-flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeARadius=R;
  flangeALength=L;
  return;
}

void
CornerPipeGenerator::setBFlange(const double R,const double L)
  /*!
    Set all the B-flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeBRadius=R;
  flangeBLength=L;
  return;
}

void
CornerPipeGenerator::setFlangePair(const double AR,const double AL,
			     const double BR,const double BL)
  /*!
    Set all the flange values
    \param AR :: radius of front flange
    \param AL :: front flange length
    \param BR :: radius of back flange
    \param BL :: back flange length
   */
{
  flangeARadius=AR;
  flangeALength=AL;
  flangeBRadius=BR;
  flangeBLength=BL;

  return;
}


template<typename CF>
void
CornerPipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
    This uses third
  */
{
  wallThick=CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();

  return;
}

template<typename CF>
void
CornerPipeGenerator::setAFlangeCF()
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
CornerPipeGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

void
CornerPipeGenerator::setMat(const std::string& M,
			    const std::string& F)
  /*!
    Set Material
    \param M :: Main wall material
    \param F :: Flange material
  */
{
  wallMat=M;
  flangeMat=F;
  return;
}

void
CornerPipeGenerator::generatePipe(FuncDataBase& Control,
				  const std::string& keyName,
				  const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("CornerPipeGenerator","generatorPipe");

  Control.addVariable(keyName+"CornerRadius",cornerRadius);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Length",length);
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
  template void CornerPipeGenerator::setCF<CF8_TDC>();
  template void CornerPipeGenerator::setCF<CF16_TDC>();
  template void CornerPipeGenerator::setCF<CF16>();
  template void CornerPipeGenerator::setCF<CF18_TDC>();
  template void CornerPipeGenerator::setCF<CF25>();
  template void CornerPipeGenerator::setCF<CF34_TDC>();
  template void CornerPipeGenerator::setCF<CF35_TDC>();
  template void CornerPipeGenerator::setCF<CF37_TDC>();
  template void CornerPipeGenerator::setCF<CF40_22>();
  template void CornerPipeGenerator::setCF<CF40>();
  template void CornerPipeGenerator::setCF<CF50>();
  template void CornerPipeGenerator::setCF<CF63>();
  template void CornerPipeGenerator::setCF<CF66_TDC>();
  template void CornerPipeGenerator::setCF<CF100>();
  template void CornerPipeGenerator::setCF<CF120>();
  template void CornerPipeGenerator::setCF<CF150>();
  template void CornerPipeGenerator::setCF<CF350>();
  template void CornerPipeGenerator::setAFlangeCF<CF25>();
  template void CornerPipeGenerator::setAFlangeCF<CF34_TDC>();
  template void CornerPipeGenerator::setAFlangeCF<CF37_TDC>();
  template void CornerPipeGenerator::setAFlangeCF<CF40_22>();
  template void CornerPipeGenerator::setAFlangeCF<CF40>();
  template void CornerPipeGenerator::setAFlangeCF<CF63>();
  template void CornerPipeGenerator::setAFlangeCF<CF66_TDC>();
  template void CornerPipeGenerator::setAFlangeCF<CF100>();
  template void CornerPipeGenerator::setAFlangeCF<CF120>();
  template void CornerPipeGenerator::setAFlangeCF<CF150>();
  template void CornerPipeGenerator::setAFlangeCF<CF350>();
  template void CornerPipeGenerator::setBFlangeCF<CF25>();
  template void CornerPipeGenerator::setBFlangeCF<CF34_TDC>();
  template void CornerPipeGenerator::setBFlangeCF<CF37_TDC>();
  template void CornerPipeGenerator::setBFlangeCF<CF40_22>();
  template void CornerPipeGenerator::setBFlangeCF<CF40>();
  template void CornerPipeGenerator::setBFlangeCF<CF63>();
  template void CornerPipeGenerator::setBFlangeCF<CF66_TDC>();
  template void CornerPipeGenerator::setBFlangeCF<CF100>();
  template void CornerPipeGenerator::setBFlangeCF<CF120>();
  template void CornerPipeGenerator::setBFlangeCF<CF150>();
  template void CornerPipeGenerator::setBFlangeCF<CF350>();

///\endcond TEMPLATE


}  // NAMESPACE setVariable
