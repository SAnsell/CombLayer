/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/PortItemGenerator.cxx
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
#include "PortItemGenerator.h"

namespace setVariable
{

PortItemGenerator::PortItemGenerator() :
  length(12.0),radius(5.0),wallThick(0.5),
  flangeLen(1.0),flangeRadius(1.0),
  capThick(0.0),windowThick(0.0),windowRadius(0.0),
  wallMat("Stainless304"),capMat("Aluminium"),
  windowMat("LeadGlass"),outerVoidMat("Void"),
  outerVoid(1)
  /*!
    Constructor and defaults
  */
{}

PortItemGenerator::PortItemGenerator(const PortItemGenerator& A) :
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  flangeLen(A.flangeLen),flangeRadius(A.flangeRadius),
  capThick(A.capThick),windowThick(A.windowThick),
  windowRadius(A.windowRadius),wallMat(A.wallMat),
  capMat(A.capMat),windowMat(A.windowMat),outerVoidMat(A.outerVoidMat),
  outerVoid(A.outerVoid)
  /*!
    Copy constructor
    \param A :: PortItemGenerator to copy
  */
{}

PortItemGenerator&
PortItemGenerator::operator=(const PortItemGenerator& A)
  /*!
    Assignment operator
    \param A :: PortItemGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      length=A.length;
      radius=A.radius;
      wallThick=A.wallThick;
      flangeLen=A.flangeLen;
      flangeRadius=A.flangeRadius;
      capThick=A.capThick;
      windowThick=A.windowThick;
      windowRadius=A.windowRadius;
      wallMat=A.wallMat;
      capMat=A.capMat;
      windowMat=A.windowMat;
      outerVoidMat=A.outerVoidMat;
      outerVoid=A.outerVoid;
    }
  return *this;
}


PortItemGenerator::~PortItemGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
PortItemGenerator::setCF(const double L)
  /*!
    Set pipe/flange to CF-X format
    \param L :: length of tube
  */
{
  length=L;
  radius=CF::innerRadius;
  wallThick=CF::wallThick;
  flangeLen=CF::flangeLength;
  flangeRadius=CF::flangeRadius;
  capThick=CF::flangeLength;

  return;
}


void
PortItemGenerator::setPort(const double L,const double R,
			   const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param R :: lenght of port tube
    \param T :: Thickness of port tube
   */
{
  radius=R;
  length=L;
  wallThick=T;
  return;
}

void
PortItemGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeRadius=R;
  flangeLen=L;
  return;
}

void
PortItemGenerator::setPlate(const double T,const std::string& PM)
  /*!
    Set the support flange (top) plate thickness and material
    \param T :: Plate thickness
    \param PM :: material for plate
   */
{
  capThick=T;
  capMat=PM;
  return;
}


void
PortItemGenerator::setWindowPlate(const double capT,
				  const double winT,
				  const double winRad,
				  const std::string& CM,
				  const std::string& WM)
  /*!
    Set the support flange (top) plate thickness and material
    \param capT :: Plate thickness
    \param winT :: window thickness
    \param winRad :: window radius [-ve for fractional]
    \param CM :: material for cap
    \param WM :: material for window
   */
{
  capThick=capT;
  windowThick=winT;
  windowRadius=winRad;
  capMat=CM;
  windowMat=WM;
  return;
}

void
PortItemGenerator::setNoPlate()
  /*!
    Remove the outer plate
    \param T :: Plate thickness
    \param PM :: material for plate
   */
{
  capThick=0.0;
  capMat="Void";
  return;
}


void
PortItemGenerator::generatePort(FuncDataBase& Control,
				const std::string& keyName,
				const Geometry::Vec3D& C,
				const Geometry::Vec3D& A) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param C :: centre
    \param A :: axis
  */
{
  ELog::RegMethod RegA("PortItemGenerator","generatePort");

  Control.addVariable(keyName+"PortType","Standard");
  
  const double WThick((windowThick < -Geometry::zeroTol) ?
		      -windowThick*capThick : windowThick);

  const double WRadius((windowRadius < -Geometry::zeroTol)  ?
		       -windowRadius*flangeRadius : windowRadius);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Wall",wallThick);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLen);
  Control.addVariable(keyName+"CapThick",capThick);
  Control.addVariable(keyName+"WindowThick",WThick);
  Control.addVariable(keyName+"WindowRadius",WRadius);

  Control.addVariable(keyName+"Centre",C);
  Control.addVariable(keyName+"Axis",A.unit());

  Control.addVariable(keyName+"OuterVoid",static_cast<int>(outerVoid));
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"CapMat",capMat);
  Control.addVariable(keyName+"WindowMat",windowMat);
  Control.addVariable(keyName+"OuterVoidMat",outerVoidMat);

  return;

}

void
PortItemGenerator::generateAnglePort(FuncDataBase& Control,
				     const std::string& keyName,
				     const Geometry::Vec3D& C,
				     const Geometry::Vec3D& AAxis,
				     const Geometry::Vec3D& BAxis,
				     const double bLength) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param C :: centre
    \param AAxis :: axis of first section
    \param BAxis :: axis after turn
    \param bLength :: length of second section
  */
{
  ELog::RegMethod RegA("PortItemGenerator","generateAnglePort");

  generatePort(Control,keyName,C,AAxis);
  Control.addVariable(keyName+"PortType","Angle");
  
  Control.addVariable(keyName+"BAxis",BAxis.unit());
  Control.addVariable(keyName+"LengthB",length);
  Control.addVariable(keyName+"LengthB",bLength);

  return;
}


///\cond TEMPLATE

template void PortItemGenerator::setCF<CF34_TDC>(const double);
template void PortItemGenerator::setCF<CF35_TDC>(const double);
template void PortItemGenerator::setCF<CF37_TDC>(const double);
template void PortItemGenerator::setCF<CF40>(const double);
template void PortItemGenerator::setCF<CF40_22>(const double);
template void PortItemGenerator::setCF<CF50>(const double);
template void PortItemGenerator::setCF<CF63>(const double);
template void PortItemGenerator::setCF<CF100>(const double);
template void PortItemGenerator::setCF<CF120>(const double);
template void PortItemGenerator::setCF<CF150>(const double);
template void PortItemGenerator::setCF<CF350>(const double);

///\endcond  TEMPLATE


}  // NAMESPACE setVariable
