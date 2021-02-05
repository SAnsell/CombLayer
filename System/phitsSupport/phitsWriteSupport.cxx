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
#include <map>
#include <algorithm>
#include <functional>

#include "Vec3D.h"

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

void
writePHITSComment(std::ostream& OX,const size_t depth,
		  const std::string& Line )
 /*!
   Write out comments for PHITS
   \param OX :: ostream to write to
   \param depth :: step depth
   \param Line :: comment line
*/
{
  const size_t MaxLine(72);

  std::string::size_type pos(0);
  std::string X=Line.substr(0,MaxLine);
  std::string::size_type posB=X.find_last_of(" ");

  const std::string spc((depth+1)*2,' ');
  while (posB!=std::string::npos && 
	 X.length()>=MaxLine)
    {
      pos+=posB+1;
      const std::string Out=X.substr(0,posB);
      if (!isEmpty(Out))
	OX<<"#"<<spc<<X.substr(0,posB)<<std::endl;

      X=Line.substr(pos,MaxLine);
      posB=X.find_last_of(" ");
    }
  if (!isEmpty(X))
    OX<<"#"<<spc<<X<<std::endl;

  return;
}

template<typename T>
void
writePHITS(std::ostream& OX,const size_t depth,
	   const std::string& unit,const T value,
	   const std::string& comment)
 /*!
   Write out the line neatly for PHITS
  \param OX :: ostream to write to
  \param depth :: step depth
  \param unit :: unit name
  \param value :: value
*/
{
  constexpr size_t equalPt(20);     // distance to name
  writePHITSOpen(OX,depth,unit);
  OX<<std::setw(equalPt)<<value;
  if (!comment.empty()) OX<<"# "<<comment;
  OX<<std::endl;
  return;
}

template<>
void
writePHITS(std::ostream& OX,const size_t depth,
	   const std::string& unit,
	   const Geometry::Vec3D value,
	   const std::string& comment)
 /*!
   Write out the line neatly for PHITS
  \param OX :: ostream to write to
  \param depth :: step depth
  \param unit :: unit name
  \param value :: value
*/
{
  writePHITSOpen(OX,depth,unit);
  OX<<"Vec3D("<<value<<")";
  if (!comment.empty()) OX<<"# "<<comment;
  OX<<std::endl;
  return;
}

void
writePHITSCellSet(std::ostream& OX,
		    const size_t depth,
		    const std::map<int,double>& cellValues)
/*!
  \param OX :: ostream to write to
  \param depth :: step depth
  \param unit :: unit name
  \param cellValues :: 
 */
{
  // ugly double loop

  if (cellValues.empty()) return;
  std::map<int,double>::const_iterator mc=
    cellValues.begin();

  int AI(mc->first);
  int BI(mc->first);
  double AVal(mc->second);


  std::ostringstream cx;
  for( ;mc!=cellValues.end();mc++)
    {
      const int& iVal=mc->first;
      const double& cVal=mc->second;

      // failed [need new point
      if (std::abs(AVal-cVal)>1e-6)
	{

	  if (BI == AI )  // single point
	      StrFunc::writePHITSItems(OX,depth,AI,AVal);
	  else
	    {
	      cx.str("");
	      cx<<"( "<<AI<<" - "<<BI<<" )"<<AVal;
	      StrFunc::writePHITSOpen(OX,depth,cx.str());
	    }
	  AI=iVal;
	  BI=iVal;
	  AVal=cVal;
	}
      else
	{
	  BI=iVal;
	}
    }

  // UGLY Tail part:
  if (BI == AI)  // single point
    StrFunc::writePHITSItems(OX,depth,AI,AVal);
  else 
    {
      cx.str("");
      cx<<"( "<<AI<<" - "<<BI<<" )"<<AVal;
      StrFunc::writePHITSOpen(OX,depth,cx.str());
    }
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
  constexpr size_t equalPt(14);     // distance to name

  const std::string spc((depth+1)*2,' ');
  OX<<spc;
  
  for(const std::string& item : units)
    {
      OX<<std::left<<std::setw(equalPt)<<item;
    }
  OX<<std::endl;
  return;
}


template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const bool,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const double,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const char*,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const int,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const size_t,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const long int,const std::string&);
template void
writePHITS(std::ostream&,const size_t,const std::string&,
	   const std::string,const std::string&);


template void
writePHITSCont(std::ostream&,const size_t,const size_t,const double&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const std::string&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const int&);
template void
writePHITSCont(std::ostream&,const size_t,const size_t,const size_t&);


}  // NAMESPACE StrFunc
