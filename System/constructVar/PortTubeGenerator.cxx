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
#include "PortTubeGenerator.h"

namespace setVariable
{

PortTubeGenerator::PortTubeGenerator() :
  wallThick(0.5),
  portAXStep(0.0),portAZStep(0.0),
  portAWallThick(0.5),portATubeLength(5.0),portATubeRadius(4.0),
  portBXStep(0.0),portBZStep(0.0),
  portBWallThick(0.5),portBTubeLength(5.0),portBTubeRadius(4.0),
  flangeALen(1.0),flangeARadius(1.0),
  flangeBLen(1.0),flangeBRadius(1.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

PortTubeGenerator::PortTubeGenerator(const PortTubeGenerator& A) : 
  wallThick(A.wallThick),portAXStep(A.portAXStep),
  portAZStep(A.portAZStep),portAWallThick(A.portAWallThick),
  portATubeLength(A.portATubeLength),portATubeRadius(A.portATubeRadius),
  portBXStep(A.portBXStep),portBZStep(A.portBZStep),
  portBWallThick(A.portBWallThick),portBTubeLength(A.portBTubeLength),
  portBTubeRadius(A.portBTubeRadius),flangeALen(A.flangeALen),
  flangeARadius(A.flangeARadius),flangeBLen(A.flangeBLen),
  flangeBRadius(A.flangeBRadius),
  voidMat(A.voidMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: PortTubeGenerator to copy
  */
{}

PortTubeGenerator&
PortTubeGenerator::operator=(const PortTubeGenerator& A)
  /*!
    Assignment operator
    \param A :: PortTubeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      wallThick=A.wallThick;
      portAXStep=A.portAXStep;
      portAZStep=A.portAZStep;
      portAWallThick=A.portAWallThick;
      portATubeLength=A.portATubeLength;
      portATubeRadius=A.portATubeRadius;
      portBXStep=A.portBXStep;
      portBZStep=A.portBZStep;
      portBWallThick=A.portBWallThick;
      portBTubeLength=A.portBTubeLength;
      portBTubeRadius=A.portBTubeRadius;
      flangeALen=A.flangeALen;
      flangeARadius=A.flangeARadius;
      flangeBLen=A.flangeBLen;
      flangeBRadius=A.flangeBRadius;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}

PortTubeGenerator::~PortTubeGenerator() 
 /*!
   Destructor
 */
{}


  
void
PortTubeGenerator::setPort(const double R,const double L,
			   const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param R :: lenght of port tube
    \param T :: Thickness of port tube
   */
{
  portATubeRadius=R;
  portATubeLength=L;
  portAWallThick=T;
  portBTubeRadius=R;
  portBTubeLength=L;
  portBWallThick=T;
  return;
}

void
PortTubeGenerator::setPortLength(const double LA,const double LB)
  /*!
    Set both the port lengths
    \param LA :: length of in-port tube
    \param LB :: length of out-port tube
   */
{
  portATubeLength=LA;
  portBTubeLength=LB;
  return;
}

void
PortTubeGenerator::setAPort(const double R,const double L,
			  const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param R :: lenght of port tube
    \param T :: Thickness of port tube
   */
{
  portATubeRadius=R;
  portATubeLength=L;
  portAWallThick=T;
  return;
}

void
PortTubeGenerator::setBPort(const double R,const double L,
			  const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param R :: lenght of port tube
    \param T :: Thickness of port tube
   */
{
  portBTubeRadius=R;
  portBTubeLength=L;
  portBWallThick=T;
  return;
}

void
PortTubeGenerator::setAPortOffset(const double XS,const double ZS)
  /*!
    Set the port offset relative to the origin line
    \param XS :: X Step
    \param ZS :: Z Step
   */
{
  portAXStep=XS;
  portAZStep=ZS;
  return;
}

void
PortTubeGenerator::setBPortOffset(const double XS,const double ZS)
  /*!
    Set the port offset relative to the origin line
    \param XS :: X Step
    \param ZS :: Z Step
   */
{
  portBXStep=XS;
  portBZStep=ZS;
  return;
}
  
void
PortTubeGenerator::setAFlange(const double R,const double L)
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
PortTubeGenerator::setBFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeBRadius=R;
  flangeBLen=L;
  return;
}

template<typename CF>
void
PortTubeGenerator::setAPortCF()
  /*!
    Setter for Port A
   */
{
  portATubeRadius=CF::innerRadius;
  portAWallThick=CF::wallThick;
  flangeARadius=CF::flangeRadius;
  flangeALen=CF::flangeLength;
  
  return;
}

template<typename CF>
void
PortTubeGenerator::setBPortCF()
  /*!
    Setter for prot B
   */
{
  portBTubeRadius=CF::innerRadius;
  portBWallThick=CF::wallThick;
  flangeBRadius=CF::flangeRadius;
  flangeBLen=CF::flangeLength;
  
  return;
}

template<typename CF>
void
PortTubeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  setAPortCF<CF>();
  setBPortCF<CF>();
  return;
}
  
void
PortTubeGenerator::generateTube(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,const double radius,
				const double length) const
 /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param radius :: radius of inner void
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("PortTubeGenerator","generatorBox");
  

  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);
	
  Control.addVariable(keyName+"InPortXStep",portAXStep);
  Control.addVariable(keyName+"InPortZStep",portAZStep);
  Control.addVariable(keyName+"InPortThick",portAWallThick);
  Control.addVariable(keyName+"InPortRadius",portATubeRadius);
  Control.addVariable(keyName+"InPortLen",portATubeLength);

  Control.addVariable(keyName+"OutPortXStep",portBXStep);
  Control.addVariable(keyName+"OutPortZStep",portBZStep);
  Control.addVariable(keyName+"OutPortThick",portBWallThick);
  Control.addVariable(keyName+"OutPortRadius",portBTubeRadius);
  Control.addVariable(keyName+"OutPortLen",portBTubeLength);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALen);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
       
  return;

}

///\cond TEMPLATE
  template void PortTubeGenerator::setCF<CF40>();
  template void PortTubeGenerator::setCF<CF63>();
  template void PortTubeGenerator::setCF<CF100>();
  template void PortTubeGenerator::setAPortCF<CF40>();
  template void PortTubeGenerator::setAPortCF<CF63>();
  template void PortTubeGenerator::setAPortCF<CF100>();
  template void PortTubeGenerator::setBPortCF<CF40>();
  template void PortTubeGenerator::setBPortCF<CF63>();
  template void PortTubeGenerator::setBPortCF<CF100>();
  
///\end TEMPLATE

}  // NAMESPACE setVariable
