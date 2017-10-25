/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/pairRange.cxx
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
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "pairRange.h"

std::ostream&
operator<<(std::ostream& OX,const pairRange& PR)
  /*!
    Write NRange to a stream (used condenced
    format)
    \param OX :: output stream
    \param PR :: NRange unit
    \return OX in modified state
  */
{
  PR.write(OX);
  return OX;
}


pairRange::pairRange() 
  /*!
    Default constructor
  */
{}

pairRange::pairRange(const pairRange& A) : 
  Items(A.Items)
  /*!
    Copy Constructor
    \param A :: pairRange object to copy
  */
{ }

pairRange::pairRange(const std::string& N)
  /*!
    Constructor based on a simple string
    \param N :: String to pass
  */
{ 
  const int errN=processString(N);
  if(errN)
    throw ColErr::InvalidLine(N,"pairRange::constructor");
}


pairRange&
pairRange::operator=(const pairRange& A)
  /*!
    Assignment operator=
    \param A :: Object to copy
    \return *this
  */
{
  if(&A!=this)
    {
      Items=A.Items;
    }
  return *this;
}

bool
pairRange::operator==(const pairRange& A) const
  /*!
    boolean equal operator
    \param A :: Object to check
    \return true if equal
  */
{
  if(&A==this) return 1;
  
  if (Items.size()!=A.Items.size()) return 0; 
  
  for(size_t i=0;i<Items.size();i++)
    {
      const PTYPE::value_type& AValue=Items[i];
      const PTYPE::value_type& BValue=A.Items[i];
      if (std::abs(AValue.first-BValue.first)>Geometry::zeroTol ||
          std::abs(AValue.second-BValue.second)>Geometry::zeroTol)
        return 0;
    }
  return 1;
}

pairRange::~pairRange()
  /*!
    Destructor
  */
{}

const std::pair<double,double>&
pairRange::operator[](const size_t Index) const
  /*!
    Gets an individual value
    \param Index :: Index value 
    \return value
  */
{
  ELog::RegMethod RegA("pairRange","operator[]");

  if (Index>=Items.size())
    throw ColErr::IndexError<size_t>(Index,Items.size(),"Index");

  return Items[Index];
}


pairRange&
pairRange::operator+=(const pairRange& A)
  /*!
    Add two ranges (append A to the back of this)
    \param A :: Object to add
    \return *this
  */
{
  Items.insert(Items.end(),A.Items.begin(),A.Items.end());
  return *this;
}

int 
pairRange::processString(const std::string& N)
  /*!
    Process a string of the type 
      - 3.4 5.6 5.6 0
    The first component of the string pairs must
    be increasing
     
    \param N :: String 
    \retval 0 :: success
    \retval -1 :: string empty
    \retval -2 :: Unable to process first nubmer
  */
{
  ELog::RegMethod RegA("pairRange","processString");

  if(N.empty())
    return -1;
  // Work with part of the strin first

  std::string MS(N);
  // From now on in we have a string type (definately)
  // check if the next number is a number or doesn't
  // exist.
  Items.clear();
  double E,V;
  while(StrFunc::section(MS,E) && 
	StrFunc::section(MS,V))
    {
      Items.push_back(PTYPE::value_type(E,V));
    }
  return 0;
}

void
pairRange::setVector(const std::vector<double>& E,
		     const std::vector<double>& V)
  /*!
    Takes the vector and writes it into the pairRange.
    \param E :: Energy values
    \param V :: Values unit
  */
{
  ELog::RegMethod RegA("pairRange","setVector");
  
  if (E.size()!=V.size())
    throw ColErr::MisMatch<size_t>(E.size(),V.size(),RegA.getBase());

  Items.clear();
  
  for(size_t i=0;i<E.size();i++)
    {
      Items.push_back(PTYPE::value_type(E[i],V[i]));
    }
  return;
}

void
pairRange::addComp(const double E,const double V)
  /*!
    Adds a component. Condensation needs 
    to be done by the user later.
    \param E :: New Energy
    \param V :: New value
   */
{
  Items.push_back(PTYPE::value_type(E,V));
  return;
}

void
pairRange::condense(const double Tol)
  /*!
    Determine intervals 
    \param Tol :: Tolerance value
  */
{
  size_t i=1;
  while(i<=Items.size())
    {
      if (NRange::identVal(Tol,Items[i-1].first,Items[i].first))
	Items.erase(Items.begin()+static_cast<ssize_t>(i));
      else
	i++;
    }
  return;
}

size_t
pairRange::count() const
  /*!
    Determine the number of points in a range
    \return Number of data points in range
  */
{
  return Items.size();
}

size_t
pairRange::writeVector(PTYPE& Vec) const
  /*!
    Sets the values into a vector
    \param Vec :: Vector to set
    \return Number of componnets
  */
{
  ELog::RegMethod RegA("pairRange","writeVector");

  Vec.resize(Items.size());  
  copy(Items.begin(),Items.end(),Vec.begin());
  return Vec.size();
}


void
pairRange::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  PTYPE::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      OX<<vc->first<<" "<<vc->second<<" ";
    }
  return;
}
