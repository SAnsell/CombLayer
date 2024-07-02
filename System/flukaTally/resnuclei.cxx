/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaTally/resnuclei.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "OutputLog.h"
#include "writeSupport.h"

#include "flukaTally.h"
#include "resnuclei.h"

namespace flukaSystem
{

///\cond STATIC
int resnuclei::zOut=0;
int resnuclei::mOut=0;
///\endcond STATIC

resnuclei::resnuclei(const std::string& KN,const int outID,
		     const int tapeID) :
  flukaTally(KN,outID,tapeID),
  cellA(0)
  /*!
    Constructor
    \param KN :: KeyName
    \param outID :: Identity number of tally [fortranOut]
  */
{}

resnuclei::resnuclei(const resnuclei& A) :
  flukaTally(A),AMax(A.AMax),ZMax(A.ZMax),cellA(A.cellA)
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
      AMax=A.AMax;
      ZMax=A.ZMax;
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
resnuclei::setZaid(const int Z,const int A)
/*!
  Set the max number for range
  \param Z :: Z (charge) max
  \param A :: Atomic number
*/
{
  AMax=A;
  ZMax=Z;
  if (zOut<ZMax)
    zOut=ZMax;

  const int NMax=AMax-ZMax;
  if (mOut < NMax)
    mOut=NMax;               // dont thing manual correct

  return;
}


void
resnuclei::write(std::ostream& OX) const
  /*!
    Write out decay information
    \param System :: Simulation  tallies
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  //  const int M=AMax-ZMax+5;
  cx<<"RESNUCLEI  3 "<<outputUnit<<" "<<zOut+7<<" "<<mOut+5<<
    " R"<<cellA<<" 1 ";
  cx<<" "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);
  return;
}

}  // NAMESPACE flukaSystem
