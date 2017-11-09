/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/gridUnit.cxx
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
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "gridUnit.h"

namespace constructSystem
{

std::ostream&
operator<<(std::ostream& OX,const gridUnit& A)
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

gridUnit::gridUnit(const size_t nLink,const long int aI,
		   const long int bI,const Geometry::Vec3D& C) : 
  empty(0),cut(0),iA(aI),iB(bI),Centre(C),gridLink(nLink),surfKey(nLink),
  cellNumber(0),boundaryClosed(std::pair<size_t,size_t>(0,0))
  /*!
    Constructor
    \param nLink :: number of links
    \param aI :: Index A
    \param bI :: Index B
    \param C :: Centre point
  */
{
  clearLinks();
}

gridUnit::gridUnit(const size_t nLink,const long int aI,
		   const long int bI,const bool cF,
		   const Geometry::Vec3D& C) : 
  empty(0),cut(cF),iA(aI),iB(bI),Centre(C),gridLink(nLink),surfKey(nLink),
  cellNumber(0),boundaryClosed(std::pair<size_t,size_t>(0,0))
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

gridUnit::gridUnit(const gridUnit& A) : 
  empty(A.empty),cut(A.cut),iA(A.iA),iB(A.iB),
  Centre(A.Centre),gridLink(A.gridLink),cylSurf(A.cylSurf),
  surfKey(A.surfKey),cellNumber(A.cellNumber),
  boundaryClosed(A.boundaryClosed),cutStr(A.cutStr)
  /*!
    Copy constructor
    \param A :: gridUnit to copy
  */
{}

gridUnit&
gridUnit::operator=(const gridUnit& A)
  /*!
    Assignment operator
    \param A :: gridUnit to copy
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
      boundaryClosed=A.boundaryClosed;
      cutStr=A.cutStr;
    }
  return *this;
}

void
gridUnit::clearLinks()
  /*!
    Clear all the links
    Note that links are NOT managed.
  */
{
  for(gridUnit*& vc : gridLink)
    vc=0;

  return;
}

void
gridUnit::setBoundary(const size_t A,const size_t B)
  /*!
    Set the boundary side piont [+1 index / 0 imples none]
    This is note checked to be within nLink
    \param A :: First index
    \param b :: Second index
   */
{
  boundaryClosed.first=A;
  boundaryClosed.second=B;
  return;
}
  
void
gridUnit::clearBoundary() 
  /*!
    Clear a surface point
   */
{
  boundaryClosed.first=0;
  boundaryClosed.second=0;
  return;
}

int
gridUnit::clearBoundary(const size_t side)
  /*!
    Test and set a closed boundary
    \param side :: Side to test
    \return 1 if boundary valid
  */
{
  // NO Work
  if (boundaryClosed.first != side+1 &&
      boundaryClosed.second != side+1) return 0;

  const size_t linkSurf=(boundaryClosed.first == side+1 )
    ? boundaryClosed.second-1 : boundaryClosed.first-1;

  if (linkSurf>=surfKey.size())
    throw ColErr::IndexError<size_t>
      (linkSurf,surfKey.size(),"linkSurf exceeds surfKey");
    
  boundaryClosed.first=0;
  boundaryClosed.second=0;
  return surfKey[linkSurf];
}
  
void
gridUnit::setCyl(const int surfN)
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
gridUnit::addCyl(const int surfN)
  /*!
    Add a central cylinder
    \param surfN :: Cylinder number
   */
{
  cylSurf.push_back(surfN);
  return;
}

bool
gridUnit::isComplete() const 
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
gridUnit::hasLink(const size_t index) const
/*!
  Determine if the surface has an key
  \param index :: gird surface index
  \return true if link is set
*/
{
  return (!gridLink[index % gridLink.size()] ) ? 0 : 1;
}

bool
gridUnit::hasSurfLink(const size_t index) const
/*!
  Determine if the surface has an key
  \param index :: gird surface index
  \return true if surface is set
*/
{
  return (surfKey[index % surfKey.size()]==0) ? 0 : 1;
}

size_t
gridUnit::nLinks() const
  /*!
    Accessor to the number of links
    \return number of links					       
   */
{
  return static_cast<size_t>
    (std::count_if(surfKey.begin(),surfKey.end(),
                   std::bind(std::not_equal_to<int>(),
                             std::placeholders::_1,0)));
}

void
gridUnit::setLink(const size_t Index,gridUnit* GUnit) 
  /*!
    Set the gridUnit links
    \param Index :: Index for the gridUnit
    \param GUnit :: Grid unit
   */
{
  gridLink[Index % gridLink.size()] = GUnit;
  return;
}

void
gridUnit::setSurf(const size_t Index,const int surfN) 
  /*!
    Set the surface nubmer
    \param Index :: Index fo the surface
    \param surfN :: Surface number
   */
{
  surfKey[Index % surfKey.size()] = surfN;
  return;
}

std::string
gridUnit::getShell() const
  /*!
    Get binding shell for the grid unit
    \return Shell string
   */
{
  std::ostringstream cx;
  std::string extra=" (";
  for(const int SN : surfKey)
    {
      if (SN)
        {
          cx<<extra<<-SN;
          extra=" : ";
        }
    }
  cx<<") ";
  return cx.str();
}

std::string
gridUnit::getInner() const
  /*!
    Get binding shell for the grid unit
    \return Shell string
   */
{
  std::ostringstream cx;
  cx<<" ";
  for(const int SN : surfKey)
    if (SN)
      cx<<SN<<" ";
  return cx.str();
}

  
void
gridUnit::write(std::ostream& OX) const
  /*!
    Simple write function
    \param OX :: Output stream
  */
{

  OX<<"["<<iA<<"]["<<iB<<"] = "<<Centre<<"\n";
  return;
}
  
}  // NAMESPACE constructSystem
