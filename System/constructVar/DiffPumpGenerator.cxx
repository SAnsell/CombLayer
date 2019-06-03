/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/DiffPumpGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "DiffPumpGenerator.h"

namespace setVariable
{

DiffPumpGenerator::DiffPumpGenerator() :
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

DiffPumpGenerator::DiffPumpGenerator(const DiffPumpGenerator& A) : 
  width(A.width),height(A.height),apertureWidth(A.apertureWidth),
  apertureHeight(A.apertureHeight),flangeRadius(A.flangeRadius),
  flangeThick(A.flangeThick),flangeVoidWidth(A.flangeVoidWidth),
  flangeVoidHeight(A.flangeVoidHeight),flangeVoidThick(A.flangeVoidThick),
  magnetWidth(A.magnetWidth),magnetLength(A.magnetLength),
  magnetThick(A.magnetThick),magnetGap(A.magnetGap),
  mat(A.mat),magnetMat(A.magnetMat),flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: DiffPumpGenerator to copy
  */
{}

DiffPumpGenerator&
DiffPumpGenerator::operator=(const DiffPumpGenerator& A)
  /*!
    Assignment operator
    \param A :: DiffPumpGenerator to copy
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


DiffPumpGenerator::~DiffPumpGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
DiffPumpGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  flangeRadius=CF::flangeRadius;
  flangeThick=CF::flangeLength;
  
  return;
}



void
DiffPumpGenerator::generatePump(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: length
  */
{
  ELog::RegMethod RegA("DiffPumpGenerator","generatorValve");
  
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

template void DiffPumpGenerator::setCF<CF100>();
template void DiffPumpGenerator::setCF<CF150>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
