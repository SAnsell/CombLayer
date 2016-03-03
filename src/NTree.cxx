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

  clearAll();


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
NTree::addUnits(const std::vector& Obj) 
  /*!
    Adds units to the list of type vector
    to but not included as fullcomponent
    \param Obj :: vecotr object to add to the list
  */
{
  typename std::vector::const_iterator vc;
  for(vc=Obj.begin();vc!=Obj.end();vc++)
    Items.push_back(CompUnit(0,*vc,""));

  return;
}

void
NTree::addComp(const std::vector& Obj) 
  /*!
    Adds a component to the list of type vector<int/double>
    to be included in a bracket form. E.g. ( 4 5 6 7)
    \param Obj :: vecotr object to add to the list
  */
{
  Items.push_back(CompUnit(1,0,"("));

  typename std::vector::const_iterator vc;
  for(vc=Obj.begin();vc!=Obj.end();vc++)
    Items.push_back(CompUnit(0,*vc,""));

  Items.push_back(CompUnit(1,0,")"));
  return;
}

template<typename Unit>
void
NTree::splitComp()
  /*!
    Take the outer bracket appart
  */
{
  ELog::RegMethod RegA("NTree","splitComp");
  
  const CompUnit LeftB(1,0,"(");
  const CompUnit RightB(1,0,")");
      
  typename std::vector<CompUnit>::iterator ac;    
  // Search for match brackets
  ac=find(Items.begin(),Items.end(),LeftB);
  if (ac==Items.end()) return;

  typename std::vector<CompUnit>::iterator bc(ac+1);
  size_t bCnt(1);
  do
    {
      if (*bc==RightB)
        bCnt--;
      else if (*bc==LeftB)
        bCnt++;
    }
  while(bCnt && ++bc!=Items.end());

  if (!bCnt)
    {
      std::rotate(ac,ac+1,Items.end());
      std::rotate(bc,bc+1,Items.end());
      Items.pop_back();
      Items.pop_back();
    }
  return;    
}

  
void
NTree::addComp(const Unit& Obj) 
  /*!
    Adds a component to the list of type int/double
    \param Obj :: object to add to the list
  */
{
  Items.push_back(CompUnit(0,Obj,""));
  return;
}

void
NTree::addComp(const std::string& Obj) 
  /*!
    Adds a string component to the list
    \param Obj :: object to add to the list
  */
{
  Items.push_back(CompUnit(1,0,Obj));
  return;
}

std::vector
NTree::actualItems() const
  /*!
    Express just the actual items
    \return list of items
  */
{
  typename std::vector<CompUnit>::const_iterator vc;
  std::vector Out;
  for(vc=Items.begin();vc!=Items.end();vc++)
    if (vc->first==0)
      Out.push_back(vc->second);

  return Out;
}

int
NTree::changeItem(const Unit& oldI,const Unit& newI)
  /*!
    Change an actual Item
    \param oldI :: Old value
    \param newI :: New value
    \return 1 on succes / 0 on failure
  */
{
  typename std::vector<CompUnit>::iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->first==0 && vc->second==oldI)
	{
	  vc->second=newI;
	  return 1;
	}
    }
  return 0;
}

void
NTree::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  typename std::vector<CompUnit>::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (!vc->first)
	OX<<vc->second;
      else
	OX<<vc->third;
      OX<<" ";
    }
  return;
}

}

