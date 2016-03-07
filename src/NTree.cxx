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
#include "NTree.h"



std::ostream&
operator<<(std::ostream& OX,const NTree& NT)
  /*!
    Write NTree to a stream (used condenced
    format)
    \param NT :: NTree unit
    \param OX :: output stream
    \return OX in modified state
  */
{
  NT.write(OX);
  return OX;
}

NTree::NTree() 
  /*!
    Default constructor
  */
{}

NTree::NTree(const NTree& A) : 
  itemType(A.itemType),numInt(A.numInt),numDbl(A.numDbl),
  repeats(A.repeats),subTree(A.subTree)
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

  std::string Part;
  while(StrFunc::section(fullUnit,Part))
    {
      // Bracket [Note must over cut Part]]: 
      if (Part[0]=='(' &&
	  StrFunc::sectionBracket(fullUnit,Part) )
	{
	  NTree BItem;
	  if (BItem.processString(Part))
	    {
	      
	      
	  if (
	}

    }
  
  
  
      
  
  std::string MS(N);
  // Change all ( and ) into <spc>)<spc>'
  std::string::size_type pos(0);
  pos=MS.find_first_of(")(",pos);
  while(pos!=std::string::npos)
    {
      if (MS[pos]=='(')
	MS.replace(pos,1," ( ");
      else
	MS.replace(pos,1," ) ");
      pos+=3;
      pos=MS.find_first_of(")(",pos);
    }

  // From now on in we have a string type (definately)
  // check if the next number is a number or doesn't
  // exist.
  Unit Value;
  std::string Comp;
  
  while(StrFunc::section(MS,Comp))
    {
      // First determine if it is "T" or "C"
      if (Comp=="(" || Comp==")")
        {
	  Items.push_back(CompUnit(1,0,Comp));
	}
      else if (Comp=="T" || Comp=="C")
        {
	  Items.push_back(CompUnit(1,0,Comp));
	}
      else if (StrFunc::section(Comp,Value))   // Found a number 
        {
	  Items.push_back(CompUnit(0,Value,""));
	}
      else              // Maybe a = string 
        {
	  std::string AssignString;
	  std::string::size_type pos=Comp.find("=");
	  if (pos==std::string::npos)
	    {
	      AssignString=Comp;
	      if (!StrFunc::section(MS,Comp))    // No further bit so must be bad
		return -2;
	      pos=Comp.find("=");
	      if (pos==std::string::npos)        // No = so must be fail
		return -2;
	      AssignString+="=";
	      Comp.erase(0,pos+1);               // Dump up to the '='
	    }
	  else
	    {
	      AssignString=Comp.substr(0,pos+1);
	    }

	  Comp.erase(0,pos+1);               // Dump up to the '='
	  if (Comp.empty() && !StrFunc::section(MS,Comp))      // No bit to add
	    return -2;
	  if (!StrFunc::section(Comp,Value))
	    return -3;
	  Items.push_back(CompUnit(1,0,AssignString));
	  Items.push_back(CompUnit(0,Value,""));
	}
    }
  return 0;
}

int
NTree::count() const
  /*!
    Counts the number of terms (bracketed)
    \return Number of units found 
  */
{
  if (Items.empty())
    return 0;
  typename  std::vector<CompUnit>::const_iterator vc;
  int count(0);
  int brack(0);
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->first==1)
        {
	  if (vc->third=="(")
	    {
	      if (!brack)
		count++;
	      brack++;
	    }
	  else if (vc->third==")")
	    brack--;
	  else if (brack==0 && vc->third!="C") 
	    count++;
	}
      else if (!brack)
        {
	  count++;
	}
    }
  return count;
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
      itemType.push_back(IType::dbl);
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
NTree::addComp(const double& DVal) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param DVal :: vector of values to add to the 
  */
{
  numDbl.emplace(itemType.size(),D);
  itemType.push_back(IType::dbl);
  return;
}

void
NTree::addComp(const int& IVal) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param IVal :: vector of values to add to the 
  */
{
  numInt.emplace(itemType.size(),I);
  itemType.push_back(IType::integer);
  return;
}

void
NTree::addComp(const std::string& Item) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param Itm :: string to process
  */
{
  return;
}




std::vector<double>
NTree::actualDbl() const
  /*!
    Express just the actual items
    \return list of items
  */
{

  std::vector<double> Out;
  for(vc=Items.begin();vc!=Items.end();vc++)
    if (vc->first==0)
      Out.push_back(vc->second);

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
  for(size_t index=0;i<itemType.size();indeX++)
    {x
      const size_t& iT(itemType[index]);
      switch(it)
	{
	case IType::j:
	  cx<<"j ";
	  break;
	case IType::integer:
	  cx<<numInt[index]<<" ";
	  break;
	case IType::dble:
	  cx<<numDbl[index]<<" ";
	  break;
	case IType::repeat:
	  cx<<repeats[index]<<"r ";
	  break;
	case IType::interval:
	  cx<<repeats[index]<<"i ";
	  break;
	case IType::log:
	  cx<<repeats[index]<<"log ";
	  break;
	case IType::ntree:
	  cx<<"( "<<subTree[index].str()<<") ";
	  break;
	default:
	  throw ColErr::InContainerError<size_t>(iT,"Item not known");
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



