/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/resnuclei.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Mesh3D.h"

#include "flukaTally.h"
#include "flukaTallyModification.h"
#include "resnuclei.h"

namespace flukaSystem
{

resnuclei::resnuclei(const int outID) :
  flukaTally("resn"+std::to_string(outID),outID),
  cellA(0)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

resnuclei::resnuclei(const std::string& KN,const int outID) :
  flukaTally(KN,outID),cellA(0)
  /*!
    Constructor
    \param KN :: KeyName
    \param outID :: Identity number of tally [fortranOut]
  */
{}

resnuclei::resnuclei(const resnuclei& A) :
  flukaTally(A),cellA(A.cellA)
  /*!
    Copy constructor
    \param A :: resnuclei to copy
  */
{}

resnuclei&
resnuclei::operator=(const resnuclei& A)
  /*!
    Assignment operator
    \param A :: resnuclei to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      cellA=A.cellA;
    }
  return *this;
}

resnuclei*
resnuclei::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new resnuclei(*this);
}

resnuclei::~resnuclei()
  /*!
    Destructor
  */
{}

  
void
resnuclei::write(std::ostream& OX) const
  /*!
    Write out decay information			
    \param System :: Simulation  tallies
    \param OX :: Output stream
   */
{

  std::ostringstream cx;
  
  cx<<"RESNUCLEI  3.0 "<<outputUnit<<" - - "
    <<" R"<<cellA<<" 1.0 ";
  cx<<" resn"<<std::to_string(std::abs(outputUnit));
  StrFunc::writeFLUKA(cx.str(),OX);
  return;
}

}  // NAMESPACE flukaSystem

