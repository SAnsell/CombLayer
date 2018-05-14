/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/JawValveGenerator.cxx
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
#include "JawValveGenerator.h"

namespace setVariable
{

JawValveGenerator::JawValveGenerator() :
  length(7.0),width(24.0),height(46.0),depth(10.5),
  wallThick(0.5),portRadius(5.0),portThick(1.0),portLen(1.0),
  voidMat("Void"),wallMat("Stainless304"),jawWidth(2.0),
  jawHeight(1.0),jawThick(0.2),jawGap(0.1)
  /*!
    Constructor and defaults
  */
{}

JawValveGenerator::JawValveGenerator(const JawValveGenerator& A) : 
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),portRadius(A.portRadius),
  portThick(A.portThick),portLen(A.portLen),voidMat(A.voidMat),
  wallMat(A.wallMat),jawWidth(A.jawWidth),jawHeight(A.jawHeight),
  jawThick(A.jawThick),jawGap(A.jawGap),jawMat(A.jawMat)
  /*!
    Copy constructor
    \param A :: JawValveGenerator to copy
  */
{}

JawValveGenerator&
JawValveGenerator::operator=(const JawValveGenerator& A)
  /*!
    Assignment operator
    \param A :: JawValveGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      portRadius=A.portRadius;
      portThick=A.portThick;
      portLen=A.portLen;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      jawWidth=A.jawWidth;
      jawHeight=A.jawHeight;
      jawThick=A.jawThick;
      jawGap=A.jawGap;
      jawMat=A.jawMat;
    }
  return *this;
}

JawValveGenerator::~JawValveGenerator() 
 /*!
   Destructor
 */
{}


void
JawValveGenerator::setSlits(const double W,const double H,
			    const double T,const std::string& Mat)
  /*!
    Set the jaw valve generator
    \param W :: Width					       
    \param H :: Height
    \param T :: thickness
    \param Mat :: Material
   */
{
  jawWidth=W;
  jawHeight=H;
  jawThick=T;
  jawMat=Mat;
  return;
}

void
JawValveGenerator::setOuter(const double L,const double W,
			    const double H,const double D)
  /*!
    set outer dimentions
    \param L :: Length
    \param W :: Width
    \param H :: Height
   */
{
  length=L;
  width=W;
  height=H;
  depth=D;
  return;
}
  
void
JawValveGenerator::setPort(const double R,const double L,
			 const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param L :: lenght of port tube
    \param T :: Thickness of port tube (outer radius extention)
   */
{
  portRadius=R;
  portLen=L;
  portThick=T;
  return;
}

template<typename CF>
void
JawValveGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  portRadius=CF::innerRadius;
  portThick=CF::flangeRadius-CF::innerRadius;   //  total 
  portLen=CF::flangeLength;
  depth=1.1*CF::flangeRadius;
  height=2.5*CF::flangeRadius;
  width=2.1*CF::flangeRadius;
  
  return;
}

  

void
JawValveGenerator::generateSlits(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep,
				 const double xOpen,
				 const double zOpen) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param closedFlag :: true if valve closed
  */
{
  ELog::RegMethod RegA("JawValveGenerator","generatorValve");
  

  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);

  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"PortRadius",portRadius);	
  Control.addVariable(keyName+"PortThick",portThick);
  Control.addVariable(keyName+"PortLen",portLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  // stuff for jaws:
  const std::string jawName=keyName+"Jaw";
  Control.addVariable(jawName+"Gap",jawGap);
  Control.addVariable(jawName+"XOpen",xOpen);
  Control.addVariable(jawName+"XOpen",xOpen);
  Control.addVariable(jawName+"XOffset",0.0);
  Control.addVariable(jawName+"XThick",jawThick);
  Control.addVariable(jawName+"XHeight",jawWidth);  // note reverse
  Control.addVariable(jawName+"XWidth",jawHeight);  // note reverse

  Control.addVariable(jawName+"ZOpen",zOpen);
  Control.addVariable(jawName+"ZOffset",0.0);
  Control.addVariable(jawName+"ZThick",jawThick);
  Control.addVariable(jawName+"ZHeight",jawHeight); 
  Control.addVariable(jawName+"ZWidth",jawWidth);  
  
  
  Control.addVariable(jawName+"XJawMat",jawMat);
  Control.addVariable(jawName+"ZJawMat",jawMat);

  return;

}

///\cond TEMPLATE

  template void JawValveGenerator::setCF<CF40>();
  template void JawValveGenerator::setCF<CF63>();
  template void JawValveGenerator::setCF<CF100>();


///\endcond TEMPLATE

  
}  // NAMESPACE setVariable
