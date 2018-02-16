/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/flukaTally.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <sstream>
#include <fstream>
#include <complex>
#include <cmath>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "flukaTally.h"

namespace flukaSystem
{

std::ostream&
operator<<(std::ostream& OX,const flukaTally& TX)
  /*!
    Generic writing function
    \param OX :: Output stream
    \param TX :: flukaTally
    \return Output stream
   */
{
  TX.write(OX);
  return OX;
}

flukaTally::flukaTally(const int ID)  :
  outputUnit(ID)
  /*!
    Constructor 
    \param ID :: flukaTally ID number
  */
{}

flukaTally::flukaTally(const flukaTally& A)  :
  outputUnit(A.outputUnit),comments(A.comments)
  /*!
    Copy constructor
    \param A :: flukaTally object to copy
  */
{}

flukaTally*
flukaTally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new flukaTally(*this);
}

flukaTally&
flukaTally::operator=(const flukaTally& A) 
  /*!
    Assignment operator 
    \param A :: flukaTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      outputUnit=A.outputUnit;
      comments=A.comments;
    }
  return *this;
}

flukaTally::~flukaTally()
 /*!
   Destructor
 */
{}

void
flukaTally::setComment(const std::string& C)
  /*!
    Set the comment line
    \param C :: New comment
  */
{
  comments=C;
  return;
}

void
flukaTally::write(std::ostream&) const
  /*!
    Writes out the flukaTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{  
  return;
}

}  // NAMESPACE flukaSystem
