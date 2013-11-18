/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/KGroup.cxx
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
#include <sstream>
#include <cmath>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <boost/bind.hpp>

#include "Exception.h"
#include "Triple.h"
#include "support.h"
#include "NRange.h"
#include "KGroup.h"


std::ostream&
operator<<(std::ostream& OX,const KGroup& NR)
  /*!
    Write KGroup to a stream (used condenced
    format)
    \param NR :: KGroup unit
    \param OX :: output stream
    \return OX in modified state
  */
{
  NR.write(OX);
  return OX;
}

KGroup::KGroup() 
  /*!
    Default constructor
  */
{ }

KGroup::KGroup(const KGroup& A) : 
  KeyItem(A.KeyItem),Values(A.Values)
  /*!
    Copy Constructor
    \param A :: KGroup object to copy
  */
{ }

KGroup::KGroup(const std::string& N)
  /*!
    Constructor based on a simple string
    \param N :: String to pass
  */
{ 
  const int errN=processString(N);
  if(errN)
    throw ColErr::InvalidLine(N,"KGroup::constructor");
}

KGroup&
KGroup::operator=(const KGroup& A)
  /*!
    Assignment operator=
    \param A :: Object to copy
    \return *this
  */
{
  if(&A!=this)
    {
      KeyItem=A.KeyItem;
      Values=A.Values;
    }
  return *this;
}

KGroup::~KGroup()
  /*!
    Destructor
  */
{ }

int 
KGroup::count() const
  /*!
    Determines the number of values in the range of 
    the group
    \return number of values
  */
{
  return Values.count();
}

std::vector<double>
KGroup::actualItems() const
  /*!
    Gets the indiviual items from the NRange
    \return Vector of values
   */
{
  std::vector<double> Out;
  Values.writeVector(Out);
  return Out;
}

int 
KGroup::processString(const std::string& N)
  /*!
    Process a string of the types: 
      - Keys D1 D2 D3  ...
      - Dx Dy Dz ...
      
    \param N :: String to process
    \retval 0 :: success
    \retval -1 :: string empty / fails
  */
{
  if(N.empty())
    return -1;
  // Work with part of the string first

  KeyItem="";
  Values.clear();
  return addComp(N);
}
  
int
KGroup::addComp(const std::vector<double>& Obj) 
  /*!
    Adds a component to the list of type vector<double>
    \param Obj :: vecotr object to add to the list
    \return 0 :: always success
  */
{
  for_each(Obj.begin(),Obj.end(),boost::bind(&NRange::addComp,Values,_1));
  return 0;
}


int
KGroup::addComp(const double V) 
  /*!
    Adds a component to the list of type int/double
    \param V :: Value to add to the list
    \return 0 :: always success
  */
{
  Values.addComp(V);
  return 0;
}

int
KGroup::addComp(const std::string& Obj) 
  /*!
    Adds a string component to the list, each componenet must
    be a variable/number. The routine also checks that Obj
    may be preceeded by a number of "strings", if and only if
    KeyItem is empty, then it can be set by a leading set of 
    strings. 

    If anything fails, then the addComp returns -1 and there
    is no change to the KGroup item.

    \param Obj :: object to add to the list
    \return 0 ::  success
    \retval -1 :: Failuire
  */
{
  std::string Line=Obj;
  int keyFlag(0);
  std::string KX; 
  std::ostringstream cx;

  if (KeyItem.empty())
    {
      double V;
      while(!Line.empty() && !StrFunc::convert(Line,V))
        {
	  StrFunc::section(Line,KX);
	  cx<<KX<<" ";
	  keyFlag=1;
	}
      if (Line.empty() && keyFlag)
        {
	  KeyItem=cx.str();
	  return 0;
	}
    }
  NRange TR;
  if (!TR.processString(Line))
    {
      Values+=TR;
      if (keyFlag)
	KeyItem=cx.str();
      return 0;
    }

  return -1;
}

void
KGroup::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  OX<<KeyItem<<" "<<Values;
  return;
}

void
KGroup::writeMCNPX(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  std::ostringstream cx;
  cx<<KeyItem<<" "<<Values;
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}


