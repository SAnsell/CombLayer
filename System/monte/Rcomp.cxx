/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Rcomp.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "RotCounter.h"
#include "BnId.h"
#include "Rcomp.h"

namespace MonteCarlo 
{

std::ostream&
operator<<(std::ostream& OX,const Rcomp& A) 
  /*!
    Stream operator (uses Rcomp::write)
    \param OX :: output stream
    \param A :: Rcomp unit to output
    \returns Output stream
  */
   
{
  OX<<A.display();
  return OX;
}
 

//
// ------------- RCOMP ---------------- 
//

Rcomp::Rcomp(const JoinForm Tx) :
  Intersect((Tx==Union) ? 0 : 1)
  /*!
    Standard Constructor 
    \param Tx :: 1 it means intersect 0 it meanse union
  */
{}

Rcomp::Rcomp(const Rcomp& A) :
  Intersect(A.Intersect),Units(A.Units),
  Comp(A.Comp)
  /*!
    Copy constructor
    \param A :: Rcomp object to copy
  */
{ }

Rcomp&
Rcomp::operator=(const Rcomp& A)
  /*!
    The assignment operator carries out a 
    down level deletion of the Comp vector
    \param A :: Object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      Intersect=A.Intersect;
      Units=A.Units;
      Comp=A.Comp;
    }
  return *this;
}


Rcomp::~Rcomp()
  /*!
    Destructor deletes each Comp Unit and
    down the chain.
  */
{ }

int
Rcomp::operator!=(const Rcomp& A) const
  /*!
    Inequality operator
    \param A :: other Rcomp item to compare
    \returns Not this==A
  */
{
  return 1-(this->operator==(A));
}

int
Rcomp::operator==(const Rcomp& A) const
  /*!
    Equals operator requires that the
    Units are equal and the Comp units
    are equal. Sort of the class is required.
    \param A :: other Rcomp item to compare
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
  if (!Units.empty())
    {
      std::vector<int>::const_iterator vc,xc;
      xc=A.Units.begin();
      for(vc=Units.begin();vc!=Units.end();xc++,vc++)
	if (*vc != *xc)
	  return 0;
    }

  // Both Empty :: thus equal
  if (Comp.empty())    
    return 1;

  // Assume that comp Units are sorted.
  std::vector<Rcomp>::const_iterator acv,xcv;
  acv=A.Comp.begin();
  for(xcv=Comp.begin();xcv!=Comp.end() &&
	*xcv==*acv ;xcv++,acv++) ;
  return (xcv==Comp.end()) ? 1 : 0;
}

int
Rcomp::operator<(const Rcomp& A) const
  /*!
    Comparitor operator:: Comparies the 
    unit list (which is already sorted)
    part by part.  The sort is ASSUMED.
    Then the Comp units (which are not)
    Order (low first)
      - Singulars
      - Intersections
      - Units (largest negative)
      - Rcomps.
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
  std::vector<int>::const_iterator uc,ac;
  ac=A.Units.begin();
  uc=Units.begin();
  for(;ac!=A.Units.end() && uc!=Units.end();uc++,ac++)
    if (*uc!=*ac)
      return (*uc < *ac);

  if (ac!=A.Units.end())
    return 0;
  if (uc!=Units.end())
    return 1;

  // PROCESS CompUnits.
  std::vector<Rcomp>::const_iterator ux,ax;
  ux=Comp.begin();
  ax=A.Comp.begin();
  for(;ux!=Comp.end() && ax!=A.Comp.end();ux++,ax++)
    {
      if (*ax!=*ux)
	return (*ux < *ax);
    }
  if (uc!=Units.end())
    return 1;
  // everything idential or A.comp bigger:
  return 0;
}

Rcomp&
Rcomp::operator+=(const Rcomp& A)
  /*!
    Operator + (union addition)
    \param A :: Object to union/or/. with this
    \returns *this
  */
{
  if (Intersect)                // If this is an intersection
    {                            // we need to have a union 
      Rcomp Ax=Rcomp(*this);    //make copy
      Units.clear();                    // remove everthing else
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

Rcomp&
Rcomp::operator-=(const Rcomp& A)
  /*!
    Operator - (removal)
    by complementary subtraction or the remainder.
    Complementary subtraction: is by making A-B == A*(B')
    -- Note : (a+b)-b => 0
    
    \param A :: Object to subtract
    \return *this 
  */
{
  ELog::RegMethod RegA("Rcomp","operator-=");
  
  Rcomp Aprime(A);
  Aprime.complement();
  return Rcomp::operator*=(Aprime);
}

int
Rcomp::operator>(const Rcomp& A) const
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


Rcomp&
Rcomp::operator*=(const Rcomp& A)
  /*!
    This carries out the intersection operation 
    with A. e.g. (a+b) * (ced) == (a+b)ced.
    The equivilent to making the statement [this AND A]
    \param A :: Rcomp unit to intersect
    \returns *this
  */
{
  ELog::RegMethod RegA("Rcomp","operator*");
  if (!Intersect)                // If this is an intersection
    {                            // we need to have a union 
      Rcomp Ax=Rcomp(*this);    //make copy
      Units.clear();                    // remove everthing else
      Comp.clear();
      Intersect=1;                      
      addComp(Ax);                     // add oldThis to the list
    }

  if (A.Intersect)                  // can add components
    {
      copySimilar(A);
    }
  else 
    {
      addComp(A);       // combine components
    }
  removeEqComp();
  joinDepth();  
  return *this;
}

// ---------------------------------------------------
//  PRIVATE FUNCTIONS 
// ---------------------------------------------------

void
Rcomp::deleteComp()
  /*!
    Deletes everything in Composite
  */
{
  Comp.clear();
  return;
}

void
Rcomp::addComp(const Rcomp& AX)
  /*!
    Adds a pointer to the Comp list.
    If the pointer is singular, extracts the
    object and adds that componenet.
    Assumes that the component is sorted and inserts appropiately.
    \param AX :: Rcomp component to add
  */
{
  ELog::RegMethod RegA("Rcomp","addComp");
  
  std::pair<int,int> Stype=AX.size();
  if (Stype.first+Stype.second==0)
    throw ColErr::EmptyValue<size_t>("Pair Count zero");

  if (AX.isSingle() ||
      AX.Intersect==Intersect)       //single unit component/Conjoint
    {
      for(const int aup : AX.Units)
        {
	  std::vector<int>::iterator ipt;
	  ipt=lower_bound(Units.begin(),Units.end(),aup);
	  if (ipt==Units.end() || *ipt!=aup)     // Only insert if new
	    Units.insert(ipt,aup);
	}
      for(const Rcomp& acp : AX.Comp)
        {
	  std::vector<Rcomp>::iterator cpt;
	  cpt=lower_bound(Comp.begin(),Comp.end(),acp);
	  if (cpt==Comp.end() || *cpt!=acp)       // Only insert if new
	    Comp.insert(cpt,acp);
	}
      return;
    }
  // Type different insertion
  std::vector<Rcomp>::iterator cpt;
  cpt=lower_bound(Comp.begin(),Comp.end(),AX);
  if (cpt==Comp.end() || *cpt!=AX)                       // Only insert if new
    Comp.insert(cpt,AX);
  return;
}

void
Rcomp::addUnitItem(const int Item)
  /*!
    Adds a unit to the Unit list (if it doesn't exist). 
    \param Item :: Unit to add
  */
{
  // Quick and cheesy insertion if big.
  std::vector<int>::iterator ipt;
  ipt=lower_bound(Units.begin(),Units.end(),Item);
  if (ipt==Units.end() || *ipt!=Item)                       // Only insert if new
    Units.insert(ipt,Item);
  return;
}




int
Rcomp::copySimilar(const Rcomp& A)
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

  if (!A.Units.empty())
    {
      Units.insert(Units.end(),A.Units.begin(),A.Units.end());
      sort(Units.begin(),Units.end());
    }

  // Add components 
  std::vector<Rcomp>::const_iterator vc;
  for(vc=A.Comp.begin();vc!=A.Comp.end();vc++)
    addComp(*vc);
  return 0;
}



// -------------------------------------
//   PUBLIC FUNCTIONS 
// -------------------------------------

void
Rcomp::Sort()
  /*!
    Function to sort the components of the lists.
    Decends down the Comp Tree.
  */
{
  std::sort(Units.begin(),Units.end());
  // Sort each decending object first
  for_each(Comp.begin(),Comp.end(),std::mem_fun_ref(&Rcomp::Sort));
  // use the sorted components to sort our component list
  std::sort(Comp.begin(),Comp.end());
  return;
}
  
int
Rcomp::isNull() const
  /*!
    \returns 1 if there are no memebers
  */
{
  return ((!Units.size() && !Comp.size()) ? 1 : 0);
}

int
Rcomp::isSimple() const
  /*!
    Determines if there are not complex components.
    \retval 1 :: Comp is empty
    \retval 0 :: Contains Components.
  */
{
  return (Comp.empty() ? 1 : 0);
}

int
Rcomp::isSingle() const
  /*!
    Deterimines if the item's singular
    \retval 1 if only one/zero item
    \retval 0 if more than one item.
  */
{
  return (Comp.size()+Units.size()>1 ? 0 : 1);
}


int
Rcomp::removeEqComp()
  /*!
    Remove identical items.
    Items need to be at the same base level
    \returns number removed.
  */
{
  ELog::RegMethod RegA("Rcomp","removeEqComp");
  // First check all the Comp units:
  long int cnt(0);
  std::vector<Rcomp>::iterator dx;
  sort(Comp.begin(),Comp.end());
  dx=unique(Comp.begin(),Comp.end());
  cnt+=std::distance(dx,Comp.end());
  Comp.erase(dx,Comp.end());

  // Units are sorted

  sort(Units.begin(),Units.end());
  std::vector<int>::iterator ux=unique(Units.begin(),Units.end());
  cnt+=std::distance(ux,Units.end());
  Units.erase(ux,Units.end());
  return static_cast<int>(cnt);
}

void
Rcomp::getLiteralsFreq(std::map<int,int>& literalMap) const
  /*!
    get a map of the literals and the frequency
    that they occur.
    This keeps the +/- part of the literals separate
    \param literalMap :: Map the get the frequency of the 
    literals 
  */
{
  for(const int UI : Units)
    {
      std::map<int,int>::iterator mc=
	literalMap.find(UI);
      if (mc!=literalMap.end())
	mc->second++;
      else
	literalMap.emplace(UI,1);
    }
  for(const Rcomp& AC : Comp)
    AC.getLiteralsFreq(literalMap);

  return;
}

void
Rcomp::getLiterals(std::set<int>& literals) const
  /*!
    Get a map of the literals and the frequency
    that they occur.
    This keeps + and - literals separate
    \param literals :: Set of literals
  */
{
  for(const int UI : Units)
    literals.insert(UI);

  for(const Rcomp& AC : Comp)
    AC.getLiterals(literals);

  return;
}


  
std::vector<int>
Rcomp::getKeys() const
  /*!
    Get the key numbers in the system
    \return Key of literals
  */
{
  std::set<int> litMap;       // keynumber :: number of occurances
  getLiterals(litMap);
  return std::vector<int>(litMap.begin(),litMap.end());
}


int
Rcomp::isTrue(const std::map<int,int>& Base) const
  /*!
    Determines if the rule is true, given
    the Base state.
    \param Base :: map of <Surface, State> 
      -- this uses 1 : true  : 0 don't care : -1 false 
    \returns 1 if could be true and 0 if definately false
    \note : Both parts of this function can be merged
     but it makes debugging a nightmare
  */
{
  ELog::RegMethod RegA("Rcomp","isTrue");
  
  if (Units.empty() &&  Comp.empty())
    return 1;
  //
  // Process units first:
  if (Intersect)
    {
      for(const int SN : Units)
	{
	  const int USN(std::abs(SN));
	  std::map<int,int>::const_iterator bc=
	    Base.find(USN);
	  if (bc==Base.end())
	    throw ColErr::InContainerError<int>(SN,"Unit not found");
	  if (bc->second*SN<0)
	    return 0;
	}
      for(const Rcomp& RC : Comp)
	if (!RC.isTrue(Base))
	  return 0;

      return 1;
    }

  // UNION:
  for(const int SN : Units)
    {
      const int USN(std::abs(SN));
      std::map<int,int>::const_iterator bc=
	Base.find(USN);
      if (bc==Base.end())
	throw ColErr::InContainerError<int>(SN,"Unit not found");
      if (bc->second*SN>=0)    // don't care can be 1/0
	return 1;
    }
  for(const Rcomp& RC : Comp)
    if (RC.isTrue(Base))
      return 1;
  
  return 0;
}


int 
Rcomp::contains(const Rcomp& A) const
  /*!
    Checks the Units of A to  see if they are in this->Units.
    Assumes that Units is sorted.
    \param A :: Object to cross compare
    \retval 0 :: all literals in A are in this
    \retval 1 :: A is unique from this
  */
{
  std::vector<int>::const_iterator vc,tc;
  tc=Units.begin();
  for(vc=A.Units.begin();vc!=A.Units.end();vc++)
    {
      while(tc!=Units.end() && *tc<*vc)
	tc++;
      if (tc==Units.end() || *tc!=*vc)
	return 0;
    }
  return 1;
}

int
Rcomp::joinDepth()
  /*!
    Searchs down the tree to find if any
    singles exist and up-promotes them.
    \throw ColErr::ExBase on mal-formed state
    \returns number of up-promotions.
  */
{
  ELog::RegMethod RegA("Rcomp","joinDepth");
  
  // Deal with case that this is a singular object::
  const std::pair<int,int> topSize(this->size());
  if ((topSize.first+topSize.second)==0)
    throw ColErr::ExBase(-2,"Pair Count wrong");
   //SINGULAR
  int cnt(0);        // number upgraded
  if (topSize.first==0 && topSize.second==1)            // Comp to up-premote
    {
      const Rcomp* Lower=itemC(0);                    // returns pointer
      Units.clear();
      Intersect=Lower->Intersect;
      Units=Lower->Units;
      if (!Lower->Comp.empty())
        {
	  std::vector<Rcomp> tmpComp=Lower->Comp;
	  Comp=tmpComp;
	}
      cnt++;
    }
  // Loop over each component.  (if identical type upshift and remove)
  for(size_t ix=0;ix<Comp.size();ix++)
    {
      const Rcomp& AX=Comp[ix];
      const std::pair<int,int> compSize(AX.size());
      if ((compSize.first+compSize.second)==0)      
	throw ColErr::ExBase(-2,"Pair Count wrong");

      // If Singular then can be up premoted.
      if (compSize.first==1 && compSize.second==0)         // UNITS only
	{
	  Units.push_back(AX.itemN(0));
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
	  Units.insert(Units.end(),AX.Units.begin(),AX.Units.end());
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
  for(Rcomp& RC : Comp)
    cnt+=RC.joinDepth();

  return cnt;
}


std::pair<int,int>
Rcomp::size() const
  /*!
    Gets the size of the Units and the Comp
    \returns size of Unit, Comp
  */
{
  return std::pair<int,int>(static_cast<int>(Units.size()),
			    static_cast<int>(Comp.size()));
}

int
Rcomp::itemN(const size_t Index) const
  /*!
    Assessor function to get a unit number
    \param Index :: Number of Unit to aquire
    \returns Units[Index] or 0 on failure
  */
{
  if (Index<Units.size())
    return Units[Index];
  return 0;
}

const Rcomp*
Rcomp::itemC(const size_t Index) const
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
Rcomp::complement() 
  /*!
    Take a complement of the current object
    This will reverse the type since 
    union<->intersection as a+b -> a'b' and
    ab -> a'+b'
  */
{
  ELog::RegMethod RegA("Rcomp","complement");
  Intersect=1-Intersect;
  transform(Units.begin(),Units.end(),
            Units.begin(),std::bind2nd(std::multiplies<int>(),-1) );
  sort(Units.begin(),Units.end());      /// Resort the list. use reverse?

  for_each(Comp.begin(),Comp.end(),
	    std::mem_fun_ref(&Rcomp::complement) );
  sort(Comp.begin(),Comp.end());
  return;
}

void
Rcomp::writeFull(std::ostream& OX,const size_t Indent) const
  /*!
    Real pretty print out statement  :-)
    \param OX :: output stream
    \param Indent :: level of indentation (allows a cascaded call system)
  */
{
  OX<<std::string(Indent,' ');
  OX<<((Intersect==1) ? "Inter" : "Union");
  OX<<" "<<Units.size()<<" "<<Comp.size()<<std::endl;
  OX<<std::string(Indent,' ');
  OX<<display()<<std::endl;
  for(const Rcomp& RC : Comp)
    RC.writeFull(OX,Indent+2);

  return;
}

std::string
Rcomp::display() const
  /*!
    Real pretty print out statement  
    \returns Full string of the output in abc+efg type form
  */
{
  std::stringstream cx;

  for(const int SN : Units)
    {
      cx<<
      if (!Intersect && ic!=Units.begin())
	cx<<'+';
      std::set<int>
  for(ic=Units.begin();ic!=Units.end();ic++)
    {
      signSplit(*ic,sign,val);
      if (val<27)
	cx<<static_cast<char>(static_cast<int>('a')+(val-1));
      else if (val<53)
	cx<<static_cast<char>(static_cast<int>('A')+(val-27));
      else
	cx<<"%"<<val-52;
      if (sign<0)
	cx<<'\'';
    }
  // Now do composites
  std::vector<Rcomp>::const_iterator vc;
  for(vc=Comp.begin();vc!=Comp.end();vc++)
    {
      if (!Intersect && (vc!=Comp.begin() || !Units.empty()))
	cx<<'+';
      //      if ( join && (*vc)->type() )
      if ( !vc->Intersect )
	cx<<'('<<vc->display()<<')';
      else
	//	cx<<'('<<vc->display()<<')';
	cx<<vc->display();
    }
  return cx.str();
}


} // NAMESPACE MonteCarlo
