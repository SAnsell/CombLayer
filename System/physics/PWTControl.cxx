/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PWTControl.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
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
#include "Triple.h"
#include "NRange.h"
#include "PWTControl.h"

namespace physicsSystem
{
  
PWTControl::PWTControl()
  /*!
    Constructor
  */
{}

PWTControl::PWTControl(const PWTControl& A) :
  MapItem(A.MapItem),
  renumberMap(A.renumberMap)
  /*!
    Copy constructor
    \param A :: PWTControl to copy
  */
{}

PWTControl&
PWTControl::operator=(const PWTControl& A)
  /*!
    Assignment operator
    \param A :: PWTControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      MapItem=A.MapItem;
      renumberMap=A.renumberMap;
    }
  return *this;
}

PWTControl::~PWTControl()
  /*!
    Destructor
  */
{}

void
PWTControl::clear()
  /*!
    Clear control
  */
{
  MapItem.erase(MapItem.begin(),MapItem.end());
  renumberMap.erase(renumberMap.begin(),renumberMap.end());
  return;
}


void
PWTControl::setUnit(const int cellN,const double V)
  /*!
    set a Pwt component
    \param cellN :: Cell number
    \param V :: Value of unit
  */
{
  setUnit(MapSupport::Range<int>(cellN),V);
  return;
}

void
PWTControl::setUnit(const MapSupport::Range<int>& cellN,
		    const double V)
  /*!
    Add a ext component
    \param cellN :: Cell number(s)
    \param V :: value
  */
{
  ELog::RegMethod RegA("PWTControl","setUnit");
  MapItem.insert(MTYPE::value_type(RTYPE(cellN),V));
  return;
}
 
void
PWTControl::renumberCell(const int originalCell,const int newCell)
  /*!
    Renumber cell
    \param originalCell :: original cell number
    \param newCell :: new cell number
  */
{
  ELog::RegMethod RegA("PWTControl","renumberCell");

  if (originalCell!=newCell)
    {
      // first check if any work is required:
      std::map<RTYPE,double>::const_iterator mc=
	MapItem.find(RTYPE(originalCell));
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
PWTControl::write(std::ostream& OX,
		  const std::vector<int>& cellOutOrder,
		  const std::set<int>& ) const
  /*!
    Write out the card
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
    \param voidCells :: List of void cells [placeholder]
  */
{
  ELog::RegMethod RegA("PWTControl","write");

  if (MapItem.empty()) return;


  NRange A;
  std::vector<double> PWTCell;
  bool active(0);
  for(const int CN : cellOutOrder)
    {
      // Get renumber :
      int CNActive(CN);
      std::map<int,int>::const_iterator rmc=renumberMap.find(CN);
      if (rmc!=renumberMap.end())
	CNActive=rmc->second;
      // find if in range
      MTYPE::const_iterator mc=MapItem.find(RTYPE(CNActive));
      if (mc!=MapItem.end())       // NO need to test void cells
	{
	  PWTCell.push_back(mc->second);
	  active=1;
	}
      else
	PWTCell.push_back(-1.0);
    }
  if (!active) return;  // NO WORK
  A.setVector(PWTCell);
  A.condense();
  
  std::ostringstream cx;
  cx<<"pwt ";
  A.write(cx);
  StrFunc::writeMCNPX(cx.str(),OX);
   
  return;
}


} // NAMESPACE physicsCards
      
   
