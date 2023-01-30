/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/TubeDetBoxGenerator.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "TubeDetBoxGenerator.h"

namespace setVariable
{

TubeDetBoxGenerator::TubeDetBoxGenerator() :
  centRadius(2.85),tubeRadius(2.5),
  wallThick(0.3),height(60.0),
  gap(0.25),outerThick(1.0),
  detMat("He3_10Bar"),wallMat("Aluminium"),
  outerMat("B4C"),filterMat("Hafnium")
  /*!
    Constructor and defaults
  */
{}

TubeDetBoxGenerator::TubeDetBoxGenerator(const TubeDetBoxGenerator& A) : 
  centRadius(A.centRadius),tubeRadius(A.tubeRadius),
  wallThick(A.wallThick),height(A.height),gap(A.gap),
  outerThick(A.outerThick),detMat(A.detMat),wallMat(A.wallMat),
  outerMat(A.outerMat),filterMat(A.filterMat)
  /*!
    Copy constructor
    \param A :: TubeDetBoxGenerator to copy
  */
{}

TubeDetBoxGenerator&
TubeDetBoxGenerator::operator=(const TubeDetBoxGenerator& A)
  /*!
    Assignment operator
    \param A :: TubeDetBoxGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      centRadius=A.centRadius;
      tubeRadius=A.tubeRadius;
      wallThick=A.wallThick;
      height=A.height;
      gap=A.gap;
      outerThick=A.outerThick;
      detMat=A.detMat;
      wallMat=A.wallMat;
      outerMat=A.outerMat;
      filterMat=A.filterMat;
    }
  return *this;
}

TubeDetBoxGenerator::~TubeDetBoxGenerator()
 /*!
   Destructor
 */
{}

void
TubeDetBoxGenerator::generateBox(FuncDataBase& Control,
				 const std::string& keyName,
				 const Geometry::Vec3D& xyz,
				 const size_t nDet) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param xyz :: Position offset
    \param nDet :: number of detectors
  */
{
  ELog::RegMethod RegA("TubeDetBoxGenerator","generatorBox");

  Control.addVariable(keyName+"NDetectors",nDet);

  
  Control.addVariable(keyName+"Offset",xyz);
  Control.addVariable(keyName+"CentRadius",centRadius);
  Control.addVariable(keyName+"TubeRadius",tubeRadius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Gap",gap);
  Control.addVariable(keyName+"OuterThick",outerThick);

  Control.addVariable(keyName+"DetMat",detMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"OuterMat",outerMat);
  Control.addVariable(keyName+"FilterMat",filterMat);
  
  return;

}

}  // NAMESPACE setVariable
