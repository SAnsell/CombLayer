/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/HorseCollarGenerator.cxx
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include "HorseCollarGenerator.h"

namespace setVariable
{

HorseCollarGenerator::HorseCollarGenerator() :
  length(30.0),
  internalRadius(12),mainRadius(20),
  mainMat("Void")
  /*!
    Constructor and defaults
  */
{}

HorseCollarGenerator::HorseCollarGenerator(const HorseCollarGenerator& A) : 
  length(A.length),
  internalRadius(A.internalRadius),mainRadius(A.mainRadius),
  mainMat(A.mainMat)
  /*!
    Copy constructor
    \param A :: HorseCollarGenerator to copy
  */
{}

HorseCollarGenerator&
HorseCollarGenerator::operator=(const HorseCollarGenerator& A)
  /*!
    Assignment operator
    \param A :: HorseCollarGenerator to copy
    \return *this
  */
{
  if (this!=&A)
  {
    length=A.length;
    internalRadius=A.internalRadius;
    mainRadius=A.mainRadius;
    mainMat=A.mainMat;
  }
  return *this;
}

HorseCollarGenerator::~HorseCollarGenerator()
 /*!
   Destructor
 */
{}

void
HorseCollarGenerator::setMaterial(const std::string& MM)
  /*!
    Set the materials
    \param MM :: Main material
  */
{
  ELog::RegMethod RegA("HorseCollarGenerator","setMaterial");
  mainMat=MM;
  return;
}

void
HorseCollarGenerator::generateHorseCollar(FuncDataBase& Control,
                                          const std::string& keyName,
                                          const double yStep,
                                          const double length,
                                          const double internalRadius,
                                          const double mainRadius) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param yStep :: y-offset
    \param length :: Length
    \param internalRadius :: Internal Radius
    \param mainRadius :: Main Radius
  */
{
  ELog::RegMethod RegA("HorseCollarGenerator","generatorHorseCollar");
  
  Control.addVariable(keyName+"XStep",0.0);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",0);
  Control.addVariable(keyName+"XYangle",0.0);
  Control.addVariable(keyName+"Zangle",0.0);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"InternalRadius",internalRadius);
  Control.addVariable(keyName+"MainRadius",mainRadius);
  
  Control.addVariable(keyName+"MainMat",mainMat);
  
  if (mainRadius-internalRadius<Geometry::zeroTol)
    ELog::EM<<"Failure: Main Radius="<<mainRadius<<" Internal Radius="<<internalRadius<<ELog::endErr;
  return;

}

}  // NAMESPACE setVariable
