/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/PDControl.cxx
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
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <complex>
#include <set>
#include <list>
#include <map>
#include <iterator>
#include <algorithm>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "writeSupport.h"
#include "Triple.h"
#include "NRange.h"

#include "PDControl.h"

namespace tallySystem
{

PDControl::PDControl(const int TN) :
  tallyNumber(TN)
  /*!
    Constructor
    \param TN :: tally number
  */
{}

PDControl::PDControl(const PDControl& A) :
  tallyNumber(A.tallyNumber),
  pdFactor(A.pdFactor)
  /*!
    Copy Constructor
    \param A :: PDControl to copy
  */
{}

PDControl&
PDControl::operator=(const PDControl& A) 
  /*!
    Assignment operator
    \param A :: PDControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      pdFactor=A.pdFactor;
    }
  return *this;
}

PDControl::~PDControl()
  /*!
    Destructor
  */
{}


void
PDControl::clear()
  /*!
    Clear everything
   */
{
  pdFactor.clear();
  return;
}


void
PDControl::renumberCell(const int oldCellN,const int newCellN)
  /*!
    Renumbers a cell 
    \param oldCellN :: Old cell number
    \param newCellN :: New cell nubmer
  */
{
  ELog::RegMethod RegA("PDControl","renumberCell");

  if (pdFactor.empty() || oldCellN==newCellN) return;
  
  std::map<int,double>::iterator mc;
  
  if (pdFactor.find(newCellN)!=pdFactor.end())
    throw ColErr::InContainerError<int>(newCellN,"New cellN exists");
  mc=pdFactor.find(oldCellN);
  if (mc==pdFactor.end())
    throw ColErr::InContainerError<int>(oldCellN,"Old cellN not found ");

  // Doesn't invalidate
  const double factor=mc->second;
  pdFactor.erase(mc);
  pdFactor.emplace(newCellN,factor);

  return; 
}

void
PDControl::setImp(const int cellN,const double V)
  /*!
    Set a cell number
   */
{
  pdFactor[cellN]=V;
  return;
}


void
PDControl::write(std::ostream& OX) const
 /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("PDControl","write");
  
  if (!pdFactor.empty())
    {
      std::ostringstream cx;
      cx<<"pd"<<tallyNumber<<" ";

      NRange A;
      std::vector<double> Index;
      for(const auto& [ CN, fcValue ] : pdFactor)
	Index.push_back(fcValue);

      A.setVector(Index);
      A.condense();
      A.write(cx);
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}

} // NAMESPACE physicsSystem
