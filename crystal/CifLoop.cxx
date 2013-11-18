/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystal/CifLoop.cxx
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
#include <sstream>
#include <string>
#include <map>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "loopItem.h"
#include "CifLoop.h"

namespace Crystal
{

CifLoop::CifLoop() :
  status(0),iSize(0)
  /*!
    CifLoop Constructor
  */
{}

CifLoop::CifLoop(const CifLoop& A) :
  status(A.status),iSize(A.iSize),
  Index(A.Index),Lines(A.Lines)
  /*!
    CifLoop Copy Constructor
    \param A :: CifLoop 
  */
{}

CifLoop&
CifLoop::operator=(const CifLoop& A)
  /*!
    Assignment operator
    \param A :: CifLoop
    \return *this
  */
{
  if (this!=&A)
    {
      status=A.status;
      iSize=A.iSize;
      Index=A.Index;
      Lines=A.Lines;
    }
  return *this;
}

void
CifLoop::clearAll()
  /*!
    Clear and reset status
  */
{
  status=(status==4) ? 1 : 0; 
  Lines.clear(); 
  Index.erase(Index.begin(),Index.end());
  iSize=0;
  return;
}

bool
CifLoop::hasKey(const std::string& keyName) const
  /*!
    Determine if the current loop has a particular key
    \param keyName :: Key to test
    \return true/false on found/not found
  */
{
  std::map<std::string,size_t>::const_iterator mc=Index.find(keyName);
  return (mc==Index.end()) ? 0 : 1;
}  

void
CifLoop::insertKey(const std::string& KStr)
  /*!
    Insert a key string
    \param KStr :: Key to insert [including leading _ ]
  */
{
  std::map<std::string,size_t>::const_iterator mc=Index.find(KStr);
  if (mc==Index.end())
    {
      Index.insert(std::map<std::string,size_t>::value_type(KStr,iSize));
      iSize++;
    }
  return;
}

int
CifLoop::insertData(const std::string& Data)
  /*!
    Insert a data item to the last item if possible
    \param Data :: Data Item
    \return 1 if a new Dat Line is required
  */
{
    if (Lines.empty()) return -1;
    return Lines.back().addNext(Data);

}

int
CifLoop::getIntFromKey(const std::vector<std::string>& SMap,
		       std::vector<size_t>& Out) const
  /*!
    Get a map of items
    \param SMap :: order and items to find
    \param Out :: Index vector
    \return Number of keys found
  */
{
  Out.clear();
  int cnt(0);
  std::vector<std::string>::const_iterator vc;
  for(vc=SMap.begin();vc!=SMap.end();vc++)
    {
      std::map<std::string,size_t>::const_iterator mc=Index.find(*vc);
      if (mc==Index.end())
	Out.push_back(static_cast<size_t>(-1));
      else
	{
	  Out.push_back(mc->second);
	  cnt++;
	}
    }
  return cnt;
}

int
CifLoop::registerLine(std::string& Line)
  /*!
    Given a line item determine if loop action required
    \param Line :: Key item to add to list
    \retval 0 :: Not a loop action
    \retval 1 :: Loop Action
  */
{
  ELog::RegMethod RControl("CifLoop","registerLine");
  std::string Item;

  while(!Line.empty())
   {
     if (status==0 || status==3)  // NOT in loop:
       {
	 // No item:
	 if (!StrFunc::convert(Line,Item) ||
	     Item!="loop_" ) 
	   return 0;                
	 StrFunc::section(Line,Item);
	 status=1;                // KEY zonex
       }
  
     if (status==1)                // KEY ITEM
       {
	 if (StrFunc::convert(Line,Item))
	   {
	     if (Item[0]=='_')
	       {
		 StrFunc::section(Line,Item);
		 insertKey(Item);
	       }
	     else
	       {
		 Lines.push_back(loopItem(iSize));
		 status=2;
	       }
	   }
       }

     if (status==2)           // Data SECTION
       { 
	 if (StrFunc::convert(Line,Item))
	   {
	     if (Item=="loop_")
	       {
		 StrFunc::section(Line,Item);
		 status=4;
		 removeLastLine();
		 return 0;
	       }
	     else if (Item[0]=='_')
	       {
		 status=3;
		 removeLastLine();
		 return 0;
	       }
	     while(StrFunc::quoteBlock(Line,Item))
	       {
		 if (insertData(Item))
		   Lines.push_back(loopItem(iSize));
	       }				
	   }
       }
   }
  return 1;
}

void
CifLoop::removeLastLine()
  /*!
    Remove Any item that has insufficient size
  */
{
  if (Lines.empty()) 
    return;
  if (Lines.back().isEmpty())
    {
      Lines.pop_back();
      return;
    }
  if (!Lines.back().isFull())
    {
      ELog::EM<<"Incomplete item "<<Lines.back()<<ELog::endWarn;
    }
  return;
}


template<typename T>
int
CifLoop::getItem(const size_t lineIndex,const size_t pointIndex,T& Obj) const
  /*!
    Get a specific item and convert
    \param lineIndex :: line index     
    \param pointIndex :: data item index
    \param Obj :: Out value
    \return 1 on succes / 0 on failure
  */

{
  if (lineIndex>=Lines.size())
    throw ColErr::IndexError<size_t>(lineIndex,Lines.size(),
				     "CifLoop::getItem");
  
  return Lines[lineIndex].getItem(pointIndex,Obj);

}

///\cond TEMPLATE
template int CifLoop::getItem(const size_t,const size_t,double&) const;
template int CifLoop::getItem(const size_t,const size_t,int&) const;
template int CifLoop::getItem(const size_t,const size_t,std::string&) const;
///\endcond TEMPLATE

}  // NAMESPACE Crystal
