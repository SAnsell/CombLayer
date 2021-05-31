/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/GateValveGenerator.cxx
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
#include "GateValveGenerator.h"

namespace setVariable
{

GateValveGenerator::GateValveGenerator() :
  length(7.0),radius(-1.0),width(24.0),height(46.0),depth(10.5),
  wallThick(0.5),portARadius(CF40::flangeRadius),
  portAThick(1.0),portALen(1.0),
  portBRadius(CF40::flangeRadius),portBThick(1.0),portBLen(1.0),
  bladeLift(12.0),bladeThick(1.0),bladeRadius(5.5),
  liftWidth(10.0),liftHeight(14.0),voidMat("Void"),
  bladeMat("Aluminium"),wallMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}


GateValveGenerator::~GateValveGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
GateValveGenerator::setCylCF()
  /*!
    Set pipe/flange to CF format
  */
{
  setAPortCF<CF>();
  setBPortCF<CF>();
  radius=1.1*CF::flangeRadius;
  liftWidth=1.2*CF::innerRadius;
  liftHeight=2.2*CF::flangeRadius;
  width=2.1*CF::flangeRadius;
  bladeRadius=1.1*CF::innerRadius;
  bladeLift=2.2*CF::innerRadius;

  return;
}

template<typename CF>
void
GateValveGenerator::setCubeCF()
  /*!
    Set pipe/flange to CF format
  */
{
  setAPortCF<CF>();
  setBPortCF<CF>();
  radius=-1.0;
  depth=1.1*CF::flangeRadius;
  height=3.5*CF::flangeRadius;
  width=2.1*CF::flangeRadius;
  bladeRadius=1.1*CF::innerRadius;
  bladeLift=2.2*CF::innerRadius;

  return;
}


void
GateValveGenerator::setOuter(const double L,const double W,
			     const double H,const double D)
  /*!
    set outer dimentions
    \param L :: Length
    \param W :: Width
    \param H :: Height
    \param D :: Depth
   */
{
  length=(L>Geometry::zeroTol) ? L : length;
  width=(W>Geometry::zeroTol) ? W : width;
  height=(H>Geometry::zeroTol) ? H : height;
  depth=(D>Geometry::zeroTol) ? D : depth;
	
  return;
}

void
GateValveGenerator::setPort(const double R,const double L,
			    const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param L :: lenght of port tube
    \param T :: Thickness of port tube (outer radius extention)
   */
{
  setAPort(R,L,T);
  setBPort(R,L,T);
  return;
}

void
GateValveGenerator::setAPort(const double R,const double L,
			 const double T)
  /*!
    Set the first port
    \param R :: radius of port tube
    \param L :: lenght of port tube
    \param T :: Thickness of port tube (outer radius extention)
   */
{
  portARadius=R;
  portALen=L;
  portAThick=T;
  return;
}

void
GateValveGenerator::setBPort(const double R,const double L,
			     const double T)
  /*!
    Set the back port
    \param R :: radius of port tube
    \param L :: lenght of port tube
    \param T :: Thickness of port tube (outer radius extention)
   */
{
  portBRadius=R;
  portBLen=L;
  portBThick=T;
  return;
}

template<typename innerCF,typename outerCF>
void
GateValveGenerator::setPortPairCF()
  /*!
    Setter for ports based on two radii
  */
{
  if (outerCF::flangeRadius>innerCF::innerRadius+Geometry::zeroTol)
    {
      portARadius=innerCF::innerRadius;
      portALen=innerCF::flangeLength;
      portAThick=outerCF::flangeRadius-innerCF::innerRadius;
      portBRadius=innerCF::innerRadius;
      portBLen=innerCF::flangeLength;
      portBThick=outerCF::flangeRadius-innerCF::innerRadius;
    }

  return;
}

template<typename CF>
void
GateValveGenerator::setAPortCF()
  /*!
    Setter for port A
  */
{
  // rad/len/thick
  setAPort(CF::innerRadius,CF::flangeLength,
	   CF::flangeRadius-CF::innerRadius);

  return;
}

template<typename CF>
void
GateValveGenerator::setBPortCF()
  /*!
    Setter for port A
  */
{
  // rad/len/thick
  setBPort(CF::innerRadius,CF::flangeLength,
	   CF::flangeRadius-CF::innerRadius);

  return;
}


void
GateValveGenerator::generateValve(FuncDataBase& Control,
				  const std::string& keyName,
				  const double yStep,
				  const int closedFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: y-offset
    \param closedFlag :: true if valve closed
  */
{
  ELog::RegMethod RegA("GateValveGenerator","generatorValve");


  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"Length",length);
  if (radius>Geometry::zeroTol)
    {
      Control.addVariable(keyName+"Radius",radius);
      Control.addVariable(keyName+"LiftWidth",liftWidth);
      Control.addVariable(keyName+"LiftHeight",liftHeight);
    }
  else
    {
      Control.addVariable(keyName+"Width",width);
      Control.addVariable(keyName+"Height",height);
      Control.addVariable(keyName+"Depth",depth);
    }

  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"PortARadius",portARadius);
  Control.addVariable(keyName+"PortAThick",portAThick);
  Control.addVariable(keyName+"PortALen",portALen);

  Control.addVariable(keyName+"PortBRadius",portBRadius);
  Control.addVariable(keyName+"PortBThick",portBThick);
  Control.addVariable(keyName+"PortBLen",portBLen);

  Control.addVariable(keyName+"Closed",closedFlag);

  Control.addVariable(keyName+"BladeLift",bladeLift);
  Control.addVariable(keyName+"BladeThick",bladeThick);
  Control.addVariable(keyName+"BladeRadius",bladeRadius);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"BladeMat",bladeMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}

///\cond TEMPLATE

template void GateValveGenerator::setCylCF<CF40>();
template void GateValveGenerator::setCylCF<CF40_22>();
template void GateValveGenerator::setCylCF<CF63>();
template void GateValveGenerator::setCylCF<CF100>();

template void GateValveGenerator::setCubeCF<CF40>();
template void GateValveGenerator::setCubeCF<CF40_22>();
template void GateValveGenerator::setCubeCF<CF63>();
template void GateValveGenerator::setCubeCF<CF100>();

template void GateValveGenerator::setAPortCF<CF40>();
template void GateValveGenerator::setAPortCF<CF40_22>();
template void GateValveGenerator::setAPortCF<CF63>();
template void GateValveGenerator::setAPortCF<CF100>();

template void GateValveGenerator::setBPortCF<CF40>();
template void GateValveGenerator::setBPortCF<CF40_22>();
template void GateValveGenerator::setBPortCF<CF63>();
template void GateValveGenerator::setBPortCF<CF100>();

template void GateValveGenerator::setPortPairCF<CF40,CF63>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
