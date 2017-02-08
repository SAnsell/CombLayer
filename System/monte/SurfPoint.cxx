/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/RuleItems.cxx
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Track.h"
#include "Surface.h"
#include "Rules.h"
#include "Token.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "Object.h"


// -------------------------------------------------------------
//         SURF KEYS
//---------------------------------------------------------------

SurfPoint::SurfPoint() : Rule(),
  key(0),keyN(0),sign(1) 
  /*!
    Constructor with null key/number
  */
{}

SurfPoint::SurfPoint(const Geometry::Surface* SPtr,
		     const int SN) :
  Rule(),
  key(SPtr),keyN(abs(SN)),sign((SN>=0) ? 1 : -1)
  /*!
    Constructor with surface / number
    \param SPtr :: Surface Pointer
    \param SN :: Surface number
  */
{}

SurfPoint::SurfPoint(const SurfPoint& A) : Rule(),
  key(A.key),keyN(A.keyN),sign(A.sign)
  /*!
    Copy constructor
    \param A ::SurfPoint to copy
  */
{}

SurfPoint*
SurfPoint::clone() const
  /*!
    Clone constructor
    \return new(*this)
  */
{
  return new SurfPoint(*this);
}


SurfPoint&
SurfPoint::operator=(const SurfPoint& A) 
  /*!
    Assigment operator
    \param A :: Object to copy
    \returns *this
  */
{
  if (&A!=this)
    {
      key=A.key;
      keyN=A.keyN;
      sign=A.sign;
    }
  return *this;
}

SurfPoint::~SurfPoint()
  /*!
    Destructor
  */
{}

void
SurfPoint::setLeaf(Rule* nR,const int)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that nR is of type SurfPoint
    \param nR :: new rule
    \param  :: ignored
  */
{
  SurfPoint* newX = dynamic_cast<SurfPoint*>(nR);
  if (newX)
    *this = *newX;
  return;
}

void
SurfPoint::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that nR is of type SurfPoint
    \param aR :: new rule
  */
{
  SurfPoint* newX = dynamic_cast<SurfPoint*>(aR);
  if (newX)
    *this = *newX;
  return;
}

int
SurfPoint::findLeaf(const Rule* A) const
  /*!
    Determines if this rule is a particular leaf value
    uses memory address to compare.
    \param A :: Rule to check
    \returns 0 if true and -1 if false.
  */
{
  return (this==A) ? 0 : -1;
}

Rule*
SurfPoint::findKey(const int KeyNum)
  /*!
    Finds the leaf with the surface number KeyN
    \param KeyNum :: Number to search for
    \retval 0 :: no leaf with that key number availiable
    \retval Rule* if an appropiate leaf is found
  */
{
  return (KeyNum==keyN) ? this : 0;
}


void
SurfPoint::setKeyN(const int Ky)
  /*!
    Sets the key and the sign 
    \param Ky :: key value (+/- is used for sign)
  */
{
  sign= (Ky<0) ? -1 : 1;
  keyN= sign*Ky;
  return;
}

void
SurfPoint::setKey(const Geometry::Surface* Spoint)
  /*!
    Sets the key pointter
    \param Spoint :: new key values
  */
{
  key=Spoint;
  return;
}

int
SurfPoint::simplify() 
  /*!
    Impossible to simplify a simple 
    rule leaf. Therefore returns 0
    \returns 0
  */
{
  return 0;
}

bool
SurfPoint::isDirectionValid(const Geometry::Vec3D& Pt,
			    const int ExSN) const
  /*! 
    Determines if a point  is valid. IF ExSN is +ve then 
    it is assumed that we are on the +ve side of the surf
    \param Pt :: Point to test
    \param ExSN :: Exclude surf number [signed]
    \retval 1 :: Pt is the +ve side of the 
    surface or on the surface
    \retval 0 :: Pt is on the -ve side of the surface
  */
{
  if (!key) return 0;
  if (abs(ExSN)==keyN)
      return ((sign*ExSN>0) ? 1 : 0);
  return (key->side(Pt)*sign)>=0 ? 1 : 0;
}

bool
SurfPoint::isValid(const Geometry::Vec3D& Pt,
		   const int ExSN) const
  /*! 
    Determines if a point  is valid.  
    \param Pt :: Point to test
    \param ExSN :: Exclude surf number 
    \retval 1 :: Pt is the +ve side of the 
    surface or on the surface
    \retval 0 :: Pt is on the -ve side of the surface
  */
{
  if (!key) return 0;
  if (abs(ExSN)==keyN) return 1;
  return (key->side(Pt)*sign)>=0 ? 1 : 0;
}

bool
SurfPoint::isValid(const Geometry::Vec3D& Pt,
		   const std::set<int>& ExSN) const
  /*! 
    Determines if a point  is valid.  
    \param Pt :: Point to test
    \param ExSN :: Exclude surf numbers 
    \retval 1 :: Pt is the +ve side of the 
    surface or on the surface
    \retval 0 :: Pt is on the -ve side of the surface
  */
{
  if (!key) return 0;
  if (ExSN.find(keyN)!=ExSN.end()) return 1;
  return (key->side(Pt)*sign)>=0 ? 1 : 0;
}

bool
SurfPoint::isValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  
    \param Pt :: Point to test
    \retval 1 :: Pt is the +ve side of the 
    surface or on the surface
    \retval 0 :: Pt is on the -ve side of the surface
  */
{
  if (key)
    return (key->side(Pt)*sign)>=0 ? 1 : 0;
  else
    return 0;
}

int
SurfPoint::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  
    \param SN :: Surface number
    \param Pt :: Point to test
    \return Surf false : true if Pt != SN [otherwize control]
  */
{
  if (!key) return 0;
  if (keyN==abs(SN)) 
    return (sign>0) ? 2 : 1;
  
  return (key->side(Pt)*sign)>=0 ? 3 : 0;
}

bool
SurfPoint::isValid(const std::map<int,int>& MX) const
  /*! 
    Use MX to determine if the surface truth etc is 
    valid. If second == 0 then always return true, else
    signed control
    \param MX :: map of key + logical value XOR sign

    \returns MX.second if key found or 0
  */
{
  std::map<int,int>::const_iterator lx=MX.find(keyN);
  return (lx->second*sign)>=0 ? 1 : 0;
}

void
SurfPoint::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  TVec.push_back(Token(sign*keyN));
}

std::string
SurfPoint::display() const
  /*!
    Returns the signed surface number as a string.
    \returns string of the value
  */
{
  std::stringstream cx;
  cx<<sign*keyN;
  return cx.str();
}

std::string
SurfPoint::displayFluka() const
  /*!
    Returns the signed surface number as
    a string.
    \returns string of the value
  */
{
  std::stringstream cx;
  if (sign>0)
    cx<<"-s"<<keyN;
  else
    cx<<"+s"<<keyN;

  return cx.str();
}

std::string
SurfPoint::displayPOVRay() const
  /*!
    Returns the signed surface number as
    a string.
    \returns string of the value
  */
{
  std::stringstream cx;
  cx<< "object {s" << keyN;
  if (sign>0)
    cx << " inverse";
  cx<<"}" << std::endl;

  return cx.str();
}

std::string
SurfPoint::display(const Geometry::Vec3D& Pt) const
  /*!
    Returns the signed surface number as
    a string.
    \param Pt :: Point to test validity
    \returns string of the value
  */
{
  std::stringstream cx;
  if (key)
    cx<<sign*keyN<<"["<<this->isValid(Pt)<<"]";
  else
    cx<<sign*keyN<<"[NK]";
  return cx.str();
}

std::string
SurfPoint::displayAddress() const
  /*!
    Returns the memory address as 
    a string.
    \returns memory address as string
  */
{
  std::stringstream cx;
  cx<<reinterpret_cast<long int>(this);
  return cx.str();
}
