/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/textTally.cxx
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
#include <sstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "textTally.h"

namespace tallySystem
{

textTally::textTally(const int ID) :
  Tally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

textTally::textTally(const textTally& A) :
  Tally(A),Lines(A.Lines)
  /*!
    Copy Constructor
    \param A :: textTally object to copy
  */
{ }

textTally*
textTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new textTally(*this);
}

textTally&
textTally::operator=(const textTally& A) 
  /*!
    Assignment operator =
    \param A :: textTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      Lines=A.Lines;
    }
  return *this;
}

textTally::~textTally()
  /*!
    Destructor
  */
{}

int
textTally::addLine(const std::string& LX)
  /*!
    Adds a string, if this fails
    it return Tally::addLine.
    Currently it only searches for:
    - f  : Initial line (then surface list)
    \param LX :: Line to search
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("textTally","addLine");
  Lines.push_back(LX);
  return 0;
}


void
textTally::renumberCell(const int oldN,const int newN)
  /*!
    Renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the cell with
   */
{
  ELog::RegMethod RegA("textTally","renumberCell");
  return;
}


void
textTally::renumberSurf(const int oldN,const int newN)
  /*!
    Renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the surface with
   */
{
  ELog::RegMethod RegA("textTally","renumberCell");
  return;
}

void
textTally::write(std::ostream& OX) const
  /*!
    Writes out the surface tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  if (!isActive())
    return;
  std::stringstream cx;
  for(size_t i=0;i<Lines.size();i++)
    StrFunc::writeMCNPX(Lines[i],OX);
  Tally::write(OX);
  return;
}

} // NAMESPACE tallySystem
