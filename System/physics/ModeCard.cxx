/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ModeCard.cxx
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
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <complex>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "ModeCard.h"

namespace physicsSystem
{

ModeCard::ModeCard() 
  /*!
    Constructor
  */
{}

ModeCard::ModeCard(const std::string& Items) 
  /*!
    Constructor 
    \param Items :: Items list on mode card
  */
{
  std::string P;
  std::string N=Items;
  while(StrFunc::section(N,P))
    particles.push_back(P);
}
  
ModeCard::ModeCard(const ModeCard& A) :
  particles(A.particles)
  /*!
    Copy Constructor
    \param A :: ModeCard to copy
  */
{}

ModeCard&
ModeCard::operator=(const ModeCard& A) 
  /*!
    Assignment operator
    \param A :: ModeCard to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particles=A.particles;
    }
  return *this;
}

ModeCard::~ModeCard()
  /*!
    Destructor
  */
{}

void
ModeCard::clear()
  /*!
    Clear everything
   */
{
  particles.clear();
  return;
}

void
ModeCard::addElm(const std::string& EN) 
  /*!
    Adds an element to the particles list
    (note it also checks the element ??)
    \param EN :: element identifier
  */
{
  if (std::find(particles.begin(),particles.end(),EN)==particles.end())
    particles.push_back(EN);
  return;
}

int
ModeCard::hasElm(const std::string& E) const
  /*!
    Finds the item on the list if it exists
    \param E :: Particle string to find
    \return 0 :: failure , 1 :: success
  */
{
  std::list<std::string>::const_iterator vc;
  vc=find(particles.begin(),particles.end(),E);
  return (vc==particles.end()) ? 0 : 1;
}

int
ModeCard::removeParticle(const std::string& PT)
  /*!
    Removes a particle type
    \param PT :: Particle type
    \return 1 :: success (or zero for failure)
  */
{
  std::list<std::string>::iterator vc;
  vc=find(particles.begin(),particles.end(),PT);
  if (vc==particles.end())
    return 0;
  particles.erase(vc);
  return 1;
}

void
ModeCard::write(std::ostream& OX) const
  /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
  */
{
  std::ostringstream cx;
  cx<<"mode ";
  // Write out cut:n,x list
  copy(particles.begin(),particles.end(),
       std::ostream_iterator<std::string>(cx," "));
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

}  // NAMESPACE physicsSystem
