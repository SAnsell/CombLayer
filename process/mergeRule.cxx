/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/mergeRule.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Transform.h"
#include "Rules.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Token.h"
#include "HeadRule.h"
#include "Object.h"
#include "AttachSupport.h"
#include "pairBase.h"
#include "pairItem.h"
#include "surfDBase.h"
#include "mergeRule.h"

namespace ModelSupport
{

mergeRule::mergeRule() : 
  surfDBase()
  /*!
    Constructor
  */
{}

mergeRule::mergeRule(const mergeRule& A) : 
  surfDBase(A),
  innerString(A.innerString),outerString(A.outerString),
  innerReplace(A.innerReplace),outerReplace(A.outerReplace),
  surfItems(A.surfItems)
  /*!
    Copy constructor
    \param A :: mergeRule to copy
  */
{}

mergeRule&
mergeRule::operator=(const mergeRule& A)
  /*!
    Assignment operator
    \param A :: mergeRule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      surfDBase::operator=(A);
      innerString=A.innerString;
      outerString=A.outerString;
      innerReplace=A.innerReplace;
      outerReplace=A.outerReplace;
    }
  return *this;
}


mergeRule*
mergeRule::clone() const
  /*!
    Clone copy constructor
    \return new(*this);
  */
{
  return new mergeRule(*this);
}

mergeRule::~mergeRule()
  /*!
    Delete operator
    \return new(*this);
  */
{
  std::vector<pairBase*>::iterator vc;
  for(vc=surfItems.begin();vc!=surfItems.end();vc++)
    delete *vc;
}



void
mergeRule::populate()
  /*!
    Populates all the surface
  */
{
  ELog::RegMethod RegA("mergeRule","populate");
  return;
}


int
mergeRule::createSurf(const double,int&)
  /*!
    Populates all the surface
  */
{
  ELog::RegMethod RegA("mergeRule","createSurf");
  return 0;
}


void
mergeRule::processInnerOuter(const int outerFlag,
			     std::vector<Token>& Cell) const
  /*!
    Process the inner cells
    \param outerFlag :: Decide if outer/inner prog.
    \param Cell :: Cell to process
  */
{
  ELog::RegMethod RegA("mergeRule","processInnerOuter");
  return;
}

// REAL STUFF:

void
mergeRule::addRule(const pairBase* PBPtr)
  /*!
    Adds an manages the pairBase object
   */
{
  ELog::RegMethod RegA("mergeRule","addRule");
  if (PBPtr)
    surfItems.push_back(PBPtr->clone());
  return;
}

void 
mergeRule::setInner(const std::string& IStr)
  /*!
    Set and process inner string 
    \param IStr :: inner string
   */
{
  innerString=IStr;
  return;
}

void 
mergeRule::setInnerReplace(const std::string& IStr)
  /*!
    Set and process inner string 
    \param IStr :: inner string
   */
{
  innerReplace=IStr;
  return;
}

void 
mergeRule::setOuter(const std::string& OStr)
  /*!
    Set and process outer string 
    \param IStr :: outer string
   */
{
  outerString=OStr;
  return;
}

void 
mergeRule::setOuterReplace(const std::string& OStr)
  /*!
    Set and process outer string 
    \param OStr :: outer string
   */
{
  outerReplace=OStr;
  return;
}

std::vector<size_t>
mergeRule::locateComp(const std::vector<Token>& cellTVec,
		      const std::vector<Token>& partTVec) const
  /*!
    Locate a component item in the cell item
    \param cellTVec :: Cell vector to search
    \param partTVec :: part to find
   */
{
  std::vector<size_t> Out;
  return Out;
}

bool
mergeRule::locateComponents(const std::vector<Token>& cellTVec)
  /*!
    locates the components items within the TVec list
   */
{
  ELog::RegMethod RegA("mergeRule","locateComponents");
  
  TVec=cellTVec;

  // Does this throw?
  MonteCarlo::Object ObjA(1,0,0.0,innerString);
  MonteCarlo::Object ObjB(2,0,0.0,outerString);

  const std::vector<Token> ITK=ObjA.cellVec();
  const std::vector<Token> OTK=ObjB.cellVec();

  return 1;  
}


} // NAMESPACE ModelSupport

