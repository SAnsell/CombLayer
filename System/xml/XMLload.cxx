/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLload.cxx
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
#include <vector>
#include <list>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "XMLload.h"

namespace XML
{

XMLload::XMLload() : 
  cPos(-1)
  /*!
    Constructor
  */
{}
  
XMLload::XMLload(const std::string& Fname) :
  cPos(-1)
  /*!
    Constructor
    \param Fname :: File to open
  */
{
  openFile(Fname);
}

XMLload::~XMLload()
  ///< Destructor
{}

int
XMLload::operator++()
  /*!
    Increase the count by one
    \retval 0 :: valid
    \retval -1 :: at begin
    \retval 1 :: at end 
   */
{
  cPos++;
  if (cPos>=static_cast<long int>(Lines.length()))
    getNext();
  return (cPos>=static_cast<long int>(Lines.length())) ? 1 : 0;
}

int
XMLload::operator--()
  /*!
    Increase the count by one
    \retval 0 :: valid
    \retval -1 :: at begin
    \retval 1 :: at end 
  */
{
  cPos--;
  return (cPos<0) ? -1 : 0; 
}

int
XMLload::operator+=(const int Index) 
  /*!
    Increase the count by Index
    \param Index :: Amount to add						
    \retval 0 :: valid
    \retval -1 :: at begin
    \retval 1 :: at end 
  */
{
  cPos+=Index;
  if (cPos>=static_cast<int>(Lines.length()))
    getNext();
  if (cPos<0)
    {
      cPos=-1;
      return -1;
    }
  return (cPos>=static_cast<int>(Lines.length())) ? 1 : 0;
}

int
XMLload::operator-=(const int Index) 
  /*!
    Increase the count by Index
    \param Index :: Amount to add						
    \retval 0 :: valid
    \retval -1 :: at begin
    \retval 1 :: at end 
  */
{
  return operator+=(-Index);
}

void
XMLload::setPos(const long int pS)
  /*!
    Set the counter position
    \param pS :: New Position
   */
{
  
  const long int LN=static_cast<long int>(Lines.length());
  cPos= (pS<LN) ? pS : LN;
  return;
}

int
XMLload::openFile(const std::string& Fname)
  /*!
    Open a new file
    \param Fname :: file
    \retval 0 :: sucess 
    \retval -1 :: fail
   */
{
  ELog::RegMethod RegA("XMLload","openFile");
  if (Fname.empty()) return -1;
  IX.close();

  IX.open(Fname.c_str());
  if (!IX.good())
    ELog::EM<<"Failed to open file :"<<Fname<<":"<<ELog::endWarn;
  cPos=-1;
  Lines.erase(0);
  getNext();
  cPos=(Lines.empty() ? -1 : 0);
  return (cPos<0) ? -1 : 0;
}

void
XMLload::getNext()
  /*!
    Get some extra lines if required.
    Does not adjust cPos
  */
{
  IX.clear();
  const int NExtra(10);
  int cnt(NExtra);
  do
    {
      std::string LComp=StrFunc::getAllLine(IX,2048);
      if (IX.gcount()>0)
	Lines+=" "+StrFunc::stripMultSpc(LComp);
      cnt--;
    } while(IX.good() && cnt);
  stripComment();
  return;
}

int
XMLload::pop()
  /*!
    Remove evertything up to cPos
    \retval 1 :: valid input available 
    \retval 0 :: All used up
  */
{ 
  if (cPos>0)
    Lines.erase(0,static_cast<size_t>(cPos));
  if (cPos<0)
    Lines.erase(0);
  cPos=0;
  if (Lines.empty())
    getNext();
  cPos=Lines.empty() ? -1 : 0;
  return (cPos) ? 0 : 1;
}

char
XMLload::current() const
  /*!
    Get the current character
    \return current values or 0 if cPos out of range
  */
{
  return (cPos<0 || cPos>=static_cast<int>(Lines.length()))  
    ? static_cast<char>(0) : Lines[static_cast<size_t>(cPos)];
}


int
XMLload::get(char& c) 
  /*!
    Get next character. Does not consume input
    \param c :: character that is current.
    \return 0 if invalid or character
  */
{
  if(!operator++())  // ensures that cPos >=0
    {
      c=Lines[static_cast<size_t>(cPos)];
      return 1;
    }
  return 0;
}

int
XMLload::good()
  /*!
    Determine if the file still has stuff in it
    \return 0 on failure / 1 on success
  */
{
  if (Lines.empty())
    getNext();
  if ((cPos>0 && cPos>=static_cast<int>(Lines.size())-1) ||
      !Lines.empty())
    return 1;
  // Failed
  return 0;
}
  
std::string
XMLload::getSection()
  /*!
    Get a current space deliminted unit.
    \return Item (or empty string)
  */
{			
  if (cPos<0)
    return std::string("");

  std::string out;
  size_t nL(Lines.length());
  while(cPos>=0 && !isspace(Lines[static_cast<size_t>(cPos)]))
    {
      out+=Lines[static_cast<size_t>(cPos)];
      cPos++;
      if (cPos==static_cast<long int>(nL))
	getNext();
      nL=Lines.length();
    }
  return out;
}

void
XMLload::eraseSection(const size_t A,const size_t B)
  /*!
    Strip a section out of the line
    \param A :: First point
    \param B :: Second point 
  */
{
  const size_t Apt=std::min(A,B);
  const size_t Bpt=std::min(std::max(A,B),Lines.length());
  if (Apt < Lines.length())
    {
      // Doesn't matter if it extends beyond 
      Lines.erase(Apt,Bpt-Apt+1);      
    }
  if (Lines.empty())
    cPos=-1;
  else if (cPos>=static_cast<long int>(Apt) && 
	   cPos<=static_cast<long int>(Bpt))
    cPos=static_cast<long int>(Apt)-1;
  else if (cPos>static_cast<long int>(Apt))
    cPos-=Bpt-Apt+1;
  return;
}


int
XMLload::stripComment() 
  /*!
    After a \<!-- for a comment is read
    until --\> closes the system
    and pass the line object
    \retval 0 :: Success
    \retval -1 : failed to find end
  */
{
  // Now read body (until close)
  size_t LN=Lines.length();
  size_t comment=0;
  int quote=0; 
  size_t i = (cPos>0) ? static_cast<size_t>(cPos) : 0;
  for(;i<LN || (comment && IX.good());i++)
    {
      if (i==LN)
        {
	  getNext();
	  LN=Lines.length();
	  if (i==LN)
	    return -1;
	}
      if (Lines[i]=='\'' && (i==0 || Lines[i-1]!='\\'))
	quote=1-quote;
      if (!quote)
        {
	  if (!comment && Lines[i]=='<' 
	      && Lines.substr(i,4)=="<!--")
	    comment=i+1;
	  if (comment && Lines[i]=='>' &&
	      Lines.substr(i-2,3)=="-->")
	    {
	      eraseSection(comment-1,i);
	      comment=0;
	      LN=Lines.length();
	    }
	}
    }
  return 0;
}


}   // NAMESPACE XML


