/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/fortranWrite.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "fortranWrite.h"

/// GLOBAL FUNCTIONS 

namespace StrFunc
{

std::ostream& 
operator<<(std::ostream& OX,const fortranWrite& A)
  /*!
    Write out to a stream
    \param OX :: ostream to write 
    \param A :: Object to write
    \return Current state of stream
   */
{
  A.write(OX);
  return OX;
}

fortranWrite::fortranWrite(const std::string& FmtStr) 
  /*!
    Constructor 
    \param FmtStr :: Format string						
  */
{
  parse(FmtStr);
}

template<>
fortranWrite&
fortranWrite::operator%(const int& I)
  /*!
    Consume next item in stream
    \param I :: Integer to consume
    \return this
  */
{
  ELog::RegMethod RegA("fortranWrite","operator%(int)");

  // Process zeros
  while (FmtInfo.front().type==0)
    consumeZeroType();

  
  const FmtID& FD= FmtInfo.front();

  if (FD.type==1 || FD.type==2)      // Int / double
    outStr<<std::setw(FD.lenA)<<I;
  FmtInfo.pop_front();
  
  return *this;
}

void
fortranWrite::consumeZeroType()
  /*!
    Deals with 5x type units 
    Checked call [no possiblity that FD.empty / FD.type!=0
  */
{
  // Assumed to be zero type and sufficient items on stream [
  const FmtID& FD= FmtInfo.front();
  
  outStr<<std::string(static_cast<size_t>(FD.lenA),' ');
  FmtInfo.pop_front();
  return;
}
  
void
fortranWrite::parse(const std::string& FmtStr)
  /*!
    Initialiation of the FmtStr to process the output
    \param FmtStr :: Format string
   */
{
  ELog::RegMethod RegA("fortranWrite","parse");

  std::vector<std::string> parts=StrFunc::splitParts(FmtStr,',');
  for(std::string& A : parts)
    {
      const std::string PUnit(A);
      size_t Cnt(1);
      StrFunc::sectPartNum(A,Cnt);  // maybe have pre-count
      if (A[0]=='I')
	{
	  A[0]=' ';
	  int lenCnt(1);
	  if (A.size()!=1 && !StrFunc::section(A,lenCnt))
	    throw ColErr::InvalidLine(PUnit,"Failed unit:",0);

	  for(size_t i=0;i<Cnt;i++)
	    FmtInfo.push_back(FmtID(1,lenCnt,0));
	}
      else if (A[0]=='F')
	{
	}
    }
  return;
}

void
fortranWrite::write(std::ostream& OX) const
  /*!
    Write out the format/data into the stream
    \param OX :: Output stream
  */
{
  OX<<outStr.str();
  return;
}

} // NAMESPACE StrFunc
