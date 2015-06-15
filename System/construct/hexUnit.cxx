/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   construct/hexUnit.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <boost/bind.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "hexUnit.h"

namespace constructSystem
{

std::ostream&
operator<<(std::ostream& OX,const hexUnit& A)
   /*!
     Write out info
     \param OX :: Output stream
     \param A :: tubeUnit to write
     \return Stream
   */
{
  A.write(OX);
  return OX;
}

hexUnit::hexUnit(const int aI,const int bI,
		   const Geometry::Vec3D& C) : 
  empty(0),cut(0),iA(aI),iB(bI),Centre(C),hexLink(6),surfKey(6),
  cellNumber(0)
  /*!
    Constructor
    \param aI :: Index A
    \param bI :: Index B
    \param C :: Centre point
  */
{
  clearLinks();
}

hexUnit::hexUnit(const int aI,const int bI,const bool cF,
		   const Geometry::Vec3D& C) : 
  empty(0),cut(cF),iA(aI),iB(bI),Centre(C),hexLink(6),surfKey(6),
  cellNumber(0)
  /*!
    Constructor
    \param aI :: Index A
    \param bI :: Index B
    \param cF :: Cut flag
    \param C :: Centre point
  */
{
  clearLinks();
}

hexUnit::hexUnit(const hexUnit& A) : 
  empty(A.empty),cut(A.cut),iA(A.iA),iB(A.iB),
  Centre(A.Centre),hexLink(A.hexLink),cylSurf(A.cylSurf),
  surfKey(A.surfKey),cellNumber(A.cellNumber),
  cutStr(A.cutStr)
  /*!
    Copy constructor
    \param A :: hexUnit to copy
  */
{}

hexUnit&
hexUnit::operator=(const hexUnit& A)
  /*!
    Assignment operator
    \param A :: hexUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      empty=A.empty;
      cut=A.cut;
      iA=A.iA;
      iB=A.iB;
      Centre=A.Centre;
      cylSurf=A.cylSurf;
      surfKey=A.surfKey;
      cellNumber=A.cellNumber;
      cutStr=A.cutStr;
    }
  return *this;
}

int
hexUnit::hexIndex(const size_t index)
  /*!
    Convert Index[0-5] into an modification of the base index
    \param index :: value between 0 and 5 
    \return 1000 point offset index
   */
{
  switch (index % 6) 
    {
    case 0:
      return 1000;   // 1,0
    case 1:
      return 1;    // 0,1
    case 2:
      return -999;     // -1,1
    case 3:
      return -1000;  // -1,0 
    case 4:
      return -1;   // -1,1
    case 5:
      return 999;      //  1,-1
    }
  // return to avoid compiler warning
  return 0;
}

void
hexUnit::clearLinks()
  /*!
    Clear all the links
   */
{
  for(hexUnit*& vc : hexLink)
    vc=0;

  return;
}

void
hexUnit::setCyl(const int surfN)
  /*!
    Set a central cylinder
    \param surfN :: Cylinder number
   */
{
  cylSurf.clear();
  cylSurf.push_back(surfN);
  return;
}

void
hexUnit::addCyl(const int surfN)
  /*!
    Add a central cylinder
    \param surfN :: Cylinder number
   */
{
  cylSurf.push_back(surfN);
  return;
}

bool
hexUnit::isComplete() const 
  /*!
    Deterimine if the item is on the boundary.
    Note if empty ==2 [i.e. already accounted for
    then this returns true 
    \return true is fullSize links.
  */
{
  return (find(surfKey.begin(),surfKey.end(),0) ==
     surfKey.end()) ? 1 : 0;
}

bool 
hexUnit::isConnected(const hexUnit& TU) const 
  /*!
    Determine if a point is connected. 
    \param TU :: Tube unit to check
    \return true if within one unit
  */
{
  const int dA=iA-TU.iA;
  const int dB=iB-TU.iB;
  if ((dA*dB==0 || dA+dB==0) && (dA==1 || dB==1 || dB==-1 || dA==-1)) 
    return 1;

  return 0;
}

bool
hexUnit::hasLink(const size_t index) const
/*!
  Determine if the surface has an key
  \param index :: HEx surface index
*/
{
  ELog::RegMethod RegA("hexUnit","hasLink");
  if (index>5)
    throw ColErr::IndexError<size_t>(index,5,"index");
    
  return (surfKey[index]==0) ? 0 : 1;
}

size_t
hexUnit::nLinks() const
  /*!
    Accessor to the number of links
    \return number of links					       
   */
{
  return static_cast<size_t>
    (std::count_if(surfKey.begin(),surfKey.end(),
		   boost::bind(std::not_equal_to<int>(),_1,0)));
}

void
hexUnit::setSurf(const size_t Index,const int surfN) 
  /*!
    Set the surface nubmer
    \param Index :: Index fo the surface
    \param surfN :: Surface number
   */
{
  surfKey[Index % 6] = surfN;
  return;
}

std::string
hexUnit::getShell() const
  /*!
    Get binding shell for the hex unit
    \return Shell string
   */
{
  std::ostringstream cx;
  std::string extra=" (";
  for(size_t i=0;i<6;i++)
    {
      if (surfKey[i])
	{
	  cx<<extra<<-surfKey[i];
	  extra=" : ";
	}
    }
  cx<<") ";
  return cx.str();
}

std::string
hexUnit::getInner() const
  /*!
    Get binding shell for the hex unit
    \return Shell string
   */
{
  std::ostringstream cx;
  cx<<" ";
  for(size_t i=0;i<6;i++)
    if (surfKey[i])
      cx<<surfKey[i]<<" ";
  return cx.str();
}

void
hexUnit::write(std::ostream& OX) const
  /*!
    Simple write function
    \param OX :: Output stream
  */
{

  OX<<"["<<iA<<"]["<<iB<<"] = "<<Centre<<"\n";
  return;
}
  
}  // NAMESPACE shutterSystem
