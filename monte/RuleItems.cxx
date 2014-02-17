/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/RuleItems.cxx
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
    throw ColErr::ExBase(2,"Intersection::display incomplete type");

  
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

// -------------------------------------------------------------
//         UNION
//---------------------------------------------------------------

Union::Union() : 
  Rule(),A(0),B(0)
  /*!
    Standard Constructor with null leaves
  */
{}

Union::Union(Rule* Parent,Rule* Ix,Rule* Iy) : Rule(Parent),
  A(Ix),B(Iy)    
  /*!
    Union constructor from two Rule ptrs.
    - Sets A,B's parents to *this
    - Allowed to control Ix and Iy
    \param Parent :: Rule that is the parent to this 
    \param Ix :: Rule A
    \param Iy :: Rule B 
  */
{
  if (A)
    A->setParent(this);
  if (B)
    B->setParent(this);
}

Union::Union(Rule* Ix,Rule* Iy) : Rule(),
  A(Ix),B(Iy)
  /*!
    Union constructor from two Rule ptrs 
    - Sets A,B's parents to *this.
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

Union::Union(const Union& Iother) : Rule(Iother),
   A(0),B(0)
  /*!
    Copy constructor:
    Does a clone on the sub-tree below
    \param Iother :: Union to copy
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

Union&
Union::operator=(const Union& Iother) 
  /*!
    Assignment operator :: Does a deep copy
    of the leaves of Iother. 
    \param Iother :: Union to assign to it. 
    \returns this union (copy).
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

Union::~Union()
  /*!
    Delete operator : deletes both leaves
  */
{
  delete A;
  delete B;
}

Union*
Union::clone() const
  /*!
    Clone allows deep virtual coping
    \returns new Union copy.
  */
{
  return new Union(*this);
}

void
Union::setLeaf(Rule* nR,const int side)
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

void
Union::setLeaves(Rule* aR,Rule* bR)
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

int 
Union::findLeaf(const Rule* R) const
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
Union::findKey(const int KeyN)
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

int
Union::isComplementary() const 
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

int
Union::simplify() 
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
  if (!commonType())
    return 0;
  return 0;
}

int
Union::pairValid(const int SN,const Geometry::Vec3D& Vec) const
  /*!
    Calculates if Vec is within the object
    \param SN :: Surface number
    \param Vec :: Point to test
    \return RuleA(ab) | RuleB(ab) 
  */
{
  const int flagA=(A) ? A->pairValid(SN,Vec) : 0;
  const int flagB=(B) ? B->pairValid(SN,Vec) : 0;
  return flagA | flagB;
}

bool
Union::isValid(const Geometry::Vec3D& Vec,const int ExSN) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \param ExSN :: Excluded Surface
    \retval  1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  return ((A && A->isValid(Vec,ExSN)) ||
	  (B && B->isValid(Vec,ExSN))) ? 1 : 0;
}

bool
Union::isDirectionValid(const Geometry::Vec3D& Vec,
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
  return (A->isDirectionValid(Vec,ExSN) || 
	  B->isDirectionValid(Vec,ExSN)) ? 1 : 0;
}

bool
Union::isValid(const Geometry::Vec3D& Vec,
	       const std::set<int>& ExSN) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \param ExSN :: Excluded Surface
    \retval  1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  return ((A && A->isValid(Vec,ExSN)) ||
	  (B && B->isValid(Vec,ExSN))) ? 1 : 0;
}

bool
Union::isValid(const Geometry::Vec3D& Vec) const
  /*!
    Calculates if Vec is within the object
    \param Vec :: Point to test
    \retval  1 ::  Vec is within object 
    \retval 0 :: Vec is outside object.
  */
{
  return ((A && A->isValid(Vec)) ||
	  (B && B->isValid(Vec))) ? 1 : 0;
}

bool
Union::isValid(const std::map<int,int>& MX) const
  /*!
    Use MX to determine if the surface truth etc is 
    valie
    \param MX :: map of key + logical value XOR sign
    \retval 1 :: if either side is valid
    \retval 0 :: Neither side is valid
  */
{
  return ((A && A->isValid(MX)) ||
	  (B && B->isValid(MX))) ? 1 : 0;
}

void
Union::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::displayVec incomplete type");

  if (A->type()==1)
    {
      TVec.push_back(Token('('));
      A->displayVec(TVec);
      TVec.push_back(Token(')'));
    }
  else
    A->displayVec(TVec);
  
  TVec.push_back(Token(':'));

  if (B->type()==1)
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
Union::display() const
  /*!
    Display the union in the form
    (N:M) where N,M are the downward
    rules
    \returns bracket string 
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::display incomplete type");
  if (A->type()==1)
    out="( "+A->display()+" )";
  else
    out=A->display();

  out+=" : ";
  
  if (B->type()==1)
    out+="( "+B->display()+" )";
  else
    out+=B->display();

  return out;
}
std::string
Union::display(const Geometry::Vec3D& Pt) const
  /*!
    Display the union in the form
    (N:M) where N,M are the downward
    rules
    \param Pt :: Test point to show true/false
    \returns bracket string 
  */
{
  std::string out;
  if (!A || !B)
    throw ColErr::ExBase(2,"Intersection::display incomplete type");
  if (A->type()==1)
    out="( "+A->display(Pt)+" )";
  else
    out=A->display(Pt);

  out+=" : ";
  
  if (B->type()==1)
    out+="( "+B->display(Pt)+" )";
  else
    out+=B->display(Pt);

  return out;
}

std::string
Union::displayAddress() const
  /*!
    Returns the memory address as 
    a string. Displays addresses of leaves
    \returns String of address
  */
{
  std::stringstream cx;
  cx<<" [ "<<reinterpret_cast<long int>(this);

  if (A && B)
    cx<<" ] ("+A->displayAddress()+" : "+B->displayAddress()+") ";
  else if (A)
    cx<<" ] ("+A->displayAddress()+" : 0x0 ) ";
  else if (B)
    cx<<" ] ( 0x0 : "+B->displayAddress()+") ";
  else
    cx<<" ] ( 0x0 : 0x0 ) ";
  return cx.str();
}


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
    \param SN :: Exclude surf number 
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
    \param SN :: Exclude surf numbers 
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
    Returns the signed surface number as
    a string.
    \returns string of the value
  */
{
  std::stringstream cx;
  cx<<sign*keyN;
  return cx.str();
}

std::string
SurfPoint::display(const Geometry::Vec3D& Pt) const
  /*!
    Returns the signed surface number as
    a string.
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

//----------------------------------------
//       COMPOBJ
//----------------------------------------

CompObj::CompObj() : Rule(),
  objN(0),key(0)
  /*!
    Constructor
  */
{}

CompObj::CompObj(const CompObj& A) : 
  Rule(A),
  objN(A.objN),key(A.key)
  /*!
    Standard copy constructor
    \param A :: CompObj to copy
   */
{}

CompObj&
CompObj::operator=(const CompObj& A)
  /*!
    Standard assignment operator
    \param A :: CompObj to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Rule::operator=(A);
      objN=A.objN;
      key=A.key;
    }
  return *this;
}

CompObj::~CompObj()
  /*!
    Destructor
  */
{}


CompObj*
CompObj::clone() const
  /*!
    Clone of this
    \return new copy of this
  */
{
  return new CompObj(*this);
}

void
CompObj::setObjN(const int Ky)
  /*!
    Sets the object Number
    \param Ky :: key value 
  */
{
  objN= Ky;
  return;
}

void
CompObj::setLeaf(Rule* aR,const int)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type SurfPoint
    \param aR :: new rule
    \param  :: Null side point
  */
{
  CompObj* newX = dynamic_cast<CompObj*>(aR);
  // Make a copy
  if (newX)
    *this = *newX;
  return;
}

void
CompObj::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type CompObj
    \param aR :: new rule
    \param :: [PlaceHolder] Null other rule
  */
{
  CompObj* newX = dynamic_cast<CompObj*>(aR);
  if (newX)
    *this = *newX;
  return;
}

Rule*
CompObj::findKey(const int)
  /*!
    This is a complementary object and we dont
    search into CompObjs. If that is needed
    then the CompObj should be removed first
    \param :: [PlaceHolder] Null index key
    \return 0
  */
{
  return 0;
}

int
CompObj::findLeaf(const Rule* A) const
  /*!
    Check to see if this is a copy of a given Rule
    \param A :: Rule Ptr to find
    \retval 0 on success -ve on failuire
  */
{
  return (this==A) ? 0 : -1;
}

bool
CompObj::isValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  Pt :: Point to test
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  if (key)
    return (key->isValid(Pt)) ? 0 : 1;
  return 1;
}


bool
CompObj::isValid(const Geometry::Vec3D& Pt,const int ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  Pt :: Point to test
    \param  ExSN :: Excluded points
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  if (key)
    return (key->isValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

bool
CompObj::isDirectionValid(const Geometry::Vec3D& Pt,
			  const int ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  Pt :: Point to test
    \param  ExSN :: Excluded points
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  if (key)
    return (key->isDirectionValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

bool
CompObj::isValid(const Geometry::Vec3D& Pt,
		 const std::set<int>& ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  Pt :: Point to test
    \param  ExSN :: Excluded points
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  if (key)
    return (key->isValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

int
CompObj::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  with the surface
    SN false/true
    \param SN :: Surface number
    \param Pt :: Point to test
    \return valid(SN->false) : valid(SN->true);
  */
{
  if (key)
    {
      const int Aout=key->pairValid(SN,Pt);
      // now make opposite [bitwize not & 3]
      return (~Aout) & 3;
    }
  return 3;
}

bool
CompObj::isValid(const std::map<int,int>& SMap) const
  /*! 
    Determines if a point  is valid.  
    \param  SMap :: map of surface number and true values
    \returns :: status
  */
{
  return (key) ? !(key->isValid(SMap)) : 1;
}

int
CompObj::simplify() 
  /*!
    Impossible to simplify a simple 
    rule leaf. Therefore returns 0
    \returns 0
  */
{
  return 0;
}

void
CompObj::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  TVec.push_back(Token(2,'#',objN));
  return;
}

std::string
CompObj::display() const
  /*!
    Displays the object as \#number
    \returns string component
  */
{
  std::stringstream cx;
  cx<<"#"<<objN;
  return cx.str();
}

std::string
CompObj::display(const Geometry::Vec3D& Pt) const
  /*!
    Displays the object as \#number
    \param Pt :: Test point to show true/false
    \returns string component
  */
{
  std::stringstream cx;
  cx<<"#"<<objN<<"["<<key->isValid(Pt)<<"]";
  return cx.str();
}

std::string
CompObj::displayAddress() const
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

// -----------------------------------------------
// BOOLVALUE
// -----------------------------------------------


BoolValue::BoolValue(const int S) : 
  Rule(),status(BoolValue::procStatus(S))
  /*!
    Constructor
  */
{}

BoolValue::BoolValue(const BoolValue& A) : 
  Rule(A),
  status(A.status)
  /*!
    Copy Constructor 
    \param A :: BoolValue to copy
  */
{}

BoolValue&
BoolValue::operator=(const BoolValue& A)
  /*!
    Assignment operator 
    \param A :: BoolValue to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Rule::operator=(A);
      status=A.status;
    }
  return *this;
}

BoolValue*
BoolValue::clone() const
  /*!
    Clone constructor
    \return new(*this)
   */
{
  return new BoolValue(*this);
}

BoolValue::~BoolValue()
  /*!
    Destructor
  */
{}


int
BoolValue::procStatus(const int S)
  /*!
    Proc the status
    \param S :: Value
    \return +/- or 0
   */
{
  if (S)
    return (S>0) ? 1 : -1;
  return 0;
}
  


void
BoolValue::setLeaf(Rule* aR,const int)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type SurfPoint
    \param aR :: new rule
    \param  :: Null side point
  */
{
  BoolValue* newX = dynamic_cast<BoolValue*>(aR);
  if (newX)
    *this = *newX;
  return;
}

void
BoolValue::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type SurfPoint
    \param aR :: new rule
    \param :: Null other rule
  */
{
  BoolValue* newX = dynamic_cast<BoolValue*>(aR);
  if (newX)
    *this = *newX;
  return;
}

int
BoolValue::findLeaf(const Rule* A) const
  /*!
    Find a leaf
    \param A :: rule to check
    \return 0 if first leaf / -1 if failed
  */
{
  return (this==A) ? 0 : -1;
}

bool
BoolValue::isValid(const Geometry::Vec3D&,const int) const
  /*! 
    Determines if a point  is valid.  
    \param  :: Point to test
    \returns status
  */
{
  return status;
}

bool
BoolValue::isDirectionValid(const Geometry::Vec3D&,const int) const
  /*! 
    Determines if a point  is valid.  
    \param  :: Point to test
    \returns status
  */
{
  return status;
}

bool
BoolValue::isValid(const Geometry::Vec3D&,const std::set<int>&) const
  /*! 
    Determines if a point  is valid.  
    \param  :: Point to test
    \returns status
  */
{
  return status;
}

bool
BoolValue::isValid(const Geometry::Vec3D&) const
  /*! 
    Determines if a point  is valid.  
    \param  :: Point to test
    \returns status
  */
{
  return status;
}

int
BoolValue::pairValid(const int,const Geometry::Vec3D&) const
  /*! 
    Determines if a point  is valid.  with the surface
    SN false/true
    \param :: [PlaceHolder] Surface number
    \param :: Point to test
    \return valid(SN->false) : valid(SN->true);
  */
{
  return (status) ? 3 : 0;
}

bool
BoolValue::isValid(const std::map<int,int>&) const
  /*! 
    Determines if a point  is valid.  
    \param  :: map of surface number and true values
    \returns :: status
  */
{
  return status;
}

int
BoolValue::simplify()
  /*!
    Bool value is always in simplest form
    \return 0 
  */
{
  return 0;
}

void
BoolValue::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  TVec.push_back(Token(2,0,status));
  return;
}

std::string
BoolValue::display() const
  /*!
    \returns string of 
     - "true" "false" or "unknown"
  */

{
  switch(status) 
    {
    case 1:
      return " True ";
    case -1:
      return " False ";
    }
  return " Unkown ";
}

std::string
BoolValue::display(const Geometry::Vec3D&) const
  /*!
    \returns string of 
     - "true" "false" or "unknown"
  */

{
  switch(status) 
    {
    case 1:
      return " True ";
    case -1:
      return " False ";
    }
  return " Unkown ";
}


std::string
BoolValue::displayAddress() const
  /*!
    Returns the memory address as 
    a string.
    \returns string of Address
  */
{
  std::stringstream cx;
  cx<<reinterpret_cast<long int>(this);
  return cx.str();
}
  

//----------------------------------------
//       COMPGRP
//----------------------------------------

CompGrp::CompGrp() : 
  Rule(),A(0)
  /*!
    Constructor
  */
{}

CompGrp::CompGrp(Rule* Parent,Rule* Cx) :
  Rule(Parent),A(Cx)
  /*!
    Constructor to build parent and complent tree
    \param Parent :: Rule that is the parent to this
    \param Cx :: Complementary object below
  */
{
  if (Cx)
    Cx->setParent(this);
}

CompGrp::CompGrp(const CompGrp& Cother) : 
  Rule(Cother),A(0)
  /*!
    Standard copy constructor
    \param Cother :: CompGrp to copy
   */
{
  if (Cother.A)
    {
      A=Cother.A->clone();
      A->setParent(this);
    }
}

CompGrp&
CompGrp::operator=(const CompGrp& Cother)
  /*!
    Standard assignment operator
    \param Cother :: CompGrp to copy
    \return *this
   */
{
  if (this!=&Cother)
    {
      Rule::operator=(Cother);
      if (Cother.A)
        {
	  Rule* Xa=Cother.A->clone();
	  delete A;
	  A=Xa;
	  A->setParent(this);
	}
    }
  return *this;
}

CompGrp::~CompGrp()
  /*!
    Destructor
  */
{
  delete A;
}


CompGrp*
CompGrp::clone() const
  /*!
    Clone of this
    \return new copy of this
  */
{
  return new CompGrp(*this);
}

void
CompGrp::setLeaf(Rule* aR,const int)
  /*!
    Replaces a leaf with a rule.
    No deletion is carried out
    \param aR :: new rule
    \param  :: [Placeholder] side to use 
  */
{
  A=aR;
  if (A)
    A->setParent(this);
  return;
}

void
CompGrp::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    No deletion is carried out but sets the parents.
    \param aR :: new rule
    \param :: Null other rule
  */
{
  A=aR;
  if (A)
    A->setParent(this);
  return;
}

Rule*
CompGrp::findKey(const int)
  /*!
    This is a complementary object and we dont
    search into CompGrps. If that is needed
    then the CompGrp should be removed first
    \param  :: [PlaceHolder] Null index key
    \return 0
  */
{
  return 0;
}

int
CompGrp::findLeaf(const Rule* R) const
  /*!
    Check to see if this is a copy of a given Rule
    \param R :: Rule Ptr to find
    \retval 0 on success -ve on failuire
  */
{
  return (A==R) ? 0 : -1;
}

bool
CompGrp::isValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    Note the complementary reverse in the test.
    \param  Pt :: Point to test
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt)) ? 0 : 1;
  return 1;
}

bool
CompGrp::isValid(const Geometry::Vec3D& Pt,const int ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    Note the complementary reverse in the test.
    \param  Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

bool
CompGrp::isDirectionValid(const Geometry::Vec3D& Pt,
			  const int ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    Note the complementary reverse in the test.
    \param  Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isDirectionValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

bool
CompGrp::isValid(const Geometry::Vec3D& Pt,
		 const std::set<int>& ExSN) const
  /*! 
    Determines if a point  is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    Note the complementary reverse in the test.
    \param  Pt :: Point to test
    \param ExSN :: Excluded surfaces
    \retval not valid in the object 
    \retval true is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt,ExSN)) ? 0 : 1;
  return 1;
}

int
CompGrp::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  with the surface
    SN false/true
    \param SN :: Surface number
    \param Pt :: Point to test
    \return valid(SN->false) : valid(SN->true);
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    {
      const int Aout=A->pairValid(SN,Pt);
      // now make opposite [bitwize not & 3]
      return (~Aout) & 3;
    }
  return 3;
}

bool
CompGrp::isValid(const std::map<int,int>& SMap) const
  /*! 
    Determines if a point  is valid.  
    \param  SMap :: map of surface number and true values
    \returns :: status
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(SMap)) ? 0 : 1;
  return 1;
}

int
CompGrp::simplify() 
  /*!
    Impossible to simplify a simple 
    rule leaf. Therefore returns 0
    \returns 0
  */
{
  return 0;
}

void
CompGrp::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  if (A)
    {
      TVec.push_back(Token(2,'#',-1));
      TVec.push_back(Token(2,'(',0));
      A->displayVec(TVec);
      TVec.push_back(Token(2,')',0));
    }
  return;
}

std::string
CompGrp::display() const
  /*!
    Displays the object as \#( object )
    \returns string component
  */
{
  std::stringstream cx;
  if (A)
    cx<<"#( "<< A->display()<<" )";
  return cx.str();
}

std::string
CompGrp::display(const Geometry::Vec3D& Pt) const
  /*!
    Displays the object as \#( object )
    \param Pt :: Test point to show true/false
    \returns string component
  */
{
  std::stringstream cx;
  if (A)
    cx<<"#( "<< A->display(Pt)<<" )";
  return cx.str();
}

std::string
CompGrp::displayAddress() const
  /*!
    Returns the memory address as 
    a string.
    \returns memory address as string
  */
{
  std::stringstream cx;
  cx<<"#( ["<<reinterpret_cast<long int>(this) <<"] ";
  if (A)
    cx<<A->displayAddress();
  else
    cx<<"0x0";
  cx<<" ) ";
  return cx.str();
}


//----------------------------------------
//       CONTGRP
//----------------------------------------

ContGrp::ContGrp() : 
  Rule(),A(0)
  /*!
    Constructor
  */
{}

ContGrp::ContGrp(Rule* Parent,Rule* Cx) :
  Rule(Parent),A(Cx)
  /*!
    Constructor to build parent and container tree
    \param Parent :: Rule that is the parent to this
    \param Cx :: Container object below
  */
{
  if (Cx)
    Cx->setParent(this);
}

ContGrp::ContGrp(const ContGrp& Cother) : 
  Rule(Cother),A(0)
  /*!
    Standard copy constructor
    \param Cother :: ContGrp to copy
   */
{
  if (Cother.A)
    {
      A=Cother.A->clone();
      A->setParent(this);
    }
}

ContGrp&
ContGrp::operator=(const ContGrp& Cother)
  /*!
    Standard assignment operator
    \param Cother :: ContGrp to copy
    \return *this
   */
{
  if (this!=&Cother)
    {
      Rule::operator=(Cother);
      if (Cother.A)
        {
	  Rule* Xa=Cother.A->clone();
	  delete A;
	  A=Xa;
	  A->setParent(this);
	}
    }
  return *this;
}

ContGrp::~ContGrp()
  /*!
    Destructor
  */
{
  delete A;
}


ContGrp*
ContGrp::clone() const
  /*!
    Clone of this
    \return new copy of this
  */
{
  return new ContGrp(*this);
}

void
ContGrp::setLeaf(Rule* aR,const int)
  /*!
    Replaces a leaf with a rule.
    No deletion is carried out
    \param aR :: new rule
    \param  :: [placeholder] side to use 
  */
{
  A=aR;
  if (A)
    A->setParent(this);
  return;
}

void
ContGrp::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    No deletion is carried out but sets the parents.
    \param aR :: new rule
    \param :: Null other rule
  */
{
  A=aR;
  if (A)
    A->setParent(this);
  return;
}

Rule*
ContGrp::findKey(const int)
  /*!
    This is a container object and we dont
    search into ContGrps. If that is needed
    then the ContGrp should be removed first
    \param  :: [placeholder] Null index key
    \return 0
  */
{
  return 0;
}

int
ContGrp::findLeaf(const Rule* R) const
  /*!
    Check to see if this is a copy of a given Rule
    \param R :: Rule Ptr to find
    \retval 0 on success -ve on failuire
  */
{
  return (A==R) ? 0 : -1;
}

bool
ContGrp::isValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \retval Valid in the object 
    \retval False is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt)) ? 1 : 0;
  return 0;
}


bool
ContGrp::isValid(const Geometry::Vec3D& Pt,const int ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval Valid in the object 
    \retval False is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

bool
ContGrp::isDirectionValid(const Geometry::Vec3D& Pt,
			  const int ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval Valid in the object 
    \retval False is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isDirectionValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

bool
ContGrp::isValid(const Geometry::Vec3D& Pt,
		 const std::set<int>& ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surfaces
    \retval Valid in the object 
    \retval False is no object is set
  */
{
  // Note:: if isValid is true then return 0:
  if (A)
    return (A->isValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

int
ContGrp::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point  is valid.  with the surface
    SN false/true
    \param SN :: Surface number
    \param Pt :: Point to test
    \return valid(SN->false) : valid(SN->true);
  */
{
  // Note:: if isValid is true then return 0:
  return (A) ? A->pairValid(SN,Pt) : 0;
}

bool
ContGrp::isValid(const std::map<int,int>& SMap) const
  /*! 
    Determines if a point  is valid.  
    \param  SMap :: map of surface number and true values
    \returns :: status
  */
{
  // Note:: if isValid is true then return 1:
  if (A)
    return (A->isValid(SMap)) ? 1 : 0;
  return 1;
}

int
ContGrp::simplify() 
  /*!
    Impossible to simplify a simple 
    rule leaf. Therefore returns 0
    \returns 0
  */
{
  return 0;
}

void
ContGrp::displayVec(std::vector<Token>& TVec) const
  /*!
    Tokenizes the rule
    \param TVec :: Token to copy
  */
{
  if (A)
    A->displayVec(TVec);
  return;
}

std::string
ContGrp::display() const
  /*!
    Displays the object as \#number
    \returns string component
  */
{
  return (A) ? A->display() : "";
}

std::string
ContGrp::display(const Geometry::Vec3D& Pt) const
  /*!
    Displays the object as \#number
    \param Pt :: Test point to show true/false
    \returns string component
  */
{
  return (A) ? A->display(Pt) : "";
}

std::string
ContGrp::displayAddress() const
  /*!
    Returns the memory address as 
    a string.
    \returns memory address as string
  */
{
  std::stringstream cx;
  cx<<"%( ["<<reinterpret_cast<long int>(this) <<"] ";
  if (A)
    cx<<A->displayAddress();
  else
    cx<<"0x0";
  cx<<" ) ";
  return cx.str();
}

//----------------------------------------
//       CONTOBJ
//----------------------------------------

ContObj::ContObj() : Rule(),
  objN(0),key(0)
  /*!
    Constructor
  */
{}

ContObj::ContObj(const ContObj& A) : 
  Rule(A),
  objN(A.objN),key(A.key)
  /*!
    Standard copy constructor
    \param A :: ContObj to copy
   */
{}

ContObj&
ContObj::operator=(const ContObj& A)
  /*!
    Standard assignment operator
    \param A :: ContObj to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Rule::operator=(A);
      objN=A.objN;
      key=A.key;
    }
  return *this;
}

ContObj::~ContObj()
  /*!
    Destructor
  */
{}


ContObj*
ContObj::clone() const
  /*!
    Clone of this
    \return new copy of this
  */
{
  return new ContObj(*this);
}

void
ContObj::setObjN(const int Ky)
  /*!
    Sets the object Number
    \param Ky :: key value 
  */
{
  objN= Ky;
  return;
}

void
ContObj::setLeaf(Rule* aR,const int)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type SurfPoint
    \param aR :: new rule
    \param :: Null side point
  */
{
  ContObj* newX = dynamic_cast<ContObj*>(aR);
  // Make a copy
  if (newX)
    *this = *newX;
  return;
}

void
ContObj::setLeaves(Rule* aR,Rule*)
  /*!
    Replaces a leaf with a rule.
    This REQUIRES that aR is of type ContObj
    \param aR :: new rule
    \param :: Null other rule
  */
{
  ContObj* newX = dynamic_cast<ContObj*>(aR);
  if (newX)
    *this = *newX;
  return;
}

Rule*
ContObj::findKey(const int)
  /*!
    This is a complementary object and we dont
    search into ContObjs. If that is needed
    then the ContObj should be removed first
    \param  :: Null index key
    \return 0
  */
{
  return 0;
}

int
ContObj::findLeaf(const Rule* A) const
  /*!
    Check to see if this is a copy of a given Rule
    \param A :: Rule Ptr to find
    \retval 0 on success -ve on failuire
  */
{
  return (this==A) ? 0 : -1;
}

int
ContObj::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  SN :: Surface number to iterater over
    \param  Pt :: Point to test
    \retval not valid in the object 
    \retval false if no object is set
  */
{
  return (key) ? key->pairValid(SN,Pt) : 0;
}

bool
ContObj::isValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param  Pt :: Point to test
    \retval not valid in the object 
    \retval false if no object is set
  */
{
  if (key)
    return (key->isValid(Pt)) ? 1 : 0;
  return 0;
}

bool
ContObj::isValid(const Geometry::Vec3D& Pt,const int ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval not valid in the object 
    \retval false if no object is set
  */
{
  if (key)
    return (key->isValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

bool
ContObj::isDirectionValid(const Geometry::Vec3D& Pt,
			  const int ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval not valid in the object 
    \retval false if no object is set
  */
{
  if (key)
    return (key->isDirectionValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

bool
ContObj::isValid(const Geometry::Vec3D& Pt,
		 const std::set<int>& ExSN) const
  /*! 
    Determines if a point is valid.  
    Checks to see if the point is valid in the object
    and returns ture if it is not valid.
    \param Pt :: Point to test
    \param ExSN :: Excluded surface
    \retval not valid in the object 
    \retval false if no object is set
  */
{
  if (key)
    return (key->isValid(Pt,ExSN)) ? 1 : 0;
  return 0;
}

bool
ContObj::isValid(const std::map<int,int>& SMap) const
  /*! 
    Determines if a point  is valid.  
    \param  SMap :: map of surface number and true values
    \returns :: status
  */
{
  return (key) ? (key->isValid(SMap)) : 0;
}

int
ContObj::simplify() 
  /*!
    Impossible to simplify a simple 
    rule leaf. Therefore returns 0
    \returns 0
  */
{
  return 0;
}

void
ContObj::displayVec(std::vector<Token>&) const
  /*!
    Tokenizes the rule
    \param  :: Token to copy
  */
{
  ELog::RegMethod RegA("ContObj","displayVec");
  ELog::EM<<"Calling uncompleted code"<<ELog::endErr;
  return;
}

std::string
ContObj::display() const
  /*!
    Displays the object as \%number
    \returns string component
  */
{
  std::stringstream cx;
  cx<<"%"<<objN;
  return cx.str();
}

std::string
ContObj::display(const Geometry::Vec3D& Pt) const
  /*!
    Displays the object as \%number
    \param Pt :: Test point to show true/false
    \returns string component
  */
{
  std::stringstream cx;
  cx<<"%"<<objN<<"["<<key->isValid(Pt)<<"]";
  return cx.str();
}

std::string
ContObj::displayAddress() const
  /*!
    Returns the memory address as 
    a string.
    \returns memory address as string
  */
{
  std::stringstream cx;
  cx<<reinterpret_cast<unsigned long int>(this);
  return cx.str();
}


/// \Cond TEMPLATE
template class DTriple<Rule*,int,Rule*>;
/// \Endcond TEMPLATE

