/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/DiffPumpGenerator.cxx
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
  length(20.0),width(10.0),height(10.0),
  innerLength(16.0),
  captureWidth(2.0),captureHeight(2.0),
  magWidth(5.0),magHeight(5.0),
  innerRadius(CF16::innerRadius),
  innerThick(CF16::wallThick),
  portLength(3.0),portRadius(CF40::innerRadius),
  portThick(CF40::wallThick),
  flangeRadius(CF40::flangeRadius),
  flangeLength(CF40::flangeLength),
  voidMat("Void"),
  pipeMat("Stainless304L"),
  mainMat("Stainless304L"),
  magnetMat("NbFeB"),
  flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

DiffPumpGenerator::~DiffPumpGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
DiffPumpGenerator::setCF()
  /*!
    Setter for flange beam direction flanges
  */
{
  innerRadius=CF::innerRadius;
  innerThick=CF::innerThick;

  return;
}

template<typename CF>
void
DiffPumpGenerator::setPortCF(const double L)
  /*!
    Setter for flange/port beam direction flanges
    \param L :: length
  */
{
  portLength=L;
  portRadius=CF::innerRadius;
  portThick=CF::wallThick;
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}

void
DiffPumpGenerator::generatePump(FuncDataBase& Control,
				const std::string& keyName) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("DiffPumpGenerator","generatePump");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"InnerLength",innerLength);
  Control.addVariable(keyName+"CaptureWidth",captureWidth);
  Control.addVariable(keyName+"CaptureHeight",captureHeight);
  Control.addVariable(keyName+"MagWidth",magWidth);
  Control.addVariable(keyName+"MagHeight",magHeight);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"InnerThick",innerThick);
  Control.addVariable(keyName+"PortLength",portLength);
  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"PortThick",portThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"MagnetMat",magnetMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;
  
}

///\cond TEMPLATE

template void DiffPumpGenerator::setPortCF<CF40>(const double);



///\endcond TEPLATE

}  // NAMESPACE setVariable
