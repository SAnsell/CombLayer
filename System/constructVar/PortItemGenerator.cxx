/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/PortItemGenerator.cxx
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
#include "PortItemGenerator.h"

namespace setVariable
{

PortItemGenerator::PortItemGenerator() :
  length(12.0),radius(5.0),wallThick(0.5),
  flangeLen(1.0),flangeRadius(1.0)

  /*!
    Constructor and defaults
  */
{}

PortItemGenerator::PortItemGenerator(const PortItemGenerator& A) : 
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  flangeLen(A.flangeLen),flangeRadius(A.flangeRadius)
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
  ELog::RegMethod RegA("PortItemGenerator","generatorPort");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Wall",wallThick);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLen);

  Control.addVariable(keyName+"Centre",C);
  Control.addVariable(keyName+"Axis",A.unit());
  
  return;

}


///\cond TEMPLATE

template void PortItemGenerator::setCF<CF40>(const double);
template void PortItemGenerator::setCF<CF50>(const double);
template void PortItemGenerator::setCF<CF63>(const double);
template void PortItemGenerator::setCF<CF100>(const double);

///\endcond  TEMPLATE
  

}  // NAMESPACE setVariable
