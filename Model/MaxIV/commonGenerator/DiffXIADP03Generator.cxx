/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/DiffXIADP03Generator.cxx
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
#include "DiffXIADP03Generator.h"

namespace setVariable
{


DiffXIADP03Generator::DiffXIADP03Generator() :
  width(15.28),height(6.52),
  apertureWidth(0.7),apertureHeight(0.7),
  flangeRadius(CF150::flangeRadius),
  flangeThick(CF150::flangeLength),
  flangeVoidWidth(14.61),flangeVoidHeight(5.71),
  flangeVoidThick(4.44),magnetWidth(12.7),
  magnetLength(36.83),magnetThick(2.54),
  magnetGap(0.17),
  mat("Stainless304"),magnetMat("Fe2O3"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}


DiffXIADP03Generator::DiffXIADP03Generator(const CF40&) :
  width(6.4),height(6.4),
  apertureWidth(0.7),apertureHeight(0.7),
  flangeRadius(CF40::flangeRadius),
  flangeThick(CF40::flangeLength),
  flangeVoidWidth(5.0),flangeVoidHeight(5.0),
  flangeVoidThick(CF40::flangeLength+1.2),magnetWidth(4.7),
  magnetLength(10.0),magnetThick(1.54),
  magnetGap(0.17),
  mat("Stainless304"),magnetMat("Fe2O3"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{

}

DiffXIADP03Generator::DiffXIADP03Generator(const DiffXIADP03Generator& A) : 
  width(A.width),height(A.height),apertureWidth(A.apertureWidth),
  apertureHeight(A.apertureHeight),flangeRadius(A.flangeRadius),
  flangeThick(A.flangeThick),flangeVoidWidth(A.flangeVoidWidth),
  flangeVoidHeight(A.flangeVoidHeight),flangeVoidThick(A.flangeVoidThick),
  magnetWidth(A.magnetWidth),magnetLength(A.magnetLength),
  magnetThick(A.magnetThick),magnetGap(A.magnetGap),
  mat(A.mat),magnetMat(A.magnetMat),flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: DiffXIADP03Generator to copy
  */
{}

DiffXIADP03Generator&
DiffXIADP03Generator::operator=(const DiffXIADP03Generator& A)
  /*!
    Assignment operator
    \param A :: DiffXIADP03Generator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      width=A.width;
      height=A.height;
      apertureWidth=A.apertureWidth;
      apertureHeight=A.apertureHeight;
      flangeRadius=A.flangeRadius;
      flangeThick=A.flangeThick;
      flangeVoidWidth=A.flangeVoidWidth;
      flangeVoidHeight=A.flangeVoidHeight;
      flangeVoidThick=A.flangeVoidThick;
      magnetWidth=A.magnetWidth;
      magnetLength=A.magnetLength;
      magnetThick=A.magnetThick;
      magnetGap=A.magnetGap;
      mat=A.mat;
      magnetMat=A.magnetMat;
      flangeMat=A.flangeMat;
    }
  return *this;
}


DiffXIADP03Generator::~DiffXIADP03Generator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
DiffXIADP03Generator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  flangeRadius=CF::flangeRadius;
  flangeThick=CF::flangeLength;
  
  return;
}



void
DiffXIADP03Generator::generatePump(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: length
  */
{
  ELog::RegMethod RegA("DiffXIADP03Generator","generatorValve");
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"ApertureHeight",apertureHeight);
  Control.addVariable(keyName+"ApertureWidth",apertureWidth);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeThick",flangeThick);
  Control.addVariable(keyName+"FlangeVoidWidth",flangeVoidWidth);
  Control.addVariable(keyName+"FlangeVoidHeight",flangeVoidHeight);
  Control.addVariable(keyName+"FlangeVoidThick",flangeVoidThick);

  Control.addVariable(keyName+"MagnetWidth",magnetWidth);
  Control.addVariable(keyName+"MagnetLength",magnetLength);
  Control.addVariable(keyName+"MagnetThick",magnetThick);
  Control.addVariable(keyName+"MagnetGap",magnetGap);
  
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"MagnetMat",magnetMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  
  return;

}

///\cond TEMPLATE

template void DiffXIADP03Generator::setCF<CF40>();
template void DiffXIADP03Generator::setCF<CF100>();
template void DiffXIADP03Generator::setCF<CF150>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
