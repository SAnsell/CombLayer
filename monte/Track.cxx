/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/Track.cxx
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>

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
#include "OutputLog.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Track.h"


const double surfaceTolerance(1e-5);       ///< Below this two point touch.

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OX,const TUnits& A)
  /*!
    Output operator
    \param OX :: Output stream
    \param A :: TUnits item to write
    \return stream
  */
{
  A.write(OX);
  return OX;
}

TUnits::TUnits(const Geometry::Vec3D& A,const Geometry::Vec3D& B,
	       const Object* OC) :
  InitPt(A),ExitPt(B),length(A.Distance(B)),OCPtr(OC)
  /*!
    Constructor
    \param A :: First point
    \param B :: Last point
    \param OC :: Object that track traverses
  */
{}

TUnits::TUnits(const TUnits& A) :
  InitPt(A.InitPt),ExitPt(A.ExitPt),
  length(A.length),OCPtr(A.OCPtr)
  /*!
    Copy constructor
    \param A :: TUnits to copy
  */
{}

TUnits&
TUnits::operator=(const TUnits& A) 
  /*!
    Assignment operator
    \param A :: TUnits to copy
    \return *this
  */
{
  if (this!=&A)
    {
      InitPt=A.InitPt;
      ExitPt=A.ExitPt;
      length=A.length;
      OCPtr=A.OCPtr;
    }
  return *this;
}

void
TUnits::write(std::ostream& OX) const
  /*!
    Write out a TUnits object
    \param OX :: Output stream
  */
{
  OX<<InitPt<<" --> "<<ExitPt<<" ("<<length<<") in ";
  if (OCPtr)
    OX<<OCPtr->getName();
  else
    OX<<"void";
  return;
}

Track::Track() :
  iPt(0,0,0),iObj(0)
  /*!
    Constructor
  */ 
{}

Track::Track(const Geometry::Vec3D& StartPt,
	     const Object* initObj) : 
  iPt(StartPt),iObj(initObj)
  /*!
    Constructor
    \param StartPt :: Initial Point
    \param initObj :: inital object identifier
  */ 
{}

Track::Track(const Geometry::Vec3D& StartPt) :
  iPt(StartPt),iObj(0)
  /*!
    Constructor
    \param StartPt :: Initial Point
  */ 
{}

Track::Track(const Track& A) :
  iPt(A.iPt),iObj(A.iObj),
  Items(A.Items)
  /*!
    Copy Constructor
    \param A :: Track to copy
  */ 
{}

Track&
Track::operator=(const Track& A)
  /*!
    Assignment operator
    \param A :: Track to copy
    \return *this
  */ 
{
  if (this != &A)
    {
      iPt=A.iPt;
      iObj=A.iObj;
      Items=A.Items;
    }
  return *this;
}

Track::~Track()
  /*!
    Destructor
  */
{}

void 
Track::setFirst(const Geometry::Vec3D& StartPoint)
  /*!
    Sets the first Point
    \param StartPoint :: First Point
  */
{
  iPt=StartPoint;
  return;
}

void
Track::addPoint(const Object* ID,const Geometry::Vec3D& Pt) 
  /*!
    Objective is to merge in partial information
    about the beginning and end of the tracks.

    \param ID :: Id number of the object
    \param Pt :: Point to go
   */
{
  Items.push_back(TUnits(iPt,Pt,ID));
  iObj=ID;
  iPt=Pt;
  return;
}
  



} // NAMESPACE MonteCarlo
