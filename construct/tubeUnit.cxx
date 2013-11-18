/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   construct/tubeUnit.cxx
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
#include <boost/shared_ptr.hpp>
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
#include "surfRegister.h"
#include "tubeUnit.h"

namespace constructSystem
{

std::ostream&
operator<<(std::ostream& OX,const tubeUnit& A)
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

tubeUnit::tubeUnit(const int aI,const int bI,
		   const Geometry::Vec3D& C) : 
  empty(0),iA(aI),iB(bI),Centre(C) 
  /*!
    Constructor
    \param aI :: Index A
    \param bI :: Index B
    \param C :: Centre point
  */
{}

tubeUnit::tubeUnit(const tubeUnit& A) : 
  empty(A.empty),iA(A.iA),iB(A.iB),
  Centre(A.Centre),cylSurf(A.cylSurf),
  linkPlanes(A.linkPlanes)
  /*!
    Copy constructor
    \param A :: tubeUnit to copy
  */
{}

tubeUnit&
tubeUnit::operator=(const tubeUnit& A)
  /*!
    Assignment operator
    \param A :: tubeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      empty=A.empty;
      iA=A.iA;
      iB=A.iB;
      Centre=A.Centre;
      cylSurf=A.cylSurf;
      linkPlanes=A.linkPlanes;
    }
  return *this;
}

void 
tubeUnit::clearLinks()
  /*!
    Clear all the link points/planes
  */
{
  linkPts.erase(linkPts.begin(),linkPts.end());
  linkPlanes.erase(linkPlanes.begin(),linkPlanes.end());
  return;
}  

bool
tubeUnit::isComplete(const size_t Size) const 
  /*!
    Deterimine if the item is on the boundary.
    Note if empty ==2 [i.e. already accounted for
    then this returns true 
    \return true is fullSize links.
  */
{
  return (linkPts.size()!=Size) ? 0 : 1;
}

bool 
tubeUnit::isConnected(const tubeUnit& TU) const 
  /*!
    Determine if a point is connected. 
    \param TU :: Tube unit to check
    \return true if within one unit
  */
{
  return ( ((iA==TU.iA+1 || iA==TU.iA-1) && iB==TU.iB) ||
    ((iB==TU.iB+1 || iB==TU.iB-1) && iA==TU.iA) ) ? 1 : 0;
}

size_t
tubeUnit::nLinks() const
  /*!
    Accessor to the number of links
    \return number of links					       
   */
{
  ELog::RegMethod RegA("tubeUnit","nLinks");

  return linkPts.size();
  // typedef std::map<int,tubeUnit*>::value_type VT;
  // return static_cast<size_t>(count_if(linkPts.begin(),linkPts.end(),
  // 	    boost::bind(&tubeUnit::isEmpty,
  // 	        boost::bind(&VT::second,_1))));
}

tubeUnit*
tubeUnit::getItem(const int jA,const int jB) 
  /*!
    Use the net structure to deterimine the item at a particular 
    coordinate
    \param jA :: Aim point
    \param jB :: Aim point
    \return tubeUnit / 0 on failure
   */
{
  if (jA==iA && jB==iB) 
    return this;

  MTYPE::iterator mc;
  if (jA>iA)    
    {
      for(mc=linkPts.begin();mc!=linkPts.end();mc++)
	if (mc->second->iA>iA)
	  return mc->second->getItem(jA,jB);
    }
  else if (jA<iA)
    {
      for(mc=linkPts.begin();mc!=linkPts.end();mc++)
	if (mc->second->iA < iA)
	  return mc->second->getItem(jA,jB);
    }

  if (jB>iB)    
    {
      for(mc=linkPts.begin();mc!=linkPts.end();mc++)
	if (mc->second->iB > iB)
	  return mc->second->getItem(jA,jB);
    }
  else if (jA<iA)
    {
      for(mc=linkPts.begin();mc!=linkPts.end();mc++)
	if (mc->second->iB < iB)
	  return mc->second->getItem(jA,jB);
    }
  return 0;
}

const tubeUnit*
tubeUnit::getLink(const size_t index) const
  /*!
    Use the net structure to deterimine the item at a particular 
    coordinate
    \param index :: index of unit
    \return tubeUnit / 0 on failure
   */
{
  ELog::RegMethod RegA("tubeUnit","getLink");
  const int key=getMapIndex(index);
  MTYPE::const_iterator mc=linkPts.find(key);
  if (mc!=linkPts.end())
    return mc->second;
  return 0;
}

void
tubeUnit::addLink(tubeUnit* TUPtr)
  /*!
    Creates a link between this and TU.
    It is currently not checked that jA and jB are correct
    \param TUPtr :: tubeUnit to connect to 
  */
{
  if (TUPtr && isConnected(*TUPtr))
    {	
      const int jA=TUPtr->iA;
      const int jB=TUPtr->iB;

      const int mapX=(jA-iA)*100+(jB-iB);
      linkPts.insert(MTYPE::value_type(mapX,TUPtr));
      linkPlanes.insert(PTYPE::value_type(mapX,0));

      // Note that mapX is NOT the same for the TUPtr object:
      TUPtr->linkPts.insert(MTYPE::value_type(-mapX,this));
      TUPtr->linkPlanes.insert(PTYPE::value_type(-mapX,0));
    }
  return;
}

int
tubeUnit::getMapIndex(const size_t I)
  /*!
    Determines the relative map index based on the iteration
    value
    \param I :: Index value
  */
{
  switch (I)
    {
    case 0:                // lower
      return -100;
    case 1:                 // left
      return -1;
    case 2:                // top
      return 100;
    case 3:               // right
      return 1;
    }
  ELog::RegMethod RegA("tubeUnit","getMapIndex");
  throw ColErr::InContainerError<size_t>(I,"I");
}
  
	
bool
tubeUnit::midPlane(const size_t index,Geometry::Vec3D& Pt,
		      Geometry::Vec3D& Axis) const
  /*!
    Create the mid point between each of the objects
    \param index :: Index of point
    \param Pt :: Centre point of plane
    \param Axis :: Axis direction going from low - high
    \return 1 if valid / 0 if no link
  */
{
  ELog::RegMethod RegA("tubeUnit","getMidPoint");
  const int mapIndex=getMapIndex(index);
  MTYPE::const_iterator mc=linkPts.find(mapIndex);
  if (mc==linkPts.end())  return 0;
  
  const tubeUnit* A=mc->second;
  Pt=(A->Centre+Centre)/2.0;
  Axis=(Centre-A->Centre).unit();
  return 1;
}

void
tubeUnit::setCyl(const int surfN)
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
tubeUnit::addCyl(const int surfN)
  /*!
    Add a central cylinder
    \param surfN :: Cylinder number
   */
{
  cylSurf.push_back(surfN);
  return;
}

void
tubeUnit::addPlane(const size_t index,const int surfN)
  /*!
    Add a central cylinder
    \param index :: Index number
    \param surfN :: Cylinder number
   */
{
  ELog::RegMethod RegA("tubeUnit","addPlane");

  const int mapIndex=getMapIndex(index);
  PTYPE::iterator mc=linkPlanes.find(mapIndex);
  if (mc==linkPlanes.end())
    throw ColErr::InContainerError<size_t>(index,"index");
  
  mc->second=surfN;
  return;
}

std::string
tubeUnit::getCell(const ModelSupport::surfRegister& SMap,
		  const size_t cylIndex) const
  /*!
    Create the surface cell for a particular index, counting
    from the centre out
    \param SMap :: surface register for the build region
    \param cylIindex :: index of cylindrical layer 
   */
{
  ELog::RegMethod RegA("tubeUnit","getCell");

  if (cylIndex>cylSurf.size())
    return "";
  
  std::ostringstream cx;
  cx<<" ";
  // Outer cell is bound by planes:
  if (cylIndex==cylSurf.size())
    {
      PTYPE::const_iterator pc;
      MTYPE::const_iterator mc;
      for(pc=linkPlanes.begin(),mc=linkPts.begin();
	  pc!=linkPlanes.end() && mc!=linkPts.end();
	  mc++,pc++)
	{
	  cx<< SMap.realSurf(pc->second)<<" ";
	}
      if (!cylSurf.empty())
	cx<< SMap.realSurf(cylSurf[cylIndex-1])<<" ";
    }
  else if (cylIndex)
    {
      cx<< SMap.realSurf(cylSurf[cylIndex-1])<<" ";
      cx<< -SMap.realSurf(cylSurf[cylIndex])<<" ";
    }
  // Inner cylinder:
  else if (cylIndex==0 )
    {
      cx<< -SMap.realSurf(cylSurf[0])<<" ";
    }

  return cx.str();
}

std::string
tubeUnit::writeID() const
{
  std::ostringstream cx;
  cx<<iA<<" "<<iB;
  return cx.str();
}

void
tubeUnit::write(std::ostream& OX) const
  /*!
    Simple write function
    \param OX :: Output stream
  */
{
  static char typeKey[]="CEF";

  OX<<typeKey[empty % 3]<<"["<<iA<<"]["<<iB<<"] = "<<Centre<<"\n";
  int cnt(1);
  MTYPE::const_iterator mc;
  for(mc=linkPts.begin();mc!=linkPts.end();mc++)
    {
      if (mc->second)
	OX<<"LP["<<cnt<<"] : "<<mc->second->iA<<" "
	  <<mc->second->iB<<" "
	  <<mc->second->Centre<<"\n";
      else
	OX<<"LP["<<cnt<<"] : Zero\n";
      cnt++;
    }
  return;
}
  
}  // NAMESPACE shutterSystem
