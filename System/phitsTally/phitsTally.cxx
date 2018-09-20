/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTally.cxx
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
#include <set>
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
#include "particleConv.h"
#include "phitsTally.h"

namespace phitsSystem
{

std::ostream&
operator<<(std::ostream& OX,const phitsTally& TX)
  /*!
    Generic writing function
    \param OX :: Output stream
    \param TX :: phitsTally
    \return Output stream
   */
{
  TX.write(OX);
  return OX;
}

phitsTally::phitsTally(const int ID)  :
  idNumber(ID)
  /*!
    Constructor 
    \param ID :: phitsTally ID number
  */
{}


phitsTally*
phitsTally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new phitsTally(*this);
}

phitsTally::~phitsTally()
 /*!
   Destructor
 */
{}

void
phitsTally::setComment(const std::string& C)
  /*!
    Set the comment line
    \param C :: New comment
  */
{
  comments=C;
  return;
}


void
phitsTally::setParticle(const std::string&)
  /*!
    Null op call for non-particle detectors
  */
{}

void
phitsTally::setEnergy(const bool,const double,
		      const double,const size_t)
  /*!
    Null op call for non-energy detectors
  */
{}

void
phitsTally::setAngle(const bool,const double,
		      const double,const size_t)
 /*!
   Null op call for non-angle detectors
 */
{}
  

void
phitsTally::write(std::ostream&) const
  /*!
    Writes out the phitsTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  return;
}

}  // NAMESPACE phitsSystem
