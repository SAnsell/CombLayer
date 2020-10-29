/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/phitsWriteSupport.cxx
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
#include <iterator>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"

#include "support.h"
#include "phitsWriteSupport.h"

/*! 
  \file phitsWriteSupport.cxx 
*/

namespace  StrFunc
{

template<typename T>
void
writePHITSCont(std::ostream& OX,const size_t depth,
	       const size_t index,const T& value)
 /*!
   Write out the line neatly for PHITS
   using a continued 4 stream
   \param OX :: ostream to write to
   \param depth :: step depth
   \param current index :: 
   \param value :: value
*/
{
  if (!index % 4)
    {
      if (index) OX<<"\n";
      OX<<std::string((depth+1)*2,' ');
    }
  OX<<value<<" ";
  return;
}

template<typename T>
void
writePHITS(std::ostream& OX,const size_t depth,
	   const std::string& unit,const T value)
 /*!
   Write out the line neatly for PHITS
  \param OX :: ostream to write to
  \param depth :: step depth
  \param unit :: unit name
  \param value :: value
*/
{
  writePHITSOpen(OX,depth,unit);
  OX<<value<<std::endl;
  return;
}

void
writePHITSOpen(std::ostream& OX,
	       const size_t depth,
	       const std::string& unit)
 /*!
   Write out an open line as <spces>unit = 
  \param OX :: ostream to write to
  \param depth :: step depth
  \param unit :: unit name
*/
{
  constexpr size_t equalPt(20);     // distance to name
  const std::string spc((depth+1)*2,' ');

  const int width(static_cast<int>(equalPt-(depth+1)*2-unit.size()));
  if (width>0)
    OX<<spc<<std::left<<unit<<std::setw(width)<<" ";
  else
    OX<<spc<<unit;
  OX<<"= ";
  return;
}

void
writePHITSTableHead(std::ostream& OX,
		    const size_t depth,
		    const std::vector<std::string>& units)
 /*!
   Write out an open line as a table set
   \param OX :: ostream to write to
   \param depth :: step depth
   \param units :: unit names
 */
{
  constexpr size_t equalPt(12);     // distance to name

  const std::string spc((depth+1)*2,' ');
  OX<<spc;
  
  for(const std::string& item : units)
    {
      const int width(equalPt-item.size());
      
      if (width>0)
	OX<<std::left<<std::setw(width)<<item;
      else
	OX<<item;
    }
  OX<<std::endl;
  return;
}


template void
writePHITS(std::ostream&,const size_t,const std::string&,const bool);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const double);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const char*);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const int);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const size_t);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const long int);
template void
writePHITS(std::ostream&,const size_t,const std::string&,const std::string);


template void
writePHITSCont(std::ostream&,const size_t,const size_t,const double&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const std::string&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const int&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const size_t&);


}  // NAMESPACE StrFunc
