/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/writeSupport.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <fmt/core.h>

#include "Exception.h"
#include "Vec3D.h"

#include "support.h"

/*! 
  \file writeSupport.cxx 
*/

namespace  StrFunc
{

std::string
flukaNum(const long int I)
  /*!
    Process a number into a fluka style string
    \param I :: Number to use
  */
{
  const double D=static_cast<double>(I);
  if (D > 1e8 || D < -1e7)
    return fmt::format("{:10.5g}",D);

  return fmt::format("{:10.1f}",D);
}

std::string
flukaNum(const double D,const double zeroTol,
	 const double exponentTol)
  /*!
    Process a number into a fluka style string
    \param D :: Number to use
  */
{

  const static std::vector<std::string> FNum
    ({
      "{:10.8f}",
      "{:10.7f}",
      "{:10.6f}",
      "{:10.5f}",
      "{:10.4f}",
      "{:10.3f}"
      });

  const static std::vector<std::string> FG
    ({
      "{:10.8g}",
      "{:10.7g}",
      "{:10.6g}",
      "{:10.5g}",
      "{:10.4g}",
      "{:10.3g}"
    });

  const double lowExpTol(1.0/exponentTol);

  if (D>-zeroTol && D<zeroTol)  // float point limits
    return fmt::format("{:10.6f}",0.0);

  std::string out;  
  if (std::abs(D)<exponentTol && std::abs(D) >lowExpTol)      // +ve low range
    {
      for(const std::string& FMTStr : FNum)
	{
	  out=fmt::format(fmt::runtime(FMTStr),D);
	  if (out.size()<=10) return out;
	}
    }
  else
    {
      for(const std::string& FMTStr : FG)
	{
	  out=fmt::format(fmt::runtime(FMTStr),D);
	  if (out.size()<=10) return out;
	}
    }
  return out;
}

void
writeFLUKA(const std::string& Line,std::ostream& OX,
	   const double zeroTol,const double expTol)
  /*!
    Write out the line in the fixed FLUKA format WHAT(1-6).
    Replace " - " by space to write empty WHAT cards.
    \param Line :: full FLUKA line
    \param OX :: ostream to write to
  */
{
  // this is expensive
  std::istringstream iss(Line);
  std::vector<std::string> whats(std::istream_iterator<std::string>{iss},
				 std::istream_iterator<std::string>());

  size_t i(1);
  long int I;
  double D;

  for (std::string& w : whats)
    {
      if (w=="-") w=" ";
      if (i % 8==0) // never a number
	OX<<std::setw(10)<<std::left<<w<<std::endl;
      else
	{
	  if (StrFunc::convert(w,I))
	    OX<<flukaNum(I);
	  else if (StrFunc::convert(w,D))
	    OX<<flukaNum(D,zeroTol,expTol);
	  else
	    {
	      if (w.size()>2 && w.size()<12 && w[0]=='%')
		w=w.substr(1);
	      else if (w.size()>10)
		throw ColErr::InvalidLine(w,"String to long for FLUKA");
	      if (i % 8==1)
		OX<<std::setw(10)<<std::left<<w;	
	      else
		OX<<std::setw(10)<<std::right<<w;
	    }
	}      
      i++;
    }
  if (i % 8 != 1) OX<<std::endl;
  return;
}

void
writeFLUKAhead(const std::string& HeadUnit,
	       const std::string& EndUnit,
	       const std::string& Line,
	       std::ostream& OX)
  /*!
    Write out the line in the fixed FLUKA format WHAT(1-6).
    Replace " - " by space to write empty WHAT cards.
    \param HeadUnit :: head name
    \param EndUnit :: end name
    \param Line :: full FLUKA line
    \param OX :: ostream to write to
  */
{
  const size_t NItem(6);
  // this is expensive
  std::istringstream iss(Line);
  std::vector<std::string> whats(std::istream_iterator<std::string>{iss},
				 std::istream_iterator<std::string>());

  size_t i(0);
  for (const std::string& w : whats)
    {
      if (!i) OX<<std::setw(10)<<std::left<<HeadUnit;
      OX<<std::setw(10)<<std::right<<w;
      i++;
      if (i==NItem)
	{
	  OX<<EndUnit<<std::endl;
	  i=0;
	}
    }
  
  if (i)
    {
      for(;i<NItem;i++)
	OX<<std::string(10,' ');
      OX<<EndUnit<<std::endl;
    }
  
  return;
}

void
writeMCNPX(const std::string& Line,std::ostream& OX)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  (split on a space or comma)
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
*/
{
  writeControl(Line,OX,72,8);
  return;
}

void
writeMCNPXcont(const std::string& Line,std::ostream& OX)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  as if it is a split line
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
*/
{
  writeControl(Line,OX,72,-8);
  return;
}

void
writeControl(const std::string& Line,std::ostream& OX,
	     const size_t LNmax,int insertDepth)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  (split on a space or comma)
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
  \param LNmax :: Maximium char count in a line
  \param insertDepth :: second line insert depth [-ve to include first line]
*/
{
  // Line
  size_t spcLen(0);
  if (insertDepth<0)
    {
      insertDepth *= -1;
      spcLen=static_cast<size_t>(insertDepth);
    }

  std::string::size_type pos(0);
  std::string X=Line.substr(0,LNmax-spcLen);    
  std::string::size_type posB=X.find_last_of(" ,");
  while(X.length() == LNmax-spcLen &&
	posB!=std::string::npos)
    {
      pos+=posB+1;
      if (!isspace(X[posB])) posB++;  // skip pass comma 
      X=removeOuterSpace(X.substr(0,posB));
      if (!isEmpty(X))
	OX<<std::string(spcLen,' ')<<X<<std::endl;

      spcLen=static_cast<size_t>(insertDepth);
      X=Line.substr(pos,LNmax-spcLen);
      posB=X.find_last_of(" ,");
    }
    
  X=removeOuterSpace(X);
  if (!isEmpty(X))
    OX<<std::string(spcLen,' ')<<X<<std::endl;
  return;
}

void
writeMCNPXcomment(const std::string& Line,std::ostream& OX,
		  const std::string commentString)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0->72 after that 8 to 72
  (split on a space or comma). It puts int all in a comment
  so lines are cut by 2
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
  \param commentString :: standard comment pre-string
*/
{
  const size_t MaxLine(72);

  std::string::size_type pos(0);
  std::string X=Line.substr(0,MaxLine);
  std::string::size_type posB=X.find_last_of(" ,");
  size_t spc(0);
  while (posB!=std::string::npos && 
	 X.length()>=MaxLine-spc)
    {
      pos+=posB+1;
      if (!isspace(X[posB]))
	posB++;
      const std::string Out=X.substr(0,posB);
      if (!isEmpty(Out))
        {
	  OX<<commentString;
	  if (spc)
	    OX<<std::string(spc,' ');
	  OX<<X.substr(0,posB)<<std::endl;
	}
      spc=8;
      X=Line.substr(pos,MaxLine-spc);
      posB=X.find_last_of(" ,");
    }
  if (!isEmpty(X))
    {
      OX<<commentString;
      if (spc)
	OX<<std::string(spc,' ');
      OX<<X<<std::endl;
    }
  return;
}

std::vector<std::string>
splitComandLine(std::string Line)
/*!
  Split the line based on "-Not a number" commands
  \param Line :: full MCNPX line
  \return vector of components
*/
{
  std::vector<std::string> outVec;
  std::string unit;
  std::string part;
  size_t offset(0);
  while(StrFunc::section(Line,part))
    {
      if (part.size()>=2 && part[0]=='-' &&
	  !std::isdigit(part[1]))
	{
	  if (offset)
	    {
	      outVec.push_back(unit);
	      unit=std::string(offset,' ');
	    }
	  offset=3;
	}
      unit+=" "+part;
    }
  outVec.push_back(unit);
  return outVec;
}

}  // NAMESPACE StrFunc
