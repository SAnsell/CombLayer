/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ExtControl.cxx
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
#include "MapRange.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "EUnit.h"
#include "ExtControl.h"

namespace physicsSystem
{
  
ExtControl::ExtControl()
  /*!
    Constructor
  */
{}

ExtControl::ExtControl(const ExtControl& A) :
  particles(A.particles),
  MapItem(A.MapItem),
  CentMap(A.CentMap),
  renumberMap(A.renumberMap)
  /*!
    Copy constructor
    \param A :: ExtControl to copy
  */
{}

ExtControl&
ExtControl::operator=(const ExtControl& A)
  /*!
    Assignment operator
    \param A :: ExtControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particles=A.particles;
      MapItem=A.MapItem;
      CentMap=A.CentMap;
      renumberMap=A.renumberMap;
    }
  return *this;
}

ExtControl::~ExtControl()
  /*!
    Destructor
  */
{}

void
ExtControl::clear()
  /*!
    Clear control
  */
{
  particles.clear();
  MapItem.erase(MapItem.begin(),MapItem.end());
  CentMap.erase(CentMap.begin(),CentMap.end());
  renumberMap.erase(renumberMap.begin(),renumberMap.end());
  return;
}

void
ExtControl::addElm(const std::string& EN) 
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
ExtControl::setVect(const size_t index,const Geometry::Vec3D& V)
  /*!
    Add a vector to the centre list
    \param index :: vector index
    \param V :: Vector to add
  */
{
  CentMap[index]=V;
  return;
}

size_t
ExtControl::addVect(const Geometry::Vec3D& V)
  /*!
    Add a vector to the centre list 
    \param V :: Vector to add
    \return number 
  */
{
  size_t index(1);
  while(CentMap.find(index)!=CentMap.end())
    index++;
  
  CentMap.insert(std::map<size_t,Geometry::Vec3D>::value_type(index,V));
  return index;
}

int
ExtControl::addUnitList(int& cellN,const std::string& unitStr)
  /*!
    Process a controlled string [no var/comments etc]
    \param cellN :: Starting cell nubmer
    \param unitStr :: string to process
    \return 1/0 on success failure
  */
{
  ELog::RegMethod RegA("ExtControl","addUnitList");

  std::vector<std::string> UOut=
    StrFunc::splitParts(unitStr,' ');
  for(const std::string& Unit : UOut)
    {
      if (!addUnit(cellN,Unit))
	return 0;
    }
  return 1;
}

int
ExtControl::addUnit(const int& cellN,const std::string& unitStr)
  /*!
    Add a ext component
    \param cellN :: Cell number
    \param unitStr :: String of MCNP form
    \return 1 on success / 0 on fail
  */
{
  return addUnit(MapSupport::Range<int>(cellN),unitStr);
}

  
int
ExtControl::addUnit(const MapSupport::Range<int>& cellN,
		    const std::string& unitStr)
  /*!
    Add a ext component
    \param cellN :: Cell number(s)
    \param unitStr :: String of MCNP form
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("ExtControl","addComp");
  
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
      index=0;
      if (fabs(D) < Geometry::zeroTol)
	mFound=0;

      // Option 0 / 1 [no letters]
      if (Unit.empty())
	{
	  MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					   EUnit(mFound*2,0,D,0)));
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
	  MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					   EUnit(mFound,0,0.0,0)));
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
	    MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					     EUnit(mFound,0,D,VNum)));
	  else
	    MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					     EUnit(2*mFound,0,D,VNum)));
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
	MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					 EUnit(mFound,dirType,D,0)));
      else if (D>1.1)
	MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					 EUnit(3*mFound,dirType,0.0,0)));
      else 
	MapItem.insert(MTYPE::value_type(RTYPE(cellN),
					 EUnit(2*mFound,dirType,D,0)));
      return 1;
    }
  // MUST BE invalid :
  ELog::EM<<"Failed to understand : "<<Unit<<ELog::endErr;
  return 0;
}


 
void
ExtControl::renumberCell(const int originalCell,const int newCell)
  /*!
    Renumber cell
    \param originalCell :: original cell number
    \param newCell :: new cell number
  */
{
  ELog::RegMethod RegA("ExtControl","renumberCell");

  if (originalCell!=newCell)
    {
      // first check if any work is required:
      std::map<MapSupport::Range<int>,EUnit>::const_iterator mc=
	MapItem.find(MapSupport::Range<int>(originalCell));
      if (mc==MapItem.end())
	return;
      
      // second check if in renumbered list (either way?)
      std::map<int,int>::const_iterator mcN=
	renumberMap.find(newCell);
      
      if (mcN!=renumberMap.end())
	{
	  if (mcN->second==originalCell) return;   // work already done
	  throw ColErr::InContainerError<int>
	    (newCell,"Cell is already remapped:" +
	     StrFunc::makeString(mcN->second)+")");
	}
      renumberMap.insert(std::map<int,int>::value_type(newCell,originalCell));
    }
  return;
}
  
void
ExtControl::writeHeader(std::ostream& OX) const
  /*!
    Write the particle header set
    \param OX :: Output stream
   */
{
  OX<<"ext:";
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
ExtControl::write(std::ostream& OX,
		  const std::vector<int>& cellOutOrder,
		  const std::set<int>& voidCells) const
  /*!
    Write out the card
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
    \param voidCells :: List of void cells
  */
{
  ELog::RegMethod RegA("ExtControl","write");

  if (!MapItem.empty())
    {
      std::ostringstream cx;
      writeHeader(cx);
      for(const int CN : cellOutOrder)
	{
	  int CNActive(CN);
	  std::map<int,int>::const_iterator rmc=renumberMap.find(CN);
	  if (rmc!=renumberMap.end())
	    {
	      CNActive=rmc->second;
	    }
	  MTYPE::const_iterator mc=MapItem.find(RTYPE(CNActive));
	  if (mc!=MapItem.end() &&
	      voidCells.find(CN)==voidCells.end())
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
      
   
