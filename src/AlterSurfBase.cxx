/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/AlterSurfBase.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "Exception.h"
#include "MapSupport.h"
#include "Triple.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "AlterSurfBase.h"

AlterSurfBase::AlterSurfBase(FuncDataBase& A) :
  ConVar(A)
  /*!
    Constructor. Requires that the FuncDataBase object
    has been fully populated.
    \param A :: Reference to FuncDataBase object 
  */
{}

AlterSurfBase::AlterSurfBase(const AlterSurfBase& A) :
  ConVar(A.ConVar),AMap(A.AMap),rDead(A.rDead)
  /*!
    Basic copy constructor
    \param A :: AlterSurfBase object to copy
  */
{}

AlterSurfBase&
AlterSurfBase::operator=(const AlterSurfBase& A) 
  /*!
    Assignment operator 
    \param A :: AlterSurfBase object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ConVar=A.ConVar;
      AMap=A.AMap;
      rDead=A.rDead;
    }
  return *this;
}

AlterSurfBase::~AlterSurfBase()
  /*!
    Destructor
  */
{}

void
AlterSurfBase::reInit()
  /*!
    Reinitialize change 
  */
{
  AMap.erase(AMap.begin(),AMap.end());
  setMap();
  setMapZero();
  std::set<int>::const_iterator vc;
  for(vc=rDead.begin();vc!=rDead.end();vc++)
    setChange(*vc,0);

  return;
}

void
AlterSurfBase::addDeadCell(const int CellN)
  /*!
    Add a dead cell.
    \param CellN :: Cell number
  */
{
  if (rDead.find(CellN)==rDead.end())
    {
      rDead.insert(CellN);
      setChange(CellN,0);
    }
  return;
}

void
AlterSurfBase::setChange(const int A,const int B)
  /*!
    Given a new surf->surf global mapping
    this function adds to the AMap.
    It also updates the remainder of the map 
    so that any X->A become X->B and a
    B->Y which then make A->Y not A->B
    \param A :: Surface to change
    \param B :: Surface to be modified to (signed)
  */
{
  if (A<=0)
    throw ColErr::RangeError<int>(A,1,99999,"AlterSurfBase::setChange");
  if (A==B || A==-B)
    throw ColErr::MisMatch<int>(A,B,"AlterSurfBase::setChange");
  
  std::map<int,int>::iterator vc;
  const int signB((B>0) ? 1 : -1);
  const int outPlane(B*signB);   // +ve only [search positive ]

  if (outPlane)
    {
      vc=AMap.find(outPlane);
      if (vc!=AMap.end())
	AMap[A]=vc->second*signB;
      else
	AMap[A]=B;
    }
  else
    AMap[A]=B;

  // Positive search FIRST:
  vc=find_if(AMap.begin(),AMap.end(),
	     MapSupport::valEqual<int,int>(A));
  while (vc!=AMap.end())
    {
      vc->second= B;
      vc=find_if(vc,AMap.end(),
		 MapSupport::valEqual<int,int>(A));
    }

  // Negative search FIRST:
  vc=find_if(AMap.begin(),AMap.end(),
	     MapSupport::valEqual<int,int>(-A));
  while (vc!=AMap.end())
    {
      vc->second= -B;
      vc=find_if(vc,AMap.end(),
		 MapSupport::valEqual<int,int>(-A));
    }
  return;
}

