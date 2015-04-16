/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physics/LSwitchCard.cxx
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
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <complex>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "LSwitchCard.h"
 
namespace physicsSystem
{ 

std::ostream&
operator<<(std::ostream& OX,const LSwitchCard& A)
  /*!
    Write out the cut to the stream
    \param OX :: Output Stream
    \param A :: LSwitchCard object
    \return Output stream
   */
{
  A.write(OX);
  return OX;
}

LSwitchCard::LSwitchCard() :
  lcaActive(0),leaActive(0),lcbActive(0),
  lcaVal(10,0),leaVal(8,0),lcbVal(2,0)
  /*!
    Constructor
  */
{}

LSwitchCard::LSwitchCard(const LSwitchCard& A) : 
  lcaActive(A.lcaActive),leaActive(A.leaActive),lcbActive(A.lcbActive),
  lcaVal(A.lcaVal),leaVal(A.leaVal),lcbVal(A.lcbVal)
  /*!
    Copy constructor
    \param A :: LSwitchCard to copy
  */
{}

LSwitchCard&
LSwitchCard::operator=(const LSwitchCard& A)
  /*!
    Assignment operator
    \param A :: LSwitchCard to copy
    \return *this
  */
{
  if (this!=&A)
    {
      lcaActive=A.lcaActive;
      leaActive=A.leaActive;
      lcbActive=A.lcbActive;
      lcaVal=A.lcaVal;
      leaVal=A.leaVal;
      lcbVal=A.lcbVal;
    }
  return *this;
}


LSwitchCard::~LSwitchCard()
  /*!
    Destructor
  */
{}

std::vector<int>&
LSwitchCard::getVec(const std::string& Key) 
  /*!
    Get a reference to the value vector
    \param Key :: Key Name
    \return Reference
   */
{
  if (Key=="lca")
    return lcaVal;
  if (Key=="lea")
    return leaVal;
  if (Key=="lcb")
    return lcbVal;
  
  throw ColErr::InContainerError<std::string>(Key,ELog::RegMethod::getFull());
}

size_t&
LSwitchCard::getActive(const std::string& Key) 
  /*!
    Get a reference to the active value
    \param Key :: Key Name
    \return Reference
   */
{
  if (Key=="lca")
    return lcaActive;
  if (Key=="lea")
    return leaActive;
  if (Key=="lcb")
    return lcbActive;
  
  throw ColErr::InContainerError<std::string>(Key,"getActive");
}

const std::vector<int>&
LSwitchCard::getVec(const std::string& Key) const
  /*!
    Get a reference to the value vector
    \param Key :: Key Name
    \return Reference
   */
{
  if (Key=="lca")
    return lcaVal;
  if (Key=="lea")
    return leaVal;
  if (Key=="lcb")
    return lcbVal;
  
  throw ColErr::InContainerError<std::string>(Key,"LSwitchCar::getVec");
}


size_t
LSwitchCard::getActive(const std::string& Key) const
  /*!
    Get a reference to the active value
    \param Key :: Key Name
    \return Reference
   */
{
  if (Key=="lca")
    return lcaVal.size();
  if (Key=="lea")
    return leaVal.size();
  if (Key=="lcb")
    return lcbVal.size();
  
  throw ColErr::InContainerError<std::string>
    (Key,"LSwitchCard::getActive");
}

void
LSwitchCard::setValues(const std::string& Key,const std::string& Line) 
  /*!
    Set the values from a card
    \param Key :: Line to set
    \param Line :: Line to process	       
  */
{
  ELog::RegMethod RegA("LSwitchCard","setValues(string)");

  std::vector<int>& KV=getVec(Key);
  size_t& active=getActive(Key);
  const size_t maxSize=KV.size();

  std::string LX=Line;
  int Item;

  active=0;
  while(active<maxSize && StrFunc::section(LX,Item))
    {
      KV[active]=Item;
      active++;
    }
  if (active!=maxSize || !StrFunc::isEmpty(LX))
    ELog::EM<<"Failed to fully process :"<<Line<<ELog::endErr;

  return;
}

void
LSwitchCard::setValue(const std::string& Key,
		      const size_t ID,
		      const int value)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param Key :: Key name of object
    \param ID :: Cell number
    \param value :: new value
  */
{
  ELog::RegMethod RegA("LSwitchCard","setValue");

  std::vector<int>& KV=getVec(Key);
  size_t& active=getActive(Key);
  const size_t maxSize=KV.size();
  if (ID>=maxSize) 
    throw ColErr::IndexError<size_t>(ID,maxSize,"ID");
  if (active<ID)
    active=ID;

  KV[ID]=value;
  return;
}

int
LSwitchCard::getValue(const std::string& Key,
		      const size_t ID) const
  /*!
    Get the Value  for a given cell.
    \param Key :: Card item to use
    \param ID :: Physics value to index
    \return Phys card value
  */
{
  ELog::RegMethod RegA("LSwitchCard","getValue");
  
  const std::vector<int>& KV=getVec(Key);
  const size_t aSize=getActive(Key);
  if (ID>=aSize)
    throw ColErr::IndexError<size_t>(ID,aSize,"ID");

  return KV[ID];
}

void
LSwitchCard::write(std::ostream& OX) const
  /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
  */
{
  boost::format FMT("%04d ");
  std::ostringstream cx;
  if (lcaActive)
    {
      cx<<"lca ";
      for(size_t i=0;i<lcaActive;i++)
	{
	  if (i!=3)
	    cx<<lcaVal[i]<<" ";
	  else
	    cx<<(FMT % lcaVal[i]);
	}
      StrFunc::writeMCNPX(cx.str(),OX);      
    }
  if (leaActive)
    {
      cx.str("");
      cx<<"lea ";
      for(size_t i=0;i<leaActive;i++)
	cx<<leaVal[i]<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);      
    }
  if (lcbActive)
    {
      cx.str("");
      cx<<"lcb ";
      for(size_t i=0;i<lcbActive;i++)
	cx<<lcbVal[i]<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);      
    }

  return;
}



}  // NAMESPACE physicsSystem
