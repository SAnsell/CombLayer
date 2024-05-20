/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/GTFGateValveGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell and Konstantin Batkov
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
#include "GTFGateValveGenerator.h"

namespace setVariable
{

GTFGateValveGenerator::GTFGateValveGenerator() :
  length(7.0),radius(-1.0),width(24.0),height(46.0),depth(10.5),
  wallThick(0.5),portARadius(CF40::flangeRadius),
  portAThick(1.0),portALen(1.0),
  portBRadius(CF40::flangeRadius),portBThick(1.0),portBLen(1.0),
  bladeLift(12.0),bladeThick(1.09),
  bladeRadius(3.5),
  bladeCutThick(0.4),
  bladeScrewHousingRadius(0.4),
  clampWidth(25.0),
  clampDepth(10.9),
  clampHeight(10.9),
  clampBulkHeight(17.0),
  clampBaseThick(1.9),
  clampBaseWidth(32),
  clampTopWidth(10.0),
  lsFlangeWidth(18.7),
  lsFlangeDepth(2.2),
  lsFlangeHeight(2.0),
  lsFlangeHoleRadius(1.5),
  lsShaftRadius(1.9),
  lsShaftThick(0.3),
  lsShaftLength(12.4),
  lsShaftFlangeRadius(4.0),
  lsShaftFlangeThick(1.0),
  liftWidth(10.0),liftHeight(14.0),voidMat("Void"),
  bladeMat("Aluminium"),wallMat("SS316L"),
  clampMat("SS316L"),
  lsFlangeMat("Copper") // TODO
  /*!
    Constructor and defaults
  */
{}


GTFGateValveGenerator::~GTFGateValveGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
GTFGateValveGenerator::setCylCF()
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
GTFGateValveGenerator::setCubeCF()
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
GTFGateValveGenerator::setOuter(const double L,const double W,
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
GTFGateValveGenerator::setPort(const double R,const double L,
			    const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param L :: length of port tube
    \param T :: Thickness of port tube (outer radius extention)
   */
{
  setAPort(R,L,T);
  setBPort(R,L,T);
  return;
}

void
GTFGateValveGenerator::setAPort(const double R,const double L,
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
GTFGateValveGenerator::setBPort(const double R,const double L,
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
GTFGateValveGenerator::setPortPairCF()
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
GTFGateValveGenerator::setAPortCF()
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
GTFGateValveGenerator::setBPortCF()
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
GTFGateValveGenerator::generateValve(FuncDataBase& Control,
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
  ELog::RegMethod RegA("GTFGateValveGenerator","generatorValve");


  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"LiftWidth",liftWidth);
  Control.addVariable(keyName+"LiftHeight",liftHeight);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);

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
  Control.addVariable(keyName+"BladeCutThick",bladeCutThick);
  Control.addVariable(keyName+"BladeScrewHousingRadius",bladeScrewHousingRadius);

  Control.addVariable(keyName+"ClampWidth",clampWidth);
  Control.addVariable(keyName+"ClampDepth",clampDepth);
  Control.addVariable(keyName+"ClampHeight",clampHeight);
  Control.addVariable(keyName+"ClampBulkHeight",clampBulkHeight);
  Control.addVariable(keyName+"ClampBaseThick",clampBaseThick);
  Control.addVariable(keyName+"ClampBaseWidth",clampBaseWidth);
  Control.addVariable(keyName+"ClampTopWidth",clampTopWidth);
  Control.addVariable(keyName+"LSFlangeWidth",lsFlangeWidth);
  Control.addVariable(keyName+"LSFlangeDepth",lsFlangeDepth);
  Control.addVariable(keyName+"LSFlangeHeight",lsFlangeHeight);
  Control.addVariable(keyName+"LSFlangeMatHoleRadius",lsFlangeHoleRadius);
  Control.addVariable(keyName+"LSShaftRadius",lsShaftRadius);
  Control.addVariable(keyName+"LSShaftThick",lsShaftThick);
  Control.addVariable(keyName+"LSShaftLength",lsShaftLength);
  Control.addVariable(keyName+"LSShaftFlangeRadius",lsShaftFlangeRadius);
  Control.addVariable(keyName+"LSShaftFlangeThick",lsShaftFlangeThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"BladeMat",bladeMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"ClampMat",clampMat);
  Control.addVariable(keyName+"LSFlangeMat",lsFlangeMat);

  return;

}

///\cond TEMPLATE

template void GTFGateValveGenerator::setCylCF<CF40>();
template void GTFGateValveGenerator::setCylCF<CF40_22>();
template void GTFGateValveGenerator::setCylCF<CF63>();
template void GTFGateValveGenerator::setCylCF<CF100>();

template void GTFGateValveGenerator::setCubeCF<CF40>();
template void GTFGateValveGenerator::setCubeCF<CF40_22>();
template void GTFGateValveGenerator::setCubeCF<CF63>();
template void GTFGateValveGenerator::setCubeCF<CF100>();

template void GTFGateValveGenerator::setAPortCF<CF40>();
template void GTFGateValveGenerator::setAPortCF<CF40_22>();
template void GTFGateValveGenerator::setAPortCF<CF63>();
template void GTFGateValveGenerator::setAPortCF<CF100>();

template void GTFGateValveGenerator::setBPortCF<CF40>();
template void GTFGateValveGenerator::setBPortCF<CF40_22>();
template void GTFGateValveGenerator::setBPortCF<CF63>();
template void GTFGateValveGenerator::setBPortCF<CF100>();

template void GTFGateValveGenerator::setPortPairCF<CF40,CF63>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
