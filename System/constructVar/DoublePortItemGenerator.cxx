/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/DoublePortItemGenerator.cxx
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
#include "PortItemGenerator.h"
#include "DoublePortItemGenerator.h"

namespace setVariable
{

DoublePortItemGenerator::DoublePortItemGenerator() :
  PortItemGenerator(),partLength(6.0),radiusB(8.0)
  /*!
    Constructor and defaults
  */
{}

DoublePortItemGenerator::DoublePortItemGenerator
(const DoublePortItemGenerator& A) :
  PortItemGenerator(A),
  partLength(A.partLength),radiusB(A.radiusB)

  /*!
    Copy constructor
    \param A :: DoublePortItemGenerator to copy
  */
{}

DoublePortItemGenerator&
DoublePortItemGenerator::operator=(const DoublePortItemGenerator& A)
  /*!
    Assignment operator
    \param A :: DoublePortItemGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      PortItemGenerator::operator=(A);
      partLength=A.partLength;
      radiusB=A.radiusB;
    }
  return *this;
}


DoublePortItemGenerator::~DoublePortItemGenerator() 
 /*!
   Destructor
 */
{}

void
DoublePortItemGenerator::generatePort(FuncDataBase& Control,
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
  ELog::RegMethod RegA("DoublePortItemGenerator","generatePort");

  
  Control.addVariable(keyName+"ExternPartLength",partLength);
  Control.addVariable(keyName+"RadiusB",radiusB);
  PortItemGenerator::generatePort(Control,keyName,C,A);
  
  return;

}


  

}  // NAMESPACE setVariable
