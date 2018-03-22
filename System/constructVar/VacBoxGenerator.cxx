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
#include "VacBoxGenerator.h"

namespace setVariable
{

VacBoxGenerator::VacBoxGenerator() :
  wallThick(0.5),
  portAXStep(0.0),portAZStep(0.0),
  portAWallThick(0.5),portATubeLength(5.0),portATubeRadius(4.0),
  portBXStep(0.0),portBZStep(0.0),
  portBWallThick(0.5),portBTubeLength(5.0),portBTubeRadius(4.0),
  flangeALen(1.0),flangeARadius(1.0),flangeBLen(1.0),flangeBRadius(1.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

VacBoxGenerator::VacBoxGenerator(const VacBoxGenerator& A) : 
  wallThick(A.wallThick),portAXStep(A.portAXStep),
  portAZStep(A.portAZStep),portAWallThick(A.portAWallThick),
  portATubeLength(A.portATubeLength),portATubeRadius(A.portATubeRadius),
  portBXStep(A.portBXStep),portBZStep(A.portBZStep),
  portBWallThick(A.portBWallThick),portBTubeLength(A.portBTubeLength),
  portBTubeRadius(A.portBTubeRadius),flangeALen(A.flangeALen),
  flangeARadius(A.flangeARadius),flangeBLen(A.flangeBLen),
  flangeBRadius(A.flangeBRadius),voidMat(A.voidMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: VacBoxGenerator to copy
  */
{}

VacBoxGenerator&
VacBoxGenerator::operator=(const VacBoxGenerator& A)
  /*!
    Assignment operator
    \param A :: VacBoxGenerator to copy
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

VacBoxGenerator::~VacBoxGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
VacBoxGenerator::setAPortCF()
  /*!
    Setter for Port A
   */
{
  portATubeRadius=CF::innerRadius;
  portAWallThick=CF::wallThick;
  // extra only 
  flangeARadius=CF::flangeRadius; 
  flangeALen=CF::flangeLength;
  
  return;
}

template<typename CF>
void
VacBoxGenerator::setBPortCF()
  /*!
    Setter for prot B
   */
{
  portBTubeRadius=CF::innerRadius;
  portBWallThick=CF::wallThick;
  // extra only 
  flangeBRadius=CF::flangeRadius; 
  flangeBLen=CF::flangeLength;
  
  return;
}

template<typename CF>
void
VacBoxGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  setAPortCF<CF>();
  setBPortCF<CF>();
  return;
}

void
VacBoxGenerator::setPortLength(const double LA,const double LB)
  /*!
    Set both the ports lengths
    \param LA :: Length of port A
    \param LB :: Length of port A

   */
{
  portATubeLength=LA;
  portBTubeLength=LB;
  return;
}

void
VacBoxGenerator::setPort(const double R,const double L,
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
VacBoxGenerator::setAPort(const double R,const double L,
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
VacBoxGenerator::setBPort(const double R,const double L,
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
VacBoxGenerator::setAPortOffset(const double XS,const double ZS)
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
VacBoxGenerator::setBPortOffset(const double XS,const double ZS)
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
VacBoxGenerator::setFlange(const double R,const double L)
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
VacBoxGenerator::generateBox(FuncDataBase& Control,const std::string& keyName,
			     const double yStep,const double width,const
			     double height,const double depth,
			     const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("VacBoxGenerator","generatorBox");
  

  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"VoidHeight",height);
  Control.addVariable(keyName+"VoidDepth",depth);
  Control.addVariable(keyName+"VoidWidth",width);
  Control.addVariable(keyName+"VoidLength",length);

  Control.addVariable(keyName+"WallThick",wallThick);
	
  Control.addVariable(keyName+"PortAXStep",portAXStep);
  Control.addVariable(keyName+"PortAZStep",portAZStep);
  Control.addVariable(keyName+"PortAWallThick",portAWallThick);
  Control.addVariable(keyName+"PortATubeRadius",portATubeRadius);
  Control.addVariable(keyName+"PortATubeLength",portATubeLength);

  Control.addVariable(keyName+"PortBXStep",portBXStep);
  Control.addVariable(keyName+"PortBZStep",portBZStep);
  Control.addVariable(keyName+"PortBWallThick",portBWallThick);
  Control.addVariable(keyName+"PortBTubeRadius",portBTubeRadius);
  Control.addVariable(keyName+"PortBTubeLength",portBTubeLength);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALen);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"FeMat",wallMat);
       
  return;

}

///\cond  TEMPLATE
  template void VacBoxGenerator::setCF<CF40>();
  template void VacBoxGenerator::setCF<CF63>();
  template void VacBoxGenerator::setCF<CF100>();
  template void VacBoxGenerator::setAPortCF<CF40>();
  template void VacBoxGenerator::setAPortCF<CF63>();
  template void VacBoxGenerator::setAPortCF<CF100>();
  template void VacBoxGenerator::setBPortCF<CF40>();
  template void VacBoxGenerator::setBPortCF<CF63>();
  template void VacBoxGenerator::setBPortCF<CF100>();

///\endcond  TEMPLATE

  
}  // NAMESPACE setVariable
