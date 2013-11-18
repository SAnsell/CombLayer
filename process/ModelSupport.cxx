/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/ModelSupport.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "support.h"
#include "regexSupport.h"
#include "surfRegister.h"
#include "ModelSupport.h"

namespace ModelSupport
{

std::string
getExclude(const int Offset)
  /*!
    Given a cell number construct the composite form
    for addition to getComposite
    \param Offset :: Cell value
    \return Cell exclude string
  */
{
  std::ostringstream cx;
  cx<<" #"<<Offset<<"T ";
  return cx.str();
}


std::string
getComposite(const int Offset,const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    \param Offset :: Offset nubmer  to add
    \param BaseString :: BaseString number
    \return String with offset components
   */ 
{
  boost::regex Re("(^|\\D+)(\\d*)",boost::regex::perl);
  std::ostringstream cx;
  std::vector<std::string> Out;

  StrFunc::StrFullSplit(BaseString,Re,Out);

  int cellN;
  for(size_t i=0;i<Out.size();i++)
    {
      if (!Out[i].empty())
        {
	  if (Out[i][0]=='T')
	    {
	      Out[i][0]=' ';
	      if (StrFunc::convert(Out[i],cellN))
		cx<<cellN;
	      else
		cx<<Out[i];
	    }
	  else if (StrFunc::convert(Out[i],cellN))
	    cx<<((cellN>0) ? cellN+Offset : cellN-Offset);
	  else
	    cx<<Out[i];
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,const int Offset,
	     const int MinorOffset,
	     const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
//  boost::regex Re("(^|\\D+)(\\d*)",boost::regex::perl);
  boost::regex Re("(^|[^-]|[^0-9]*)([M|T|\\d|-]*)",boost::regex::perl);
  std::ostringstream cx;
  std::vector<std::string> Out;

  StrFunc::StrFullSplit(BaseString,Re,Out);
  int cellN;
  int TrueNum,MinorNum;
  for(size_t i=0;i<Out.size();i++)
    {
      const size_t oL=Out[i].length();
      if (oL)
	{
	  TrueNum=MinorNum=0;
	  if (Out[i][oL-1]=='T')
	    {
	      Out[i][oL-1]=' ';
	      TrueNum=1;
	    }
	  else if (Out[i][oL-1]=='M')
	    {
	      Out[i][oL-1]=' ';
	      MinorNum=1;
	    }
	  if (StrFunc::convert(Out[i],cellN))
	    {
	      if (TrueNum)
		cx<<SMap.realSurf(cellN);
	      else if (MinorNum)
		cx<<((cellN>0) ? SMap.realSurf(cellN+MinorOffset) 
		     : SMap.realSurf(cellN-MinorOffset));
	      else
		cx<<((cellN>0) ? SMap.realSurf(cellN+Offset) 
		     : SMap.realSurf(cellN-Offset));
	    }
	  else
	    cx<<Out[i];
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,
	     const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,Offset,BaseString);
}

std::string
getSetComposite(const surfRegister& SMap,
	     const int Offset,const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc. However, if a number is missing then 
    leave its component blank.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
//  boost::regex Re("(^|\\D+)(\\d*)",boost::regex::perl);
  boost::regex Re("(^|[^-]|[^0-9]*)([T|\\d|-]*)",boost::regex::perl);
  std::ostringstream cx;
  std::vector<std::string> Out;

  StrFunc::StrFullSplit(BaseString,Re,Out);
  int cellN;
  int TrueNum=0;
  for(size_t i=0;i<Out.size();i++)
    {
      const size_t oL=Out[i].length();
      if (oL)
	{
	  TrueNum=0;
	  if (Out[i][oL-1]=='T')
	    {
	      Out[i][oL-1]=' ';
	      TrueNum=1;
	    }
	  if (StrFunc::convert(Out[i],cellN))
	    {
	      int CN(cellN);
	      if (!TrueNum)
		CN+=(cellN>0) ? Offset : -Offset;
	      if (SMap.hasSurf(CN))
		cx<<SMap.realSurf(CN);
	    }
	  else
	    cx<<Out[i];
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfN)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfN :: Index item to add
    \return String with offset components
   */
{
  std::ostringstream cx;
  cx<<" "<<((surfN>0) ? SMap.realSurf(surfN+Offset) 
       : SMap.realSurf(surfN-Offset));
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfNA,
	     const int surfNB)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfNA :: Index item to add
    \param surfNB :: Index item to add
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,surfNA)+
    getComposite(SMap,Offset,surfNB);
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfNA,
	     const int surfNB,const int surfNC)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfNA :: Index item to add
    \param surfNB :: Index item to add
    \param surfNC :: Index item to add
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,surfNA)+
    getComposite(SMap,Offset,surfNB)+
    getComposite(SMap,Offset,surfNC);
}

}  // NAMESPACE ModelSupport
