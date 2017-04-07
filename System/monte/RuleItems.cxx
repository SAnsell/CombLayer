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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "Token.h"
#include "HeadRule.h"
#include "Object.h"


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


std::string
CompObj::displayFluka() const
  /*!
    Display the union in the form
    |N M| where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("CompObj","displayFluka");
  throw ColErr::AbsObjMethod("CompObj::displayFluka");
}

std::string
CompObj::displayPOVRay() const
  /*!
    Display the union in the POV-Ray form
    union{N M} where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("CompObj","displayPOVRay");
  throw ColErr::AbsObjMethod("CompObj::displayPOVRay");
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
  

std::string
BoolValue::displayFluka() const
  /*!
    Display the union in the form
    |N M| where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("BoolValue","displayFluka");
  throw ColErr::AbsObjMethod("BoolValue::displayFluka");
}

std::string
BoolValue::displayPOVRay() const
  /*!
    Display the union in the POVRay form
    union{N M} where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("BoolValue","displayPOVRay");
  throw ColErr::AbsObjMethod("BoolValue::displayPOVRay");
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


std::string
CompGrp::displayFluka() const
  /*!
    Display the union in the form
    |N M| where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("CompGrp::displayFluka");
}

std::string
CompGrp::displayPOVRay() const
  /*!
    Display the union in the POV-Ray form
    union{N M} where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("CompGrp::displayPOVRay");
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


std::string
ContGrp::displayFluka() const
  /*!
    Display the union in the form
    |N M| where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("ContGrp::displayFluka");
}

std::string
ContGrp::displayPOVRay() const
  /*!
    Display the union in the POV-Ray form
    union {N M} where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("ContGrp::displayPOVRay");
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

std::string
ContObj::displayFluka() const
  /*!
    Display the union in the form
    |N M| where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("ContObj::displayFluka");
}

std::string
ContObj::displayPOVRay() const
  /*!
    Display the union in the POV-Ray form
    union{N M} where N,M are the downward rules
    \returns bracket string 
  */
{
  ELog::RegMethod RegA("","");
  throw ColErr::AbsObjMethod("ContObj::displayPOVRay");
}
