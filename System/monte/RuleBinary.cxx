/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/RuleBinary.cxx
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
#include <climits>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "support.h"
#include "mathSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "BnId.h"
#include "RotCounter.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "RuleBinary.h"


std::ostream&
operator<<(std::ostream& OX,const RuleBinary& A)
  /*!
    Write out to a stream
    \param OX :: Stream
    \param A :: RuleBinary Object to write
    \return Stream state
   */
{
  A.write(OX);
  return OX;
}

// --------------------------------------
//        STATIC FUNCTIONS 
//----------------------------------------
int
RuleBinary::addBinCounter(std::vector<size_t>& Index,
			  const size_t N,size_t& I) 
  /*!
    Static funciton::
    Objective is a rooling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \param Index :: Variables to rotate
    \param N :: Depth to go to
    \param I :: Index depth that the function achieved
    \returns 1 :: on loop over
  */
{
  size_t Npart=N-1;
  for(I=Index.size();I>0 &&
	Index[I-1]==Npart;I--,Npart--) ;
  if (!I) return 1;

  Index[I-1]++;
  for(;I<Index.size();I++)
    Index[I]=Index[I-1]+1;
  return 0;
}

// --------------------------------------
//        REAL FUNCTIONS 
//----------------------------------------

RuleBinary::RuleBinary() :
  TopRule(0)
  /*!
    Default constructor
  */
{}

RuleBinary::RuleBinary(Rule* Rval) :
  TopRule(Rval)
  /*!
    Constructor
    \param Rval :: Top of tree object for a basic tree rule
  */
{}

RuleBinary::RuleBinary(const RuleBinary& A) :
  TopRule(A.TopRule),SurfMap(A.SurfMap),
  SurfKeys(A.SurfKeys),DNFobj(A.DNFobj),
  EPI(A.EPI)
  /*!
    Copy constructor
    \param A :: RuleBinary object to copy
  */
{}

RuleBinary&
RuleBinary::operator=(const RuleBinary& A) 
  /*!
    Assignment operator
    \param A :: RuleBinary object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      TopRule=A.TopRule;
      SurfMap=A.SurfMap;
      SurfKeys=A.SurfKeys;
      DNFobj=A.DNFobj;
      EPI=A.EPI;
    }
  return *this;
}

RuleBinary::~RuleBinary() 
  /*!
    Distructor is not responsible for 
    TopRule.
  */
{}

void
RuleBinary::populateBase(std::map<int,int>& Base) const
  /*!
    Populates a map with the surface names
    such that  Base[SurfName]=position in vector 
    \param Base :: map to be cleared and populatied
  */
{
  Base.erase(Base.begin(),Base.end());
  int cnt(0);
  std::vector<int>::const_iterator vc;
  for(vc=SurfKeys.begin();vc!=SurfKeys.end();vc++)
    Base[*vc]=cnt++;
  return;
}

void
RuleBinary::setBase(std::map<int,int>& Base,const BnId& State) const
  /*!
    Sets the components within base with true/false
    \param Base :: map to be filles
    \param State :: Values to use for the state.
  */
{
  for(size_t i=0;i<SurfKeys.size();i++)
    Base[SurfKeys[i]]=(State[i]==1) ? 1 : 0;
  return;
}


size_t
RuleBinary::makeSurfIndex()
  /*!
    Creates and polulates
    the SurfIndex map
    \returns number of items found
  */
{
  ELog::RegMethod RegA("RuleBinary","makeSurfIndex");

  if (!TopRule) return 0;

  SurfKeys.clear();
  SurfMap.erase(SurfMap.begin(),SurfMap.end());
  // collect base keys and populate the cells
  TopRule->getKeyList(SurfKeys);  
  std::vector<int>::const_iterator xv;
  int index=0;
  for(xv=SurfKeys.begin();xv!=SurfKeys.end();xv++)
    SurfMap[index++]=(*xv);

  return SurfMap.size();
}

size_t
RuleBinary::fillDNFitems(const size_t Vnumber,
			 const std::vector<size_t>& mValues)
  /*!
    Fills tval with the list of items found
    in the mValues into the DNF. Maps surfaces on 
    a 1-1 system
    \param Vnumber :: number of independent variable
    \param mValues :: binary true condition.
    \returns number of items added
  */
{
  if (Vnumber<=0)
    return 0;
  DNFobj.clear();
  std::vector<size_t>::const_iterator vc;
  for(vc=mValues.begin();vc!=mValues.end();vc++)
    DNFobj.push_back(BnId(Vnumber,*vc));
  
  return DNFobj.size();
}

std::vector<int>
RuleBinary::commonFactor(const size_t level) const
  /*! 
    Find common factors of the EPI
    \param level :: number of EPI without a particular key
    \return index of positions with 1 if a True position
    and -1 if a False position.
  */
{
  const size_t Dsize(DNFobj.size());
  std::vector<size_t> Tcount(Dsize);            // create True count
  std::vector<size_t> Fcount(Dsize);            // create False
  std::vector<BnId>::const_iterator ec;     // iterator over EPI
  
  // Search over each epi, keep a score of how many true/false
  // uses of each item
  int status;
  for(ec=EPI.begin();ec!=EPI.end();ec++)
    for(size_t i=0;i<Dsize;i++)            // check each different 
      {
	status=(*ec)[i];
	if (status==1)
	  Tcount[i]++;
	else if (status==-1)
	  Fcount[i]++;
      }

  // Only if Tcount or Fcount is DSize is their a totally common
  // factor. If we are looking for a near common factor, then 
  // level can be set and those items which it occurs are used
  std::vector<int> Out(Dsize);
  for(size_t i=0;i<Dsize;i++)            
    {
      if (Dsize-Tcount[i]==level)
	Out[i]=1;
      else if (Dsize-Fcount[i]==level)
	Out[i]= -1;
    }
  return Out;
}


Rule*
RuleBinary::createTree() const
  /*!
    Build the rule table.
    Assuming that the DNF has been simplified.
    Build Surface numbers for later
    Object with the populate() method
    \returns :: a topTree of the minimised system
  */
{
  ELog::RegMethod RegA("RuleBinary","createTree");

  if (EPI.empty() || SurfKeys.empty())
    return 0;
  std::vector<Rule*> EPIrules;
  Rule* tRule; // Tempory Rule space

  for(const BnId& epi : EPI)
    {
      // Loop over each rule to extract the 
      // intersection components
      std::vector<Rule*> Sitems;      // place for surface items
      for(size_t i=0;i<epi.Size();i++)
	{
	  const int status= epi[i];
	  if (status)
	    {
	      SurfPoint* sRule=new SurfPoint();
	      sRule->setKeyN(status * SurfKeys[i]);
	      Sitems.push_back(sRule);
	    }
	}

      if (Sitems.size()==1)
	EPIrules.push_back(Sitems.front());
      else if (Sitems.size()>1)
	{
	  tRule=new Intersection(Sitems[0],Sitems[1]);
	  for(unsigned si=2;si<Sitems.size();si++)
	    tRule=new Intersection(tRule,Sitems[si]);
	  EPIrules.push_back(tRule);
	}
    }

  // Join up all the EPI rule groups
  if (EPIrules.size()==0)       // No work/no rule
    return 0;
  if (EPIrules.size()==1)
    return EPIrules.front();
  tRule=new Union(EPIrules[0],EPIrules[1]);

  for(size_t si=2;si<EPIrules.size();si++)
    tRule=new Union(tRule,EPIrules[si]);
  return tRule;
}

int
RuleBinary::createDNFitems()
  /*!
    Creates the DNF items (ie the binary list
    of true statements)
    Variable :: DNFobj is filled
    \return size of DNF object found
  */
{
  if (!TopRule || SurfKeys.size()<1)
    return 0;
  DNFobj.clear();
  // Get size from surfkeys

  std::map<int,int> Base;        // keyNumber :: state map
  populateBase(Base);
  BnId State(SurfKeys.size(),0);                 //zero base
  do
    {
      setBase(Base,State);
      if (TopRule->isValid(Base))
	DNFobj.push_back(State);
    } while(++State);
  
  return static_cast<int>(DNFobj.size());
}

int
RuleBinary::group() 
  /*!
    This method finds the principle implicants.
    \returns number of PIs found.
  */
{
  ELog::RegMethod RegA("RuleBinary","group");
  
  if (DNFobj.empty())   // no work to do return.
    return 0;
  // Note: need to store PI components separately
  // since we don't want to loop continuously through them
  std::vector<BnId> Work;       // Working copy
  std::vector<BnId> Comp;       // Store for PI componends
  std::vector<BnId> Tmod;       // store modified components
  int changeCount(0);           // Number change
  std::vector<BnId>::iterator uend;     // itor to remove unique
  // Need to make an initial copy.
  Work=DNFobj;
  do
    {
      sort(Work.begin(),Work.end());
      uend=unique(Work.begin(),Work.end());
      Work.erase(uend,Work.end());

      Tmod.clear();                  // erase all at the start
      //set PI status to 1
      for(BnId& wItem : Work)
	wItem.setPI(1);

      //Collect into pairs
      std::vector<BnId>::iterator vc;
      for(vc=Work.begin();vc!=Work.end();vc++)
	{
	  std::vector<BnId>::iterator oc=vc+1;
	  for(oc=vc+1;oc!=Work.end();oc++)
	    {
	      std::pair<int,BnId> cVal=vc->makeCombination(*oc);
	      if (cVal.first<0)  //Diffs >1 
		break;

	      if (cVal.first==1)   // was complementary
		{
		  Tmod.push_back(cVal.second);
		  oc->setPI(0);         
		  vc->setPI(0);
		  changeCount++;       // 1 changed
		}
	    }
	}	

      for(const BnId& wItem : Work)
	if (wItem.PIstatus()==1)
	  {
	    ELog::EM<<"Comp == "<<wItem<<ELog::endDiag;
	    Comp.push_back(wItem);
	  }
      
      // Now make unique:
      Work=Tmod;
    } while (!Tmod.empty());

  return makeEPI(Comp);
}

int
RuleBinary::makeEPI(std::vector<BnId>& PIform)
  /*!
    Takes a PI list and converts into a Essential prime implicate
    list.
    \param PIform :: List of rules in Prime Implicant form
         It is set on exit.
    \returns :: 1 if successful and 0 if failed
  */
{
  ELog::RegMethod RegA("RuleBinary","makeEPI");
  ELog::EM<<"PIFormt == "<<PIform.size()<<"++"<<ELog::endDiag;
  const int debug(0);
  if (PIform.empty()) return 0;
  EPI.clear(); 
  std::vector<int> EPIvalue;
  // Make zeroed matrix.
  Geometry::MatrixBase<int> 
    Grid(PIform.size(),DNFobj.size());   // Map (efficient??

  
  std::vector<size_t> DNFactive(DNFobj.size());       // DNF that active
  std::vector<size_t> PIactive(PIform.size());        // PI that are active
  std::vector<size_t> DNFscore(DNFobj.size());        // Number in each channel
  
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
	  ELog::EM<<"Error with DNF / EPI determination at "
		  <<ic<<"\n";
	  ELog::EM<<" Items "<<DNFobj[ic]<<ELog::endErr;
	  return 0;
	}
    }
  /// DEBUG PRINT 
  if (debug)
    {
      for(size_t pc=0;pc!=PIform.size();pc++)
	{
	  ELog::EM<<PIform[pc]<<":";
	  for(unsigned int ic=0;ic!=DNFobj.size();ic++)
	    ELog::EM<<((Grid[pc][ic]) ? " 1" : " 0");
	  ELog::EM<<ELog::endDiag;
	}
    }
  /// END DEBUG

  std::vector<size_t>::const_iterator dx;
  std::vector<size_t>::iterator ddx;     // DNF active iterator
  std::vector<size_t>::iterator px;     // PIactive iterator

  // First remove singlets:
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
		  *ddx = ULONG_MAX;      //mark for deletion (later)
	  // Can remove PIactive now.
	      PIactive.erase(px,px+1);
	    }
	}
    }
  // Remove dead items from active list
  DNFactive.erase(
		  remove_if(DNFactive.begin(),DNFactive.end(),
			    std::bind2nd(std::equal_to<size_t>(),0)),
		  DNFactive.end());


  /// DEBUG PRINT 
  if (debug)
    {
      ELog::EM<<"START OF TABLE "<<ELog::endDiag;
      ELog::EM<<"DNF size =="
	      <<DNFactive.size()<<ELog::endTrace;
      ELog::EM<<"Final PIactive size == "
	      <<PIactive.size()<<ELog::endTrace;
      
      for(px=PIactive.begin();px!=PIactive.end();px++)
	{
	  ELog::EM<<PIform[*px]<<":";
	  for(ddx=DNFactive.begin();ddx!=DNFactive.end();ddx++)
	    ELog::EM<<((Grid[*px][*ddx]) ? " 1" : " 0");
	  ELog::EM<<ELog::endTrace;
	}
      ELog::EM<<"END OF TABLE "<<ELog::endDiag;
    }

  // Ok -- now the hard work...
  // need to find shortest "combination" that spans
  // the remaining table.
  
  // First Make a new matrix for speed.  Useful ???
  Geometry::MatrixBase<int>
    Cmat(PIactive.size(),DNFactive.size());  // corrolation matrix
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
      do 
	{
	  size_t di;
	  for(di=0;di<Dsize;di++)   //check each orignal positionx
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
	      PIform=EPI;
	      return 1;
	    }
	} while(!(++Index));
    }

  ELog::EM<<"All are primary implicates "<<ELog::endTrace;
  //OH well that means every PIactive is a EPI :-(
  for(px=PIactive.begin();px!=PIactive.end();px++)
    EPI.push_back(PIform[*px]);
  return 1;
}



void
RuleBinary::write(std::ostream& OX) const
  /*!
    Print to stream 
    \param OX :: ostream parameter
  */
{
  std::vector<BnId>::const_iterator vc;
  int cnt(0);
  OX<<"DNF: ";
  for(const BnId& id : DNFobj)
    {
      if (cnt)
	{
	  OX<<" ^ ";
	  if (!(cnt%4)) OX<<std::endl<<"     ";
	}
      OX<<id.display();
      cnt++;
    }
  OX<<std::endl;
  cnt=0;
  OX<<"EPI: ";
  for(const BnId& epi : EPI)
    {
      if (cnt)
	{
	  OX<<" ^ ";
	  if (!(cnt%4)) OX<<std::endl<<"     ";
	}
      OX<<epi.display();
      cnt++;
    }
  return;
}
