/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/VacBoxGenerator.cxx
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
#include "PipeTubeGenerator.h"

namespace setVariable
{

PipeTubeGenerator::PipeTubeGenerator() :
  radius(2.5),wallThick(0.5),
  flangeALen(1.0),flangeARadius(1.0),
  flangeBLen(1.0),flangeBRadius(1.0),
  ACap(0.0),BCap(0.0),
  voidMat("Void"),wallMat("Stainless304"),
  capMat(wallMat)
  /*!
    Constructor and defaults
  */
{}

PipeTubeGenerator::PipeTubeGenerator(const PipeTubeGenerator& A) : 
  radius(A.radius),wallThick(A.wallThick),flangeALen(A.flangeALen),
  flangeARadius(A.flangeARadius),flangeBLen(A.flangeBLen),
  flangeBRadius(A.flangeBRadius),voidMat(A.voidMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: PipeTubeGenerator to copy
  */
{}

PipeTubeGenerator&
PipeTubeGenerator::operator=(const PipeTubeGenerator& A)
  /*!
    Assignment operator
    \param A :: PipeTubeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      radius=A.radius;
      wallThick=A.wallThick;
      flangeALen=A.flangeALen;
      flangeARadius=A.flangeARadius;
      flangeBLen=A.flangeBLen;
      flangeBRadius=A.flangeBRadius;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}

  
PipeTubeGenerator::~PipeTubeGenerator() 
 /*!
   Destructor
 */
{}

void
PipeTubeGenerator::setPipe(const double R,const double W,
			   const double FR,const double FL)
  /*!
    Set the pipe by hand
    \param R :: Radius
    \param W :: Wall thick
    \param FR :: Flange Radius
    \param FL :: Flange length
  */
{
  radius=R;
  wallThick=W;
  flangeARadius=FR;
  flangeALen=FL;
  flangeBRadius=FR;
  flangeBLen=FL;
  ACap=0.0;
  BCap=0.0;
  return;
}
  
template<typename CF>
void
PipeTubeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  radius=CF::innerRadius;
  wallThick=CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();

  return;
}

template<typename CF>
void
PipeTubeGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALen=CF::flangeLength;
  ACap=0.0;
  return;
}

template<typename CF>
void
PipeTubeGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLen=CF::flangeLength;
  BCap=0.0;
  return;
}

void
PipeTubeGenerator::setAFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeARadius=R;
  flangeALen=L;
  return;
}

void
PipeTubeGenerator::setBFlange(const double R,const double L)
  /*!
    Set the B-flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeBRadius=R;
  flangeBLen=L;
  return;
}

void
PipeTubeGenerator::setFlangeCap(const double AC,const double BC)
  /*!
    Set the flange cap values
    \param AC :: Flange cap A
    \param BC :: Flange cap B
   */
{
  ACap=AC;
  BCap=BC;
  return;
}

void
PipeTubeGenerator::setCap(const bool AFlag,const bool BFlag)
  /*!
    Set the flange cap values
   */
{
  ACap= (AFlag) ? flangeALen : 0;
  BCap= (BFlag) ? flangeBLen : 0;
  return;
}

  
  
void
PipeTubeGenerator::generateTube(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double length) const
 /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("PipeTubeGenerator","generatorTube");
  

  Control.addVariable(keyName+"YStep",yStep);   // step + flange
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);
	
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALen);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLen);

  Control.addVariable(keyName+"FlangeACap",ACap);
  Control.addVariable(keyName+"FlangeBCap",BCap);
  Control.addVariable(keyName+"FlangeCapMat",capMat);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
       
  return;

}

///\cond TEMPLATE
  template void PipeTubeGenerator::setCF<CF40>();
  template void PipeTubeGenerator::setCF<CF63>();
  template void PipeTubeGenerator::setCF<CF100>();
  template void PipeTubeGenerator::setCF<CF120>();
  template void PipeTubeGenerator::setCF<CF150>();
  template void PipeTubeGenerator::setAFlangeCF<CF40>();
  template void PipeTubeGenerator::setAFlangeCF<CF63>();
  template void PipeTubeGenerator::setAFlangeCF<CF100>();
  template void PipeTubeGenerator::setAFlangeCF<CF120>();
  template void PipeTubeGenerator::setAFlangeCF<CF150>();
  template void PipeTubeGenerator::setBFlangeCF<CF40>();
  template void PipeTubeGenerator::setBFlangeCF<CF63>();
  template void PipeTubeGenerator::setBFlangeCF<CF100>();
  template void PipeTubeGenerator::setBFlangeCF<CF120>();
  template void PipeTubeGenerator::setBFlangeCF<CF150>();
  
///\endcond TEMPLATE

}  // NAMESPACE setVariable
