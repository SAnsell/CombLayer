/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/MXcards.cxx
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
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Zaid.h"
#include "MXcards.h"

namespace MonteCarlo
{

MXcards::MXcards(std::string  P) :
  particle(std::move(P))
  /*!
    Constructor
    \param P :: particle string
  */
{} 

MXcards::MXcards(const MXcards& A) : 
  particle(A.particle),items(A.items)
  /*!
    Copy constructor
    \param A :: MXcards to copy
  */
{}

MXcards&
MXcards::operator=(const MXcards& A)
  /*!
    Assignment operator
    \param A :: MXcards to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particle=A.particle;
      items=A.items;
    }
  return *this;
}


MXcards::~MXcards()
  /*!
    Standard Destructor
  */
{}
  
bool
MXcards::hasZaid(const size_t Index) const
  /*!
    Find is a modification has been set
    \param Index :: Zaid
    \return true if set / false otherwize
  */
{
  return (items.find(Index)==items.end()) ? 0 : 1;
}

void
MXcards::setZaid(const size_t Index,const std::string& IS)
  /*!
    Set a given zaid set the item value
    \param Index :: Zaid
    \param IS :: Modification string
  */
{
  ELog::RegMethod RegA("MXcards","setZaid");
  
  items.emplace(Index,IS);
  return;
}

const std::string&
MXcards::getZaid(const size_t Index) const
  /*!
    Return the modification string
    \param Index :: Zaid
    \return modification string
  */
{
  ELog::RegMethod RegA("MXcards","getZaid");
  
  std::map<size_t,std::string>::const_iterator mc=
    items.find(Index);

  if (mc==items.end())
    throw ColErr::InContainerError<size_t>(Index,"Zaid not found");
  return mc->second;
}

void 
MXcards::write(std::ostream& OX,
	       const std::vector<Zaid>& ZaidList) const
  /*!
    Write out the information about the material
    in a humman readable form (same as mcnpx file)
    \param OX :: Output stream
  */
{
  if (!particle.empty()) 
    OX<<":"<<particle;
  OX<<" ";
	
  for(const Zaid& ZI : ZaidList)
    {
      if (ZI.getZ())
	{
	  std::map<size_t,std::string>::const_iterator mc=
	    items.find(ZI.getZaidNum());
	  if (mc!=items.end())
	    OX<<mc->second<<" ";
	  else
	    OX<<"j ";
	}
    }
  return;
} 

}  // NAMESPACE MonteCarlo
