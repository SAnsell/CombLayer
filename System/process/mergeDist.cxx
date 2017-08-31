/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/mergeDist.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>
 
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
#include "Rules.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Token.h"
#include "AttachSupport.h"
#include "surfDBase.h"
#include "mergeDist.h"

namespace ModelSupport
{


mergeDist::mergeDist() : 
  InRule(0),OutRule(0)
  /*!
    Constructor
  */
{}

mergeDist::mergeDist(const mergeDist& A) : 
  surfDBase(A),
  SMap(A.SMap),InRule(A.InRule),OutRule(A.OutRule),sSurf(A.sSurf),
  expandValue(A.expandValue),bSurf(A.bSurf),innerToken(A.innerToken),
  outerToken(A.outerToken)
  /*!
    Copy constructor
    \param A :: mergeDist to copy
  */
{}

mergeDist&
mergeDist::operator=(const mergeDist& A)
  /*!
    Assignment operator
    \param A :: mergeDist to copy
    \return *this
  */
{
  if (this!=&A)
    {
      surfDBase::operator=(A);
      SMap=A.SMap;
      sSurf=A.sSurf;
      expandValue=A.expandValue;
      innerToken=A.innerToken;
      outerToken=A.outerToken;
    }
  return *this;
}


mergeDist*
mergeDist::clone() const
  /*!
    Clone copy constructor
    \return new(*this);
  */
{
  return new mergeDist(*this);
}


mergeDist::~mergeDist()
  /*!
    Delete operator
    \return new(*this);
  */
{
  delete InRule;
  delete OutRule;
}



void
mergeDist::clearRules()
  /*!
    Simple helper function to remove old rules
  */
{
  delete InRule;
  delete OutRule;
  InRule=0;
  OutRule=0;
  return;
}

void
mergeDist::addRules() 
  /*!
    Adds the surface object to the rules in a simple way
  */
{
  ELog::RegMethod RegA("mergeDist","addRules");

  clearRules();


  // std::vector<Geometry::Surface*>::const_iterator ac;  
  // const Geometry::Surface* SPtr(0);
  // for(size_t i=0;i<OSPtr.size();i++)
  //   {
  //     SPtr = OSPtr[i];
  //     const int dirSign=secDir[i];
  //     attachSystem::addUnion(dirSign*SPtr->getName(),SPtr,InRule);
  //     attachSystem::addIntersection(-dirSign*SPtr->getName(),SPtr,OutRule);
  //   }
  return;
}

void 
mergeDist::addSurf(const int SN,const double fullDist)
  /*!
    Set the expand distance and surface
   */
{
  sSurf.push_back(SN);
  expandValue.push_back(fullDist);
  bSurf.push_back(0);
  return;
}

int
mergeDist::createSurf(const double fraction,int& newItem)
  /*!
    Expands the surface to the correct point
    if the two planes have opposite.
    \param newItem :: Plane number to start with
    \param fraction :: Weight between the surface and the outer plane number
    \return plane number created
   */
{
  ELog::RegMethod RegA("mergeDist","createSurf");

  for(size_t i=0;i<sSurf.size();i++)
    {
      Geometry::Surface* SX=
	ModelSupport::surfaceCreateExpand(bSurf[i],expandValue[i]*fraction);
      SX->setName(newItem);
      // Note SX may have a different surface number
      SMap.registerSurf(newItem++,SX);
    }
  return newItem;
}

void
mergeDist::populate()
  /*!
    Populates all the surface
  */
{
  ELog::RegMethod RegA("mergeDist","populate");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  for(size_t i=0;i<sSurf.size();i++)
    {
      const Geometry::Surface* SX=SurI.getSurf(sSurf[i]);
      if (!SX)
	ELog::EM<<"Failed on primary surface "<<sSurf[i]<<ELog::endErr;
      bSurf[i]=SX;
    }
  
  return;
} 

void
mergeDist::processInnerOuter(const int,
			     std::vector<Token>&) const
/*!
  Process the cells 
  \param outerFlag :: Decide if outer/inner process.
  \param Cell :: Cell to process
*/
{
  ELog::RegMethod RegA("mergeDist","processInnerOuter");

  return;
}

} // NAMESPACE ModelSupport
