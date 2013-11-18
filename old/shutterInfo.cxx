/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/shutterInfo.cxx
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
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "shutterInfo.h"
#include "ShutterCreate.h"

namespace shutterSystem
{

const int shutterInfo::lSide(90022);
const int shutterInfo::rSide(90023);
const int shutterInfo::lWindow(90027);
const int shutterInfo::rWindow(90028);

const int shutterInfo::lTorpedo(92011);
const int shutterInfo::rTorpedo(92012);
const int shutterInfo::tDTorpedo(91004);
const int shutterInfo::bDTorpedo(91003);
const int shutterInfo::tCTorpedo(91001);
const int shutterInfo::bCTorpedo(91002);


const int shutterInfo::lShutter(92015);
const int shutterInfo::rShutter(92016);
const int shutterInfo::tDShutter(93013);
const int shutterInfo::bDShutter(93014);
const int shutterInfo::tCShutter(93003);
const int shutterInfo::bCShutter(93004);
const int shutterInfo::tCIRShutter(93023);
const int shutterInfo::bCIRShutter(93024);

const int shutterInfo::lBulkInner(94021);
const int shutterInfo::rBulkInner(94022);
const int shutterInfo::lBulkOuter(94023);
const int shutterInfo::rBulkOuter(94024);
const int shutterInfo::bBulkInner(94006);
const int shutterInfo::tBulkInner(94005);
const int shutterInfo::bBulkOuter(94016);
const int shutterInfo::tBulkOuter(94015);

const int shutterInfo::cTop(91002);
const int shutterInfo::cBase(91001);
const int shutterInfo::dTop(91004);
const int shutterInfo::dBase(91003);
const int shutterInfo::vDivide(40000);
double shutterInfo::steelInner(0.0);
double shutterInfo::steelOuter(0.0);

shutterInfo::shutterInfo(const int I,const double A,const int T) : 
  number(I),angle(M_PI*A/180.0),type(convType(T)),
  ModCentre(0,0,0),Axis(0,sin(angle),-cos(angle))
  /*!
    Constructor
    \param I :: Index number
    \param A :: Angle
    \param T :: Type 0 coupled / 1 decoupled
   */
{}

shutterInfo::shutterInfo(const shutterInfo& A) :
  number(A.number),angle(A.angle),
  type(A.type),ModCentre(A.ModCentre),Axis(A.Axis)
  /*!
    Copy Constructor
    \param A :: shutterInfo to copy
   */
{}

shutterInfo&
shutterInfo::operator=(const shutterInfo& A) 
  /*!
    Assignment operator
    \param A :: shutterInfo to copy
    \return *this
   */
{
  if (this!=&A)
    {
      number=A.number;
      angle=A.angle;
      type=A.type;
      ModCentre=A.ModCentre;
      Axis=A.Axis;
    }
  return *this;
}

MODTYPE
shutterInfo::convType(const int T)
  /*!
    Convert an interger into an Enumerated type
    \param T :: type to convert
    \return MODTYPE
   */
{
  switch (T)
    {
    case hydrogen:
      return hydrogen;
    case groove:
      return groove;
    case narrow:
      return narrow;
    case broad:
      return broad;
    case chipIR:
      return chipIR;
    }
  throw ColErr::IndexError<int>(T,5,"shutterInfo::setType");
}

Geometry::Vec3D
shutterInfo::getNormal(const Geometry::Vec3D& PA) const 
  /*!
    Calculate the norm of the shutter
    \param PA :: Vector in the plane
    \return Vector normal to both
  */
{
  return PA*Axis;
}

std::string
shutterInfo::shutterSides(const int compFlag) const
  /*!
    Return the shutter box string given the number
    \param compFlag :: Do complement
    \return Shutterbox (full)
  */
{
  std::ostringstream cx;
  if (compFlag)
    cx<<-(lShutter+10*number)<<" : "<<rShutter+10*number<<" ";
  else
    {
      cx<<lShutter+10*number<<" "<<-(rShutter+10*number)<<" ";
      cx<<((angle<0) ? -vDivide : vDivide)<<" ";
    }
  return cx.str();
}

std::string
shutterInfo::bulkInnerSides(const int compFlag) const
  /*!
    Return the bulk string given the 
    \param compFlag :: Do complement
    \return bulkbox (full)
  */
{
  std::ostringstream cx;
  if (compFlag)
    cx<<-(lBulkInner+10*number)<<" : "<<rBulkInner+10*number<<" ";
  else
    {
      cx<<lBulkInner+10*number<<" "<<-(rBulkInner+10*number)<<" ";
      cx<<((angle<0) ? -vDivide : vDivide)<<" ";
    }
  return cx.str();
}

std::string
shutterInfo::bulkOuterSides(const int compFlag) const
  /*!
    Return the bulk string given the 
    \param compFlag :: Do complement
    \return bulkbox (full)
  */
{
  std::ostringstream cx;
  if (compFlag)
    cx<<-(lBulkOuter+10*number)<<" : "<<rBulkOuter+10*number<<" ";
  else
    {
      cx<<lBulkOuter+10*number<<" "<<-(rBulkOuter+10*number)<<" ";
      cx<<((angle<0) ? -vDivide : vDivide)<<" ";
    }
  return cx.str();
}

std::pair<int,int>
shutterInfo::getShutterVertSurf() const 
  /*!
    Get the vertical surface for a shutter
    \return pair<top,base>
  */
{
  std::pair<int,int> Out(-1,-1);
  if (type==narrow || type==broad)
    {
      Out.first=tDShutter;
      Out.second=bDShutter;
    }
  else if (type==groove || type==hydrogen)
    {
      Out.first=tDShutter;
      Out.second=bDShutter;
    }
  else if (type==chipIR)
    {
      Out.first=tCIRShutter;
      Out.second=bCIRShutter;
    }
  return Out;
}

Triple<int>
shutterInfo::getShutterSides() const
  /*!
    Get the side + the divider
    \return left:right:divide
   */
{
  Triple<int> Out;
  Out.first=lShutter+10*number;
  Out.second=rShutter+10*number;
  Out.third=(angle<0) ? -vDivide : vDivide;
  return Out;
}

std::string
shutterInfo::innerSurface() const
  /*!
    Topredo sides are given as a string
    \return side string
  */
{
  std::ostringstream cx;
  if (angle>0 && angle<120.0*M_PI/180.0)
    cx<<rSide<<" ";
  else if (angle>=120.0*M_PI/180.0)
    cx<<"( "<<rSide<<": "<<-rWindow<<") ";
  else if (angle<0 && angle>-120*M_PI/180.0)
    cx<<-lSide<<" ";
  else if (angle<=-120*M_PI/180.0)
    cx<<"( "<<-lSide<<": "<<-lWindow<<") ";
  return cx.str();
}

std::string
shutterInfo::torpedoTops(const int complement) const
  /*!
    This creates the top/bottom item on the torpedo tubes.
    \param complement :: if true then make outside
    \return string for tops/base
  */
{
  std::ostringstream cx;
  if (!complement)
    {
      if (type==groove || type==hydrogen)
	cx<<-cTop<<" "<<cBase<<" ";
      else 
	cx<<-dTop<<" "<<dBase<<" ";
    }
  else
    {
      if (type==groove || type==hydrogen)
	cx<<cTop<<" : "<<-cBase<<" ";
      else
	cx<<dTop<<" : "<<-dBase<<" ";
    }
  return cx.str();
}

int 
shutterInfo::calcTallyPlanes(const int layerFlag,const int windowFlag,
			     std::vector<int>& Boundary) const
  /*!
    Calculate the boundary + the surface plaen for a point tally
    \param layerFlag :: Level of point tally edge plane
    \param windowFlag :: Level of point tally window plane
    \param Boundary :: Boundaries
    \return Master Plane
  */
{
  Boundary.clear();
  switch(layerFlag)
    {
    case moderator:
      if (type==broad || type==chipIR)             // Broad
        {
	  Boundary.push_back(168);
	  Boundary.push_back(169);
	  Boundary.push_back(173);
	  Boundary.push_back(108); 
	}
      else if (type==narrow)         // Narrow
        {
	  Boundary.push_back(173);
	  Boundary.push_back(108);
	  Boundary.push_back(170);
	  Boundary.push_back(171); 
	}
      else if (type==groove)        // Groove.
        {
	  Boundary.push_back(144);
	  Boundary.push_back(145);
	  Boundary.push_back(33003);
	  Boundary.push_back(33004); 
	}
      else if (type==hydrogen)        // Hydrogen
        {
	  Boundary.push_back(146);
	  Boundary.push_back(147);
	  Boundary.push_back(30007);
	  Boundary.push_back(30008); 
	}
      break;
    case reflector:
      if (type==broad)    // Wish
        {
	  Boundary.push_back(168);
	  Boundary.push_back(169);
	  Boundary.push_back(108);
	  Boundary.push_back(173);
	}
      else if (type==narrow)   // Narrow
        {
	  Boundary.push_back(170);
	  Boundary.push_back(171);
	  Boundary.push_back(108);
	  Boundary.push_back(173);
	}
      else if (type==hydrogen)  // hydrogen
        {
	  Boundary.push_back(146);
	  Boundary.push_back(147);
	  Boundary.push_back(30008);
	  Boundary.push_back(30007);
	}
      else // Methane groove
        {
	  Boundary.push_back(144);
	  Boundary.push_back(145);
	  Boundary.push_back(31004);
	  Boundary.push_back(31003);
	}
      break;
    case torpedo:
      Boundary.push_back(lTorpedo+10*number);
      Boundary.push_back(rTorpedo+10*number);
      if (type==narrow || type==broad || type==chipIR)
        {
	  Boundary.push_back(tDTorpedo);
	  Boundary.push_back(bDTorpedo);
	}
      else if (type==groove || type==hydrogen)
        {
	  Boundary.push_back(tCTorpedo);
	  Boundary.push_back(bCTorpedo);
	}
      break;
    case shutter:
      Boundary.push_back(lShutter+10*number);
      Boundary.push_back(rShutter+10*number);
      if (type==narrow || type==broad)
        {
	  Boundary.push_back(tDShutter);
	  Boundary.push_back(bDShutter);
	}
      else if (type==groove || type==hydrogen)
        {
	  Boundary.push_back(tCShutter);
	  Boundary.push_back(bCShutter);
	}
      else if (type==chipIR)
        {
	  Boundary.push_back(tCIRShutter);
	  Boundary.push_back(bCIRShutter);
	}	
	
      break;
    case inner:
      Boundary.push_back(lBulkInner+10*number);
      Boundary.push_back(rBulkInner+10*number);
      Boundary.push_back(tBulkInner);
      Boundary.push_back(bBulkInner);
      break;
    case outer:
      Boundary.push_back(lBulkOuter+10*number);
      Boundary.push_back(rBulkOuter+10*number);
      Boundary.push_back(tBulkOuter);
      Boundary.push_back(bBulkOuter);
      break;
    case bulk:
      Boundary.push_back(lBulkOuter+10*number);
      Boundary.push_back(rBulkOuter+10*number);
      Boundary.push_back(tBulkOuter);
      Boundary.push_back(bBulkOuter);
      break;
    default:
      ELog::EM<<"shutterInfo::calcTallySurf unknown layerFlag "
	      <<layerFlag<<ELog::endErr;
      exit(1);
    }
  // Default back plane:
  if (Boundary.size()<5)
    {
      if (type==groove || type==hydrogen)
	Boundary.push_back((angle<0) ? -32 : 32);
      else
	Boundary.push_back((angle<0) ? -10102 : 10102);
    }
  return getWindowSurf(windowFlag);
}

int
shutterInfo::getWindowSurf(const int windowFlag) const
  /*!
    Gets the window plane
    \param windowFlag :: Level of window
    \return viewed surface
  */
{
  ELog::RegMethod RegA("shutterInfo","getWindowSurf");

  switch(windowFlag)
    {
    case moderator:
      if (type==hydrogen) return 40003; // hydrogen
      if (type==narrow) return 132;
      if (type==groove) return  39003;  // Groove
      if (type==broad) return 133;
      break;
    case reflector:
      if (type==hydrogen) return 191; 
      if (type==groove) return 43002;
      if (type==broad) return 133;
      if (type==narrow) return 132;
      break;
    case torpedo:
      if (angle>M_PI*124.0/180.0) return 90028;
      if (angle<-M_PI*124.0/180.0)return 90027;
      return (angle<0) ? 90022 : 90023;
    case shutter:
      return 92001;
    case inner:
      return 92002;
    case outer:
      return 94001;
    case bulk:
      return 94002;
    default:
      ELog::EM<<"shutterInfo::getWindowSurf unknown windowFlag "
	      <<windowFlag<<ELog::endErr;
      return 0;
    }
  ELog::EM<<"shutterInfo::getWindowSurf unknown moderator type"
	  <<type<<ELog::endErr;
  return 0;
}


} // NAMESPACE shutterSystem
