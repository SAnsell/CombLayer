/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Acomp.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <climits>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "MapSupport.h"
#include "RotCounter.h"
#include "BnId.h"
#include "AcompTools.h"
#include "Acomp.h"

namespace MonteCarlo 
{

std::ostream&
operator<<(std::ostream& OX,const Acomp& A) 
  /*!
    Stream operator (uses Acomp::write)
    \param OX :: output stream
    \param A :: Acomp unit to output
    \returns Output stream
  */
   
{
  OX<<A.display();
  return OX;
}

//
// ------------- ACOMP ---------------- 
//

Acomp::Acomp(const JoinForm Tx) :
  trueFlag(0),Intersect((Tx==Union) ? 0 : 1)
  /*!
    Standard Constructor 
    \param Tx :: 0 it means intersect 1 it means union
  */
{}

Acomp::Acomp(const int Tx) :
  trueFlag(0),Intersect(Tx ? 1 : 0)
  /*!
    Standard Constructor 
    \param Tx :: 0 intersect / 1 union
  */
{}

Acomp::Acomp(const Acomp& A) :
  trueFlag(A.trueFlag),Intersect(A.Intersect),
  Units(A.Units),Comp(A.Comp)
  /*!
    Copy constructor
    \param A :: Acomp object to copy
  */
{ }

Acomp&
Acomp::operator=(const Acomp& A)
  /*!
    The assignment operator carries out a 
    down level deletion of the Comp vector
    \param A :: Object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      trueFlag=A.trueFlag;
      Intersect=A.Intersect;
      Units=A.Units;
      Comp=A.Comp;
    }
  return *this;
}


Acomp::~Acomp()
  /*!
    Destructor deletes each Comp Unit and
    down the chain.
  */
{ }

int
Acomp::operator!=(const Acomp& A) const
  /*!
    Inequality operator
    \param A :: other Acomp item to compare
    \returns Not this==A
  */
{
  return 1-(this->operator==(A));
}

int
Acomp::operator==(const Acomp& A) const
  /*!
    Equals operator requires that the
    Units are equal and the Comp units
    are equal. Sort of the class is required.
    \param A :: other Acomp item to compare
    \returns A==*this
  */
{
  if (&A==this) return 1;

  // Size not equal then definately not equal
  if (A.Units.size()!=Units.size() ||
      A.Comp.size()!=Comp.size() )
    return 0;

  // Intersect not the same (unless size==1)
  if (A.Intersect!=Intersect && 
      Units.size()+Comp.size()!=1)          // Intersect type is not relevent 
    return 0;                               // if singular.

  // If Units not empty compare each and determine
  // if equal.
  if (Units!=A.Units) return 0;

  // Both Empty :: thus equal
  if (Comp.empty())
    return A.trueFlag==trueFlag;

  // Assume that comp Units are sorted.
  std::vector<Acomp>::const_iterator acv,xcv;
  acv=A.Comp.begin();
  for(xcv=Comp.begin();xcv!=Comp.end() &&
	*xcv==*acv ;xcv++,acv++) ;
  return (xcv==Comp.end()) ? 1 : 0;
}
  
int
Acomp::operator<(const Acomp& A) const
  /*!
    Comparitor operator:: Comparies the 
    unit list (which is already sorted)
    part by part.  The sort is ASSUMED.
    Then the Comp units (which are not)
    Order (low first)
      - Singulars
      - Intersections
      - Units (largest negative)
      - Acomps.
      \param A :: Object to compare
      \returns this<A
  */
{
  // PROCESS Singular
  const int TS=isSingle();         // is this one component
  const int AS=A.isSingle(); 
  if (TS!=AS)                     
    return (TS>AS) ? 1 : 0;
  // PROCESS Intersection/Union
  if (!TS && Intersect!=A.Intersect)
    return Intersect;                  // Union==0 therefore this > A 
  
  // PROCESS Units. (order : then size)

  if (Units!=A.Units)
    return Units<A.Units;
  

  // PROCESS CompUnits.  
  std::vector<Acomp>::const_iterator ux,ax;
  ux=Comp.begin();
  ax=A.Comp.begin();
  for(;ux!=Comp.end() && ax!=A.Comp.end();ux++,ax++)
    {
      if (*ax!=*ux)
	return (*ux < *ax);
    }
  if (ux!=Comp.end())
    return 1;
  // everything idential or A.comp bigger:
  return 0;
}

Acomp&
Acomp::operator+=(const Acomp& A)
  /*!
    Operator + (union addition)
    \param A :: Object to union/or/. with this
    \returns *this
  */
{
  if (Intersect)                       // If this is an intersection
    {                                  // we need to have a union 
      Acomp Ax=Acomp(*this);           //make copy
      Units.clear();                   // remove everthing else
      Comp.clear();
      Intersect=0;                      // make this into a Union
      addComp(Ax);                     // add oldThis to the list
    }
  // Now add A to the union.
  if (!A.Intersect)                 // Can add components
    copySimilar(A);
  else 
    addComp(A);       // add components

  removeEqComp();     // remove equal units
  joinDepth();        // Up shift suitable objects.
  return *this;
}

Acomp&
Acomp::operator-=(const Acomp& A)
  /*!
    Operator - (removal)
    by complementary subtraction or the remainder.
    Complementary subtraction: is by making A-B == A*(B')
    -- Note : (a+b)-b => 0
    
    \param A :: Object to subtract
    \return *this 
  */
{
  ELog::RegMethod RegA("Acomp","operator-=");
  
  Acomp Aprime(A);
  Aprime.complement();
  return Acomp::operator*=(Aprime);
}

int
Acomp::operator>(const Acomp& A) const
  /*! 
    Operator> takes first to last precidence.
    Uses operator<  to obtain value.
    Note it does not uses 1-(A<this)
    \param  A :: object to compare
    \returns this>A
  */
{
  return A.operator<(*this);
}


Acomp&
Acomp::operator*=(const Acomp& A)
  /*!
    This carries out the intersection operation 
    with A. e.g. (a+b) * (ced) == (a+b)ced.
    The equivilent to making the statement [this AND A]
    \param A :: Acomp unit to intersect
    \returns *this
  */
{
  ELog::RegMethod RegA("Acomp","operator*");
  if (!Intersect)                // If this is an intersection
    {                            // we need to have a union 
      Acomp Ax=Acomp(*this);    //make copy
      Units.clear();                    // remove everthing else
      Comp.clear();
      Intersect=1;                      
      addComp(Ax);                     // add oldThis to the list
    }

  if (A.Intersect)                  // can add components
    copySimilar(A);
  else 
    addComp(A);       // combine components
  removeEqComp();
  joinDepth();  
  return *this;
}

bool
Acomp::isEmpty() const
  /*!
    Determine if empty
    \return 1 if empty /  0 otherwize
   */
{
  if (!Units.empty()) return 0;
  for(const Acomp& AC : Comp)
    if (!AC.isEmpty()) return 0;

  return 1;
}

void
Acomp::clear()
  /*!
    Deletes everything 
  */
{
  Units.clear();
  Comp.clear();
  return;
}

// ---------------------------------------------------
//  PRIVATE FUNCTIONS 
// ---------------------------------------------------


void
Acomp::deleteComp()
  /*!
    Deletes everything in Composite
  */
{
  Comp.clear();
  return;
}

bool
Acomp::hasUnitInUnit(const int T) const
  /*!
    Determine if Acomp has a unit 
    \param 
   */
{
  return (Units.find(T)!=Units.end());
}

void
Acomp::addComp(const Acomp& AC)
  /*!
    Adds a pointer to the Comp list.
    If the pointer is singular, extracts the
    object and adds that componenet.
    Assumes that the component is sorted and inserts appropiately.
    \param AC :: Acomp component to add
  */
{
  const std::pair<size_t,size_t> Stype=AC.size();
  if (Stype.first+Stype.second==0)
    return;

  trueFlag=0;
  //single unit component/Conjoint
  if (AC.isSingle() || AC.Intersect==Intersect)       
    {
      Units.insert(AC.Units.begin(),AC.Units.end());

      std::vector<Acomp>::const_iterator acp;
      for(acp=AC.Comp.begin();acp!=AC.Comp.end();acp++)
        {
	  std::vector<Acomp>::iterator cpt;
	  cpt=lower_bound(Comp.begin(),Comp.end(),*acp);
	  if (cpt==Comp.end() || *cpt!=*acp)     // Only insert if new
	    Comp.insert(cpt,*acp);
	}
      return;
    }
  // Type different insertion
  std::vector<Acomp>::iterator cpt;
  cpt=lower_bound(Comp.begin(),Comp.end(),AC);
  if (cpt==Comp.end() || *cpt!=AC)                       // Only insert if new
    Comp.insert(cpt,AC);
  return;
}

void
Acomp::addUnitItem(const int Item)
  /*!
    Adds a unit to the Unit list (if it doesn't exist). 
    \param Item :: Unit to add
  */
{
  Units.insert(Item);
  return;
}

void
Acomp::processIntersection(const std::string& Ln)
  /*!
    Helper function ::  Takes a string that has
    no outside + operator on the outer level and constructs
    an intersection
    
    Assumes that Ln has been checked for bracket consistency/
    Units are sorted after this function is returned.
    \param Ln :: String to processed as a union 
  */
{
  ELog::RegMethod RegA("Acomp","processIntersection");
  if (Ln.empty()) return;

  // Split on first bracket level which is not the VERY first
  std::string Express;
  int blevel=0;          
  // split type
  for(size_t iu=0;iu<Ln.length();iu++)
    {
      if (Ln[iu]==')')        // maybe closing outward..
	{
	  Express+=')';
	  blevel--;
	  if (blevel==0)  // closes group
	    {
	      Acomp::removeOuterBracket(Express);
	      if (Acomp::hasPlus(Express))
		{
		  Acomp AX(Union);
		  AX.setString(Express);
		  addComp(AX);
		}
	      else
		{
		  addTokens(Express);
		}
	      Express="";
	    }
	}
      else if (Ln[iu]=='(')
	{
	  blevel++;
	  if (blevel==1 && !Express.empty())
	    {
	      addTokens(Express);
	      Express="";
	    }
	  Express+='(';
	}
      else 
	Express+=Ln[iu];
    }
  // Clean up at end
  if (!Express.empty())
    addTokens(Express);

  return;
}

void
Acomp::processUnion(const std::string& Ln)
  /*!
    Helper function ::  Takes a string that has
    a + operator on the outer level and constructs
    a pair of unions 
    
    Assumes that Ln has been checked for bracket consistency/
    Units are sorted after this function is returned.
    \param Ln :: String to processed as a union 
    \throws ExBase on failure to pass string
  */
{
  ELog::RegMethod RegA("Acomp","processUnion");

  std::vector<std::string> Express;           // Intersection Expression
  std::vector<int> outBFlag;           // Outer brackets
  // find first Component to add 

  int blevel=0;          
  size_t Item(0);
  Express.push_back(std::string(""));
  outBFlag.push_back(0);
  for(size_t iu=0;iu<Ln.length();iu++)
    {
      if (Ln[iu]==')')        // maybe closing outward..
	blevel--;
      else if (Ln[iu]=='(')
	{
	  blevel++;
	  if (blevel==1)
	    outBFlag[Item]=(Express[Item].empty()) ? 1 : 0;
	}
      // Split point
      if (!blevel && Ln[iu]=='+')
	{
	  Item++;
	  Express.push_back(std::string(""));
	  outBFlag.push_back(0);
	}
      else
	Express[Item]+=Ln[iu];
    }
  
  for(size_t i=0;i<Express.size();i++)
    {
      if (!Express[i].empty())
	{
	  const std::string StrField=(outBFlag[i]) ?
	    Express[i].substr(1,Express[i].length()-2) : Express[i];
	  Acomp AX(Inter);
	  AX.setString(StrField);   // throws:
	  addComp(AX);              // add components
	}
    }
  
  return;
}

int
Acomp::merge()
  /*!
    If a comp / (this Inter) are same type then
    a merge is possible
    \return number of successful level moves
  */
{
  // First remove local numbers and units:

  std::vector<Acomp> extraComp;
  int Out(0);
  for(Acomp& AC : Comp)
    {
      Out+=AC.merge();
      if (AC.Intersect==Intersect)
	{
	  Out++;
	  
	  Units.insert(AC.Units.begin(),AC.Units.end());
	  AC.Units.clear();
	  // move up AC.comp units
	  if (!AC.Comp.empty())
	    {
	      extraComp.insert(extraComp.end(),AC.Comp.begin(),AC.Comp.end());
	      AC.Comp.clear();
	    }
	}
    }
  Comp.insert(Comp.end(),extraComp.begin(),extraComp.end());
  // remove nulls
  std::vector<Acomp>::iterator ac=
    std::remove_if(Comp.begin(),Comp.end(),
		   [&](const Acomp& AC)->bool
		     { return AC.isNull();});
  Comp.erase(ac,Comp.end());
  return Out;
}

int
Acomp::copySimilar(const Acomp& A)
  /*!
    Class to merge two list of similar
    objects. Makes a full copy of the objects
    It requires that the Intersect is the same for both
    \param A :: Object to copy
    \returns 0 on success -1 on failure
  */
{
  // copy units and components
  if (Intersect!=A.Intersect)
    return -1;

  Units.insert(A.Units.begin(),A.Units.end());

  // Add components
  for(const Acomp& AC : A.Comp)
    addComp(AC);
  return 0;
}

void
Acomp::addUnit(const std::vector<int>& Index,
	       const BnId& BX)
  /*!
    Given a single BnId unit and an index
    adds it to the main Units object.
    \param Index :: number , surfNumber
    \param BX :: binary component
  */
{
  int flag,S;
  size_t V;
  for(size_t i=0;i<BX.Size();i++)
    {
      flag=BX[i];
      if (flag)
        {
	  signSplit(flag,S,V); // sets S and V
	  if (V>Index.size())
	    throw ColErr::IndexError<size_t>(V,Index.size(),
					     "Acomp::addUnit ");
	  Units.insert(S*Index[i]);
	}
    }
  return;
}

void
Acomp::assignDNF(const std::vector<int>& Index,const std::vector<BnId>& A)
  /*!
    Assign the object to the Essentual PI in the vector
    A. This will make the form DNF.
    \param Index :: SurfNumbers
    \param A :: Vector of BnId's that are valid
  */
{
  clear();
  if (A.empty()) return;
  
  if (A.size()==1)  //special case for single intersection 
    {
      Intersect=1;
      addUnit(Index,A[0]);
      return;
    }
  
  Intersect=0;               // union of intersection
  std::vector<BnId>::const_iterator vc;
  for(vc=A.begin();vc!=A.end();vc++)
    {
      Acomp Px(Inter);  
      Px.addUnit(Index,*vc);
      addComp(Px);
    }
  return;
}

void
Acomp::assignCNF(const std::vector<int>& Index,
		 const std::vector<BnId>& A)
  /*!
    Assign the object to the Essentual PI in the vector
    A. This will make the form DNF.
    \param Index :: SurfNumbers
    \param A :: Vector of BnId's that are valid
  */
{
  Units.clear();
  deleteComp();
  if (A.empty())
    return;
  
  if (A.size()==1)  //special case for single union
    {
      Intersect=0;
      addUnit(Index,A[0]);
      return;
    }
  
  Intersect=1;               // intersection of union 
  std::vector<BnId>::const_iterator vc;
  for(vc=A.begin();vc!=A.end();vc++)
    {
      Acomp Px(Union);      // Union
      BnId X=*vc;
      X.reverse();
      Px.addUnit(Index,X);
      addComp(Px);
    }
  return;
}


// -------------------------------------
//   PUBLIC FUNCTIONS 
// -------------------------------------


void
Acomp::Sort()
  /*!
    Function to sort the components of the lists.
    Decends down the Comp Tree.
  */
{
  // Sort each decending object first
  for(Acomp& AC : Comp)
    AC.Sort();

  // comparative on unit
  std::sort(Comp.begin(),Comp.end());
  return;
}

int
Acomp::makeReadOnce() 
  /*!
    This function attempts to make the function
    a read one form. Assumes that it is either DNF or
    CNF form
    \retval 0 if fails
    \retval 1 if success (and sets it into the appropiate form)
  */
{
  //  std::map<int,int> LitSet;
  //  getLiterals(LitSet);
/*  // Process DNF
  if (isDNF())
    return makeReadOnceForDNF();
  if (isCNF())
    return makeReadOnceForCNF();
  return 0;
*/
  return 0;
}

int
Acomp::logicalCover(const Acomp& A) const
  /*!
    Determine if this is a cover for A
    i.e. if this is true then A is true and vis versa
    \param A :: Logical state to set
    \return -1 :: this is the cover
    \retval 0 :: no cover
    \retval 1 :: A is the cover
  */
{
  std::set<int> ALit;
  std::set<int> BLit;
  this->getAbsLiterals(ALit);
  A.getAbsLiterals(BLit);

  std::vector<int> SSDiff;
  std::set_symmetric_difference
    (
     ALit.begin(),ALit.end(),
     BLit.begin(),BLit.end(),
     std::back_inserter(SSDiff));
  // not possible is mis-match symbols
  bool Aflag(0),Bflag(0);
  for(const int SN: SSDiff)
    {
      if (BLit.find(SN) == BLit.end())
	{
	  Bflag=1;
	  if (Aflag) return 0;
	}
      else
	{
	  Aflag=1;
	  if (Bflag) return 0;
	}
    } 

  std::vector<int> keyNumbers;
  if (Aflag)  // then 
    keyNumbers.assign(BLit.begin(),BLit.end());
  else
    keyNumbers.assign(ALit.begin(),ALit.end());

  std::map<int,int> Base;       // keynumber :: trueth value
  for(const int CN : keyNumbers)
    Base.emplace(CN,0);   // all false
  do
    {
      if (isTrue(Base) != A.isTrue(Base))
	return 0;
    } while (!MapSupport::iterateBinMap(Base));
  
  return 1;
}


int
Acomp::logicalIntersectCover(const Acomp& A) const
  /*!
    Determine if T/A is a cover for the intersection
    of T.A
    i.e. if this is true then A is true and vis versa
    \param A :: Logical state to set
    \retval 0 :: no cover
    \retval 1 :: T is the cover
    \retval 2 :: A is the cover
    \retval 3 :: both self cover
  */
{
  std::set<int> ALit;
  std::set<int> BLit;
  this->getAbsLiterals(ALit);
  A.getAbsLiterals(BLit);

  std::vector<int> SSDiff;
  std::set_symmetric_difference
    (
     ALit.begin(),ALit.end(),
     BLit.begin(),BLit.end(),
     std::back_inserter(SSDiff));
  // not possible is mis-match symbols
  bool ADiffFlag(0),BDiffFlag(0);
  for(const int SN: SSDiff)
    {
      if (BLit.find(SN) == BLit.end())
	{
	  BDiffFlag=1;
	  if (ADiffFlag) return 0;
	}
      else
	{
	  ADiffFlag=1;
	  if (BDiffFlag) return 0;
	}
    } 

  std::vector<int> keyNumbers;
  Acomp pairCC(1);
  pairCC.Comp.push_back(*this);
  pairCC.Comp.push_back(A);

  if (ADiffFlag)  // then
    keyNumbers.assign(BLit.begin(),BLit.end());
  else
    keyNumbers.assign(ALit.begin(),ALit.end());
  
  std::map<int,int> Base;       // keynumber :: trueth value
  for(const int CN : keyNumbers)
    Base.emplace(CN,0);   // all false

  bool tOut(1),aOut(1);
  do
    {
      const bool pairCCFlag=pairCC.isTrue(Base);
      const bool TFlag=this->isTrue(Base);
      const bool AFlag=A.isTrue(Base);

      if (TFlag!=pairCCFlag) tOut=0;
      if (AFlag!=pairCCFlag) aOut=0;
      if (!aOut && !tOut) return 0;
    } while (!MapSupport::iterateBinMap(Base));

  return (tOut ? 1 : 0 ) | (aOut ? 2 : 0 );
}

  
bool
Acomp::logicalEqual(const Acomp& A) const
  /*!
    Test that the system that is logically the same:
    \param A :: Logical state to test
    \retval 0 :: false  
    \retval 1 :: true
  */
{
  std::set<int> ALitMap;
  std::set<int> BLitMap;
  getAbsLiterals(ALitMap);
  A.getAbsLiterals(BLitMap);
  if (ALitMap!=BLitMap)
    return 0;

  const std::vector<int> keyNumbers(ALitMap.begin(),ALitMap.end());    
  std::map<int,int> Base;       // keynumber :: trueth value
  for(const int CN : ALitMap)
    Base.emplace(CN,0);

  do
    {
      if (isTrue(Base) != A.isTrue(Base))
	return 0;
    } while (!MapSupport::iterateBinMap(Base));

  return 1;
}


int
Acomp::isNull() const
  /*!
    \returns 1 if there are no memebers
  */
{
  return ((!Units.size() && !Comp.size()) ? 1 : 0);
}

int
Acomp::isDNF() const
  /*!
    Determines if the component is in
    DNF form. This Acomp needs to be a union
    of intersection. 
    \retval 1 ::  DNF form
    \retval 0 ::  not DNF form
  */
{
  // If an intersect (single component) 
  if (Intersect)
    return (Comp.empty()) ? 1 : 0;
  // Else needs to be union of Intersections.
  std::vector<Acomp>::const_iterator cc;
  for(cc=Comp.begin();cc!=Comp.end();cc++)
    if (cc->Intersect==0 || !cc->isSimple())
      return 0;

  return 1;
}

int
Acomp::isCNF() const
  /*!
    Determines if the component is in
    CNF form. This Acomp needs to be an intersection
    of unions
    \retval 1 ::  DNF form
    \retval 0 ::  not DNF form
  */
{
  // If an intersect (single component) 
  if (!Intersect)
    return (Comp.empty()) ? 1 : 0;
  // Else needs to be intersection of unions
  std::vector<Acomp>::const_iterator cc;
  for(cc=Comp.begin();cc!=Comp.end();cc++)
    if (cc->Intersect==1 || !cc->isSimple())
      return 0;

  return 1;
}

bool
Acomp::hasLiteral(const int literal) const
  /*!
    Determine if has the literals
    \param literalMap :: Set of leteral valuds
    literals 
  */
{
  if (Units.find(literal)!=Units.end())
    return 1;
  for(const Acomp& AC : Comp)
    if (AC.hasLiteral(literal))
      return 1;

  return 0;
}

bool
Acomp::hasAbsLiteral(const int literal) const
  /*!
    Determine if has the literals
    \param literalMap :: Set of leteral valuds
    literals 
    \param literal :: absolute literal value
  */
{
  if (Units.find(literal)!=Units.end())
    return 1;
  if (Units.find(-literal)!=Units.end())
    return 1;
  for(const Acomp& AC : Comp)
    if (AC.hasAbsLiteral(literal))
      return 1;

  return 0;
}
  
void
Acomp::getAbsLiterals(std::set<int>& literalMap) const
  /*!
    Get a map of the literals and the frequency
    that they occur.
    This does not keep the +/- part of the literals separate
    \param literalMap :: Set of leteral valuds
    literals 
  */
{
  for(const int u : Units)
    literalMap.insert(std::abs(u));

  for(const Acomp& CC : Comp)
    CC.getAbsLiterals(literalMap);
  return;
}

void
Acomp::getLiterals(std::set<int>& literalMap) const
  /*!
    Get a map of the literals and the frequency
    that they occur.
    This keeps + and - literals separate
    \param literalMap :: Set the get the literals 
  */
{
  literalMap.insert(Units.begin(),Units.end());

  for(const Acomp& CC : Comp)
    CC.getLiterals(literalMap);

  return;
}

int
Acomp::isSimple() const
  /*!
    Determines if there are not complex components.
    \retval 1 :: Comp is empty
    \retval 0 :: Contains Components.
  */
{
  return (Comp.empty() ? 1 : 0);
}

int
Acomp::isSingle() const
  /*!
    Deterimines if the item's singular
    \retval 1 if only one/zero item
    \retval 0 if more than one item.
  */
{
  return (Comp.size()+Units.size()>1 ? 0 : 1);
}


int
Acomp::removeEqComp()
  /*!
    Remove identical items.
    \returns number removed.
  */
{
  ELog::RegMethod RegA("Acomp","removeEqComp");
  // First check all the Comp units:
  long int cnt(0);
  std::vector<Acomp>::iterator dx;

  sort(Comp.begin(),Comp.end());
  dx=unique(Comp.begin(),Comp.end());
  cnt+=std::distance(dx,Comp.end());
  Comp.erase(dx,Comp.end());

  // Units are sorted
  return static_cast<int>(cnt);
}


int
Acomp::makePI(std::vector<BnId>& DNFobj) const
  /*!
    This method finds the principle implicants.
    \param DNFobj :: A vector of Binary ID from a true 
    vectors of keyvalues.
    \returns number of PIs found.
    \todo  Can we set this up to get non-pairs
    i.e. one pass.
    
  */
{
  if (DNFobj.empty())   // no work to do return.
    return 0;
  // Note: need to store PI components separately
  // since we don't want to loop continuously through them
  std::vector<BnId> Work;       // Working copy
  std::vector<BnId> PIComp;     // Store for PI componends
  std::vector<BnId> Tmod;       // store modified components
  int changeCount(0);           // Number change
  std::vector<BnId>::iterator uend;     // itor to remove unique
  // Need to make an initial copy.
  Work=DNFobj;

  int cnt(0);
  do
    {
      cnt++;
      // Deal with tri-state objects ??
      sort(Work.begin(),Work.end());
      uend=unique(Work.begin(),Work.end());
      Work.erase(uend,Work.end());
      Tmod.clear();                  // erase all at the start

      //set PI status to 1
      for_each( Work.begin(),Work.end(),
		std::bind2nd(std::mem_fun_ref(&BnId::setPI),1) );

      //Collect into pairs which have a difference of +/- one 
      // object
      // Can sort this realive to 
      std::vector<BnId>::iterator vc;
      for(vc=Work.begin();vc!=Work.end();vc++)
	{
	  const size_t GrpIndex(vc->TrueCount()+1);
	  std::vector<BnId>::iterator oc=vc+1;

	  for(oc=vc+1;oc!=Work.end();oc++)
	    {
	      const size_t OCnt=oc->TrueCount();
			

	      if (OCnt>GrpIndex)
		break;
	      if (OCnt==GrpIndex)
	        {
		  std::pair<int,BnId> cVal=vc->makeCombination(*oc);
		  if (cVal.first==1)   // was complementary
		    {
		      Tmod.push_back(cVal.second);
		      oc->setPI(0);         
		      vc->setPI(0);
		      changeCount++;       // 1 changed
		    }
		}
	    }
	}

      for(const BnId& BI : Work)
	if (BI.PIstatus()==1)
	  PIComp.push_back(BI);
      
      Work=Tmod;
    } while (!Tmod.empty());
  // Copy over the unit.

  return makeEPI(DNFobj,PIComp);
}


int
Acomp::makeEPI(std::vector<BnId>& DNFobj,
	       std::vector<BnId>& PIform) const
  /*!
    Creates an essentual PI list (note: this is 
    not unique).
    Given the list form the EPI based on the Quine-McClusky method.

    \param DNFobj :: Object in DNF form 
    \param PIform :: List of rules in Prime Implicant form
       It is set on exit (to the EPI)
    \returns :: 1 if successful and 0 if failed
  */
{
  ELog::RegMethod RegA("Acomp","makeEPI");

  const int debug(0);
  if (PIform.empty())
    return 0;
  
  std::vector<BnId> EPI;  // Created Here.

  std::vector<int> EPIvalue;
  // Make zeroed matrix.
  Geometry::MatrixBase<int> Grid(PIform.size(),DNFobj.size()); 
  std::vector<size_t> DNFactive(DNFobj.size());       // DNF that active
  std::vector<size_t> PIactive(PIform.size());        // PI that are active
  std::vector<int> DNFscore(DNFobj.size());        // Number in each channel
  std::vector<int> PIscore(DNFobj.size());        // Number in each channel
  
  //Populate
  for(size_t pc=0;pc!=PIform.size();pc++)
    PIactive[pc]=pc;
  
  for(size_t ic=0;ic!=DNFobj.size();ic++)
    {
      DNFactive[ic]=ic;                            //populate (avoid a loop)
      for(size_t pc=0;pc!=PIform.size();pc++)
	{
	  if (PIform[pc].equivalent(DNFobj[ic]))
	    {
	      Grid[pc][ic]=1;
	      DNFscore[ic]++;
	    }
	}
      if (DNFscore[ic]==0)
	{
	  ELog::EM<<"PIForm:"<<ELog::endCrit;
	  for(const BnId& BI : PIform)
	    ELog::EM<<BI<<ELog::endDiag;

	  ELog::EM<<"Error with DNF / EPI determination at "<<ic<<ELog::endCrit;
	  ELog::EM<<" Items "<<DNFobj[ic]<<ELog::endCrit;
	  return 0;
	}
    }
  /// DEBUG PRINT 
  //  printImplicates(PIform,Grid);
  /// END DEBUG

  std::vector<size_t>::iterator dx;
  std::vector<size_t>::iterator ddx;    // DNF active iterator
  std::vector<size_t>::iterator px;     // PIactive iterator

  // 
  // First remove singlets:
  // 
  for(dx=DNFactive.begin();dx!=DNFactive.end();dx++)
    {
      if (*dx!=ULONG_MAX && DNFscore[*dx]==1)        // EPI (definately)
	{
	  for(px=PIactive.begin();
	      px!=PIactive.end() && !Grid[*px][*dx];px++) ;
	  if (px!=PIactive.end())
	    {
	      EPI.push_back(PIform[*px]);
	      // remove all minterm that the EPI covered
	      for(ddx=DNFactive.begin();ddx!=DNFactive.end();ddx++)
		if (*ddx!=ULONG_MAX && Grid[*px][*ddx])
		  *ddx= ULONG_MAX;      //mark for deletion (later)
	      // Can remove PIactive now.
	      PIactive.erase(px,px+1);
	    }
	}
    }
  // Remove dead items from active list
  DNFactive.erase(
    remove_if(DNFactive.begin(),DNFactive.end(),
	      std::bind(std::equal_to<size_t>(),
			std::placeholders::_1,ULONG_MAX)),
    DNFactive.end());


  /// DEBUG PRINT 
  if (debug)
    {
      for(px=PIactive.begin();px!=PIactive.end();px++)
	{
	  ELog::EM<<PIform[*px]<<":";
	  for(ddx=DNFactive.begin();ddx!=DNFactive.end();ddx++)
	    ELog::EM<<((Grid[*px][*ddx]) ? " 1" : " 0");
	  ELog::EM<<ELog::endDebug;
	}
      ELog::EM<<"END OF TABLE "<<ELog::endDebug;
    }

  // Ok -- now the hard work...
  // need to find shortest "combination" that spans
  // the remaining table.
  
  // First Make a new matrix for speed.  Useful ???
  Geometry::MatrixBase<int> Cmat(PIactive.size(),DNFactive.size());  // corrolation matrix
  size_t cm(0);
  for(px=PIactive.begin();px!=PIactive.end();px++)
    {
      size_t dm(0);
      for(ddx=DNFactive.begin();ddx!=DNFactive.end();ddx++)
	{
	  if (Grid[*px][*ddx])
	    Cmat[cm][dm]=1;
	  dm++;
	} 
      cm++;
    }

  const size_t Dsize(DNFactive.size());
  const size_t Psize(PIactive.size());
  //icount == depth of search ie 

  for(size_t Icount=1;Icount<Psize;Icount++)
    {
      // This counter is a ripple counter, ie 1,2,3 where no numbers 
      // are the same. BUT it is acutally 0->N 0->N 0->N
      // index by A, A+1 ,A+2  etc
      RotaryCounter<size_t> Index(Icount,Psize);
      do {
	size_t di;
	for(di=0;di<Dsize;di++)   //check each orignal position
	  {
	    size_t vecI;
	    for(vecI=0;vecI<Icount &&
		  !Cmat[Index[vecI]][di];vecI++) ;
	    if (vecI==Icount)
	      break;
	  }
	if (di==Dsize)          // SUCCESS!!!!!
	  {
	    for(size_t iout=0;iout<Icount;iout++)
	      EPI.push_back(PIform[Index[iout]]);
	    DNFobj=EPI;
	    return 1;;
	  }
      } while(!(++Index));
    }

  //OH well that means every PIactive is a EPI :-(
  for(px=PIactive.begin();px!=PIactive.end();px++)
    EPI.push_back(PIform[*px]);
  DNFobj=EPI;

  return 1;
}

std::vector<int>
Acomp::getKeys() const
  /*!
    Get the key numbers in the system
    \return Key of literals
  */
{
  std::set<int> litMap;       // keynumbers
  getAbsLiterals(litMap);
  return std::vector<int>(litMap.begin(),litMap.end());
}

int
Acomp::getDNFobject(std::vector<int>& keyNumbers,
		    std::vector<BnId>& DNFobj) const
  /*!
    Creates the DNF items (ie the binary list
    of true statements) It forms a sum of products.
    The original is not changed by the keynumbers and the 
    DNF objects are output into the function parameters.
    \param keyNumbers :: index list of the DNFobj. The
    [bitNum]->rule/key number.
    \param DNFobj :: write out the DNF object into BnId form
    \retval 0 :: on success.
    \retval -1 :: on error.
  */
{
  std::set<int> litMap;       // keynumber
  std::map<int,int> Base;         // keynumber :: value
  getAbsLiterals(litMap);
  
  if (litMap.empty())
    return -1;
  
  keyNumbers.assign(litMap.begin(),litMap.end());

  for(const int CN : litMap)
    Base.emplace(CN,0);

  DNFobj.clear();  
  BnId State(keyNumbers.size(),0);                 //zero base

  do
    {
      if (isTrue(Base))
	{
	  State.setState(keyNumbers,Base);
	  DNFobj.push_back(State);
	}
    } while (!MapSupport::iterateBinMap(Base));

  return 0;
}
 

int
Acomp::makeDNFobject()
  /*!
    Sets the object to the DNF form.
    \retval 0 on failure
    \retval Number of DNF components
  */
{
  std::vector<BnId> DNFobj;
  std::vector<int> keyNumbers;
  if (!getDNFobject(keyNumbers,DNFobj))
    {      
      if (makePI(DNFobj))
	assignDNF(keyNumbers,DNFobj);

      return static_cast<int>(DNFobj.size());
    }
  return 0;
}

int
Acomp::makeCNFobject()
  /*!
    Sets the object to the CNF form.
    \retval 0 on failure
    \retval Number of CNF components
  */
{
  std::vector<BnId> CNFobj;
  std::vector<int> keyNumbers;
  if (!getCNFobject(keyNumbers,CNFobj))
    {
      if (makePI(CNFobj))
	assignCNF(keyNumbers,CNFobj);
      return static_cast<int>(CNFobj.size());
    }
  return 0;
}

int
Acomp::getDNFpart(std::vector<Acomp>& Parts) const
  /*!
    Sets the object into parts of the DNF form
    then puts the object in the Parts section
    \param Parts:: vector of the Parts found (Acomp units without
    component)
    \returns number of parts
  */
{
  /*!
    If this is DNF then we don't want
    to calculate the DNF but just use it
  */
  if (isDNF())
    {
      Parts.clear();
      for(const int v : Units)
	{
	  Acomp Aitem(Inter);  // Intersection (doesn't matter since 1 object)
	  Aitem.addUnitItem(v);
	  Parts.push_back(Aitem);
	}
      for(const Acomp& AC : Comp)
	Parts.push_back(AC);
      return static_cast<int>(Parts.size());
    }
  
  std::vector<int> keyNumbers;
  std::vector<BnId> DNFobj;
  if (!getDNFobject(keyNumbers,DNFobj))
    {
      if (makePI(DNFobj))
	{
	  for(const BnId& BI : DNFobj)
	    {
	      // make an intersection and add components
	      Acomp Aitem(Inter); 
	      Aitem.addUnit(keyNumbers,BI);
	      Parts.push_back(Aitem);
	    }
	}	  
      return static_cast<int>(Parts.size());
    }
  return 0;
}

int
Acomp::getCNFobject(std::vector<int>& keyNumbers,
		    std::vector<BnId>& CNFobj) const
  /*!
    Creates the CNF items (ie the binary list
    of false statements) It forms a sum of products.
    The original is not changed by the keynumbers and the 
    DNF objects are output into the function parameters.
    \param keyNumbers :: index list of the CNFobj. The
    [bitNum]->rule/key number.
    \param CNFobj :: write out the CNF object into BnId form
    \retval 0 :: on success.
    \retval -1 :: on error.
  */
{
  std::set<int> litMap;       // keynumber :: number of occurances
  std::map<int,int> Base;         // keynumber :: value
  getAbsLiterals(litMap);
  
  if (litMap.size()<1)
    return -1;

  keyNumbers.assign(litMap.begin(),litMap.end());
  for(const int CN : keyNumbers)
    Base.emplace(CN,1);

  CNFobj.clear();
  BnId State(Base.size(),0);    //zero base  
  do
    {
      State.mapState(keyNumbers,Base);
      if (!isTrue(Base))
	{
	  CNFobj.push_back(State);
	}
    } while(++State);
 
  return 0;
}

int
Acomp::isTrue(const std::map<int,int>& Base) const
  /*!
    Determines if the rule is true, given
    the Base state.
    \param Base :: map of <LiteralNumber, State> 
    \returns 1 if true and 0 if false
  */
{
  int S,V;
  std::map<int,int>::const_iterator mc;

  if (Units.empty() && Comp.empty())
    return (trueFlag == -1) ? 0 : 1;

  if (Intersect)
    {
      // all must be true:
      for(const	int uv : Units)
	{
	  signSplit(uv,S,V);
	  mc=Base.find(V);

	  if (mc==Base.end())
	    throw ColErr::InContainerError<int>
	      (uv,"Acomp::isTrue Base unit not found");
	  if ((S>0 && !mc->second) ||
	      (S<0 && mc->second)) return 0;
	}
      for(const Acomp& AC : Comp)
	if (!AC.isTrue(Base))
	  return 0;

      // everything else true
      return 1;
    }
  // UNION
  // any must be true:
  for(const int uv : Units)
    {
      signSplit(uv,S,V);
      mc=Base.find(V);
      
      if (mc==Base.end())
	throw ColErr::InContainerError<int>
	  (uv,"Acomp::isTrue Base unit not found");
      
      if ((S>0 && mc->second) ||
	  (S<0 && !mc->second)) return 1;
    }

  for(const Acomp& AC : Comp)
    if (AC.isTrue(Base))
      return 1;

  // Everything else false
  return 0;
}

std::pair<Acomp,Acomp>
Acomp::algDiv(const Acomp& G)
  /*!
    Carries out algebraic division
    - Algorthm: 
    -- U => set of cubes in f with all "literats not in G", deleted
    -- V => set of cubes in F with all "literals in P", deleted
    \param G :: The divisor
    \return Pair of Divided + Remainder
  */
{
  ELog::RegMethod RegA("Acomp","algDiv");
  //First make completely DNF (if necessary)
  if (!isDNF() && !makeDNFobject())
    return std::pair<Acomp,Acomp>(Acomp(Union),Acomp(Union));

  std::set<int> Gmap; // get map of literals and frequency
  G.getLiterals(Gmap);
  if (Gmap.empty())
    return std::pair<Acomp,Acomp>(Acomp(Union),Acomp(Union));
  // Make two lists.
  // U == set of elements in F (item to be divided) (this)
  // V == set of elements in G 
  // U is ste to be 
  std::vector<Acomp> U;
  std::vector<Acomp> V;

  std::map<int,int>::const_iterator mc;
  // Only have First level components to consider
  std::vector<Acomp>::const_iterator cc; 

  int cell;
  
  std::vector<Acomp> Flist,Glist;
  if (!getDNFpart(Flist) || !G.getDNFpart(Glist))
    return std::pair<Acomp,Acomp>(Acomp(Union),Acomp(Union));

  for(const Acomp& CC : Flist)
    {
      U.push_back(Acomp(Inter)); 
      V.push_back(Acomp(Inter)); 
      Acomp& Uitem= U.back();
      Acomp& Vitem= V.back();
      UTYPE::const_iterator ccItem=CC.Units.begin();
      while( (cell = *ccItem) )
        {
	  if (Gmap.find(cell)!=Gmap.end())
	    Uitem.addUnitItem(cell);
	  else
	    Vitem.addUnitItem(cell);
	  ccItem++;
	}
    }

  Acomp H(Inter);       // H is an intersection group
  Acomp Hpart(Union);    //Hpart is a union group
  for(const Acomp& CC : Glist)
    {
      std::vector<Acomp>::const_iterator ux,vx;
      vx=V.begin();
      for(ux=U.begin();ux!=U.end() && vx!=V.end();vx++,ux++)
	{
	  if (!vx->isNull() && ux->contains(CC))
	    {
	      Hpart.addComp(*vx);
	    }
	}
      // H exists then combine (note: must do this if a composite divisor)
      if (!Hpart.isNull())
        {
	  H*=Hpart;
	  H.joinDepth();        // Up shift suitable objects.
	  Hpart.Comp.clear();
	  Hpart.Units.clear();   // Just in case

	}

    }
  if (!H.isDNF())
    H.makeDNFobject();
  Acomp Rem(*this);
  Acomp Factor(H);
  Factor*=G;
  Rem-=Factor;
  return std::pair<Acomp,Acomp>(H,Rem);
}

int 
Acomp::contains(const Acomp& A) const
  /*!
    Checks the Units of A to  see if they are in this->Units.
    *this can contain MORE signed-literals than A.
    \param A :: Object to cross compare
    \retval 0 :: All literals in A are in this
    \retval 1 :: A is unique from this
  */
{
  for(const int AU : A.Units)
    if (Units.find(AU)==Units.end())
      return 0;
  
  return 1;
}

int
Acomp::joinDepth()
  /*!
    Searchs down the tree to find if any
    singles exist and up-promotes them.
    \throw ColErr::ExBase on mal-formed state
    \returns number of up-promotions.
  */
{
  ELog::RegMethod RegA("Acomp","joinDepth");
  
  // Deal with case that this is a singular object::
  const std::pair<size_t,size_t> topSize(this->size());
  if ((topSize.first+topSize.second)==0)
    throw ColErr::ExBase(-2,"Pair Count wrong");
   //SINGULAR
  int cnt(0);        // number upgraded
  if (topSize.first==0 && topSize.second==1)            // Comp to up-premote
    {
      const Acomp* Lower=itemC(0);                    // returns pointer
      Units.clear();
      Intersect=Lower->Intersect;
      Units=Lower->Units;
      if (!Lower->Comp.empty())
        {
	  std::vector<Acomp> tmpComp=Lower->Comp;
	  Comp=tmpComp;
	}
      cnt++;
    }
  // Loop over each component.  (if identical type upshift and remove)
  for(size_t ix=0;ix<Comp.size();ix++)
    {
      const Acomp& AX=Comp[ix];
      const std::pair<size_t,size_t> compSize(AX.size());
      if ((compSize.first+compSize.second)==0)      
	throw ColErr::ExBase(-2,"Pair Count wrong");

      // If Singular then can be up premoted.
      if (compSize.first==1 && compSize.second==0)         // UNITS only
	{
	  Units.insert(AX.getSinglet());
	  // delete memory and the component.
	  Comp.erase(Comp.begin()+static_cast<long int>(ix));
	  ix--;
	  cnt++;
	} 
      else if (compSize.first==0 && compSize.second==1)   // COMP only
	{
	  Comp.push_back(*AX.itemC(0)); 
	  // delete memory and the component.
	  Comp.erase(Comp.begin()+static_cast<long int>(ix));
	  ix--;
	  cnt++;
	}
      // Same type thus use the bits.
      else if (Intersect==AX.Intersect)    
	{
	  Units.insert(AX.Units.begin(),AX.Units.end());
	  Comp.insert(Comp.end(),AX.Comp.begin(),AX.Comp.end());
	  Comp.erase(Comp.begin()+static_cast<long int>(ix));
	  ix--;
	  cnt++;
	}	  
    }
  if (cnt)
    {
      Sort();
      removeEqComp();
    }
  std::vector<Acomp>::iterator xc;
  for(xc=Comp.begin();xc!=Comp.end();xc++)
    cnt+=xc->joinDepth();

  return cnt;
}

void
Acomp::setString(const std::string& Line)
  /*!
    Sort out stuff like abc'+efg
    given a inner bracket expand that etc.
    \param Line :: string of for abc'.
  */
{
  ELog::RegMethod RegA("Acomp","setString");

  Acomp CM(Union);            /// Complementary object
  // DELETE ALL
  deleteComp();
  Units.clear();
  // 
  // Process #( ) sub units
  // 
  std::string Ln = Line;
  std::string::size_type sPos=Ln.find('#');
  
  // REMOVE ALL COMPLEMENTS
  while (sPos!=std::string::npos && Ln[sPos+1]=='(')
    {
      int bLevel(1);
      size_t ePos;
      for(ePos=sPos+2;bLevel>0 && 
	    ePos<Ln.size();ePos++)
        {
	  if (Ln[ePos]=='(')
	    bLevel++;
	  else if (Ln[ePos]==')')
	    bLevel--;
	}
      if (bLevel)
	throw ColErr::InvalidLine(Ln,"Acomp::setString",
				  static_cast<size_t>(sPos));

      // std::string Part= Ln.substr(sPos,ePos-sPos);
      CM.setString(Ln.substr(sPos+2,ePos-sPos-3));
      CM.complement();
      Ln.replace(sPos,ePos-sPos,"("+CM.display()+")");
      sPos=Ln.find('#');
    }

  // REMOVE OUTER BACKET:
  size_t primBrack(0);
  if (Ln.size()>2)
    {
      while(Ln[primBrack]=='(' && Ln[Ln.size()-(primBrack+1)]==')')
	{
	  primBrack++;
	}
    }

  //
  // First :: check brackets
  //
  int blevel=0;    // bracket level
  for(size_t i=primBrack;i<Ln.size()-primBrack;i++)
    {
      if (Ln[i]=='(')
	blevel++;
      if (Ln[i]==')')
	{
	  if (!blevel)      // error condition [if not closing bracket]
	    {
	      if (!primBrack)   // maybe need the outer bracket
		{
		  deleteComp();
		  throw ColErr::InvalidLine(Ln,"Bracket test",
					    Ln.size());
		}
	      primBrack--;
	    }
	  else
	    blevel--;
	}
    }
  // If no bracket close
  if (blevel!=0)
    throw ColErr::InvalidLine(Ln,"BracketTest");

  if (primBrack)
    Ln=Ln.substr(primBrack,Ln.size()-primBrack*2);

  const size_t wpos=checkWrongChar(Ln);
  if(wpos)
    throw ColErr::InvalidLine(Ln,"Wrong char",wpos);

  // Now search to determine if we are at top level
  Intersect=1;
  for(size_t i=0;i<Ln.size() && Intersect;i++)
    {
      if (Ln[i]==':') Ln[i]='+';
      // THERE IS A union at the top level"
      if (!blevel && Ln[i]=='+')     //must be union
	{
	  Intersect=0;
	  break;
	}
      if (Ln[i]=='(') blevel++;
      if (Ln[i]==')') blevel--;
    }

  if (Intersect)
    processIntersection(Ln);
  else
    processUnion(Ln);

  return;
}

void
Acomp::orString(const std::string& Line)
  /*!
    Add a string item to the line. It is joined
    as an or/(.)
    \param Line :: string of for abc'.
  */
{
  Acomp CM(Union);
  CM.setString(Line);
  this->operator+=(CM);
  return;
}

size_t
Acomp::countComponents() const
  /*!
    Recursive method of counting components
    \return number of components
  */
{
  size_t out(!Units.empty() ? 1 : 0);
  for(const Acomp& AC : Comp)
    out+=AC.countComponents();

  return out;
}    

std::pair<size_t,size_t>
Acomp::size() const
  /*!
    Gets the size of the Units and the Comp
    \returns size of Unit, Comp
  */
{
  return std::pair<size_t,size_t>(Units.size(),Comp.size());
}

int
Acomp::getSinglet() const
  /*!
    Get a single value as an item
    \return value
   */
{
  if (Units.size()!=1 || !Comp.empty())
    throw ColErr::InContainerError<size_t>
      (Units.size(),"Units or Comp not sigular:"+display());
  return *Units.begin();
}
  

const Acomp*
Acomp::itemC(const size_t Index) const
  /*!
    Assessor function to get a Comp points
    \param Index :: Number of Comp to aquire
    \returns Comp[Index] or 0 on failure
  */
{
  if (Index>=Comp.size())
    return 0;
  return &Comp[Index];
}

void
Acomp::upMoveComp()
  /*!
    Move the comp unit up if it is singular
  */
{

  if (Comp.size()==1)
    {
      const Acomp& AC = Comp.front();
      if (Units.empty())
	{
	  Units=AC.Units;
	  // add in and then remove first [necessary ??]
	  Intersect=AC.Intersect;
	  Comp.insert(Comp.end(),AC.Comp.begin(),AC.Comp.end());
	  Comp.erase(Comp.begin());
	}
      else if (AC.isSimple() && AC.isSingle())
	{
	  Units.insert(AC.getSinglet());
	  Comp.erase(Comp.begin());
	}
    }
  return;
}

bool
Acomp::clearNulls()
  /*!
    Remove Nulls
    Currently a very very simple system to remove null intersects
    \return true if object is found null and all lower components are
    deleted [necessary???]
  */
{
  // ASSUMES sorted
  if (Intersect)
    {
      for(const int N : Units)
	{
	  if (Units.find(-N)!=Units.end())
	    {
	      trueFlag=-1;
	      clear();
	      return 1;
	    }
	}
    }
  else // UNION
    {
      for(const int N : Units)
	{
	  // always true
	  if (Units.find(-N)!=Units.end())
	    {
	      clear();
	      trueFlag=1;
	      return 1;
	    }
	}
    }
  
  // this must be carried out since Acomp might be intersect
  // even if above is not
  std::vector<Acomp>::iterator ac=
    std::remove_if(Comp.begin(),Comp.end(),
		   [&](Acomp& AC)->bool
		   { return AC.clearNulls();});
  Comp.erase(ac,Comp.end());

  return 0;
}



void
Acomp::complement() 
  /*!
    Take a complement of the current object
    This will reverse the type since 
    union<->intersection as a+b -> a'b' and
    ab -> a'+b'
  */
{
  ELog::RegMethod RegA("Acomp","complement");
  Intersect=1-Intersect;

  UTYPE newUSet;
  for(const int UN : Units)
    newUSet.insert(-UN);
  Units=newUSet;

  for(Acomp& AC : Comp)
    AC.complement();

  sort(Comp.begin(),Comp.end());
  return;
}


void
Acomp::writeFull(std::ostream& OXF,const int Indent) const
  /*!
    Real pretty print out statement  :-)
    \param OXF :: output stream
    \param Indent :: level of indentation (allows a cascaded call system)
  */
{
  for(int i=0;i<Indent;i++)
    OXF<<" ";
  OXF<<((Intersect==1) ? "Inter" : "Union");
  OXF<<" "<<Units.size()<<" "<<Comp.size()<<std::endl;
  for(int i=0;i<Indent;i++)
    OXF<<" ";
  OXF<<display()<<std::endl;
  std::vector<Acomp>::const_iterator vc;
  for(vc=Comp.begin();vc!=Comp.end();vc++)
    {
      vc->writeFull(OXF,Indent+2);
    }
  return;
}


void
Acomp::printImplicates(const std::vector<BnId>& PIform,
		       const Geometry::MatrixBase<int>& Grid) const
  /*!
    Debug function to print out 
    PI and Grid :
    \param PIform :: Principle implicates
    \param Grid :: grid form
  */

{
  ELog::RegMethod RegA("Acomp","printImplicates");

  const std::pair<size_t,size_t> RX=Grid.size();
  for(size_t pc=0;pc!=PIform.size();pc++)
     {
       ELog::EM<<PIform[pc]<<":";
       for(size_t ic=0;ic!=RX.second;ic++)
	 ELog::EM<<((Grid[pc][ic]) ? " 1" : " 0");
       ELog::EM<<ELog::endTrace;
     }
  return;
}

void 
Acomp::removeOuterBracket(std::string& Item)
  /*!
    Determine if the item is a ( xxxx ) system
    Note that we need to carefully avoid (xx)(yyy) systems
    \param Item :: Item to  check and return without outside brackets
  */
{
  ELog::RegMethod RegA("Acomp","removeOuterBracket");
  if (Item.size()>2 || Item[0]!='(')
    return;
  
  int bLevel(1);
  for(size_t i=1;i<Item.size()-1;i++)
    {
      if (Item[i]=='(')
	bLevel++;
      else if (Item[i]==')')
	{
	  bLevel--;
	  if (bLevel==0)       // Internal bracket 
	    return;
	}
    }
  Item=Item.substr(1,Item.size()-2);
  return;
}

void
Acomp::addTokens(const std::string& Ln) 
  /*!
    Adds a set of tokens
    \param Ln :: String to add
   */
{
  ELog::RegMethod RegA("Acomp","addTokens");

  int numItem;
  size_t iu(0);
  while(iu<Ln.size())
    {
      if (isalpha(Ln[iu]) || Ln[iu]=='%')
	{
	  if (Ln[iu]=='%')
	    {
	      iu++;
	      const size_t Nmove=
		StrFunc::convPartNum(Ln.substr(iu),numItem);
	      if (!Nmove)
		throw ColErr::InvalidLine(Ln,RegA.getFull(),
					  static_cast<size_t>(iu));
	      numItem+=52;
	      iu+=Nmove; 
	    }
	  else
	    {
	      numItem=(islower(Ln[iu])) ?
		static_cast<int>(1+Ln[iu]-'a') :
		static_cast<int>(27+Ln[iu]-'A');
	      iu++;
	    }
	  // iu looks at next item:
	  if (iu<Ln.length() && Ln[iu]=='\'')   // neg item
	    { 
	      numItem*=-1;
	      iu++;
	    }
	  addUnitItem(numItem);
	}
      else
	iu++;
    }
  return;
}

int
Acomp::hasPlus(const std::string& Express)
  /*!
    Determine if there is a union
    \param Express :: Expression to search
    \return true if uniion exists
  */
{
  return (Express.find('+')!=std::string::npos) ? 1 : 0;
}

size_t
Acomp::checkWrongChar(const std::string& Ln) 
  /*!
    Determine if the string in mal-formed
    \param Ln :: line
    \return position of string  +1
  */
{
  ELog::RegMethod RegA("Acomp","checkWrongChar");

  int percent(0);
  for(size_t i=0;i<Ln.size();i++)
    {
      // If % 
      if (Ln[i]=='%')
	{
	  if (percent==1) return i+1;  // fail %%
	  percent=1;
	}
      else if (isdigit(Ln[i]))     // follows %/digit
	{
	  if (!percent) return i+1;
	  percent=2;
	}
      // all other fail modes:
      else if (!isalpha(Ln[i]) && Ln[i]!='+' &&  
	       Ln[i]!=':' && Ln[i]!='(' && 
	       Ln[i]!=')' && Ln[i]!='\'')
	return i+1;
      else // basic item
	percent=0;
    }

  return (percent==1) ? Ln.size()-1 : 0;
}

std::string
Acomp::mapLogic(const std::map<int,int>& M)
  /*!
    Form a string of the logical state
    \param M :: Map
    \return :: letter map
  */
{
  std::ostringstream cx;
  for(const std::map<int,int>::value_type& mc : M)
    {
      const int MVal=(mc.second==0) ? -mc.first : mc.first;
      cx<<Acomp::strUnit(MVal)<<" ";
    }
  return cx.str();
}

int
Acomp::unitStr(std::string SR)
  /*!
    Convert a string into a literal
    \param rule to convert
  */
{
  if (SR.empty()) return 0;
  int index;
  if (SR[0]=='%')
    {
      SR[0]=' ';
      const size_t id=StrFunc::convPartNum(SR,index);
      if (id)
	{
	  if (SR.size()!=id && SR[id-1]=='\'')
	    index*= -1;
	}
      return index;
    }
  index=static_cast<int>(StrFunc::revAlphaToIndex(SR[0])+1);
  if (SR.size()>1 && SR[1]=='\'')
    index*= -1;
  return index;
}
  
std::string
Acomp::strUnit(const int IC)
  /*!
    Place a index into a letter for easier viewing
    \param IC :: Index 
    \return :: letter index
  */
{
  std::ostringstream cx;
  int sign,val;
  signSplit(IC,sign,val);
  if (val>52)
    cx<<"%"<<val-52;
  else
    cx<<StrFunc::indexToRevAlpha(val-1);
  
  if (sign<0) cx<<'\'';
  return cx.str();
}
  
std::string
Acomp::unitsDisplay() const
  /*!
    Real pretty print out statement  
    \returns Head only string of the output in abc+efg type form
  */
{
  std::ostringstream cx;

  std::string unionPlus;
  
  for(const int IC : Units)
    {
      cx<<unionPlus;
      cx<<strUnit(IC);
      if (!Intersect) unionPlus="+";
    }
  return cx.str();
}

std::string
Acomp::compDisplay(std::string unionPlus) const
  /*!
    Real pretty print out statement  
    \param unionPlus :: extra join flag
    \returns Just the component part
  */
{
  // Now do composites
  std::ostringstream cx;
  for(const Acomp& AC : Comp)
    {
      cx<<unionPlus;
      if ( !AC.Intersect )           // UNION
	cx<<'('<<AC.display()<<')';
      else
	cx<<AC.display();
     if (!Intersect) unionPlus="+";
    }
  return cx.str();
}

std::string
Acomp::display() const
  /*!
    Real pretty print out statement  
    \returns Full string of the output in abc+efg type form
  */
{
  std::stringstream cx;
  
  cx<<unitsDisplay();
  if ( Intersect==0 && !Units.empty() )
    cx<<compDisplay("+");
  else
    cx<<compDisplay();
  
    
  return cx.str();
}

std::string
Acomp::displayDepth(const int dval) const
  /*!
    Real pretty print out statement  :-)
    \param dval :: parameter to keep track of depth
    \returns Full string of print line
  */
{
  std::stringstream cx;
  std::vector<int>::const_iterator ic;


  cx<<unitsDisplay();

  // Now do composites
  std::string unionPlus;
  if ( Intersect==0 && !Units.empty() )
    unionPlus="+";

  for(const Acomp& AC : Comp)
    {

      cx<<unionPlus;
      //      if ( join && (*vc)->type() )
      if ( !AC.Intersect )
	cx<<"D["<<dval<<"][U]"
	  <<'('<<AC.displayDepth(dval+1)<<')';
      else
	cx<<"D["<<dval<<"][I]"
	  <<AC.displayDepth(dval+1);

      cx<<" EEE["<<dval<<"]";
     if (!Intersect) unionPlus="+";
    }
  return cx.str();
}



} // NAMESPACE MonteCarlo
