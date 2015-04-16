/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLiterator.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iterator>

#include "Exception.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLvector.h"
#include "XMLgrid.h"
#include "XMLgroup.h"
#include "XMLcomp.h"
#include "XMLload.h"
#include "XMLcollect.h"
#include "XMLnamespace.h" 
#include "XMLiterator.h"

namespace XML
{

XMLiterator::XMLiterator(const XMLgroup* Bptr)  :
  markNum(0),count(0),hold(0),
  GPtr(0),CPtr(0),Master(Bptr)
  /*!
    Constructor :
    \param Bptr :: Base pointer 
  */
{ 
  if (Master && Master->getObject(0))   // Maybe we are not initialised yet
    init();
}


XMLiterator::XMLiterator(const XMLiterator& A) :
  markNum(A.markNum),count(A.count),hold(A.hold),
  GPtr(A.GPtr),CPtr(A.CPtr),markGrp(A.markGrp),
  markSet(A.markSet),Master(A.Master),
  Pos(A.Pos),XG(A.XG) 
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

XMLiterator&
XMLiterator::operator=(const XMLiterator& A) 
  /*!
    Assignement operator=
    \param A :: Iterator to copy
    \return this
  */
{
  if (this!=&A)
    {
      markNum=A.markNum;
      count=A.count;
      hold=A.hold;
      GPtr=A.GPtr;
      CPtr=A.CPtr;
      markGrp=A.markGrp;
      markSet=A.markSet;
      Master=A.Master;
      Pos=A.Pos;
      XG=A.XG;
    }
  return *this;
}

void 
XMLiterator::init()
  /*!
    Initialise the stack.
    Take the current position in the XMLgroup 
  */
{
  markNum=0;
  count=0;
  hold=0;
  CPtr=Master;
  GPtr=Master;
  markGrp.clear();
  markGrp.push_back(MVAL(0,0));             // Suff
  markSet.erase(markSet.begin(),markSet.end());
  XG.clear();
  Pos.clear();
  Pos.push_back(-1);
  XG.push_back(Master);
  return;
}

int
XMLiterator::seek(const std::string& ObjName,const int index)
  /*!
    Keep the group but move to a new item
    \param ObjName :: New object name
    \param index :: Number to skip
    \return 0 :: End reached
    \return 1 :: Success
  */
{
  int cnt(0);
  XMLiterator& SK(*this); 
  do
    {
      if ((*SK) && SK->getKey()==ObjName)
        {
	  if (index<=cnt)
	    return 1;
	  cnt++;
	}
    }
  while((SK++) == 1);
  return 0;
}

int
XMLiterator::seekInternal(const std::string& ObjName,const int index)
  /*!
    Keep the group but move to a new item
    \param ObjName :: New object name
    \param index :: Number to skip
    \return 0 :: End reached
    \return 1 :: Success
  */
{
  XMLiterator& SK(*this); 
  const XMLiterator currentSK(*this);


  // Use SK: and copy back if failed
  setMark();
  int cnt(0);
  do
    {
      if ((*SK) && SK->getKey()==ObjName)
        {
	  if (index<=cnt)
	    return 1;
	  cnt++;
	}
    }
  while(SK++ == 1);
  // Failed
  SK=currentSK;
  return 0;
}


int
XMLiterator::skipGroup()
  /*!
    Skip the current block
    \retval 0 (file end) 
    \retval 1 on exit
  */
{
  setMark();
  int outFlag;
  do
    {
      outFlag= this->operator++();
    }
  while (outFlag == 1);
  return -1*outFlag;
}

int
XMLiterator::setMark()
  /*!
    Set the mark level:
    \return markNumber used 
  */
{
  markNum++;
  markGrp.push_back(MVAL(GPtr,markNum));
  markSet.insert(markNum);  
  return markNum;
}
  
int
XMLiterator::hasMark(const int Index) const
  /*!
    Determine if Index is in the current mark
    set
    \param Index :: Value to check
    \return True if Index is a valid mark 
  */
{
  std::set<int>::const_iterator vc=
    markSet.find(Index);
  return (vc==markSet.end()) ? 0 : 1;
}

void
XMLiterator::clearMark()
  /*!
    Clear the top level mark
  */
{
  std::set<int>::iterator vc=markSet.find(markGrp.back().second);
  if (vc==markSet.end())
    throw ColErr::InContainerError<int>(markGrp.back().second,
					"XMLiterator::clearMark");
  markSet.erase(vc);
  markGrp.pop_back();
  return;
}

int 
XMLiterator::operator++()
  /*!
    Get the next object / group :: Updated from the original which
    only did objects
    \retval 1 :: New real object
    \retval -1*Number of marks :: Hit Mark
    \retval 0 :: Reinitialise
  */
{
  if (Pos.empty())
    return 0;
  if (hold)
    {
      hold--;
      return 0;
    }
  int retVal(1);
  // add one to the top item on index-stack 
  CPtr=XG.back()->getObject(static_cast<size_t>(++Pos.back()));  
  count++;
  while(!CPtr)        // Failed to get next item : end of group info
    {
      if (markGrp.back().first==XG.back())
        {
	  clearMark();
	  retVal= -1;
	}
      Pos.pop_back();      
      XG.pop_back();
      if (Pos.empty())
	return 0;
      // add one to the top item on index-stack 
      CPtr=XG.back()->getObject(static_cast<size_t>(++Pos.back())); 
      count++;
    }
  const XMLgroup* Gtest=dynamic_cast<const XMLgroup*>(CPtr); // determine if it is a group
  if (Gtest)          // Found a new group
    {
      GPtr=Gtest;
      Pos.push_back(-1);
      XG.push_back(Gtest);
    }
  return retVal;
}

int 
XMLiterator::operator--() 
  /*!
    Get the previous object / group :: Updated from the original which
    only did objects
    \retval 0 :: init
    \retval 1 :: Object found
  */
{
  if (Pos.empty())
    return 0;
  // Get new index
  long int index= --Pos.back();                // Min number -1:
  do 
    {
      if (index>=0)
        {
	  CPtr=GPtr->getObject(static_cast<size_t>(index));
	  const XMLgroup* Gtest=dynamic_cast<const XMLgroup*>(CPtr);   // determine if it is a group	  
	  while (Gtest)
	    {
	      GPtr=Gtest;
	      long int gmax(-1);
	      do
	        {
		  gmax++;
		  CPtr=GPtr->getObject(static_cast<size_t>(gmax));
		} while(CPtr);
	      Pos.push_back(gmax-1);
	      Gtest=dynamic_cast<const XMLgroup*>(CPtr);   // determine if it is a drop down group
	    }
	  return 1;
	}
      if (index==-1)       // Got to top of group
        {
	  CPtr=GPtr;
	  return 1;
	}
      // Now remove group
      XG.pop_back();
      Pos.pop_back();
      if (XG.empty())
        {
	  init();
	  return 0;
	}
      GPtr=XG.back();       
      index= --Pos.back();
    } while (1);
  // -1 position:
  
  return 1;
}

int
XMLiterator::getLevel() const
  /*!
    Get the depth [At that start of a group it is -1]
    \return level 
  */
{
  const int out(static_cast<int>(Pos.size()));
  return (out && Pos.back()==-1) ? out-1 : out;
}

void
XMLiterator::writePos() const
  /*!
    Debug function to write out list
  */
{
  copy(Pos.begin(),Pos.end(),std::ostream_iterator<int>(std::cout," : "));
  std::cout<<std::endl;
  return;
}

} // NAMESPACE XML


