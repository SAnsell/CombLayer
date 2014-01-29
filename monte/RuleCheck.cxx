/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/RuleCheck.cxx
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
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "mapIterator.h"
#include "RuleCheck.h"

RuleCheck::RuleCheck() :
  TopRule(0)
  /*!
    Constructor
  */
{}


int
RuleCheck::isKey(const Rule* RPtr,const int SN) 
  /*!
    Internal function to check if the Rule Ptr is the signed nubmer
    \param RPtr :: Rule to check
    \param SN :: Surface number
    \return 1/-1 depending on the surface signe [0 on faile
  */
{
  ELog::RegMethod RegA("RuleCheck","hasKey");

  const SurfPoint* sA=
    dynamic_cast<const SurfPoint*>(RPtr);
  if (sA && sA->getKeyN()==SN)
    return sA->getSign();

  return 0;
}

void
RuleCheck::setTopRule(Rule* TR) 
  /*!
    Set the top rule [some checking to add]
    \param TR :: Top rule
  */
{
  TopRule=TR;
  return;
}

void
RuleCheck::findKeyRule(const int SN, 
		       Rule* XRule,
		       std::map<Rule*,int>& posHeads,
		       std::map<Rule*,int>& negHeads)
  /*!
    Find the map of the positive and negative instances
    of SN
    \param SN :: +ve surface number to search
    \param XRule :: Rule to cut
    \param posHeads :: Positive Key value
    \param netHeads :: negative Key value
  */
{
  ELog::RegMethod RegA("RuleCheck","findKeyRule");

  Rule* headPtr;
  Rule* leaf;
  // Parent : left/right : Child

  typedef std::map<Rule*,int> MTYPE;

  // Tree stack of rules
  std::stack<Rule*> TreeLine;   
  TreeLine.push(XRule);
  int res;
  while (!TreeLine.empty())        // need to exit on active
    {
      headPtr=TreeLine.top();
      TreeLine.pop();	  

      for(int leafIndex=0;leafIndex<2;leafIndex++)
	{
	  leaf=headPtr->leaf(leafIndex);
	  if (leaf)
	    {
	      res=isKey(leaf,SN);
	      if (res>0)
		posHeads.insert(MTYPE::value_type(headPtr,leafIndex));
	      else if (res<0)
		negHeads.insert(MTYPE::value_type(headPtr,leafIndex));
	      else
		TreeLine.push(leaf);
	    }
	}
    }
  return;
}

Rule*
RuleCheck::removeDuplicate(const std::set<int>& SurfSet,
			   const int SN)
  /*!
    Check the logic to remove the duplicate
    \param SurfSet :: Surface set
    \param SN :: Surface Number to check on
    \return Set of opposite surfaesc
  */
{
  ELog::RegMethod RegA("RuleCheck","removeDuplicates");

  if (!TopRule || TopRule->type()==0)     // One element tree (just return)
    return 0;
  
  
  // Next copy rule  [Extreme care with memory here]
  std::map<Rule*,int> negHeads;
  std::map<Rule*,int> plusHeads;

  Rule* XRule=TopRule->clone();
  findKeyRule(SN,XRule,negHeads,plusHeads);
  // Eliminate and replace step:
  
  //Check baseline
  const std::string BaseLine=checkString(XRule,SurfSet);
  
  std::map<Rule*,int>::const_iterator mc;
  for(mc=negHeads.begin();mc!=negHeads.end();mc++)
    {
      ELog::EM<<"XRule ="<<XRule->display()<<ELog::endDebug;
      //      mc->first->zeroLeaf(mc->second);
      ELog::EM<<"XRule ="<<XRule->display()<<ELog::endDebug;
      const std::string NLine=checkString(XRule,SurfSet);
      if (NLine==BaseLine)
	return XRule;
    }
  // First build the valid rule:

  // Now test over all combinations:
    
  return 0;
}

std::string
RuleCheck::checkString(const Rule* TR,
		       const std::set<int>& SurfSet) 
  /*!
    Given a rule and surface set :
    Iterate over the values
    \param TR :: TRuncated rule
    \param SurfSet :: Surface Set
    \return String
  */
{
  ELog::RegMethod RegA("RuleCheck","checkString");

  MapSupport::mapIterator MI(SurfSet);
  std::ostringstream cx;
  do
    {
      cx<<((TR->isValid(MI.getM())) ? '1' : '0');
    } while(++MI);

  return cx.str();
}

const Geometry::Surface*
RuleCheck::checkSurfPoint(std::set<int>& Active,
			  const Rule* RPtr) 
  /*!
    Internal function to check if the Rule Ptr is within 
    the active surface set as an opposite. The 
    Active surf set covers those rules that have been seen
    already. If an opposite exists and we have  not already
    dealt with the rule then return the surface pointer.

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
	  Active.insert(signSurf);
	  return sA->getKey();
	}
      Active.insert(signSurf);
     }
  return 0;
}

