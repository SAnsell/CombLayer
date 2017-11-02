/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/EUnit.cxx
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "EUnit.h"

namespace physicsSystem
{
  
EUnit::EUnit(const int fT,const size_t dT,const double aF, const size_t ID) :
  fracType(fT),dirType(dT),aFrac(aF),vID(ID)
  /*!
    Constructor
    \param fT :: Fraction type [0:nothing, 1:frac , 2:S] [Signed]
    \param dT :: Direction type [0: particle, 1: Vector]
    \param aF :: Fraction value between (0 and 1.0)
    \param ID :: ID number
  */
{}

EUnit::EUnit(const EUnit& A) : 
  fracType(A.fracType),dirType(A.dirType),aFrac(A.aFrac),
  vID(A.vID)
  /*!
    Copy constructor
    \param A :: EUnit to copy
  */
{}

EUnit&
EUnit::operator=(const EUnit& A)
  /*!
    Assignment operator
    \param A :: EUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      fracType=A.fracType;
      dirType=A.dirType;
      aFrac=A.aFrac;
      vID=A.vID;
    }
  return *this;
}

void
EUnit::write(std::ostream& OX) const
  /*!
    Write out the card
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("EUnit","write");

  std::streamsize ss = OX.precision();
  switch (fracType)
    {
    case 0:   // NOTHING
      OX<<"0 ";
      return;
    case 1:   // +S
      OX<<"S";
      break;
    case -1:   // -S
      OX<<"-S";
      break;
    case 2:
      OX<<std::setprecision(3)<<aFrac;
      break;
    case -2:
      OX<<std::setprecision(3)<<-aFrac;
      break;
    case 3:
      break;
    case -3:
      OX<<"-";
      break;
    default:
      throw ColErr::InContainerError<int>(fracType,"fracType not known");
    }

  if (vID)
    OX<<"V"<<vID;
  else if (dirType)
    OX<<"XYZ"[dirType-1];

  OX<<" ";
  OX.precision(ss);
  return;
}


} // NAMESPACE physicsCards
      
   
