/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/InsertComp.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include <memory>

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
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "InsertComp.h"

namespace attachSystem
{

InsertComp::InsertComp() :
  outerSurf(0)
  /*!
    Constructor 
  */
{}

InsertComp::InsertComp(const InsertComp& A) : 
  outerSurf((A.outerSurf) ? A.outerSurf->clone() : 0)
  /*!
    Copy constructor
    \param A :: InsertComp to copy
  */
{}

InsertComp&
InsertComp::operator=(const InsertComp& A)
  /*!
    Assignment operator
    \param A :: InsertComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      delete outerSurf;
      outerSurf=(A.outerSurf) ? A.outerSurf->clone() : 0;
    }
  return *this;
}


InsertComp::~InsertComp()
  /*!
    Deletion operator
  */
{
  delete outerSurf;
}

void
InsertComp::setInterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf(std::string)");
  MonteCarlo::Object Obj(1,0,0.0,SList);
  // Null object : Set and return
  delete outerSurf;
  outerSurf=Obj.topRule()->clone();
  return;
}

void
InsertComp::addInterSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
    \todo : Put this in Rule??
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf");
  // Null object : Set and return
  if (!outerSurf)
    {
      outerSurf=new SurfPoint(static_cast<Geometry::Surface*>(0),SN);
      return;
    }
  // This is an intersection and we want to add our rule at the base
  // Find first item that is not an intersection
  Rule* RPtr(outerSurf);
  std::deque<Rule*> curLevel;
  curLevel.push_back(outerSurf);
  while(!curLevel.empty())
    {
      RPtr=curLevel.front();
      curLevel.pop_front();
      if (RPtr->type()!=1)   // Success not an intersection
	{
	  Rule* parent=RPtr->getParent();     // grandparent
	  Rule* sA=RPtr;
	  Rule* sB=new SurfPoint(0,SN);
	  Rule* Item=new Intersection(parent,sA,sB);
	  // Find place ot insert it
	  if (!parent)
	    outerSurf=Item;
	  else
	    parent->setLeaf(Item,parent->findLeaf(RPtr));
	  return;
	}
      curLevel.push_back(RPtr->leaf(0));
      curLevel.push_back(RPtr->leaf(1));
    }
  ELog::EM<<"Failed on rule structure  "<<ELog::endErr;
  throw ColErr::ExitAbort(RegA.getFull());
  return;
}

void
InsertComp::addInterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf(std::string)");
  MonteCarlo::Object Obj(1,0,0.0,SList);
  if (Obj.topRule())
    {
      // Null object : Set and return
      if (!outerSurf)
	{
	  outerSurf=Obj.topRule()->clone();
	  return;
	}
      // This is an intersection and we want to add our rule at the base
      // Find first item that is not an intersection
      Rule* RPtr(outerSurf);
      std::deque<Rule*> curLevel;
      curLevel.push_back(outerSurf);
      while(!curLevel.empty())
	{
	  RPtr=curLevel.front();
	  curLevel.pop_front();
	  if (RPtr->type()!=1)   // Success not an intersection
	    {
	      Rule* parent=RPtr->getParent();     // grandparent
	      Rule* sA=RPtr;
	      Rule* sB=Obj.topRule()->clone();
	      Rule* Item=new Intersection(parent,sA,sB);
	      // Find place ot insert it
	      if (!parent)
		outerSurf=Item;
	      else
		parent->setLeaf(Item,parent->findLeaf(RPtr));
	      return;
	    }
	  curLevel.push_back(RPtr->leaf(0));
	  curLevel.push_back(RPtr->leaf(1));
	}
      ELog::EM<<"Failed on rule structure  "<<ELog::endErr;
      exit(1);
    }
  return;
}

std::string
InsertComp::getExclude() const
  /*!
    Calculate the write out the excluded surface
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("InsertComp","getExclude");

  if (outerSurf)
    {
      MonteCarlo::Algebra AX;
      AX.setFunctionObjStr("#("+outerSurf->display()+")");
      return AX.writeMCNPX(); 
   }
  return "";
}



  
  
}  // NAMESPACE attachSystem
