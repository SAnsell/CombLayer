/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/PipeGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "PipeGenerator.h"

namespace setVariable
{

PipeGenerator::PipeGenerator() :
  pipeType(0),pipeRadius(8.0),
  pipeHeight(16.0),pipeWidth(16.0),pipeThick(0.5),
  claddingThick(0.0),flangeARadius(12.0),flangeALen(1.0),
  flangeBRadius(12.0),flangeBLen(1.0),
  windowRadius(10.0),windowThick(0.5),
  pipeMat("Aluminium"),frontWindowMat("Silicon300K"),
  backWindowMat("Silicon300K"),
  voidMat("Void"),claddingMat("B4C")
  /*!
    Constructor and defaults
  */
{}

PipeGenerator::PipeGenerator(const PipeGenerator& A) : 
  pipeType(A.pipeType),pipeRadius(A.pipeRadius),
  pipeHeight(A.pipeHeight),pipeWidth(A.pipeWidth),
  pipeThick(A.pipeThick),claddingThick(A.claddingThick),
  flangeARadius(A.flangeARadius),flangeALen(A.flangeALen),
  flangeBRadius(A.flangeBRadius),flangeBLen(A.flangeBLen),
  windowRadius(A.windowRadius),windowThick(A.windowThick),
  pipeMat(A.pipeMat),frontWindowMat(A.frontWindowMat),
  backWindowMat(A.backWindowMat),voidMat(A.voidMat),
  claddingMat(A.claddingMat)
  /*!
    Copy constructor
    \param A :: PipeGenerator to copy
  */
{}

PipeGenerator&
PipeGenerator::operator=(const PipeGenerator& A)
  /*!
    Assignment operator
    \param A :: PipeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      pipeType=A.pipeType;
      pipeRadius=A.pipeRadius;
      pipeHeight=A.pipeHeight;
      pipeWidth=A.pipeWidth;
      pipeThick=A.pipeThick;
      claddingThick=A.claddingThick;
      flangeARadius=A.flangeARadius;
      flangeALen=A.flangeALen;
      flangeBRadius=A.flangeBRadius;
      flangeBLen=A.flangeBLen;
      windowRadius=A.windowRadius;
      windowThick=A.windowThick;
      pipeMat=A.pipeMat;
      frontWindowMat=A.frontWindowMat;
      backWindowMat=A.backWindowMat;
      voidMat=A.voidMat;
      claddingMat=A.claddingMat;
    }
  return *this;
}

PipeGenerator::~PipeGenerator() 
 /*!
   Destructor
 */
{}


void
PipeGenerator::setPipe(const double R,const double T)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
   */
{
  pipeType=0;
  pipeRadius=R;
  pipeThick=T;
  return;
}

void
PipeGenerator::setRectPipe(const double W,const double H,
                           const double T)
  /*!
    Set all the pipe values
    \param W :: full width of main pipe
    \param H :: full height of main pipe
    \param T :: Thickness
   */
{
  pipeType=1;
  pipeWidth=W;
  pipeHeight=H;
  pipeThick=T;
  return;
}

void
PipeGenerator::setWindow(const double R,const double T)
  /*!
    Set all the window values
    \param R :: radius of window [-ve to be bigger than main rad]
    \param T :: Thickness
   */
{
  windowRadius=R;
  windowThick=T;
  return;
}


void
PipeGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeARadius=R;
  flangeALen=L;
  flangeBRadius=R;
  flangeBLen=L;
  return;
}

void
PipeGenerator::setFlangePair(const double AR,const double AL,
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
  flangeALen=AL;
  flangeBRadius=BR;
  flangeBLen=BL;

  return;
}

void
PipeGenerator::setWindowMat(const std::string& M)
  /*!
    Set the window materials [back/front]
    \param M :: Material
   */
{
  frontWindowMat=M;
  backWindowMat=M;
  return;
}

void
PipeGenerator::setWindowMat(const std::string& MA,
                            const std::string& MB)
  /*!
    Set the window materials [back/front]
    \param MA :: Front Material
    \param MB :: Back Material
   */
{
  frontWindowMat=MA;
  backWindowMat=MB;
  return;
}

void
PipeGenerator::setCladding(const double T,const std::string& M)
  /*!
    Set the cladding thickess and material
    \param T :: Thickness
    \param M :: Material
   */
{
  claddingThick=T;
  claddingMat=M;
  return;
}

template<typename CF>
void
PipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  pipeType=0;
  pipeRadius=CF::innerRadius;
  pipeThick=CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  
  return;
}

template<typename CF>
void
PipeGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALen=CF::flangeLength;
  return;
}

template<typename CF>
void
PipeGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLen=CF::flangeLength;
  return;
}


void
PipeGenerator::generatePipe(FuncDataBase& Control,const std::string& keyName,
                            const double yStep,const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("PipeGenerator","generatorPipe");

  const double minRadius(pipeType ?
                         std::min(pipeWidth,pipeHeight) :
                         pipeRadius);
  double realWindowRadius=(windowRadius<0.0) ?
    minRadius-windowRadius : windowRadius;

  const double realFlangeARadius=(flangeARadius<0.0) ?
    minRadius-flangeARadius : flangeARadius;
  const double realFlangeBRadius=(flangeBRadius<0.0) ?
    minRadius-flangeBRadius : flangeBRadius;

  realWindowRadius=std::min(realWindowRadius,realFlangeARadius);
  realWindowRadius=std::min(realWindowRadius,realFlangeBRadius);

    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",yStep);   // step + flange
  if (!pipeType)
    Control.addVariable(keyName+"Radius",pipeRadius);
  else
    {
      Control.addVariable(keyName+"Width",pipeWidth);
      Control.addVariable(keyName+"Height",pipeHeight);
    }
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",pipeThick);
  Control.addVariable(keyName+"FlangeFrontRadius",realFlangeARadius);
  Control.addVariable(keyName+"FlangeBackRadius",realFlangeBRadius);
  Control.addVariable(keyName+"FlangeFrontLength",flangeALen);
  Control.addVariable(keyName+"FlangeBackLength",flangeBLen);
  Control.addVariable(keyName+"FeMat",pipeMat);
  Control.addVariable(keyName+"WindowActive",3);

  Control.addVariable(keyName+"WindowRadius",realWindowRadius);
  
  Control.addVariable(keyName+"WindowThick",windowThick);
  Control.addVariable(keyName+"WindowFrontMat",frontWindowMat);
  Control.addVariable(keyName+"WindowBackMat",backWindowMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  Control.addVariable(keyName+"CladdingThick",claddingThick);
  Control.addVariable(keyName+"CladdingMat",claddingMat);
      
  return;

}

///\cond TEMPLATE
  template void PipeGenerator::setCF<CF40>();
  template void PipeGenerator::setCF<CF63>();
  template void PipeGenerator::setCF<CF100>();
  template void PipeGenerator::setAFlangeCF<CF40>();
  template void PipeGenerator::setAFlangeCF<CF63>();
  template void PipeGenerator::setAFlangeCF<CF100>();
  template void PipeGenerator::setBFlangeCF<CF40>();
  template void PipeGenerator::setBFlangeCF<CF63>();
  template void PipeGenerator::setBFlangeCF<CF100>();
  
///\end TEMPLATE


}  // NAMESPACE setVariable
