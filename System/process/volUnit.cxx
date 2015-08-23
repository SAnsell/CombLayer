/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/volUnit.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "volUnit.h"

namespace ModelSupport
{

std::ostream&
operator<<(std::ostream& OX,const volUnit& A)
  /*!
    Write to a stream
    \param OX :: Output stream
    \param A :: volUnit to write
    \return Stream state
  */
{
  A.write(OX);
  return OX;
}

volUnit::volUnit() : 
  npts(0),lineSum(0.0)
  /*!
    Constructor
  */
{}


volUnit::volUnit(const int MN,const std::string& CM,
		 const std::vector<int>& CList) : 
  npts(0),lineSum(0.0),comment(CM),matNum(MN)
  /*!
    Constructor
    \param MN :: Material number
    \param CM :: Comment string
    \param CList :: Cell list
  */
{
  setCells(CList);
}

volUnit::volUnit(const volUnit& A) : 
  npts(A.npts),cells(A.cells),lineSum(A.lineSum),
  comment(A.comment),matNum(A.matNum)
  /*!
    Copy constructor
    \param A :: volUnit to copy
  */
{};

volUnit&
volUnit::operator=(const volUnit& A)
  /*!
    Assignment operator
    \param A :: volUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      npts=A.npts;
      cells=A.cells;
      lineSum=A.lineSum;
      comment=A.comment;
      matNum=A.matNum;
    }
  return *this;
}

volUnit::~volUnit()
  /*!
    Destructor
   */
{}

void 
volUnit::setCells(const std::vector<int>& C)
  /*!
    Set the cell list
    \param C :: Cell indexes
  */
{
  cells.erase(cells.begin(),cells.end());
  std::vector<int>::const_iterator vc;
  for(vc=C.begin();vc!=C.end();vc++)
    cells.insert(*vc);

  return;
}

void 
volUnit::addCell(const int CN)
  /*!
    Set the cell list
    \param CN :: Cell number
  */
{
  cells.insert(CN);
  return;
}

void 
volUnit::addUnit(const int CN,const double D)
  /*!
    Add a unit 
    \param CN :: cell number
    \param D :: Distance
  */
{
  std::set<int>::iterator sc=cells.find(CN);
  if (sc!=cells.end())
    {
      // if (CN==672)
      // 	ELog::EM<<"D = "<<D<<ELog::endTrace;
      npts++;
      lineSum+=D;
    }
  return;
}

void 
volUnit::addFlux(const int CN,const double R,const double D)
  /*!
    Add a unit 
    \param CN :: cell number
    \param R :: Inital distance
    \param D :: Distance
  */
{
  std::set<int>::iterator sc=cells.find(CN);
  if (sc!=cells.end())
    {
      npts++;
      lineSum+=1.0/R-1.0/(R+D);
    }
  return;
}

void 
volUnit::reset()
  /*!
    Big reset item
  */
{
  npts=0;
  lineSum=0.0;
  return;
}

double
volUnit::calcVol(const double D) const
  /*!
    Calculate the volume of the unit
    - VolofSphere * lineLenghFraction
    \param D :: Track divider
    \return Volume of tally
  */
{
  return D*lineSum;
}

double
volUnit::calcLine(const double D) const
{
  return D*npts;
}

void 
volUnit::write(std::ostream& OX) const
  /*!
    Write out the data				
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("volUnit","write");
  
  OX<<lineSum<<" ("<<npts<<")["<<lineSum/npts<<"] from ::";
  copy(cells.begin(),cells.end(),std::ostream_iterator<int>(OX,","));

  return;
}

}  // NAMESPACE ModelSupport
