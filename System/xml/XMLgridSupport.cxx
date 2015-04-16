/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLgridSupport.cxx
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
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <functional>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLgrid.h"
#include "XMLload.h"
#include "XMLread.h"
#include "XMLcollect.h"
#include "XMLnamespace.h"

namespace XML
{

void
combineGrid(XML::XMLcollect& XOut,
	    const std::string& BName,const int blockCnt)
  /*!
    Combined Grid components together (if possible)
    \param XOut :: XMLcollect to obtain merged results from
    \param BName :: Regex name to find
    \param blockCnt :: to write the time separately
  */
{
  typedef XML::XMLgrid<std::vector,double> xGridType;

  int notValidCnt(0);     // index count of items
  int gcnt(0);            // grid object cnt

  XML::XMLobject* oPtr=XOut.findObj(BName);     // Object with grid    

  // Place to put result
  XML::XMLgroup* groupPtr=
    (oPtr) ? dynamic_cast<XML::XMLgroup*>(oPtr->getParent()) : 0; 

  // Get XMLgrid object
  xGridType* gPtr=dynamic_cast<xGridType*>(oPtr);
  xGridType* gStore(0);   // Item before adding
  std::vector<xGridType*> gArray;

  int sNum(0);        
  while(oPtr)     // While findind an object
    {
      if (gPtr)
        {
	  if (!(gcnt % blockCnt))
	    {
	      if (gStore)
	        {
		  gStore->addAttribute("iStart",sNum);
		  gStore->addAttribute("iEnd",gcnt-1);
		  sNum=gcnt;
		  gArray.push_back(gStore);
		}
	      gStore=new xGridType(XOut.getCurrent(),"GridCluster");
	      for(size_t i=0;i<gPtr->getSize();i++)
		gStore->setComp(i,gPtr->getGVec(i));
	    }
	  else
	    {
	      // Add just the non-time points
	      for(size_t i=1;i<gPtr->getSize();i++)
		gStore->setComp(gStore->getSize(),gPtr->getGVec(i));
	    }
	  XOut.deleteObj(oPtr);
	  gcnt++;
	}
      else  // found object but not a grid 
        {
	  notValidCnt++;
	}
      oPtr=XOut.findObj(BName,notValidCnt);
      gPtr=dynamic_cast<xGridType*>(oPtr);
    }


  if (gStore)
    {
      gStore->addAttribute("iStart",sNum);
      gStore->addAttribute("iEnd",gcnt-1);
      gArray.push_back(gStore);
    }

  std::vector<xGridType*>::iterator vc;
  if (groupPtr)
    {
      for(vc=gArray.begin();vc!=gArray.end();vc++)
	groupPtr->addManagedObj(*vc);
    }
  else
    {
      for(vc=gArray.begin();vc!=gArray.end();vc++)
	XOut.getCurrent()->addManagedObj(*vc);
    }

  return;
}  

template<template<typename T,typename Alloc> class V,typename T,typename Alloc>
void
combineDeepGrid(XML::XMLcollect& XOut,const std::string& BName,
		const int blockCnt)
  /*!
    Combined Grid components together (if possible)
    assuming that the grid is a deep grid. e.g.
    - \<Obj> \<stuff> \</stuff> \<grid> \</grid> \</Obj>
    the requirement is that stuff is keep and that 
    
    \param XOut :: XMLcollect to obtain merged results from
    \param BName :: Regex name to find [depth]
    \param blockCnt :: to write the time separately
  */
{
  typedef XML::XMLgrid<V,T,Alloc> xGridType;
  int objectCnt(0);          // Object count
  int gcnt(0);               // grid object cnt

  XML::XMLobject* oPtr=XOut.findObj(BName,objectCnt);
  // Place to put result
  XML::XMLgroup* groupPtr=(oPtr) ? 
    dynamic_cast<XML::XMLgroup*>(oPtr->getParent()) : 0;
  
  if (!groupPtr)  // Failed
    return;
  
  // Now search the group for specifics of 
  int individualGrpCnt(0);

  // Get XMLgrid object (first)
  xGridType* gPtr=groupPtr->getType<xGridType>(individualGrpCnt);
  xGridType* gStore(0);   // Item before adding
  std::vector<xGridType*> gArray;

  int sNum(0);        
  while(gPtr)     // While finding an object
    {
      // Only if keys mathc
      if (matchPath(gPtr->getFullKey(),BName))
        {
	  if (!(gcnt % blockCnt))       // New grid object required
	    {
	      if (gStore)  // push back old object
	        {
		  gStore->addAttribute("iStart",sNum);
		  gStore->addAttribute("iEnd",gcnt-1);
		  sNum=gcnt;
		  gArray.push_back(gStore);
		}
	      gStore=new xGridType(XOut.getCurrent(),"GridCluster");
	      for(size_t i=0;i<gPtr->getSize();i++)
		gStore->setComp(i,gPtr->getGVec(i));
	    }
	  else
	    {
	      // Add just the non-time points
	      for(size_t i=1;i<gPtr->getSize();i++)
		gStore->setComp(gStore->getSize(),gPtr->getGVec(i));
	    }
	  XOut.deleteObj(gPtr);
	  gcnt++;                 // count of object in array
	}
      else
	individualGrpCnt++;     // count of extraction (repeated?)

      // RESET FOR NEXT LOOP:
      gPtr=groupPtr->getType<xGridType>(individualGrpCnt);

    }
  // Add last componenet if valid
  if (gStore)
    {
      gStore->addAttribute("iStart",sNum);
      gStore->addAttribute("iEnd",gcnt-1);
      gArray.push_back(gStore);
    }
  // Now add back grid objects into stream 
  
  typename std::vector<xGridType*>::iterator vc;
  for(vc=gArray.begin();vc!=gArray.end();vc++)
    groupPtr->addManagedObj(*vc);
  return;
}  

/// \cond TEMPLATE

template void 
combineDeepGrid<std::vector,double,std::allocator<double> >
(XML::XMLcollect&,const std::string&,const int);

/// \endcond TEMPLATE

}  // Namespace XML

