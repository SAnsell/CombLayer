/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTally.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
  keyName(std::to_string(ID)),
  idNumber(ID),epsFlag(0),vtkFlag(0),vtkFormat(1)
  /*!
    Constructor 
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const std::string& KName,const int ID)  :
  keyName(KName+std::to_string(ID)),idNumber(ID),
  epsFlag(0),vtkFlag(0),vtkFormat(1)
  /*!
    Constructor 
    \param KName :: keyname
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const phitsTally& A) : 
  keyName(A.keyName),idNumber(A.idNumber),
  comments(A.comments),epsFlag(A.epsFlag),
  vtkFlag(A.vtkFlag),vtkFormat(A.vtkFormat),fileName(A.fileName)
  /*!
    Copy constructor
    \param A :: phitsTally to copy
  */
{}

phitsTally&
phitsTally::operator=(const phitsTally& A)
  /*!
    Assignment operator
    \param A :: phitsTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      comments=A.comments;
      epsFlag=A.epsFlag;
      vtkFlag=A.vtkFlag;
      vtkFormat=A.vtkFormat;
      fileName=A.fileName;
    }
  return *this;
}


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
