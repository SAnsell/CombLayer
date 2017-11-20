/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/Debug.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Debug.h"

namespace ELog
{

debugStatus::debugStatus() : 
  status(0),counter(0)
  /*!
    Constructor
  */
{}

debugStatus&
debugStatus::Instance()
  /*!
    Singleton this
    \return debugStatus object
   */
{
  static debugStatus MR;
  return MR;
}
  
ClassCounter::ClassCounter()
  /*!
    Constructor (increased count)
  */
{
  classN++;
 }

ClassCounter::ClassCounter(const ClassCounter&)
  /*!
    Copy constructor (increased count)
  */
{
  classN++;
}

ClassCounter::~ClassCounter()
  /*!
    Destructor reduces count
  */
{
  classN--;
}

int ClassCounter::classN(0);

}
