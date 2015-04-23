/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physics/ExpControl.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <sstream>
#include <vector>
#include <list>
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
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "EUnit.h"
#include "ExpControl.h"

namespace physicsSystem
{
  
ExpControl::ExpControl()
  /*!
    Constructor
  */
{}

ExpControl::ExpControl(const ExpControl& A) :
  particles(A.particles),
  MapItem(A.MapItem),
  CentMap(A.CentMap)
  /*!
    Copy constructor
    \param A :: ExpControl to copy
  */
{}

ExpControl&
ExpControl::operator=(const ExpControl& A)
  /*!
    Assignment operator
    \param A :: ExpControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particles=A.particles;
      MapItem=A.MapItem;
      CentMap=A.CentMap;
    }
  return *this;
}

ExpControl::~ExpControl()
  /*!
    Destructor
  */
{}

void
ExpControl::clear()
  /*!
    Clear control
  */
{
  particles.clear();
  MapItem.erase(MapItem.begin(),MapItem.end());
  CentMap.erase(CentMap.begin(),CentMap.end());
  return;
}

void
ExpControl::addElm(const std::string& EN) 
  /*!
    Adds an element to the particles list
    (note it also checks the element)
    \param EN :: element identifier
  */
{
  particles.insert(EN);
  return;
}

void
ExpControl::addVect(const size_t index,const Geometry::Vec3D& V)
  /*!
    Add a vector to the centre list
    \param index :: vector index
    \param V :: Vector to add
  */
{
  if (index<=0)
    CentMap[index]=V;
  return;
}
  
int
ExpControl::addUnit(const int cellN,const std::string& unitStr)
  /*!
    Add a exp component
    \param cellN :: Cell number
    \param unitStr :: String of MCNP form
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("ExpControl","addComp");

  typedef std::map<int,EUnit>::value_type VTYPE;
  
  if (unitStr.empty())
    return 0;

  std::string Unit(unitStr);  //strip space?
  const size_t uSize(Unit.size());
  size_t index(0);
  int mFound(1);
  int sFound(0);
  
  // Starts with -ve :
  if (Unit[0]=='-')
    {
      Unit[0]=' ';
      mFound=-1;
      index++;
    }
  double D(2.0);         // Note over 1.0
  if (StrFunc::sectPartNum(Unit,D))
    {
      if (fabs(D) < Geometry::zeroTol)
	mFound=0;
      // Option 0 / 1 [no letters]
      if (Unit.empty())
	{
	  MapItem.insert(VTYPE(cellN,EUnit(mFound*2,0,D,0)));
	  return 1;
	}
    }
  else if (index<uSize &&
	   (Unit[index]=='s' || Unit[index]=='S'))
    {
      Unit[index]=' ';
      sFound=1;
      index++;
      if (index==uSize)
	{
	  MapItem.insert(VTYPE(cellN,EUnit(mFound,0,0.0,0)));
	  return 1;
	}
    }
  // Now check for X/Y/Z and V
  if (index<uSize &&   
      (Unit[index]=='V' || Unit[index]=='v'))
    {
      size_t VNum;
      Unit[index]=' ';
      if (StrFunc::section(Unit,VNum) && VNum)
	{
	  if (sFound)
	    MapItem.insert(VTYPE(cellN,EUnit(mFound,0,D,VNum)));
	  else
	    MapItem.insert(VTYPE(cellN,EUnit(2*mFound,0,D,VNum)));
	  return 1;
	}
      ELog::EM<<"Failed to understand V Cell :"<<Unit<<ELog::endErr;
      return 0;
    }
  
  if (index<uSize &&   
      ((Unit[index]=='X' || Unit[index]=='x') ||
       (Unit[index]=='Y' || Unit[index]=='y') ||
       (Unit[index]=='Z' || Unit[index]=='z')) )
    {
      const size_t dirType=static_cast<size_t>
	((std::toupper(Unit[index])-'X')+1);
      if (sFound)
	MapItem.insert(VTYPE(cellN,EUnit(mFound,dirType,D,0)));
      else if (D>1.1)
	MapItem.insert(VTYPE(cellN,EUnit(3*mFound,dirType,0.0,0)));
      else 
	MapItem.insert(VTYPE(cellN,EUnit(2*mFound,dirType,D,0)));
      return 1;
    }
  // MUST BE invalid :
  ELog::EM<<"Failed to understand : "<<Unit<<ELog::endErr;
  return 0;
}

void
ExpControl::writeHeader(std::ostream& OX) const
  /*!
    Write the particle header set
    \param OX :: Output stream
   */
{
  OX<<"exp:";
  bool first(1);
  for(const std::string& P : particles)
    {
      if (!first)
	OX<<",";
      OX<<P;
      first=0;
    }
  if (first) OX<<"n";         // default particle is neutron
  OX<<" ";
  return;
}
  
void
ExpControl::write(std::ostream& OX,
		  const std::vector<int>& cellOutOrder) const
  /*!
    Write out the card
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
  */
{
  ELog::RegMethod RegA("ExpControl","write");

  if (!MapItem.empty())
    {
      std::ostringstream cx;
      writeHeader(cx);
      for(const int CN : cellOutOrder)
	{
	  std::map<int,EUnit>::const_iterator mc=MapItem.find(CN);
	  if (mc!=MapItem.end())
	    mc->second.write(cx);
	  else
	    cx<<"0 ";
	}
      StrFunc::writeMCNPX(cx.str(),OX);

      if (!CentMap.empty())
	{
	  // Write ordered list of CentMap:
	  cx.str("");
	  cx<<"vect  ";
	  typedef std::map<size_t,Geometry::Vec3D>::value_type CMVAL;
	  for(const CMVAL& mc : CentMap)
	    cx<<" V"<<mc.first<<" "<<mc.second;

	  StrFunc::writeMCNPX(cx.str(),OX);
	}
    }
   
  return;
}


} // NAMESPACE physicsCards
      
   
