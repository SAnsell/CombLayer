/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userDump.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>
#include <boost/format.hpp>

#include "FileReport.h"
#include "OutputLog.h"
#include "writeSupport.h"

#include "flukaTally.h"
#include "userDump.h"

namespace flukaSystem
{

userDump::userDump(const int ID,const int outID) :
  userDump("dump",ID,outID)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}
userDump::userDump(const std::string& tallyName,
		   const int ID,const int outID) :
  flukaTally(tallyName,ID,outID),dumpType(1),
  outName("dump")
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userDump::userDump(const userDump& A) : 
  flukaTally(A),
  dumpType(A.dumpType),outName(A.outName)
  /*!
    Copy constructor
    \param A :: userDump to copy
  */
{}

userDump&
userDump::operator=(const userDump& A)
  /*!
    Assignment operator
    \param A :: userDump to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      dumpType=A.dumpType;
      outName=A.outName;
    }
  return *this;
}
  
userDump*
userDump::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userDump(*this);
}

userDump::~userDump()
  /*!
    Destructor
  */
{}
  
void
userDump::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  
  cx<<"USERDUMP 100 "<<outputUnit<<" "<<dumpType
    <<" 0 - - "<<outputUnit;
  StrFunc::writeFLUKA(cx.str(),OX);  
  
  return;
}

}  // NAMESPACE flukaSystem

