/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/NTree.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NTree.h"


/*
std::ostream&
operator<<(std::ostream& OX,const NTree& NT)
  /*!
    Write NTree to a stream (used condenced
    format)
    \param NT :: NTree unit
    \param OX :: output stream
    \return OX in modified state
  * /
{
  NT.write(OX);
  return OX;
}
*/
NTree::NTree() 
  /*!
    Default constructor
  */
{}

NTree::NTree(const NTree& A) :
  itemType(A.itemType),numInt(A.numInt),numDbl(A.numDbl),
  modifier(A.modifier),repeats(A.repeats),subTree(A.subTree)
   /*!
    Copy constructor
    \param A :: NTree to copy
  */
{}


NTree::NTree(const std::string& N)
  /*!
    Constructor based on a simple string
    \param N :: String to pass
  */
{ 
  const int errN=processString(N);
  if (errN)
    throw ColErr::InvalidLine(N,"NTree::constructor");
}

NTree&
NTree::operator=(const NTree& A)
  /*!
    Assignment operator
    \param A :: NTree to copy
    \return *this
  */
{
  if (this!=&A)
    {
      itemType=A.itemType;
      numInt=A.numInt;
      numDbl=A.numDbl;
      modifier=A.modifier;
      repeats=A.repeats;
      subTree=A.subTree;
    }
  return *this;
}

NTree::~NTree()
  /*!
    Destructor
  */
{}

std::vector<double>
NTree::getInterval(const size_t index) const
  /*!
    Given a value determine the log interval values
    between index-1 and index+1 [which must be double/int]
    \param index :: Index to the log/interval repeat
    \return Vector of values 
  */
{
  ELog::RegMethod RegA("NTree","getInterval");

  if (index<1 || index+1>=itemType.size())
    throw ColErr::IndexError<size_t>(index,itemType.size(),"index");

  if (itemType[index-1]!=IType::dble || itemType[index-1]!=IType::integer)
    throw ColErr::TypeMatch
      (StrFunc::makeString(static_cast<size_t>(itemType[index-1])),
       StrFunc::makeString(index-1),"Not Double/Int");
  if (itemType[index-1]!=IType::dble || itemType[index-1]!=IType::integer)
    throw ColErr::TypeMatch
      (StrFunc::makeString(static_cast<size_t>(itemType[index-1])),
       StrFunc::makeString(index-1),"Not Double/Int");
  if (itemType[index+1]!=IType::dble || itemType[index+1]!=IType::integer)
    throw ColErr::TypeMatch
      (StrFunc::makeString(static_cast<size_t>(itemType[index+1])),
       StrFunc::makeString(index+1),"Not Double/Int");

  const double DA= (itemType[index-1]==IType::dble) ?
    numDbl.find(index-1)->second :
    static_cast<double>(numInt.find(index-1)->second); 
  
  const double DB= (itemType[index-1]==IType::dble) ?
    numDbl.find(index-1)->second :
    static_cast<double>(numInt.find(index-1)->second); 

  std::vector<double> Out;
  const size_t NR=repeats.find(index)->second;
  if (itemType[index]==IType::log)
    {
      // Check negative??
      const double LDA=std::log(std::abs(DA));
      const double LDB=std::log(std::abs(DB));
      const double step((LDB-LDA)/(NR+1));
      double V(LDA);
      for(size_t i=0;i<NR;i++)
	{
	  V+=step;
	  Out.push_back(exp(V));
	}
    }

  else  // Interval
    {
      const double step((DB-DA)/(NR+1));
      double V(DA);
      for(size_t i=0;i<NR;i++)
	{
	  V+=step;
	  Out.push_back(V);
	}
    }
  return Out;
}


void
NTree::clearAll()
  /*!
    Erase the whole tree
   */
{
  itemType.clear();
  numInt.erase(numInt.begin(),numInt.end());
  numDbl.erase(numDbl.begin(),numDbl.end());
  repeats.erase(repeats.begin(),repeats.end());
  subTree.erase(subTree.begin(),subTree.end());
  return;
}

int 
NTree::processString(const std::string& N)
  /*!
    Process a string of the type 
      - {(} 3 -4 5 item=56 T C {)}
      
    The T and C characters are optional and 
    item= is optional. 

    \retval 0 :: success
    \retval -1 :: string empty
    \retval -2 :: Unable to process a number
    \retval -3 :: Error with item= part
  */
{
  if(N.empty()) return -1;
  // Work with part of the string first

  // Check brackets create sub units
  clearAll();
  long int bCnt(1);
  size_t i(0);
  while(i<N.size() && bCnt>0)
    {
      if (N[i]=='(') bCnt++;
      if (N[i]==')') bCnt--;
      i++;
    }
  if (bCnt!=1)
    throw ColErr::InvalidLine(N,"bracket mis-match",0);

  std::string fullUnit=
    StrFunc::fullBlock(N);

  std::string BPart;
  std::string Part;
  long int IItem;
  double DItem;
  while(StrFunc::sectionBracket(fullUnit,BPart)  ||
	StrFunc::section(fullUnit,Part))
    {
      size_t NCnt(1);
      StrFunc::sectPartNum(Part,NCnt);
      StrFunc::lowerString(Part);  
      // Bracket [Note must over cut Part]]: 
      if (!BPart.empty())
	{
	  // New tree:
	  NTree BItem;
	  if (!BItem.processString(Part))
	    throw ColErr::InvalidLine(Part+"::"+N,"BTree item failed");

	  subTree.emplace(itemType.size(),BItem);
	  itemType.push_back(IType::ntree);
	}
      else if (Part=="log")
	{
	  repeats.emplace(itemType.size(),NCnt);
	  itemType.push_back(IType::log);
	}
      else if (Part=="i")
	{
	  repeats.emplace(itemType.size(),NCnt);
	  itemType.push_back(IType::interval);
	}
      else if (Part=="r")
	{
	  repeats.emplace(itemType.size(),NCnt);
	  itemType.push_back(IType::repeat);
	}
      else if (Part=="j")
	{
	  repeats.emplace(itemType.size(),NCnt);
	  itemType.push_back(IType::j);
	}
      else if (Part=="c" || Part=="t")
	{
	  modifier.emplace(itemType.size(),Part);
	  itemType.push_back(IType::modifier);
	}
      else if (StrFunc::section(Part,IItem))
	{
	  numInt.emplace(itemType.size(),IItem);
	  itemType.push_back(IType::integer);
	}
      else if (StrFunc::section(Part,DItem))
	{
	  numInt.emplace(itemType.size(),DItem);
	  itemType.push_back(IType::dble);
	}
      else
	throw ColErr::InvalidLine
	  (Part+"::"+N,"Item failed");

      BPart="";
    }
  return 0;
}

size_t
NTree::count() const
  /*!
    Counts the number of terms (bracketed)
    \return Number of units found 
  */
{
  ELog::RegMethod RegA("NTree","count");
  if (itemType.empty()) return 0;
  
  return
    static_cast<size_t>(std::count(itemType.begin(),
				   itemType.end(),IType::ntree))+1;
}


void
NTree::addUnits(const std::vector<double>& DValues) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param IValues :: vector of values to add to the integers
  */
{
  for(const double D : DValues)
    {
      numDbl.emplace(itemType.size(),D);
      itemType.push_back(IType::dble);
    }
  return;
}

void
NTree::addUnits(const std::vector<int>& IValues) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param DValues :: vector of values to add to the 
  */
{
  for(const int I : IValues)
    {
      numInt.emplace(itemType.size(),I);
      itemType.push_back(IType::integer);
    }
  return;
}
 
void
NTree::addComp(const double DVal) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param DVal :: vector of values to add to the 
  */
{
  numDbl.emplace(itemType.size(),DVal);
  itemType.push_back(IType::dble);
  return;
}

void
NTree::addComp(const long int IVal) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param IVal :: vector of values to add to the 
  */
{
  numInt.emplace(itemType.size(),IVal);
  itemType.push_back(IType::integer);
  return;
}

void
NTree::addComp(const std::string&) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param Itm :: string to process
  */
{
  
  return;
}


std::vector<double>
NTree::actualValues() const
  /*!
    Express just the actual items
    \return list of items
  */
{
  ELog::RegMethod RegA("NTree","actualDbl");
  
  std::vector<double> Out;
  for(size_t index=0;index<itemType.size();index++)
    {
      const IType& IT(itemType[index]);
      switch(IT)
	{
	case IType::dble:
	  Out.push_back(numDbl.find(index)->second);
	  break;
	case IType::integer:
	  Out.push_back(static_cast<double>(numInt.find(index)->second));
	  break;
	case IType::repeat:
	  {
	    const double DBack(Out.back());
	    const size_t NR(repeats.find(index)->second);
	    for(size_t i=0;i<NR;i++)		
	      Out.push_back(DBack);
	    break;
	  }
	case IType::interval:
	  {
	    std::vector<double> OutX=
	      getInterval(index);
	    Out.insert(Out.end(),OutX.begin(),OutX.end());
            break;
	  }
	case IType::log:
	  {
	    std::vector<double> OutX=
	      getInterval(index);
	    Out.insert(Out.end(),OutX.begin(),OutX.end());
            break;
	  }
	default:
	  break;
	}
    }

  return Out;
}

std::string
NTree::str() const
  /*!
    Write out the Range to a string
  */
{
  ELog::RegMethod RegA("NTree","str");
  
  std::stringstream cx;
  for(size_t index=0;index<itemType.size();index++)
    {
      const IType& iT(itemType[index]);
      switch(iT)
	{
	case IType::j:
	  cx<<"j ";
	  break;
	case IType::integer:
	  cx<<numInt.find(index)->second<<" ";
	  break;
	case IType::dble:
	  cx<<numDbl.find(index)->second<<" ";
	  break;
	case IType::repeat:
	  {
	    const size_t NR(repeats.find(index)->second);
	    if (NR>1) cx<<NR;
	    cx<<"r ";
	  }
	  break;
	case IType::interval:
	  {
	    const size_t NR(repeats.find(index)->second);
	    if (NR>1) cx<<NR;
	    cx<<"i ";
	  }
	  break;
	case IType::modifier:
	  cx<<modifier.find(index)->second<<" ";
	  break;
	case IType::ntree:
	  cx<<"( "<<subTree.find(index)->second.str()<<") ";
	  break;
	default:
	  throw ColErr::InContainerError<size_t>
	    (static_cast<size_t>(iT),"Item not known");
	}
    }
  return cx.str();
}


 
void
NTree::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  OX<<str();
  return;
}



