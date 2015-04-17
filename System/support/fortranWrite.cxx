/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/fortranWrite.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
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

void
fortranWrite::parse(const std::string& FmtStr)
{
  ELog::RegMethod RegA("fortranWrite","parse");
  
  std::vector<std::string> parts=StrFunc::StrParts(FmtStr);
  for(auto A : parts)
    ELog::EM<<"A == "<<A<<" =="<<ELog::endDiag;
  return;
}

void
fortranWrite::write(std::ostream& OX) const
{
  OX<<std::endl;
  return;
}


} // NAMESPACE StrFunc
