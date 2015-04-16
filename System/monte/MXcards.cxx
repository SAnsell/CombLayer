/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/MXcards.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <string>
#include <cmath>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"

namespace MonteCarlo
{

MXcards::MXcards() :
  Mnum(-1),NZaid(0)
  /*!
    Default Constructor
  */
{} 

MXcards::MXcards(const int N,const size_t NZ,const std::string& P) :
  Mnum(N),NZaid(NZ),particle(P),items(NZ,"j")
  /*!
    Constructor
    \param N :: Material number
    \param NZ :: number of zaids
    \param P :: particle string
  */
{} 

MXcards::MXcards(const MXcards& A) : 
  Mnum(A.Mnum),NZaid(A.NZaid),particle(A.particle),items(A.items)
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
      Mnum=A.Mnum;
      NZaid=A.NZaid;
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

void
MXcards::setItem(const size_t Index,const std::string& IS)
  /*!
    Set a given item
    \param Index :: Index number
    \param IS :: Modification
  */
{
  ELog::RegMethod RegA("MXcards","setItem");
  
  if (Index>=NZaid)
    throw ColErr::IndexError<size_t>(Index,NZaid,"Index");
  
  items[Index]=IS;
  return;
}

void
MXcards::setItem(const std::vector<std::string>& VItems)
  /*!
    Set all teh items
    \param VItems :: Vector of items
    \param IS :: Modification
  */
{
  ELog::RegMethod RegA("MXcards","setItem(vector)");

  if (static_cast<size_t>(VItems.size())!=NZaid)
    throw ColErr::MisMatch<size_t>(VItems.size(),NZaid,"VItems!=NZaid");
  
  items=VItems;
  return;
}

void
MXcards::setCard(const int N,const std::string& P,const size_t NZ) 
  /*!
    Set the incident particle
    \param N :: Number of material
    \param P :: Particle
    \param NZ :: Number of particles
  */
{
  Mnum=N;
  particle=P;
  items.resize(NZ);
  fill(items.begin(),items.end(),"j");
  return;
}


void 
MXcards::write(std::ostream& OX) const
  /*!
    Write out the information about the material
    in a humman readable form (same as mcnpx file)
    \param OX :: Output stream
  */
{
  if (particle.empty()) return;

  std::ostringstream cx;
  cx.precision(10);
  cx<<"mx"<<Mnum<<":"<<particle<<" ";
  
  std::vector<std::string>::const_iterator vc;
  for(vc=items.begin();vc!=items.end();vc++)
    cx<<*vc<<"   ";
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
} 

}  // NAMESPACE MonteCarlo
