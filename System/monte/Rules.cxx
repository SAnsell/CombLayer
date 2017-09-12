/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Rules.cxx
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
#include <string>
#include <complex>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "MemStack.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Rules.h"

size_t
Rule::addToKey(std::vector<int>& AV,
	       const long int passN)
  /*! 
    Static function ::
    Given a vector AV increase the number from lowest
    to highest in an iterative counting chain.
    (effectively an N-bit binary number++)
    \param AV :: vector of the N-bit array
    \param passN :: number that is to be skipped [-ve for none]
    \retval bit number+1 that was set from 0 to 1
    \retval 0 if the a carry flag is set 
  */
{
  for(size_t i=0;i<AV.size();i++)
    {
      if (passN!=static_cast<long int>(i))
        {
	  if (AV[i]==1)
	    AV[i]=0;
	  else
	    return i+1;
	}
    }
  return 0;
}

const Geometry::Surface*
Rule::checkSurfPoint(std::set<int>& Active,const Rule* RPtr) const
  /*!
    Internal function to check if the Rule Ptr is within 
    the active surface set as an opposite. The 
    Active surf set is 
    \param Active :: Active surface signed map
    \param RPtr :: Rule to check
    \return Surface Pointer
  */
{
  ELog::RegMethod RegA("Rule","checkSurfPoint");

  const SurfPoint* sA=dynamic_cast<const SurfPoint*>(RPtr);
  if (sA)
    {
      const int signSurf=sA->getSign()*sA->getKeyN();
      if (Active.find(-signSurf)!=Active.end() &&
	  Active.find(signSurf)==Active.end())
	{
	  Active.erase(-signSurf);
	  return sA->getKey();
	}
      Active.insert(signSurf);
     }
  return 0;
}

std::set<const Geometry::Surface*>
Rule::getOppositeSurfaces(const Rule* TopRule) const
  /*!
    Calculate if there are any surfaces that are opposite in sign
    \return Set of opposite surfaesc
  */
{
  ELog::RegMethod RegA("Rule","getOppositeSurfaces");
  std::set<const Geometry::Surface*> out;
  if (!TopRule || TopRule->type()==0)     // One element tree (just return)
    return out;
    
  const Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  std::set<int> active;
  // Tree stack of rules
  std::stack<const Rule*> TreeLine;   
  TreeLine.push(TopRule);
  const Geometry::Surface* TSurf;  
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  
      leafA=headPtr->leaf(0);        // get leaves (two of) 
      leafB=headPtr->leaf(1);
      if (leafA)
	TreeLine.push(leafA);
      if (leafB)
	TreeLine.push(leafB);
      if ( (TSurf=checkSurfPoint(active,headPtr)) )
	out.insert(TSurf);
    }
  return out;
}

int
Rule::removeComplementary(Rule*& TopRule)
  /*!
    Given a rule tree remove any parts that 
    are complementary
    \param TopRule :: Effective topRule to change
    \retval 1 :: some simplification
    \retval 0 :: no simplification    
  */
{
  ELog::RegMethod RegA("Rule","removeComplementary");
  // Search down the rule until we get to common
  // group. Once we have found a common type
  // apply simple
 
  if (!TopRule || TopRule->type()==0)     // One element tree (just return)
    return 0; 

  int active(1);             // active units

  Rule *tmpA,*tmpB,*tmpC;       
  // Parent : left/right : Child
  DTriple<Rule*,int,Rule*> TreeComp; 

  while(active)
    {
      active=0;
      // Tree stack of rules
      std::stack<DTriple<Rule*,int,Rule*> > TreeLine;   
      TreeLine.push(DTriple<Rule*,int,Rule*>(0,0,TopRule));

      while (!active && !TreeLine.empty())        // need to exit on active
        {
	  TreeComp=TreeLine.top();
	  TreeLine.pop();	  
	  // get first rule off tree
	  tmpA=TreeComp.third;

	  if (!tmpA->commonType())          // Not a common type, then we must get branches 
	    {
	      tmpB=tmpA->leaf(0);        // get leaves (two of) 
	      tmpC=tmpA->leaf(1);
	      if (tmpB)
		TreeLine.push(DTriple<Rule*,int,Rule*>(tmpA,0,tmpB));
	      if (tmpC)
		TreeLine.push(DTriple<Rule*,int,Rule*>(tmpA,1,tmpC));
	    }
	  else    // Got something to simplify :-)
            {
	      // In the simplification we don't need 
	      // to add anything to the tree. 
	      // This call the appropiate intersection/Union
	      // TrueCount 1 is a simple simplification
	      //           -1 alway true
              //           -2 alway false 
	      const int tcount(tmpA->simplify());  
              
	      if (tcount==1)       // Deep simplification
	        {
		  active=1;
		}
	      else if (tcount== -1)  //replacement simplification
	        {
		  if (TreeComp.first)
		    {
		      TreeComp.first=tmpA->leaf(0);
		      tmpA->setLeaf(0,0);
		      delete tmpA;
		    }
		}
	    }
	}
    }
  return 1;
}

int
Rule::makeCNFcopy(Rule* &TopRule)
  /*!
    Static Function::

    Function to make the tree into CNF
    (conditional normal form) 
    \param TopRule :: Rule to simplify.

    The function makes a complete copy of the system.
    Whilst the 

    We do not use *this :: The reason is that 
    we need to change the toprule type eg from 
    an intersection to a union.
    Since the type of a class can't be changed
    it is necessary to have a static function,
    so that the place that TopRule points is changable

    \return number of changes
  */
{
  ELog::RegMethod RegA("Rules","makeCNFcopy");
  if (!TopRule) return 0;

  // Start at top to tree to find an instance
  // an intersection with stuff below.
  
  int count(0);                 // Number of changes made
  int active(1);                // Do we still have to use 
  Rule *tmpA,*tmpB,*tmpC;       // need three to play
  DTriple<Rule*,int,Rule*> TreeComp;     // Storage for top of stack
  
  /*
    The process works by below value deletion:
    This is when the tree is modified at a particular
    point, each memeber of the tree below the modification
    point is deleted and the modification point is
    replaced with a new version (normally a cloned copy).
    This is fine for small items (eg here the memory footprint
    is only 24 bytes, but not acceptable for bigger items.
    The two layer insertion (which could be done) is more
    complex but better proformance is possible.
  */

  while(active)
    {
      active=0;
      count++;
      // This stack takes a Different Triple < Parent, leaf, Child >
      // We need to store the parent so that when we change the tree
      // of a particular object. The integer records which side
      // of the branch the tree object came from 0 == LHS 1==RHS.

      std::stack<DTriple<Rule*,int,Rule*> > TreeLine;   // Tree stack of rules

      // Start by putting the top item on the Tree Stack.
      // Note that it doesn't have a parent.
      TreeLine.push(DTriple<Rule*,int,Rule*>(0,0,TopRule));   

      // Exit condition is that nothing changed last time 
      // or the tree is Empty. 

      while(!active && !TreeLine.empty())
        {
	  // Ok get and remvoe the top item from the stack.
	  TreeComp=TreeLine.top();  
	  TreeLine.pop();           
	  
	  // Get the item. (not its parent)
	  tmpA=TreeComp.third;       // get base item
	  // Now it might be a Union or Intersection 
	  // so we need to get is branches.
	  // If it is a surface item then it will not have
	  // branches and thus tmpB and tmpC will == 0
	  tmpB=tmpA->leaf(0);        // get leaves (two of) 
	  tmpC=tmpA->leaf(1);

	  // If either then we need to put copies on the stack
	  // for later consideration. 
	  // tmpA is the parent, since it is the leaves of tmpA the
	  // are written into tmpB and tmpC
	  if (tmpB)
	    TreeLine.push(DTriple<Rule*,int,Rule*>(tmpA,0,tmpB));
	  if (tmpC)
	    TreeLine.push(DTriple<Rule*,int,Rule*>(tmpA,1,tmpC));

	  // 
	  //  Time to see if we can apply rule 4 (propositional calculus)
	  //  to expand (a ^ b) v c to (a v c) ^ (b v c)
	  //
	  if (tmpA->type()==1 && tmpB && tmpC)            // it is an intersection otherwise no work to do
	    {
	      // require either the left or right to be unions.
	      if (tmpB->type()== -1 ||
		  tmpC->type()== -1)     // this is a union expand....
	        {
		  Rule* partReplace;               
		  Rule* alpha,*beta,*gamma;
		  if (tmpB->type()== -1)    // ok the LHS is a union. (a ^ b) v g ==> (a v g) ^ (b v g )
		    {
		      // Make copies of the Unions leaves (allowing for null union)
		      alpha= (tmpB->leaf(0)) ? tmpB->leaf(0)->clone() : 0;  
		      beta= (tmpB->leaf(1)) ? tmpB->leaf(1)->clone() : 0;    
		      gamma=  tmpC->clone(); 
		    } 
		  else     // RHS a v (b ^ g) ==> (a v b) ^ (a v g )
		    {
		      // Make copies of the Unions leaves (allowing for null union)
		      // Note :: alpha designated as beta , gamma plays the role of alpha
		      // in the RHS part of the above equation (allows common replace
                      // block below.
		      alpha= (tmpC->leaf(0)) ? tmpC->leaf(0)->clone() : 0;  
		      beta=  (tmpC->leaf(1)) ? tmpC->leaf(1)->clone() : 0;     
		      gamma=  tmpB->clone(); 
		    }
		  // Have bit to replace 

		  // Note:: no part of this can be memory copy
		  // hence we have to play games with a second
		  // gamma->clone()
		  partReplace=new Union(
		    new Intersection(alpha,gamma),
		    new Intersection(beta,gamma->clone()));
		  //
		  //General replacement
		  //
		  if (TreeComp.first)            // Not the top rule (so replace parents leaf)
		    TreeComp.first->setLeaf(partReplace,TreeComp.second);
		  else
		    // It is the top rule therefore, replace the toprule
		    TopRule=partReplace;  

		  // Clear up the mess and delete the rule that we have changed
		  delete tmpA;          
		  // Now we have to go back to the begining again and start again.
		  active=1;            // Exit loop
		}
	    }
	}
    }
  return count-1;  //return number of changes 
}

int
Rule::makeCNF(Rule* &TopRule)
  /*!
    Static Function::

    Function to make the tree into CNF
    (conditional normal form) 
    \param TopRule :: Rule to simplify.

    We do not use *this :: The reason is that 
    we need to change the toprule type e.g. from 
    an intersection to a union.
    Since the type of a class can't be changed
    it is necessary to have a static function,
    so that the place that TopRule points is changable

    \retval 0 on failure
    \retval 1 on success
  */
{
  // Start at top to tree to find an instance
  // an intersection with stuff below.

  ELog::RegMethod RegA("Rules","makeCNF");
  if (!TopRule)
    return 0;

  TopRule->makeParents();
  int count(0);                 // Number of changes made
  int active(1);                // Do we still have to use 
  Rule *tmpA,*tmpB,*tmpC;       // need three to play
  
  while(active)
    {
      active=0;
      count++;
      std::stack<Rule*> TreeLine;   // Tree stack of rules

      // Start by putting the top item on the Tree Stack.
      // Note that it doesn't have a parent.
      TreeLine.push(TopRule);   

      // Exit condition is that nothing changed last time 
      // or the tree is Empty. 
      if (!TopRule->checkParents())
	ELog::EM<<"Parents False"<<ELog::endErr;
      while(!active && !TreeLine.empty())
        {
	  // Ok get and remvoe the top item from the stack.
	  tmpA=TreeLine.top();  
	  TreeLine.pop();           

	  // Now it might be a Union or Intersection 
	  // so we need to get is branches.

	  tmpB=tmpA->leaf(0);        // get leaves (two of) 
	  tmpC=tmpA->leaf(1);

	  if (tmpB)
	    TreeLine.push(tmpB);
	  if (tmpC)
	    TreeLine.push(tmpC);

	  // 
	  //  Time to see if we can apply rule 4 (propositional calculus)
	  //  to expand (a ^ b) v c to (a v c) ^ (b v c)
	  //
	  if (tmpA->type()==1 && tmpB && tmpC)            // it is an intersection otherwise no work to do
	    {
	      // require either the left or right to be unions.
	      if (tmpB->type()== -1 ||
		  tmpC->type()== -1)     // this is a union expand....
	        {
		  Rule* partReplace;               
		  Rule* alpha,*beta,*gamma,*Uobj;        // Uobj to be deleted
		  if (tmpB->type()== -1)    // ok the LHS is a union. (a ^ b) v g ==> (a v g) ^ (b v g )
		    {
		      // Make copies of the Unions leaves (allowing for null union)
		      alpha= tmpB->leaf(0);
		      beta= tmpB->leaf(1);
		      gamma=  tmpC;
		      Uobj=tmpB;
		    } 
		  else     // RHS a v (b ^ g) ==> (a v b) ^ (a v g )
		    {
		      // Make copies of the Unions leaves (allowing for null union)
		      // Note :: alpha designated as beta , gamma plays the role of alpha
		      // in the RHS part of the above equation (allows common replace
                      // block below.
		      alpha= tmpC->leaf(0);
		      beta= tmpC->leaf(1);
		      gamma=  tmpB;
		      Uobj=tmpC;
		    }
		  // Have bit to replace 

		  // Note:: no part of this can be memory copy
		  // hence we have to play games with a second
		  // gamma->clone()
		  partReplace=new Union(
		    new Intersection(alpha,gamma),
		    new Intersection(beta,gamma->clone()));
		  //
		  //General replacement
		  //
		  Rule* Pnt=tmpA->getParent();                  // parent
		  if (Pnt)       // Not the top rule (so replace parents leaf)
		    {
		      const int leafN=Pnt->findLeaf(tmpA);     
		      Pnt->setLeaf(partReplace,leafN);
		    }
		  else
		    TopRule=partReplace;  

		  // Clear up the mess and delete the rule that we have changes
		  // Set the cutoffs in the correct place to avoid a complete
		  // tree deletion
		  tmpA->setLeaves(0,0);
		  delete tmpA;          
		  Uobj->setLeaves(0,0);
		  delete Uobj;          
		  // Now we have to go back to the begining again and start again.
		  active=1;            // Exit loop
		}
	    }
	}
    }
  return count-1;  //return number of changes 
}

void
Rule::removeLeaf(const int aIndex) 
  /*!
    This calls delete self!
    \param aIndex :: Leaf index
  */
{
  ELog::RegMethod RegA("Rule","removeLeaf");

  const int bIndex((aIndex) ? 0 : 1);
  Rule* LeafA=this->leaf(aIndex);
  Rule* LeafB=this->leaf(bIndex);
  if (LeafA && LeafB)
    {
      Rule* Parent=this->getParent();
      if (Parent)
	{
	  const int pIndex=(Parent->leaf(0)==this) ? 0 : 1;
	  Parent->setLeaf(LeafB,pIndex);
	  this->setLeaf(0,bIndex);  // To avoid deletion
	  delete this;
	  return;
	}
    }
 
  return;
}
	  


int
Rule::removeItem(Rule* &TRule,const int SurfN) 
  /*!
    Given an item as a surface name,
    remove the surface from the Rule tree.
    - If the found leaf is on a
    \param TRule :: Top rule to down search
    \param SurfN :: Surface key number to remove
    \return Number of instances removed
  */
{
  ELog::RegMethod RegA("Rules","removeItem");
  ELog::EM<<"THIS CODE IS BRIOKEN DONT USE UNLESS DEBUGGING IT"<<ELog::endErr;
  int cnt(0);
  Rule* Ptr=TRule->findKey(SurfN);
  while(Ptr)
    {	    
      Rule* LevelOne=Ptr->getParent();   // Must work
      Rule* LevelTwo=(LevelOne) ? LevelOne->getParent() : 0;

      if (LevelTwo)                     /// Not the top level
        {
	  // Decide which of the pairs is to be copied
	  Rule* PObj= (LevelOne->leaf(0)!=Ptr) ?
	    LevelOne->leaf(0) : LevelOne->leaf(1);
	  // 
	  LevelOne->setLeaves(0,0);          // Delete from Ptr, and copy
	  const int side=(LevelTwo->leaf(0)!=LevelOne) ? 1 : 0;
	  LevelTwo->setLeaf(PObj,side);
	}
      else if (LevelOne)                // LevelOne is the top rule
        {
	  // Decide which of the pairs is to be copied
	  Rule* PObj= (LevelOne->leaf(0)!=Ptr) ?
	    LevelOne->leaf(0) : LevelOne->leaf(1);

	  PObj->setParent(0);      /// New Top rule
	  TRule=PObj;
	  LevelOne->setLeaves(0,0);          // Delete from Ptr, and copy
	  // Note we now need to delete this 
	}
      else  // Basic surf object
        {
	  SurfPoint* SX=dynamic_cast<SurfPoint*>(Ptr);
	  if (!SX)
	    throw ColErr::CastError<void>
	      (0,std::string("Failure to cast surfPtr:")+Ptr->display());
	  SX->setKeyN(0);
	  SX->setKey(0);
	  return cnt+1;
	}
      delete Ptr;
      delete LevelOne;
      Ptr=TRule->findKey(SurfN);
      cnt++;
    }
  return cnt;
}

Rule::Rule()  : Parent(0)
  /*!
    Standard Constructor
  */
{
  ELog::MemStack::Instance().
    addMem("Rule",ELog::RegMethod::getBase(),
	   reinterpret_cast<size_t>(this));
}

Rule::Rule(const Rule&) : 
  Parent(0)
  /*!
    Constructor copies. 
    Parent set to 0
  */
{
  ELog::MemStack::Instance().
    addMem("Rule",ELog::RegMethod::getBase(),
	   reinterpret_cast<size_t>(this));
}

Rule::Rule(Rule* A) : 
  Parent(A)
  /*!
    Constructor copies. 
    Parent set to A
    \param A :: Parent value
  */
{
 ELog::MemStack::Instance().
    addMem("Rule",ELog::RegMethod::getBase(),
	   reinterpret_cast<size_t>(this));
}

Rule&
Rule::operator=(const Rule&) 
  /*!
    Assignment operator=
    does not set parent as Rules
    are cloned 
    \return *this
  */
{
  return *this;
}

Rule::~Rule()
  /*!
    Destructor
  */
{
 ELog::MemStack::Instance().
   delMem(reinterpret_cast<size_t>(this));
}

void
Rule::setParent(Rule* A)
  /*!
    Sets the parent object (not check for A==this)
    \param A :: Partent Object Ptr 
  */
{
  Parent=A;
}

Rule*
Rule::getParent() const
  /*!
    Returns the parent object 
    \return Parent
  */
{
  return Parent;
}

void
Rule::makeParents()
  /*!
    This is initialisation code that populates 
    all the parents in the rule tree.
  */
{
  ELog::RegMethod RegA("Rules","makeParents");
  std::stack<Rule*> Tree;
  Tree.push(this);
  while(!Tree.empty())
    {
      Rule* Ptmp=Tree.top();
      Tree.pop();

      if (Ptmp)
        {
	  for(int i=0;i<2;i++)
	    {
	      Rule* tmpB=Ptmp->leaf(i);
	      if (tmpB)
	        {
		  tmpB->setParent(Ptmp);
		  Tree.push(tmpB);
		}
	    }
	}
    }
  return;
}

int
Rule::checkParents() const
  /*!
    This code checks if a parent tree
    is valid
    \retval 0 on failure 
    \retval 1 on success
  */
{
  std::stack<const Rule*> Tree;
  Tree.push(this);
  while(!Tree.empty())
    {
      const Rule* Ptmp=Tree.top();
      Tree.pop();

      if (Ptmp)
        {
	  for(int i=0;i<2;i++)
	    {
	      const Rule* tmpB=Ptmp->leaf(i);
	      if (tmpB)
	        {
		  if (tmpB->getParent()!=Ptmp)
		    return 0;
		}
	    }
	}
    }
  return 1;
}

int
Rule::commonType() const
  /*!
    Function to return the common type
    of an rule.
    \retval 1 :: every rule is an intersection or component
    \retval -1 :: every rule is an union or component
    \retval 0 :: mixed rule group or only component
  */
{
  ELog::RegMethod RegA("Rules","commonType");

  // initial type
  const int rtype = this->type();       // note the dereference to get non-common comonents
  if (!rtype)
    return 0;
  // now this must be an intersection or a Union   
  std::stack<const Rule*> Tree;
  Tree.push(this->leaf(0));
  Tree.push(this->leaf(1));
  while(!Tree.empty())
    {
      const Rule* tmpA=Tree.top();
      Tree.pop();
      if (tmpA)
        {
	  if (tmpA->type() == -rtype)      // other type return void
	    return 0;           
	  const Rule* tmpB=tmpA->leaf(0);
	  const Rule* tmpC=tmpA->leaf(1);
	  if (tmpB)
	    Tree.push(tmpB);
	  if (tmpC)
	    Tree.push(tmpC);
	}
    }
  return rtype;
}

int
Rule::substituteSurf(const int SurfN,const int newSurfN,
		     const Geometry::Surface* SPtr)
  /*!
    Substitues a surface item if within a rule
    \param SurfN :: Number number to change
    \param newSurfN :: New surface number (if -ve then the key is reversed)
    \param SPtr :: New surface Pointer
    \returns number of substitutions
  */
{
  int cnt(0);
  SurfPoint* Ptr=dynamic_cast<SurfPoint*>(findKey(SurfN));
  while(Ptr)
    {
      Ptr->setKeyN(Ptr->getSign()*newSurfN);
      Ptr->setKey(SPtr);
      cnt++;
      // get next key
      Ptr=dynamic_cast<SurfPoint*>(findKey(SurfN));
    }
  return cnt;
}

size_t
Rule::getKeyList(std::vector<int>& IList)  const
  /*!
    Generate the key list given an insertion 
    type object. The list is a unique list.
    \param IList :: place to put keyList
    \returns number of object inserted 
  */
{
  ELog::RegMethod RegA("Rules","getKeyList");

  IList.clear();
  std::stack<const Rule*> TreeLine;
  TreeLine.push(this);
  while(!TreeLine.empty())
    {
      const Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      const Rule* tmpB=tmpA->leaf(0);
      const Rule* tmpC=tmpA->leaf(1);
      if (tmpB || tmpC)
        {
	  if (tmpB)
	    TreeLine.push(tmpB);
	  if (tmpC)
	    TreeLine.push(tmpC);
	}
      else
        {
	  const SurfPoint* SurX=dynamic_cast<const SurfPoint*>(tmpA);
	  if (SurX)
	    IList.push_back(SurX->getKeyN());
	  else
	    {
	      ELog::EM<<"Error with surface List"<<ELog::endErr;
	      return IList.size();
	    }
	}
    }
  std::sort(IList.begin(),IList.end());
  std::vector<int>::iterator px=std::unique(IList.begin(),IList.end());
  IList.erase(px,IList.end());
  return IList.size();
}

void
Rule::populateSurf()
  /*!
    Create a rules list for the cells
  */
{
  ELog::RegMethod RegA("Rule","populateSurf");

  std::deque<Rule*> Rst;
  Rst.push_back(this);
  Rule* TA, *TB;      //Tmp. for storage
  const ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  while(Rst.size())
    {
      Rule* T1=Rst.front();
      Rst.pop_front();
      if (T1)
        {
	  // if an actual surface process :
	  SurfPoint* KV=dynamic_cast<SurfPoint*>(T1);
	  if (KV)
	    {
	      // Ensure that we have a it in the surface list:
	      Geometry::Surface* SPtr=SurI.getSurf(KV->getKeyN());
	      if (SPtr)
		KV->setKey(SPtr);
	      else 
	        {
		  ELog::EM<<"Error finding key:"<<KV->getKeyN()<<ELog::endCrit;
		  ELog::EM<<"Full Rule ="<<this->display()<<ELog::endCrit;
		  throw ColErr::InContainerError<int>(KV->getKeyN(),"KeyVal");
		}
	    }
	  // Not a surface : Determine leaves etc and add to stack:
	  else
	    {
	      TA=T1->leaf(0);
	      TB=T1->leaf(1);
	      if (TA)
		Rst.push_back(TA);
	      if (TB)
		Rst.push_back(TB);
	    }
	}
    }
  return;
}

std::set<int>
Rule::getSurfSet() const
  /*!
    Create surface ruls list for the cells [signed]
    \return Surface Vector
  */
{
  ELog::RegMethod RegA("Rule","getSurfSet");

  std::set<int> OutSet;

  std::deque<const Rule*> Rst;
  Rst.push_back(this);
  Rule* TA, *TB;      //Tmp. for storage
  while(Rst.size())
    {
      const Rule* T1=Rst.front();
      Rst.pop_front();
      if (T1)
        {
	  // if an actual surface process :
	  const SurfPoint* KV=dynamic_cast<const SurfPoint*>(T1);
	  if (KV)
	    OutSet.insert(KV->getSignKeyN());
	  // Not a surface : Determine leaves etc and add to stack:
	  else
	    {
	      TA=T1->leaf(0);
	      TB=T1->leaf(1);
	      if (TA)
		Rst.push_back(TA);
	      if (TB)
		Rst.push_back(TB);
	    }
	}
    }
  return OutSet;
}

std::vector<const Geometry::Surface*>
Rule::getConstSurfVector() const
  /*!
    Create a rules list for the cells
    \return Surface Vector
  */
{
  ELog::RegMethod RegA("Rule","getConstSurfVector");

  std::vector<const Geometry::Surface*> Out;
  std::vector<Geometry::Surface*> NonConstOut=
    this->getSurfVector();
  std::vector<Geometry::Surface*>::const_iterator vc;
  for(vc=NonConstOut.begin();vc!=NonConstOut.end();vc++)
    Out.push_back(*vc);
  return Out;
}

std::vector<Geometry::Surface*>
Rule::getSurfVector() const
  /*!
    Create a rules list for the cells
    \return Surface Vector
  */
{
  ELog::RegMethod RegA("Rule","getSurfVector");

  std::vector<Geometry::Surface*> Out;

  std::deque<const Rule*> Rst;
  Rst.push_back(this);
  Rule* TA, *TB;      //Tmp. for storage
  const ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  while(Rst.size())
    {
      const Rule* T1=Rst.front();
      Rst.pop_front();
      if (T1)
        {
	  // if an actual surface process :
	  const SurfPoint* KV=dynamic_cast<const SurfPoint*>(T1);
	  if (KV)
	    {
	      // Ensure that we have a it in the surface list:
	      Geometry::Surface* SPtr=
		SurI.getSurf(KV->getKeyN());
	      Out.push_back(SPtr);
	    }
	  // Not a surface : Determine leaves etc and add to stack:
	  else
	    {
	      TA=T1->leaf(0);
	      TB=T1->leaf(1);
	      if (TA)
		Rst.push_back(TA);
	      if (TB)
		Rst.push_back(TB);
	    }
	}
    }
  std::sort(Out.begin(),Out.end());
  Out.erase(std::unique(Out.begin(),Out.end()),Out.end());
  return Out;
}

int
Rule::Eliminate()
  /*!
    This rule eliminates any unnecessary rules.
    \todo Currently does not work since removeItem changes the top rule
    (so should this)
    \return number of items eliminated
  */
{
  ELog::RegMethod RegA("Rules","Eliminate");
  std::map<int,int> Base;     // map of key names + test value (initially 1) 
  std::vector<int> baseVal;   // key values [current]
  std::vector<int> baseKeys;  // key names
  std::vector<int> deadKeys;  
  // collect base keys and populate the cells
  getKeyList(baseKeys);  
  std::vector<int>::const_iterator xv;
  for(xv=baseKeys.begin();xv!=baseKeys.end();xv++)
    {
      baseVal.push_back(0);
      Base[(*xv)]=1;
    }

  // For each key :: check if the Rule is equal for both cases 0 + 1
  // then loop through all combinations of the map to determine validity
  // This function is not optimised since the tree can be trimmed
  // if the test item is not in a branch.
  for(size_t TKey=0;TKey<baseKeys.size();TKey++)
    {
      //INITIALISE STUFF
      int valueTrue(1),valueFalse(1);
      size_t keyChange=0;
      int targetKey=baseKeys[TKey];
      for(size_t i=0;i<baseVal.size();i++)
        {
	  baseVal[i]=0;
	  Base[baseKeys[i]]=0;
	}

      // CHECK EACH KEY IN TURN
      while(valueTrue==valueFalse || keyChange==0)
        {
	  // Zero value
	  Base[targetKey]=0;
	  valueFalse=isValid(Base);

	  // True value
	  Base[targetKey]=1;
	  valueTrue=isValid(Base);

	  // Put everything back 
	  if (valueTrue==valueFalse)
	    {
	      keyChange=addToKey(baseVal,
		  static_cast<long int>(TKey));    // note pass index not key
	      for(size_t ic=0;ic<keyChange;ic++)
		Base[baseKeys[ic]]=baseVal[ic];
	    }
	}	  
      if (keyChange==0)     // Success !!!!!
	deadKeys.push_back(targetKey);
    }

  return static_cast<int>(deadKeys.size());
}


Rule*
Rule::procCompString(const std::string& Line) 
  /*!
    Processes the cell string. This is an function
    to process a string with and make the complement
    of it. Effectively adding #( ) to the string

    Note it is absolutely necessary to managed the object
    \param Line :: String value
    \returns RulePtr on success / 0 on fail
  */
{
  ELog::RegMethod RegA("Rule","procCompString");
  if (StrFunc::isEmpty(Line)) return 0;

  MonteCarlo::Algebra AX;
  AX.setFunctionObjStr("#( "+Line+") ");
  Rule* Ptr=procString(AX.writeMCNPX());
  return Ptr;
}

Rule*
Rule::procString(const std::string& Line) 
  /*!
    Processes the cell string. This is an internal function
    to process a string with 
    - String type has #( and ( )

    Note it is absolutely necessary to managed the object

    \param Line :: String value
    \returns RulePtr on success / 0 on fail
  */
{
  ELog::RegMethod RegA("Rule","procString");

  if (StrFunc::isEmpty(Line)) return 0;
      
  std::map<int,Rule*> RuleList;    //List for the rules 
  int Ridx=0;                     //Current index (not necessary size of RuleList 
  // SURFACE REPLACEMENT
  // Now replace all free planes/Surfaces with appropiate Rxxx
  SurfPoint* TmpR(0);       //Tempory Rule storage position
  CompObj* TmpO(0);         //Tempory Rule storage position

  std::string Ln=Line;
  // Remove all surfaces :

  std::ostringstream cx;
  char hold=' ';
  while(!Ln.empty())
    {
      if (isdigit(Ln[0]) || Ln[0]=='-')
        {
	  int SN;
	  size_t nLen=StrFunc::convPartNum(Ln,SN);
	  if (!nLen)
	    throw ColErr::InvalidLine
	      (Ln,"Failed on number convert ::",Line.size()-Ln.size());

	  // Process #Number
	  if (hold=='#')
	    {
	      TmpO=new CompObj();
	      TmpO->setObjN(SN);
	      RuleList[Ridx]=TmpO;
	      hold=' ';
	    }
	  else if (hold=='%')          // container rule
	    {
	      ContObj* TmpC=new ContObj();
	      TmpC->setObjN(SN);
	      RuleList[Ridx]=TmpC;
	      hold=' ';
	    }
	  else       // Normal rule
	    {
	      TmpR=new SurfPoint();
	      TmpR->setKeyN(SN);
	      RuleList[Ridx]=TmpR;
	    }

	  cx<<hold<<" R"<<Ridx<<" ";
	  Ridx++;
	  Ln.erase(0,nLen);
	  hold=' ';
	}
      else
	{
	  cx<<hold;
	  hold=Ln[0];
	  Ln.erase(0,1);
	}
    }
  cx<<hold;

  Ln=cx.str();
  int brack_exists=1;
  while (brack_exists)
    {
      std::string::size_type rbrack=Ln.find(')');
      std::string::size_type lbrack=Ln.rfind('(',rbrack);  
      if (rbrack!=std::string::npos && lbrack!=std::string::npos)    
        {
	  std::string Lx=Ln.substr(lbrack+1,rbrack-lbrack-1);
	  // Check to see if a #( unit 
	  int compUnit(0);
	  procPair(Lx,RuleList,compUnit);
	  Ln.replace(lbrack,1+rbrack-lbrack,Lx);     
	  // Search back and find if # ( exists.
	  size_t hCnt;
	  for(hCnt=lbrack;hCnt>0 && isspace(Ln[hCnt-1]);hCnt--) ;
	  if (hCnt && Ln[hCnt-1]=='#')
  	    {
	      hCnt--;
	      RuleList[compUnit]=procComp(RuleList[compUnit]);
	      Ln.erase(hCnt,lbrack-hCnt);
	    }
	  else if (hCnt && Ln[hCnt-1]=='%')
   	    {
	      hCnt--;
	      // Contained within code: NOT FINISHED:
	      ELog::EM<<"In % "<<Ln.substr(hCnt,lbrack-hCnt)<<ELog::endErr;
	      throw ColErr::ExitAbort("Unfinished container");
	      //	      RuleList[compUnit]=procComp(RuleList[compUnit]);
	      //	      Ln.erase(hCnt,lbrack-hCnt);
	    }
	}
      else
	brack_exists=0;
    }
  // Do outside loop...

  int nullInt;
  procPair(Ln,RuleList,nullInt);
  if (RuleList.size()!=1)
    {
      ELog::EM<<"Map size not equal to 1 == "<<RuleList.size()<<ELog::endErr;
      ELog::EM<<"Error Object::ProcString : "<<Ln<<ELog::endErr;
      return 0;
    }  
  return RuleList.begin()->second;
}

int
Rule::procPair(std::string& Ln,
		 std::map<int,Rule*>& Rlist,
		 int& compUnit) 
  /*!
    This takes a string Ln, finds the first two 
    Rxxx function, determines their join type 
    make the rule,  adds to vector then removes two old rules from 
    the vector, updates string
    --Care needed since we need to do intersections first
    \param Ln :: String to process
    \param Rlist :: Map of rules (added to)
    \param compUnit :: Last computed unit
    \retval 0 :: No rule to find
    \retval 1 :: A rule has been combined
  */
{
  ELog::RegMethod RegA("Rules","procPair");
  // String type Rnn : Rnn
  // Split string into Index Join Index Join
  // there are (join.size() + 1)==Index.size()

  std::vector<int> RUnits;
  std::vector<int> Joins;
  int Rnum;
  int flag(1);
  Ln=StrFunc::fullBlock(Ln);
  while(!Ln.empty())
    {
      if (flag)
        {
	  std::string::size_type pos=Ln.find('R');
	  Ln.erase(0,pos+1);
	  if (!StrFunc::section(Ln,Rnum)) 
	    return 0;
	  RUnits.push_back(Rnum);
	}
      else
        {
	  size_t ii;
	  for(ii=0;ii<Ln.size() && isspace(Ln[ii]);ii++) ;
	  if (ii==Ln.size()) return 0;
	  Joins.push_back(Ln[ii]==':' ? 1 : -1);
	}
      flag=1-flag;
    }
  // Now remove Intersections first:
  std::map<int,Rule*>::iterator mA;
  std::map<int,Rule*>::iterator mB;
  size_t i(0);
  while(i<Joins.size())
    {
      if (Joins[i]==-1)
        {
	  // Get rules
	  mA=Rlist.find(RUnits[i]);
	  mB=Rlist.find(RUnits[i+1]);
	  Rule* CompRule=new Intersection(mA->second,mB->second);
	  mA->second=CompRule;
	  Rlist.erase(mB);
	  RUnits.erase(RUnits.begin()+static_cast<std::ptrdiff_t>(i)+1);
	  Joins.erase(Joins.begin()+static_cast<std::ptrdiff_t>(i));
	}
      else
	i++;
    }

  // Now remove Unions
  while(!Joins.empty())
    {
      // Get rules
      mA=Rlist.find(RUnits[0]);
      mB=Rlist.find(RUnits[1]);
      Rule* CompRule=new Union(mA->second,mB->second);
      mA->second=CompRule;
      Rlist.erase(mB);
      RUnits.erase(RUnits.begin()+1);
      Joins.erase(Joins.begin());
    }
  int Ra=RUnits.front();
  std::stringstream cx;
  cx<<" R"<<Ra<<" ";
  Ln=cx.str();
  compUnit=Ra;
  return 1;
}

CompGrp*
Rule::procComp(Rule* RItem)
  /*!
    Takes a Rule item and makes it a complementary group
    \param RItem to encapsulate
    \return Complentary Group Pt [new]
  */
{
  ELog::RegMethod RegA("Rules","procComp");

  if (!RItem)
    return new CompGrp();
  
  Rule* Pptr=RItem->getParent();
  CompGrp* CG=new CompGrp(Pptr,RItem);
  if (Pptr)
    {
      const int Ln=Pptr->findLeaf(RItem);
      Pptr->setLeaf(CG,Ln);
    }
  return CG;
}
