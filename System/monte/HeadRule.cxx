/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/HeadRule.cxx
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
#include <limits>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "MemStack.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Rules.h"
#include "RuleCheck.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "HeadRule.h"

#include "SurInter.h"


std::ostream&
operator<<(std::ostream& OX,const HeadRule& A)
  /*!
    Simple stream output operator
    \param OX :: Output stream
    \param A :: HeadRule
    \return stream state
  */
{
  OX<<A.display();
  return OX;
}

HeadRule::HeadRule() :
  HeadNode(0)
  /*!
    Creates a new rule
  */
{}

HeadRule::HeadRule(const std::string& RuleStr) :
  HeadNode(0)
  /*!
    Creates a new rule
    \param RuleStr :: rule in MCNP format
  */
{
  ELog::RegMethod RegA("HeadRule","HeadRule(string)");
  if (!RuleStr.empty() && !procString(RuleStr))
    throw ColErr::InvalidLine(RuleStr,"RuleStr",0);
}

HeadRule::HeadRule(const Rule* RPtr) :
  HeadNode((RPtr) ? RPtr->clone() : 0)
  /*!
    Creates a new rule
    \param RPtr :: Rule to clone as a top rule
  */
{}

HeadRule::HeadRule(const HeadRule& A) :
  HeadNode((A.HeadNode) ? A.HeadNode->clone() : 0)
  /*!
    Copy constructor
    \param A :: Head rule to copy
  */
{}

HeadRule&
HeadRule::operator=(const HeadRule& A)  
  /*!
    Assignment operator
    \param A :: Head object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      delete HeadNode;
      HeadNode=(A.HeadNode) ? A.HeadNode->clone() : 0;
    }
  return *this;
}

HeadRule::~HeadRule()
  /*!
    Destructor
  */
{
  delete HeadNode;
}

bool
HeadRule::operator!=(const HeadRule& A) const
  /*!
    Determine if a rule is note equal
    The principle is to test each level 
    \param A :: Head rule to test
    \return result (true/false)
  */
{
  ELog::RegMethod RegA("HeadRule","operator!=");
  return !(this->operator==(A));
}

bool
HeadRule::operator==(const HeadRule& A) const
  /*!
    Determine if a rule is equal
    The principle is to test each level 
    \param A :: Head rule to test
    \return true is structurally equal
  */
{
  ELog::RegMethod RegA("HeadRule","operator==");
  if (&A==this) return 1;
  if (!HeadNode && !A.HeadNode)
    return 1;

  // One != to other 
  if (!HeadNode || !A.HeadNode)
    return 0;
  if (HeadNode->type()!=A.HeadNode->type())
    return 0;
  
  std::vector<const Rule*> AVec=
    findTopNodes();
  std::vector<const Rule*> BVec=
    A.findTopNodes();
  
  if (AVec.size()!=BVec.size()) 
    return 0;
  
  // all NON-equals
  std::vector<HeadRule> ASet;
  std::vector<HeadRule> BSet;
  std::set<int> ASurf;
  // First compare equal signed surface:
  for(const Rule* APtr : AVec)
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(APtr);
      if (SurPtr)
	ASurf.insert(SurPtr->getSignKeyN());
      else
	ASet.push_back(HeadRule(APtr));
    }

  for(const Rule* BPtr : BVec)
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(BPtr);
      if (SurPtr)
	{
	  const int SN(SurPtr->getSignKeyN());
	  if (ASurf.find(SN)==ASurf.end())
	    return 0;
	}
      else
	BSet.push_back(HeadRule(BPtr));
    }
  // First compare equal signed surface:  
  // Separate into intersect/union
  std::vector<HeadRule>::iterator mc;
  for(const HeadRule& ASub : ASet)
    {
      const int unionFlag(ASub.isUnion());
      bool found(0);
      for(mc=BSet.begin();mc!=BSet.end();mc++)
	{
	  // Same type / match
	  if (mc->isUnion()==unionFlag &&
	      ASub==*(mc))
	    {
	      BSet.erase(mc);
	      found=1;
	      break;
	    }
	}
      if (!found) return 0;
    }
  // EVERYTHING MATCHED !!!
  return 1;
}

bool
HeadRule::partMatched(const HeadRule& A) const		      
  /*!
    Given two head rule components
    \param A :: Rule to check
    \return 0 :: No match / top level / 
  */
{
  ELog::RegMethod RegA("HeadRule","partMatched");

  if (!A.HeadNode && !HeadNode) return 1;
  if (!A.HeadNode || !HeadNode) return 0;
  std::vector<const Rule*> AVec=
    findTopNodes();
  std::vector<const Rule*> BVec=
    A.findTopNodes();
 
  // all NON-equals
  std::vector<HeadRule> ASet;
  HeadRule BSet;
  std::set<int> ASurf;
  // First compare equal signed surface:
  for(const Rule* APtr : AVec)
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(APtr);
      if (SurPtr)
	ASurf.insert(SurPtr->getSignKeyN());
      else
	ASet.push_back(HeadRule(APtr));
    }

  int levelActive(1);
  for(const Rule* BPtr : BVec)         
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(BPtr);
      if (SurPtr)
	{
	  if (levelActive)
	    {
	      const int SN(SurPtr->getSignKeyN());
	      if (ASurf.find(SN)==ASurf.end())
		{
		  levelActive=0;
		  break;
		}
	    }
	}
      else
	{
	  if (A.HeadNode->type()>0)  // intersection
	    BSet.addIntersection(BPtr);
	  else
	    BSet.addUnion(BPtr);
	}
    }
  
  if (levelActive && !BSet.hasRule()) return 1;

  if (levelActive)
    {
      // Sub components MUST be contained in one rule completely:
      for(const HeadRule AS : ASet)
	{
	  if (AS.partMatched(BSet))
	    return 1;
	}
      return 0;
    }
  
  // OK CHECK EACH Minor
  for(const HeadRule AS : ASet)
    {
      if (AS.partMatched(A))
	return 1;
    }
  // EVERYTHING FAILED !!!
  return 0;
}  

bool
HeadRule::subMatched(const HeadRule& A,
		     const HeadRule& SubUnit) 
  /*!
    Sub-a part matched unit into main system
    \param A :: Rule to check find 
    \param SubUnit :: Rule to replace unit with
    \return 0 :: No match / 1 matched 
  */
{
  ELog::RegMethod RegA("HeadRule","subMatched");

  if (!A.HeadNode || !HeadNode) return 0;
  std::vector<const Rule*> AVec=
    findTopNodes();
  std::vector<const Rule*> BVec=
    A.findTopNodes();
 
  // NEED In case delete all objects
  const int typeNum(HeadNode->type());

  // all NON-equals
  HeadRule ATop;
  std::vector<HeadRule> ASet;
  HeadRule BSet;
  typedef std::map<int,const SurfPoint*> MTYPE;
  MTYPE ASurf;
  // First compare equal signed surface:
  for(const Rule* APtr : AVec)
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(APtr);
      if (SurPtr)
	{
	  ASurf.insert(MTYPE::value_type(SurPtr->getSignKeyN(),SurPtr));
	  if (typeNum>=0)
	    ATop.addIntersection(APtr);
	  else
	    ATop.addUnion(APtr);
	}
      else
	ASet.push_back(HeadRule(APtr));
    }

  int levelActive(1);
  for(const Rule* BPtr : BVec)         
    {
      const SurfPoint* SurPtr=
	dynamic_cast<const SurfPoint*>(BPtr);
      if (SurPtr)
	{
	  if (levelActive)
	    {
	      const int SN(SurPtr->getSignKeyN());
	      if (ASurf.find(SN)==ASurf.end())
		{
		  levelActive=0;
		  break;
		}
	    }
	}
      else
	{
	  if (A.HeadNode->type()>0)  // intersect
	    BSet.addIntersection(BPtr);
	  else
	    BSet.addUnion(BPtr);     // union
	}
    }
  if (levelActive && !BSet.hasRule()) 
    {
      // REMOVE TOP LEVEL:
      for(const Rule* BPtr : BVec)         
	{
	  const SurfPoint* SurPtr=
	    dynamic_cast<const SurfPoint*>(BPtr);
	  if (SurPtr)
	    {
	      const int SN(SurPtr->getSignKeyN());
	      MTYPE::const_iterator mc=ASurf.find(SN);
	      removeItem(mc->second);
	    }
	}
      if (typeNum>=0)  // intersect
	addIntersection(SubUnit);
      else
	addUnion(SubUnit);
      return 1;
    }


  if (levelActive)
    {
      // Sub components MUST be contained in one rule completely:
      for(const HeadRule AS : ASet)
	{
	  if (AS.partMatched(BSet))
	    {
	      ELog::EM<<"Level HERE"<<ELog::endDiag;
	      return 1;
	    }
	}
      return 0;
    }
  
  // OK CHECK Minors
  for(HeadRule& AS : ASet)
    {
      // Here all the rule is actually in ASET BUT
      // only one needs recontructions
      if (AS.subMatched(A,SubUnit))
	{
	  // Concatinate result
	  for(const HeadRule& AV : ASet)
	    {
	      if (typeNum>=0)
		ATop.addIntersection(AV);
	      else
		ATop.addUnion(AV);
	      *this=ATop;
	    }
	  return 1;
	}
    }
  // EVERYTHING FAILED !!!
  return 0;
}  

HeadRule
HeadRule::getLevel(const size_t levelNumber) const
  /*!
    Disect the rule to just the required level
    \param levelNumber :: Level required
    \return HeadRule of level only
   */
{
  ELog::RegMethod RegA("HeadRule","getLevel");

  HeadRule Out;
  if (!HeadNode) return Out;

  
  std::stack<Rule*> TreeLine;
  std::stack<size_t> TreeLevel;
  TreeLine.push(HeadNode);
  TreeLevel.push(0);

  size_t activeLevel(0);
  //  int statePoint(HeadNode->type());
  
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      activeLevel=TreeLevel.top();
      TreeLine.pop();
      TreeLevel.pop(); 

      // Process level:
      if (tmpA->getParent() &&
          tmpA->getParent()->type()!=tmpA->type())
	activeLevel++;
      // Process level:


      const SurfPoint* SurX=dynamic_cast<const SurfPoint*>(tmpA);
      if (SurX)
	{
          // SPECIAL CASE FOR flat top level
          if (!tmpA->getParent())
              activeLevel++;
      // Process level:
          
          if (activeLevel==levelNumber+1)
            {

              const Rule* RParent=tmpA->getParent();
              if (!RParent || RParent->type()==1)
                Out.addIntersection(tmpA);
              else if (RParent && RParent->type()== -1)
                Out.addUnion(tmpA);
              else 
                ELog::EM<<"Unable to deal with type:"
                        <<RParent->type()<<ELog::endErr;
            }
        }
      else                           // PROCESS LEAF NODE:
        {
          if (activeLevel<=levelNumber)
            {
              Rule* tmpB=tmpA->leaf(0);
              Rule* tmpC=tmpA->leaf(1);
              if (tmpB || tmpC)
                {
                  if (tmpB)
                    {
                      TreeLevel.push(activeLevel);
                      TreeLine.push(tmpB);
                    }
                  if (tmpC)
                    {
                      TreeLevel.push(activeLevel);
                      TreeLine.push(tmpC);
                    }
                }
            }
        }
    }
  return Out;
}


void
HeadRule::reset() 
  /*!
    Assuming that the head-rule needs to be reset
   */
{
  delete HeadNode;
  HeadNode=0;
  return;
}

bool
HeadRule::isComplementary() const
  /*!
    Determine if the object has a complementary object
    \retval 1 :: true 
    \retval 0 :: false
  */
{
  if (HeadNode)
    return HeadNode->isComplementary();
  return 0;
}


void
HeadRule::populateSurf()
  /*!
    Create a rules list for the cells
  */
{
  ELog::RegMethod RegA("HeadRule","populateSurf");
  if (HeadNode)
    HeadNode->populateSurf();
  return;
}

bool
HeadRule::isUnion() const
  /*!
    Is a union object
    \return true if outer system is a union
  */
{
  return (dynamic_cast<const Union*>(HeadNode)) ? 1 : 0;
}

bool
HeadRule::isValid(const Geometry::Vec3D& Pt,
		  const std::set<int>& S) const
  /*!
    Calculate if an object is valid
    \param Pt :: Point to test
    \param S :: Exclude set
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->isValid(Pt,S) : 0;
}

bool
HeadRule::isValid(const Geometry::Vec3D& Pt,
		  const int S) const
  /*!
    Calculate if an object is valid
    \param Pt :: Point to test
    \param S :: Exclude items
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->isValid(Pt,S) : 0;
}

bool
HeadRule::isValid(const Geometry::Vec3D& Pt)const
  /*!
    Calculate if an object is valid
    \param Pt :: Point to test
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->isValid(Pt) : 0;
}

bool
HeadRule::isValid(const std::map<int,int>& M)const
  /*!
    Calculate if an object is valid
    \param M :: Map of surfaces and true/false (0-- on edge)
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->isValid(M) : 0;
}

bool
HeadRule::isDirectionValid(const Geometry::Vec3D& Pt,const int S) const
  /*!
    Calculate if an object is valid
    \param Pt :: Point to test
    \param S :: Surface to treat as true/false [based on sign]
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->isDirectionValid(Pt,S) : 0;
}


int
HeadRule::pairValid(const int S,const Geometry::Vec3D& Pt)const
  /*!e
    Calculate if an object is valid
    \param S :: Surface number to alternate on
    \param Pt :: Point to test
    \return true/false 
  */
{
  return (HeadNode) ? HeadNode->pairValid(S,Pt) : 0;
}

void
HeadRule::isolateSurfNum(const std::set<int>& SN) 
  /*!
    Top down pruning of our rules to remove all the item
    in SN.
    \param SN :: surface numbers to remove
   */
{
  ELog::RegMethod RegA("HeadRule","isolateSurfNum");

  // FIRST PASS: [Eliminate -- all zero surfaces]
  std::stack<Rule*> TreeLine;
  TreeLine.push(HeadNode);
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      if (tmpA)
	{
	  Rule* tmpB=tmpA->leaf(0);
	  Rule* tmpC=tmpA->leaf(1);
	  if (tmpB || tmpC)
	    {
	      if (tmpB)
		TreeLine.push(tmpB);
	      if (tmpC)
		TreeLine.push(tmpC);
	    }
	  else
	    {
	      SurfPoint* SurX=dynamic_cast<SurfPoint*>(tmpA);
	      if (SurX && SN.find(SurX->getSignKeyN())!=SN.end())
		removeItem(SurX);
	    }
	}
    }
  
  return;
}

std::set<int>
HeadRule::getSurfSet() const
  /*!
    Get the set of surfaces  [signed]
    \return surf set
  */
{
  ELog::RegMethod RegA("HeadRule","getSurfSet");
  return (HeadNode) ? HeadNode->getSurfSet() : std::set<int>();
}

std::set<const Geometry::Surface*>
HeadRule::getOppositeSurfaces() const
  /*!
    Calculate if there are any surfaces that are opposite in sign
    \return Set of opposite surfaces
  */
{
  ELog::RegMethod RegA("HeadRule","getOppositeSurfaces");

  std::set<const Geometry::Surface*> out;
  if (!HeadNode || HeadNode->type()==0)     // One element tree (just return)
    return out;
    
  const Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  std::set<int> active;
  // Tree stack of rules
  std::stack<const Rule*> TreeLine;   
  TreeLine.push(HeadNode);
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
      if ( (TSurf=RuleCheck::checkSurfPoint(active,headPtr)) )
	out.insert(TSurf);
    }
  return out;
}


std::vector<const Geometry::Surface*>
HeadRule::getSurfaces() const
  /*!
    Calculate the surfaces that are within the top level
    \return Set of surface
  */
{
  ELog::RegMethod RegA("HeadRule","getSurfaces");
  std::vector<const Geometry::Surface*> Out;
  const SurfPoint* SP;
  if (!HeadNode) return Out;

  const Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  // Tree stack of rules
  std::stack<const Rule*> TreeLine;   
  TreeLine.push(HeadNode);
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  
      if (headPtr->type())             // MUST BE INTERSECTION/Union
	{
	  leafA=headPtr->leaf(0);        // get leaves (two of) 
	  leafB=headPtr->leaf(1);
	  if (leafA)
	    TreeLine.push(leafA);
	  if (leafB)
	    TreeLine.push(leafB);
	}
      else if (headPtr->type()==0)        // MIGHT BE SURF
	{
	  SP=dynamic_cast<const SurfPoint*>(headPtr);
	  if (SP)
	    Out.push_back(SP->getKey());
	}
    }
  return Out;
}

std::vector<int>
HeadRule::getSurfaceNumbers() const
  /*!
    Calculate the surfaces that are within the object
    \return Set of surface
  */
{
  ELog::RegMethod RegA("HeadRule","getSurfaceNumbers");
  std::vector<int> Out;
  const SurfPoint* SP;
  if (!HeadNode) return Out;

  const Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  // Tree stack of rules
  std::stack<const Rule*> TreeLine;   
  TreeLine.push(HeadNode);
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  
      if (headPtr->type())             // MUST BE INTERSECTION/Union
	{
	  leafA=headPtr->leaf(0);        // get leaves (two of) 
	  leafB=headPtr->leaf(1);
	  if (leafA)
	    TreeLine.push(leafA);
	  if (leafB)
	    TreeLine.push(leafB);
	}
      else if (headPtr->type()==0)        // MIGHT BE SURF
	{
	  SP=dynamic_cast<const SurfPoint*>(headPtr);
	  if (SP)
	    Out.push_back(SP->getKeyN());
	}
    }
  return Out;
}

int
HeadRule::getPrimarySurface() const
  /*!
    Calculate the surfaces that are within the top level
    and return the surface most likely to be the master surface
    \return single surface number / 0 if not a single primary
  */
{
  const std::vector<int> TSet=getTopSurfaces();
  if (TSet.size()!=1)
    throw ColErr::SizeError<size_t>(TSet.size(),1,
                                    "HeadRule has wrong surface count");
  return TSet.front();
}

std::vector<int>
HeadRule::getTopSurfaces() const
  /*!
    Calculate the surfaces that are within the top level
    \return Set of surface
  */
{
  ELog::RegMethod RegA("HeadRule","getOppositeSurfaces");
  std::vector<int> Out;
  const SurfPoint* SP;
  if (!HeadNode) return Out;


  const Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  // Tree stack of rules
  std::stack<const Rule*> TreeLine;   
  TreeLine.push(HeadNode);
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  
      if (headPtr->type()==1)        // MUST BE INTERSECTION
	{
	  leafA=headPtr->leaf(0);        // get leaves (two of) 
	  leafB=headPtr->leaf(1);
	  if (leafA)
	    TreeLine.push(leafA);
	  if (leafB)
	    TreeLine.push(leafB);
	}
      else if (headPtr->type()==0)        // MIGHT BE SURF
	{
	  SP=dynamic_cast<const SurfPoint*>(headPtr);
	  if (SP)
	    Out.push_back(SP->getSign()*SP->getKeyN());
	}
    }
  return Out;
}

int
HeadRule::removeTopItem(const int SN) 
  /*!
    Given a signed surface SN , removes the first instance 
    of that surface from the Rule within the top level
    \param SN :: Signed surface to remove
    \return -ve on error, 0 on not-found and 1 on success
  */
{
  ELog::RegMethod RegA("HeadRule","removeTopItem");

  if (!HeadNode) return 0;

  Rule *headPtr,*leafA,*leafB;       
  // Parent : left/right : Child

  // Tree stack of rules
  std::stack<Rule*> TreeLine;   
  TreeLine.push(HeadNode);
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  
      if (headPtr->type()==1)        // MUST BE INTERSECTION
	{
	  leafA=headPtr->leaf(0);        // get leaves (two of) 
	  leafB=headPtr->leaf(1);
	  if (leafA)
	    TreeLine.push(leafA);
	  if (leafB)
	    TreeLine.push(leafB);
	}
      else if (headPtr->type()==0)        // MIGHT BE SURF
	{
	  const SurfPoint* SP=
	    dynamic_cast<const SurfPoint*>(headPtr);
	  if (SP)
	    {
	      const int SNtest=SP->getSign()*SP->getKeyN();
	      if (SNtest==SN)
		{
		  removeItem(headPtr);
		  return 1; 
		}
	    }
	}
    }
  return 0;
}

int
HeadRule::removeUnsignedItems(const int SN) 
  /*!
    Given a signed surface SN , removes the first instance 
    of that surface from the Rule
    \param SN :: Unsigned surface to remove
    \retval -ve error,
    \retval 0  not-found 
    \retval  count of removed surfaces [success]
  */
{
  ELog::RegMethod RegA("HeadRule","removeUnsignedItems");

  const int cntA=removeItems(SN);
  if (cntA<0) return cntA;
  const int cntB=removeItems(-SN);
  if (cntB<0) return cntB;
  return cntA+cntB;
}

int
HeadRule::removeItems(const int SN) 
  /*!
    Given a signed surface SN , removes the first instance 
    of that surface from the Rule
    \param SN :: Signed surface to remove
    \return -ve on error, 0 on not-found and Number removed on success
  */
{
  ELog::RegMethod RegA("HeadRule","removeItems");

  if (!HeadNode) return -1;

  int cnt(0);
  std::stack<Rule*> TreeLine;
  TreeLine.push(HeadNode);
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      if (tmpA)
	{
	  Rule* tmpB=tmpA->leaf(0);
	  Rule* tmpC=tmpA->leaf(1);
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
		{
		  if (SurX->getKeyN()==SN)
		    {
		      removeItem(tmpA);
		      cnt++;
		    }
		}
	    }
	}
    }
  return cnt;
}

const SurfPoint*
HeadRule::findSurf(const int SN) const
  /*!
    Find a surface
    \param SN :: Surface number
    \return 0 if no pointer / first point found
  */
{
  ELog::RegMethod RegA("HeadRule","findSurf");

  if (!HeadNode) 
    return 0;

  std::stack<Rule*> TreeLine;
  TreeLine.push(HeadNode);
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      if (tmpA)
	{
	  Rule* tmpB=tmpA->leaf(0);
	  Rule* tmpC=tmpA->leaf(1);
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
	      if (SurX && SurX->getKeyN()==SN)
		return SurX;
	    }
	}
    }
  return 0;
}

const Rule*
HeadRule::findNode(const size_t LN,const size_t Index) const
  /*!
    Return a head rule based on level and Index
    \param LN :: Level Number
    \param Index :: Index Number [+1 -- 0 is all]
    \return HeaRule item [this/pointer?]
  */
{
  ELog::RegMethod RegA("HeadRule","findNode");

  if (!HeadNode) 
    return 0;
  // SPECIAL CASE : 0 - 0
  if (!LN && !Index)
    return HeadNode;

  size_t nLevel(0);
  std::stack<Rule*> TreeLine;
  std::stack<size_t> TreeLevel;
  TreeLine.push(HeadNode);
  TreeLevel.push(0);
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      size_t activeLevel=TreeLevel.top();
      TreeLine.pop();
      TreeLevel.pop();
      
      if (tmpA->getParent() && 
	  tmpA->getParent()->type()!=tmpA->type())
	{
	  if (activeLevel==LN)
	    {
	      nLevel++;
	      if (nLevel==Index) 
		return tmpA;
	    }
	  activeLevel++;
	}
      
      Rule* tmpB=tmpA->leaf(0);
      Rule* tmpC=tmpA->leaf(1);
      if (tmpB || tmpC)
	{
	  if (tmpB)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpB);
	    }
	  if (tmpC)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpC);
	    }
	}
    }
  if (!nLevel && Index==1 && 
      !HeadNode->type())
    return HeadNode; 

  return 0; 
}


std::vector<const Rule*>
HeadRule::findNodes(const size_t NL) const
  /*!
    Return a head rule nodes at NL level
    \param NL :: Level
    \return vector of nodes at level NL
  */
{
  ELog::RegMethod RegA("HeadRule","findNodes");

  std::vector<const Rule*> Out;
  if (!HeadNode) return Out;
  // Special case for single rule:
  if (NL==0 && !HeadNode->type())
    {
      Out.push_back(HeadNode);
      return Out;
    }

  std::stack<Rule*> TreeLine;
  std::stack<size_t> TreeLevel;
  TreeLine.push(HeadNode);
  TreeLevel.push(0);

  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      size_t activeLevel=TreeLevel.top();
      TreeLine.pop();
      TreeLevel.pop();

      if (tmpA->getParent() && 
	  tmpA->getParent()->type()!=tmpA->type())
	{
	  if (activeLevel==NL)
	    Out.push_back(tmpA);
	  activeLevel++;
	}

      Rule* tmpB=tmpA->leaf(0);
      Rule* tmpC=tmpA->leaf(1);
      if (tmpB || tmpC)
	{
	  if (tmpB)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpB);
	    }
	  if (tmpC)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpC);
	    }
	}
    }

  return Out; 
}

std::vector<const Rule*>
HeadRule::findTopNodes() const
  /*!
    Return a head rule nodes at first level
    \return head-nodes
  */
{
  ELog::RegMethod RegA("HeadRule","findTopNodes");
  return findNodes(0);
}

HeadRule
HeadRule::getComponent(const size_t LN,const size_t Index) const 
  /*!
    Return a head rule based on level and Index
    \param LN :: Level Number
    \param Index :: Index Number
    \return HeaRule item [this/pointer?]
  */
{
  ELog::RegMethod RegA("HeadRule","getComponent");
  HeadRule Out;

  const Rule* tmpA=findNode(LN,Index);

  if (tmpA)
    Out.addIntersection(tmpA);
  return Out;
}

size_t
HeadRule::countNLevel(const size_t LN) const
  /*!
    Determine the number of rules at level LN
    \param LN :: level to determine
    \return number at level
  */
{
  ELog::RegMethod RegA("HeadRule","countNLevel");

  if (!HeadNode) 
    return 0;

  size_t nLevel(0);
  std::stack<Rule*> TreeLine;
  std::stack<size_t> TreeLevel;
  TreeLine.push(HeadNode);
  TreeLevel.push(0);

  size_t activeLevel(0);
  //  int statePoint(HeadNode->type());
  
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      activeLevel=TreeLevel.top();
      TreeLine.pop();
      TreeLevel.pop();
      
      if (tmpA->getParent() && 
	  tmpA->getParent()->type()!=tmpA->type())
	{
	  if (activeLevel==LN)
	    nLevel++;
	  activeLevel++;
	}
      
      Rule* tmpB=tmpA->leaf(0);
      Rule* tmpC=tmpA->leaf(1);
      if (tmpB || tmpC)
	{
	  if (tmpB)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpB);
	    }
	  if (tmpC)
	    {
	      TreeLevel.push(activeLevel);
	      TreeLine.push(tmpC);
	    }
	}
    }
  return (nLevel) ? nLevel : 1;
}


int
HeadRule::level(const int SN) const
  /*!
    Given a signed surface SN finds the level [number of union/inter]
    sections 
    \param SN :: Signed surface to remove
    \return -ve on no find / level [0 offset]
  */
{
  ELog::RegMethod RegA("HeadRule","level");

  const Rule* SPtr=findSurf(SN);  // Don't care that it is a surf pointer
  if (!SPtr) return -1;
  
  int level(0);
  const Rule* Tmp=SPtr->getParent();
  if (!Tmp) return 0;
  int statePoint(Tmp->type());
  do
    {
      if (statePoint!=Tmp->type())
	{
	  statePoint=Tmp->type();
	  level++;
	}
      Tmp = Tmp->getParent();
    } while (Tmp);
  return level;
}

void
HeadRule::removeItem(const Rule* Target) 
  /*!
    Objective is to remove Target rule -
    the returned rule is either 0 if the structure 
    has been maintained or the new rule if needed
    \param Target :: Rule to be removed
    \return Replace headRule 
  */
{
  ELog::RegMethod RegA("HeadRule","removeItem");
  if (!Target) return;
  Rule* P=Target->getParent();
  if (!P)
    {
      delete Target;
      HeadNode=0;
      return;
    }
  
  const int aLeaf=(P->leaf(0)==Target) ? 0 : 1;
  const int bLeaf=1-aLeaf;
  Rule* X=P->leaf(bLeaf);

  Rule* GP=P->getParent();
  // remove leaf from P and delete
  P->setLeaf(0,bLeaf);


  if (!GP)  // New Top Rule !
    {
      delete P;
      X->setParent(0);
      HeadNode=X;
      return;
    }
  // Assume Parent good
  const int xLeaf=(GP->leaf(0)==P) ? 0 : 1;
  GP->setLeaf(X,xLeaf);
  delete P;
  return;
}

void
HeadRule::removeCommon()
  /*!
    Objective is to remove the common surfaces at the same
    effective level. i.e. 3 4 5 5: (101: 4) removes 5 but not 4.  
  */
{
  ELog::RegMethod RegA("HeadRule","removeCommon");

  if (!HeadNode) 
    return;
  std::set<int> SFound;  // surf : Level [paired]

  std::stack<Rule*> TreeLine;
  std::stack<size_t> TreeLevel;
  TreeLine.push(HeadNode);
  TreeLevel.push(0);
 
  std::set<int>::const_iterator mc;
  size_t activeLevel(0);
  //  int statePoint(HeadNode->type());
  
  while(!TreeLine.empty())
    {
      Rule* tmpA=TreeLine.top();
      activeLevel=TreeLevel.top();
      TreeLine.pop();
      TreeLevel.pop(); 

      // Process level:
      if (tmpA->getParent() && 
	  tmpA->getParent()->type()!=tmpA->type())
	activeLevel++;

      const SurfPoint* SurX=dynamic_cast<const SurfPoint*>(tmpA);
      if (SurX)
	{
	  const int SN=(100*SurX->getSignKeyN())+
	    static_cast<int>(activeLevel);
	  
	  mc=SFound.find(SN);
	  if (mc!=SFound.end())
	    removeItem(SurX);
	  else
	    SFound.insert(SN);
	}
      else                           // PROCESS LEAF NODE:
	{
	  Rule* tmpB=tmpA->leaf(0);
	  Rule* tmpC=tmpA->leaf(1);
	  if (tmpB || tmpC)
	    {
	      if (tmpB)
		{
		  TreeLevel.push(activeLevel);
		  TreeLine.push(tmpB);
		}
	      if (tmpC)
		{
		  TreeLevel.push(activeLevel);
		  TreeLine.push(tmpC);
		}
	    }
	}
    }
  return;
}



int
HeadRule::substituteSurf(const int SurfN,const int newSurfN,
			 const Geometry::Surface* SPtr)
  /*!
    Substitues a surface item if within a rule
    \param SurfN :: Number number to change [+ve]
    \param newSurfN :: New surface number (if -ve then the key is reversed)
    \param SPtr :: New surface Pointer
    \returns number of substitutions
  */
{
  ELog::RegMethod RegA("HeadRule","substitueSurf");

  // Quick check:
  if (SurfN==newSurfN) return 0;
  int cnt(0);

  SurfPoint* Ptr=dynamic_cast<SurfPoint*>(HeadNode->findKey(abs(SurfN)));
  while(Ptr)
    {
      Ptr->setKeyN(Ptr->getSign()*newSurfN);
      Ptr->setKey(SPtr);
      cnt++;
      Ptr=dynamic_cast<SurfPoint*>(HeadNode->findKey(abs(SurfN)));
    }
  return cnt;
}

void
HeadRule::makeComplement()
  /*!
    Make the rule a complement 
   */
{
  ELog::RegMethod RegA("HeadRule","makeComplement");

  if (!HeadNode) return;
  MonteCarlo::Algebra AX;
  AX.setFunctionObjStr("#( "+HeadNode->display()+") ");
  delete HeadNode;
  HeadNode=Rule::procString(AX.writeMCNPX());
  return;
}

HeadRule
HeadRule::complement() const
  /*!
    Make the rule a complement 
    and return that object
    \return #(this)
   */
{
  ELog::RegMethod RegA("HeadRule","complement");

  HeadRule A(*this);
  A.makeComplement();
  return A;
}

std::string 
HeadRule::display() const
  /*!
    Write out rule 
    \return string of Rule
  */
{
  if (!HeadNode) return "";
  
  // union test
  return (HeadNode->type()== -1) ? 
    "("+HeadNode->display()+")" :
    " "+HeadNode->display()+" ";
}

std::string 
HeadRule::displayFluka() const
  /*!
    Write out rule for fluka
    \return string of Rule
  */
{
  if (!HeadNode) return "";
  
  // union test
  return (HeadNode->type()== -1) ? 
    "("+HeadNode->displayFluka()+")" :
    " "+HeadNode->displayFluka()+" ";
}

std::string 
HeadRule::displayPOVRay() const
  /*!
    Write out rule for POV-Ray
    \return string of Rule
  */
{
  if (!HeadNode) return "";

  // union test
  return (HeadNode->type()== -1) ? 
    "pov|"+HeadNode->displayPOVRay()+"pov|" :
    " "+HeadNode->displayPOVRay()+" ";
}

std::string 
HeadRule::display(const Geometry::Vec3D& Pt) const
  /*!
    Write out rule with surf relative to point
    \param Pt :: Point to test
    \return string of Rule
  */
{
  return (HeadNode) ? HeadNode->display(Pt) : "";
}


std::string 
HeadRule::displayAddress() const
  /*!
    Write out rule with addresses
    \return string of Rule
  */
{
  return (HeadNode) ? HeadNode->displayAddress() : "";
}


void
HeadRule::displayVec(std::vector<Token>& TK) const
  /*!
    Create a token set for the rule
    \param TK :: Token vectors
  */
{
  if (HeadNode) 
    HeadNode->displayVec(TK);
  return;
}

HeadRule&
HeadRule::addIntersection(const int SN) 
  /*!
    Add a rule in addition
    \param SN :: surface number
    \return Joined HeadRule
  */
{
  return addIntersection(StrFunc::makeString(SN));
}


HeadRule&
HeadRule::addUnion(const int SN) 
  /*!
    Add a rule in additional 
    \param SN :: Surface number
    \return Joined HeadRule
   */
{
  addUnion(StrFunc::makeString(SN));
  return *this;
}

HeadRule&
HeadRule::addIntersection(const std::string& RStr) 
  /*!
    Add a rule in addition
    \param RStr :: Rule string
    \return HeadRule item
  */
{
  ELog::RegMethod RegA("HeadRule","addIntersection(string)");
  if (!RStr.empty())
    {
      HeadRule A;
      if (A.procString(RStr))
	addIntersection(A.getTopRule());
      else
	ELog::EM<<"Failed on string :"<<RStr<<ELog::endErr;
    }
  return *this; 
}

HeadRule&
HeadRule::addUnion(const std::string& RStr) 
  /*!
    Add a rule in addition [unio]
    \param RStr :: Rule string
    \return Joined HeadRule
   */
{
  ELog::RegMethod RegA("HeadRule","addUnion(string)");
  HeadRule A;
  if (A.procString(RStr))
    addUnion(A.getTopRule());
  else
    ELog::EM<<"Failed on string :"<<RStr<<ELog::endErr;
  return *this;
}

HeadRule&
HeadRule::addIntersection(const HeadRule& AHead) 
  /*!
    Add a rule in addition
    \param AHead :: Otehr head rule
    \return Joined HeadRule
  */
{
  ELog::RegMethod RegA("HeadRule","addIntersection<HeadRule>");

  if (AHead.HeadNode)
    {
      if (!HeadNode)  // Special case: if this => empty
	HeadNode=AHead.HeadNode->clone();
      else
	createAddition(1,AHead.getTopRule());
    }
  
  return *this;
}

HeadRule&
HeadRule::addUnion(const HeadRule& AHead) 
  /*!
    Add a rule in addition
    \param AHead :: Other head rule
    \return HeadRule item
  */
{
  ELog::RegMethod RegA("HeadRule","addUnion<HeadRule>");
  
  if (AHead.HeadNode)
    {
      if (!HeadNode)
	HeadNode=AHead.HeadNode->clone();
      else
	createAddition(-1,AHead.getTopRule());
    }
  return *this;
}

HeadRule&
HeadRule::addIntersection(const Rule* RPtr) 
  /*!
    Add a rule in addition
    \param RPtr :: Rule pointer
    \return HeadRule item
   */
{
  ELog::RegMethod RegA("HeadRule","addIntersection<Rule>");
  if (RPtr)
    {
      if (!HeadNode)
	HeadNode=RPtr->clone();
      else
	createAddition(1,RPtr);
    }
  return *this;
}

HeadRule&
HeadRule::addUnion(const Rule* RPtr) 
  /*!
    Add a rule in addition
    \param RPtr :: Rule pointer
    \return HeadRule item
   */
{
  ELog::RegMethod RegA("HeadRule","addUnion");
  if (RPtr)
    {
      if (!HeadNode)
	HeadNode=RPtr->clone();
      else
	createAddition(-1,RPtr);
    }
  return *this;
}

void
HeadRule::createAddition(const int InterFlag,const Rule* NRptr)
  /*!
    Function to actually do the addition of a rule to 
    avoid code repeat.
    \param InterFlag :: Intersection / Union [ 1 : -1 ] :: 0 for new 
    \param NRptr :: New Rule pointer to add
   */
{
  ELog::RegMethod RegA("HeadRule","createAddition");

  // This is an intersection and we want to add our rule at the base
  // Find first item that is not an intersection
  Rule* RPtr(HeadNode);
  std::deque<Rule*> curLevel;
  curLevel.push_back(HeadNode);
  while(!curLevel.empty())
    {
      RPtr=curLevel.front();
      curLevel.pop_front();
      if (RPtr->type() != InterFlag)  // Success not an intersection
	{
	  Rule* parent=RPtr->getParent();     // grandparent
	  Rule* sA=RPtr;
	  Rule* Item;
	  if (InterFlag==1)
	    Item=new Intersection(parent,sA,NRptr->clone()); /// ???????
	  else
	    Item=new Union(parent,sA,NRptr->clone());

	  // Find place ot insert it
	  if (!parent)
	    HeadNode =Item;
	  else
	    parent->setLeaf(Item,parent->findLeaf(RPtr));
	  return;
	}
      Rule* LeafPtr;
      if ( (LeafPtr=RPtr->leaf(0)) )
	curLevel.push_back(LeafPtr);
      if ( (LeafPtr=RPtr->leaf(1)) )
	curLevel.push_back(LeafPtr);
    }
  ELog::EM<<"Failed on rule structure  "<<ELog::endErr;
  return;  
}


int
HeadRule::procSurface(const Geometry::Surface* SPtr) 
  /*!
    Process a rule as a HeadRule
    \param SPtr :: Surface rule
    \returns 1 on success
  */
{
  ELog::RegMethod RegA("HeadRule","procSurface");

  delete HeadNode;
  HeadNode=0;
  if (SPtr)
    {
      HeadNode=new SurfPoint(SPtr,SPtr->getName());
      return 1;
    }

  return 0; 
}

int
HeadRule::procRule(const Rule* RPtr) 
  /*!
    Process a rule as a HeadRule
    \param RPtr :: Rule to add
    \returns 1 on success
  */
{
  ELog::RegMethod RegA("HeadRule","procRule");


  delete HeadNode;
  HeadNode=0;
  if (RPtr)
    {
      HeadNode=RPtr->clone();
      return 1;
    }

  return 0; 
}

int
HeadRule::procSurfNum(const int SN)
  /*!
    Processes a surface number
    \param SN :: Signed surface number
    \returns 1 on success
  */
{
  ELog::RegMethod RegA("HeadRule","procSurfNum");

  if (!SN) return 0;

  delete HeadNode;

  // Now replace all free planes/Surfaces with appropiate Rxxx
  SurfPoint* SurX=new SurfPoint();
  SurX->setKeyN(SN);
  HeadNode=SurX;

  return 1; 
}

int
HeadRule::procString(const std::string& Line) 
  /*!
    Processes the cell string. This is an internal function
    to process a string with 
    - String type has #( and ( )
    \param Line :: String value
    \returns 1 on success
  */
{
  ELog::RegMethod RegA("HeadRule","procString");

  if (StrFunc::isEmpty(Line)) return 0;

  delete HeadNode;
  HeadNode=0;
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
	      throw ColErr::ExitAbort("% handler");
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
      ELog::EM<<"Map size not equal to 1 == "<<RuleList.size()<<ELog::endCrit;
      ELog::EM<<"Error line : "<<Ln<<ELog::endErr;
      return 0;
    }  
  HeadNode=(RuleList.begin())->second;
  return 1; 
}

int
HeadRule::trackSurf(const Geometry::Vec3D& Org,
		    const Geometry::Vec3D& Unit,
		    double& D) const
  /*!
    Calculate a track of a line to a change in state surface
    \param Org :: Origin of line
    \param Unit :: Direction of line
    \param D :: Distance travelled to surface
    \return exit surface
  */
{
  ELog::RegMethod RegA("HeadRule","trackSurf");


  MonteCarlo::LineIntersectVisit LI(Org,Unit);

  const std::vector<const Geometry::Surface*> SurfList=
    this->getSurfaces();
  for(const Geometry::Surface* SPtr : SurfList)
    SPtr->acceptVisitor(LI);
  
  const std::vector<Geometry::Vec3D>& IPts(LI.getPoints());
  const std::vector<double>& dPts(LI.getDistance());
  const std::vector<const Geometry::Surface*>& surfIndex(LI.getSurfIndex());


  D= std::numeric_limits<double>::max();
  const Geometry::Surface* surfPtr=0;
  // NOTE: we only check for and exiting surface by going
  // along the line.
  int bestPairValid(0);
  for(size_t i=0;i<dPts.size();i++)
    {
      // Is point possible closer
      if ( dPts[i]>10.0*Geometry::zeroTol &&
	   dPts[i]<D )
	{
	  const int NS=surfIndex[i]->getName();	    // NOT SIGNED
	  const int pAB=isDirectionValid(IPts[i],NS);
	  const int mAB=isDirectionValid(IPts[i],-NS);
	  const int normD=surfIndex[i]->sideDirection(IPts[i],Unit);
	  if (pAB!=mAB)  // out going positive surface
	    {
	      bestPairValid=normD;
	      if (dPts[i]>Geometry::zeroTol)
		D=dPts[i];
	      surfPtr=surfIndex[i];
	    }
	}
    }    
  return (!surfPtr) ? 0 : bestPairValid*surfPtr->getName();
}

size_t
HeadRule::calcSurfSurfIntersection(std::vector<Geometry::Vec3D>& Pts) const
  /*!
    Calculate the surf-surf-surf intersecitons for the whole headrule
    Assuming triplet binding.
    \param Pts :: points found
    \return Number of points found 
   */
{
  ELog::RegMethod RegA("HeadRule","calcSurfSurfIntersection");
  
  const std::vector<const Geometry::Surface*> SurfList=
    this->getSurfaces();
  for(size_t i=0;i<SurfList.size();i++)
    for(size_t j=0;j<SurfList.size();j++)
      for(size_t k=0;k<SurfList.size();k++)
	{
	  const Geometry::Surface* SurfX(SurfList[i]);
	  const Geometry::Surface* SurfY(SurfList[j]);
	  const Geometry::Surface* SurfZ(SurfList[k]);
	  std::vector<Geometry::Vec3D> PntOut=
	    SurInter::processPoint(SurfX,SurfY,SurfZ);
	  if (!PntOut.empty())
	    {
	      std::set<int> exclude;
	      exclude.insert(SurfX->getName());
	      exclude.insert(SurfY->getName());
	      exclude.insert(SurfZ->getName());
	      for(const Geometry::Vec3D& testPt : PntOut)
		if (isValid(testPt,exclude))
		  Pts.push_back(testPt);
	    }
	}
  
  return Pts.size();
}


size_t
HeadRule::calcSurfIntersection(const Geometry::Vec3D& Org,
			       const Geometry::Vec3D& VUnit,
			       std::vector<Geometry::Vec3D>& Pts,
			       std::vector<int>& SNum) const
  /*!
    Calculate a track of a line that intersects the rule.
    The surface number is the outgoing surface number.
    \param Org :: Origin of line
    \param Unit :: Direction of line
    \param Pts :: Points
    \param SNum :: Surface number 
    \return Number of points found
  */
{
  ELog::RegMethod RegA("HeadRule","calcSurfIntersection");


  MonteCarlo::LineIntersectVisit LI(Org,VUnit);
  const Geometry::Vec3D Unit=VUnit.unit();

  const std::vector<const Geometry::Surface*> SurfList=
    this->getSurfaces();
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SurfList.begin();vc!=SurfList.end();vc++)
    (*vc)->acceptVisitor(LI);

  // IPTS contains non-exit points
  const std::vector<Geometry::Vec3D>& IPts(LI.getPoints());
  const std::vector<double>& dPts(LI.getDistance());
  const std::vector<const Geometry::Surface*>& surfIndex(LI.getSurfIndex());
  
  // Clear data
  Pts.clear();
  SNum.clear();

  // NOTE: we only check for and exiting surface by going
  // along the line.
  for(size_t i=0;i<dPts.size();i++)
    {
      const Geometry::Surface* surfPtr=surfIndex[i];
      // Is point possible closer
      const int NS=surfPtr->getName();	    // NOT SIGNED
      const int pAB=isDirectionValid(IPts[i],NS);
      const int mAB=isDirectionValid(IPts[i],-NS);
      const int normD=surfPtr->sideDirection(IPts[i],Unit);
      const double lambda=dPts[i];
      if (pAB!=mAB)  // out going positive surface
	{
	  // previously used signValue but now gone to
	  // distValue BUT not 100% sure if that is correct.
	  //	  const int signValue((pAB>0) ? 1 : -1);
	  const int distValue((lambda>0) ? 1 : -1);
	  SNum.push_back(distValue*normD*NS);
	  Pts.push_back(Org+Unit*lambda);
	}
    }    

  return SNum.size();
}

int
HeadRule::procPair(std::string& Ln,std::map<int,Rule*>& Rlist,
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
  ELog::RegMethod RegA("Object","procPair");
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

/// TO BE MODIFED
CompGrp*
HeadRule::procComp(Rule* RItem)
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

bool
HeadRule::Intersects(const HeadRule& A) const
   /*!
     Determine if two objects intersect
     Done by line intersection along planes
     and by point intersection.
     \param A :: HeadRule to use
     \return true on intersection
   */
{
  ELog::RegMethod RegA("HeadRule","Intersects");
  
  
  const std::vector<const Geometry::Surface*> 
    AVec(this->getSurfaces());
  const std::vector<const Geometry::Surface*> 
    BVec(A.getSurfaces());

  // Surf/Surf/Surf intersection
  std::vector<Geometry::Vec3D> Out;
  std::vector<const Geometry::Surface*>::const_iterator ac,bc;

  for(ac=AVec.begin();ac!=AVec.end();ac++)
    {
      for(bc=ac+1;bc!=AVec.end();bc++)
	{
	  const Geometry::Surface* ASurf(*ac);
	  const Geometry::Surface* BSurf(*bc);
	  std::set<int> boundarySet;
	  boundarySet.insert(ASurf->getName());
	  boundarySet.insert(BSurf->getName());		  
	  for(const Geometry::Surface* CSurf : BVec)
	    {
	      Out=SurInter::processPoint(ASurf,BSurf,CSurf);
	      for(const Geometry::Vec3D& vc : Out)
		{
		  ELog::EM<<"Testing "<<vc<<ELog::endDiag;
		  // Outer valid returns true if out of object
		  if (this->isValid(vc,boundarySet) &&
		      A.isValid(vc,CSurf->getName()))
		    return 1;
		}
	    }
	}
    }
  return 0;
}
