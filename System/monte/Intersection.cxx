/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Intersection.cxx
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
#include "HeadRule.h"
#include "Object.h"

Intersection::Intersection() : Rule(),
  A(0),B(0)
  /*!
    Standard Constructor with null leaves
  */
{}

Intersection::Intersection(Rule* Ix,Rule* Iy) : Rule(),
  A(Iy),B(Ix)
  /*!
    Intersection constructor from two Rule ptrs.
    - Sets A,B's parents to *this
    - Allowed to control Ix and Iy
    \param Ix :: Rule A
    \param Iy :: Rule B 
  */
{ 
  if (A)
    A->setParent(this);
  if (B)
    B->setParent(this); 
}

Intersection::Intersection(Rule* Parent,Rule* Ix,Rule* Iy) : 
  Rule(Parent),A(Ix),B(Iy)    
  /*!
    Intersection constructor from two Rule ptrs 
    - Sets A,B's parents to *this.
    \param Parent :: Set the Rule::Parent pointer
    \param Ix :: Rule A
    \param Iy :: Rule B 
  */
{
  if (A)
    A->setParent(this);
  if (B)
    B->setParent(this);
}

Intersection::Intersection(const Intersection& Iother) : 
  Rule(),A(0),B(0)
  /*!
    Copy constructor:
    Does a clone on the sub-tree below
    \param Iother :: Intersection to copy
  */
{
  if (Iother.A)
    {
      A=Iother.A->clone();
      A->setParent(this);
    }
  if (Iother.B)
    {
      B=Iother.B->clone();
      B->setParent(this);
    }
}

Intersection&
Intersection::operator=(const Intersection& Iother) 
  /*!
    Assignment operator :: Does a deep copy
    of the leaves of Iother. 
    \param Iother :: object to copy
    \returns *this
  */
{
  if (this!=&Iother)
    {
      Rule::operator=(Iother);
      // first create new copy all fresh
      if (Iother.A)
        {
	  Rule* Xa=Iother.A->clone();
	  delete A;
	  A=Xa;
	  A->setParent(this);
	}
      if (Iother.B)
        {
	  Rule* Xb=Iother.B->clone();
	  delete B;
	  B=Xb;
	  B->setParent(this);
	}
    }
  return *this;
}

Intersection::~Intersection()
  /*!
    Destructor :: responsible for the two
    leaf intersections.
  */
{
  delete A;
  delete B;
}

Intersection*
Intersection::clone() const
  /*!
    Virtual copy constructor
    \returns new Intersection(this)
  */
{
  return new Intersection(*this);
}


int
Intersection::isComplementary() const 
  /*!
    Determine is the rule has complementary
    sub components
    \retval 1 :: A side
    \retval -1 :: B side
    \retval 0 :: no complement
  */
{
  if (A && A->isComplementary())
    return 1;
  if (B && B->isComplementary())
    return -1;

  return 0;
}

void
Intersection::setLeaves(Rule* aR,Rule* bR)
  /*!
    Replaces a both with a rule.
    No deletion is carried out but sets the parents.
    \param aR :: Rule on the left
    \param bR :: Rule on the right
  */
{
  A=aR;
  B=bR;
  if (A)
    A->setParent(this);
  if (B)
    B->setParent(this);
  return;
}

void
Intersection::setLeaf(Rule* nR,const int side)
  /*!
    Replaces a leaf with a rule.
    No deletion is carried out
    \param nR :: new rule
    \param side :: side to use 
    - 0 == LHS 
    - true == RHS
  */
{
  if (side)
    {
      B=nR;
      if (B)
	B->setParent(this);
    }
  else
    {
      A=nR;
      if (A)
	A->setParent(this);
    }
  return;
}

int 
Intersection::findLeaf(const Rule* R) const
  /*!
    Finds out if the Rule is the
    same as the leaves
    \param R :: Rule pointer to compare
    \retval 0 / 1 for LHS / RHS leaf 
    \retval -1 :: neither leaf
  */
{
  if (A==R)
    return 0;
  if (B==R)
    return 1;
  return -1;
}

Rule*
Intersection::findKey(const int KeyN)
  /*!
    Finds the leaf with the surface number KeyN
    \param KeyN :: Number to search for
    \retval 0 :: no leaf with that key number availiable
    \retval Rule* if an appropiate leaf is found
  */
{
  Rule* PtrOut=(A) ? A->findKey(KeyN) : 0;
  if (PtrOut)
    return PtrOut;
  return (B) ? B->findKey(KeyN) : 0;
}

void
Intersection::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::displayVec incomplete type");

  
  if (A->type()==-1)
    {
      TVec.push_back(Token('('));
      A->displayVec(TVec);
      TVec.push_back(Token(')'));
    }
  else
    A->displayVec(TVec);
  
  if (B->type()==-1)
    {
      TVec.push_back(Token('('));
      B->displayVec(TVec);
      TVec.push_back(Token(')'));
    }
  else
    B->displayVec(TVec);

  return;
}

std::string
Intersection::display(const Geometry::Vec3D& Pt) const
  /*!
    Displaces a bracket wrapped object
    \param Pt :: Test point to show true/false
    \return Bracketed string
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::display incomplete type");
  if (A->type()==-1)
    out="( "+A->display(Pt)+" )";
  else
    out=A->display(Pt);

  out+=" ";
  
  if (B->type()==-1)
    out+="( "+B->display(Pt)+" )";
  else
    out+=B->display(Pt);
  return out;
}

std::string
Intersection::display() const
  /*!
    Displaces a bracket wrapped object
    \return Bracketed string
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::display incomplete type");
  if (A->type()==-1)
    out="( "+A->display()+" )";
  else
    out=A->display();

  out+=" ";
  
  if (B->type()==-1)
    out+="( "+B->display()+" )";
  else
    out+=B->display();
  return out;
}

std::string
Intersection::displayFluka() const
  /*!
    Displays a bracket wrapped object
    \return Bracketed string
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::displayFluka incomplete type");
  if (A->type()==-1)
    out="+( "+A->displayFluka()+" )";
  else
    out=A->displayFluka();

  out+=" ";
  
  if (B->type()==-1)
    out+="+( "+B->displayFluka()+" )";
  else
    out+=B->displayFluka();
  return out;
}

std::string
Intersection::displayPOVRay() const
  /*!
    Displays a bracket wrapped object in the POV-Ray synax
    \return Bracketed string
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::displayPOVRay incomplete type");
  if (A->type()==-1)
    out="union{ "+A->displayPOVRay()+" }";
  else
    out=A->displayPOVRay();

  out+=" ";
  
  if (B->type()==-1)
    out+="union{ "+B->displayPOVRay()+" }";
  else
    out+=B->displayPOVRay();
  return out;
}

std::string
Intersection::displayAddress() const
  /*!
    Debug function that converts the 
    the intersection ion space delimited unit
    to denote intersection.
    \returns ( leaf leaf )
  */
{
  std::stringstream cx;
  cx<<" [ "<<reinterpret_cast<long>(this);
  if (A && B)
    cx<<" ] ("+A->displayAddress()+" "+B->displayAddress()+") ";
  else if (A)
    cx<<" ] ("+A->displayAddress()+" 0x0 ) ";
  else if (B)
    cx<<" ] ( 0x0 "+B->displayAddress()+") ";
  else
    cx<<" ] ( 0x0 0x0 ) ";
  return cx.str();
}

bool
Intersection::isValid(const Geometry::Vec3D& Vec) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \retval 1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  if (!A || !B)
    return 0;
  return (A->isValid(Vec) && B->isValid(Vec)) ? 1 : 0;
}

bool
Intersection::isDirectionValid(const Geometry::Vec3D& Vec,
			       const int ExSN) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \param ExSN :: Excluded surface number [signed]
    \retval 1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  if (!A || !B)
    return 0;
  return (A->isDirectionValid(Vec,ExSN) && 
	  B->isDirectionValid(Vec,ExSN)) ? 1 : 0;
}

bool
Intersection::isValid(const Geometry::Vec3D& Vec,
		      const int ExSN) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \param ExSN :: Excluded surface number
    \retval 1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  if (!A || !B)
    return 0;
  return (A->isValid(Vec,ExSN) && B->isValid(Vec,ExSN)) ? 1 : 0;
}

bool
Intersection::isValid(const Geometry::Vec3D& Vec,
		      const std::set<int>& ExSN) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \param ExSN :: Excluded surface numbers
    \retval 1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  if (!A || !B)
    return 0;
  return (A->isValid(Vec,ExSN) && B->isValid(Vec,ExSN)) ? 1 : 0;
}

int
Intersection::pairValid(const int SN,const Geometry::Vec3D& Vec) const
  /*!
    Calculates if Vec is within the object
    \param SN :: Surface Nubmer
    \param Vec :: Point to test
    \return  RuleA(ab) & RuleB(ab)
  */
{
  if (!A || !B) return 0;
  return (A->pairValid(SN,Vec) & B->pairValid(SN,Vec));
}

bool
Intersection::isValid(const std::map<int,int>& MX) const
  /*!
    Use MX to determine if the surface truth etc is 
    valid
    \param MX :: map of key + logical value XOR sign
    \retval 1 ::  Both sides are valid
    \retval 0 :: Either side is invalid.
  */
{
  if (!A || !B)
    return 0;
  return (A->isValid(MX) && B->isValid(MX)) ? 1 : 0;
}

int
Intersection::simplify() 
  /*!
    Union simplification:: 
      - -S S simplify to True.
      - S S simplify to S 
      - -S -S simplifies to -S
      \retval 1 if clauses removed (not top)
      \retval -1 replacement of this intersection is required
                  by leaf 0
      \retval 0 if no work to do.
  */
{
  return 0;
}
