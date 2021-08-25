/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/PipeTubeGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
  flangeBRadius(A.flangeBRadius),ACap(A.ACap),BCap(A.BCap),
  voidMat(A.voidMat),wallMat(A.wallMat),capMat(A.capMat)
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
      ACap=A.ACap;
      BCap=A.BCap;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      capMat=A.capMat;
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

void
PipeTubeGenerator::setFlangeLength(const double LA,const double LB)
  /*!
    Setter for flange A/B length
    \param LA :: Length of flange A
    \param LB :: Length of flange B
   */
{
  flangeALen=LA;
  flangeBLen=LB;
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
  if (ACap>Geometry::zeroTol)
    ACap=flangeALen;

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
  if (BCap>Geometry::zeroTol)
    BCap=flangeBLen;

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
    Set the flange cap thicknesses
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
    Set the flange cap thickness to standared flange thickness
    \param AFlag :: First Cap true
    \param BFlag :: Second Cap true
   */
{
  ACap= (AFlag) ? flangeALen : 0;
  BCap= (BFlag) ? flangeBLen : 0;
  return;
}


void
PipeTubeGenerator::generateTube(FuncDataBase& Control,
				const std::string& keyName,
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

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALen);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLen);

  Control.addVariable(keyName+"FlangeACapThick",ACap);
  Control.addVariable(keyName+"FlangeBCapThick",BCap);
  Control.addVariable(keyName+"FlangeCapMat",capMat);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"CapMat",capMat);

  return;

}

void
PipeTubeGenerator::generateBlank(FuncDataBase& Control,
				 const std::string& keyName,
				 const double length) const
 /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: y-offset
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("PipeTubeGenerator","generatorBlank");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"BlankThick",wallThick);

  Control.addVariable(keyName+"FlangeRadius",flangeARadius);
  Control.addVariable(keyName+"FlangeLength",flangeALen);

  Control.addVariable(keyName+"FlangeCapThick",ACap);
  Control.addVariable(keyName+"FlangeCapMat",capMat);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"CapMat",capMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}

///\cond TEMPLATE
  template void PipeTubeGenerator::setCF<CF34_TDC>();
  template void PipeTubeGenerator::setCF<CF35_TDC>();
  template void PipeTubeGenerator::setCF<CF37_TDC>();
  template void PipeTubeGenerator::setCF<CF40_22>();
  template void PipeTubeGenerator::setCF<CF40>();
  template void PipeTubeGenerator::setCF<CF63>();
  template void PipeTubeGenerator::setCF<CF66_TDC>();
  template void PipeTubeGenerator::setCF<CF100>();
  template void PipeTubeGenerator::setCF<CF120>();
  template void PipeTubeGenerator::setCF<CF150>();
  template void PipeTubeGenerator::setCF<CF200>();
  template void PipeTubeGenerator::setCF<CF350>();
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
