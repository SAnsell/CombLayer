/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/BoundOuter.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"

#include "BoundOuter.h"

namespace attachSystem
{

BoundOuter::BoundOuter() :
  setFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

BoundOuter::BoundOuter(const BoundOuter& A) : 
  setFlag(A.setFlag),
  innerStruct(A.innerStruct),outerStruct(A.outerStruct)
  /*!
    Copy constructor
    \param A :: BoundOuter to copy
  */
{}

BoundOuter&
BoundOuter::operator=(const BoundOuter& A)
  /*!
    Assignment operator
    \param A :: BoundOuter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      setFlag=A.setFlag;
      innerStruct=A.innerStruct;
      outerStruct=A.outerStruct;
    }
  return *this;
}

BoundOuter::~BoundOuter() 
  /*!
    Destructor
  */
{}
  
void
BoundOuter::setInnerSurf(const int SN)
  /*!
    Set the inner surface based on surface numbmer
    \param SN :: Surface number
  */
{
  ELog::RegMethod RegA("BoundOuter","setInnerSurf");
  innerStruct.procSurfNum(SN);
  return;
}

  
void
BoundOuter::setInner(const HeadRule& HR)
  /*!
    Set the inner volume
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("BoundOuter","setInner");

  innerStruct=HR;
  innerStruct.makeComplement();
  setFlag ^= 1;
  return;
}

void
BoundOuter::setInnerExclude(const HeadRule& HR)
  /*!
    Set the inner volume (without inverse)
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("BoundOuter","setInnerExclude");

  innerStruct=HR;
  setFlag ^= 1;
  return;
}

void
BoundOuter::setOuter(const HeadRule& HR)
  /*!
    Set the outer volume
    \param HR :: Headrule of outer surfaces
  */
{
  ELog::RegMethod RegA("BoundOuter","setInner");
  
  outerStruct=HR;
  setFlag ^= 2;
  return;
}

void
BoundOuter::setOuterExclude(const HeadRule& HR)
  /*!
    Set the outer volume
    \param HR :: Headrule of outer surfaces
  */
{
  ELog::RegMethod RegA("BoundOuter","setInner");
  
  outerStruct=HR;
  outerStruct.makeComplement();
  setFlag ^= 2;
  return;
}
  

}  // NAMESPACE attachSystem
