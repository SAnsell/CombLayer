/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/EReport.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <string>
#include <sstream>
#include <vector>
#include "Exception.h"
#include "FileReport.h"

namespace ELog
{

void 
EReport::process(const std::string& M,const int)
  /*!
    Process an individual input
    \param M :: String to report
    \param  :: Index of severity [ignored]
  */
{
  const int storeFlag(1);  // To be sorted out later
  std::ostream& DX( (storeFlag) ? std::cout : std::cerr);
  DX<<M<<std::endl;
  return;
}

int
StreamReport::process(std::ostream& OX,const std::vector<std::string>& MessV,
		      const std::vector<int>& TVec) const
  /*!
    Stream all the data to one output stream 
    \param OX :: output stream
    \param MessV :: Buffer of message lines
    \param TVec :: Buffer of id types
    \return 1 since the message stream can be cleared
  */
{
  if (MessV.size()!=TVec.size())
    throw ColErr::MisMatch<size_t>(MessV.size(),TVec.size(),
		  "StreamReport :: Error with holding vectors");

  for(size_t i=0;i<MessV.size();i++)
    OX<<"["<<TVec[i]<<"] "<<MessV[i]<<std::endl;
  return 1;
}

} // NAMESPACE ELog
