/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/support.cxx
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
#include <iostream>
#include <iterator>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <boost/format.hpp>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "support.h"

/*! 
  \file support.cxx 
*/

namespace  StrFunc
{

void 
printHex(std::ostream& OFS,const int n)
  /*!
    Function to convert and number into hex
    output (and leave the stream un-changed)
    \param OFS :: Output stream
    \param n :: integer to convert
    \todo Change this to a stream operator
  */
{
  std::ios_base::fmtflags PrevFlags=OFS.flags();
  OFS<<"Ox";
  OFS.width(8);
  OFS.fill('0');
  hex(OFS);
  OFS << n;
  OFS.flags(PrevFlags);
  return;
} 

std::string
stripMultSpc(const std::string& Line)
  /*!
    Removes the multiple spaces in the line
    \param Line :: Line to process
    \return String with single space components
  */
{
  std::string Out;
  int spc(1);
  size_t lastReal(0);
  int quote(0);
  int dquote(0);
  for(size_t i=0;i<Line.length();i++)
    {
      if (Line[i]=='\'' && (!i || Line[i]!='\\'))
	{
	  quote=1-quote;
	  Out+=Line[i];
	}
      else if (!quote && Line[i]=='\"' && 
	       (!i || Line[i]!='\\'))
	{
	  dquote=1-dquote;
	  Out+=Line[i];
	}
      else
	{
	  if (quote || dquote ||
	      (Line[i]!=' ' && Line[i]!='\t'))
	    {
	      spc=0;
	      Out+=Line[i];
	      lastReal=Out.length();
	    }
	  else if (!spc)
	    {
	      spc=1;
	      Out+=' ';
	    }
	}
    }
  if (lastReal>0 && lastReal<Out.length())
    Out.erase(lastReal);

  return Out;
}

int
extractWord(std::string& Line,const std::string& Word,const size_t cnt)
  /*!
    Checks that as least cnt letters of 
    works is part of the string. It is currently 
    case sensative. It removes the Word if found
    \param Line :: Line to process
    \param Word :: Word to use
    \param cnt :: Length of Word for significants [default =4]
    \retval 1 on success (and changed Line) 
    \retval 0 on failure 
  */
{
  if (Word.empty())
    return 0;

  size_t minSize((cnt>Word.size()) ?  Word.size() : cnt);
  std::string::size_type pos=Line.find(Word.substr(0,minSize));
  if (pos==std::string::npos)
    return 0;
  // Pos == Start of find
  size_t LinePt=minSize+pos;
  for(;minSize<Word.size() && LinePt<Line.size()
	&& Word[minSize]==Line[LinePt];LinePt++,minSize++) ;

  Line.erase(pos,LinePt-(pos-1));
  return 1;
}

int
confirmStr(const std::string& S,const std::string& fullPhrase)
  /*!
    Check to see if S is the same as the
    first part of a phrase. (case insensitive)
    \param S :: string to check
    \param fullPhrase :: complete phrase
    \returns 1 on success 
  */
{
  const size_t nS(S.length());
  const size_t nC(fullPhrase.length());
  if (nS>nC || !nS)    
    return 0;           
  for(size_t i=0;i<nS;i++)
    if (S[i]!=fullPhrase[i])
      return 0;
  return 1;
}


int
getPartLine(std::istream& IX,std::string& Out,
	    std::string& Excess,const size_t spc)
  /*!
    Gets a line and determine if there is addition component to add
    in the case of a very long line.
    \param IX :: input stream to process
    \param Out :: string up to last 'tab' or ' '
    \param Excess :: string after 'tab or ' ' 
    \param spc :: number of char to try to read 
    \retval 1 :: more line to be found
    \retval -1 :: Error with file
    \retval 0  :: line finished.
  */
{
  static size_t size(0);
  static char* ss(0);

  if (IX.good())
    {
      if (size!=spc)
        {
	  delete [] ss;
	  ss=new char[spc+2];
	  size=spc;
	}
      if (Out.length()>spc)
	return -1;
      const long int clen=static_cast<long int>(spc-Out.length()); 
      IX.getline(ss,clen,'\n');
      ss[clen+1]=0;           // incase line failed to read completely
      Out+=static_cast<std::string>(ss);
      if (IX.gcount()==clen-1) 
	{
	  std::string::size_type pos=Out.find_last_of("\t ");
	  if (pos!=std::string::npos)
	    {
	      Excess=Out.substr(pos,std::string::npos);
	      Out.erase(pos);
	    }
	  else
	    Excess.erase(0,std::string::npos);
	  IX.clear();
	  return 1;
	}
      // No continuation line required:
      return 0;
    }
  // Fail
  return -1; 
}

std::string
removeSpace(const std::string& CLine)
  /*!
    Removes all spaces from a string 
    except those with in the form '\ '
    \param CLine :: Line to strip
    \return String without space
  */
{
  std::string Out;
  char prev='x';
  for(unsigned int i=0;i<CLine.length();i++)
    {
      if (!isspace(CLine[i]) || prev=='\\')
        {
	  Out+=CLine[i];
	  prev=CLine[i];
	}
    }
  return Out;
}
	
std::string 
getAllLine(std::istream& fh,const int spc)
  /*!
    Reads a line from the stream of max length spc.
    Trailing comments are removed. (with # or ! character)
    \param fh :: already open file handle
    \param spc :: max number of characters to read 
    \return String read.
  */
{
  static int size(0);
  static char* ss(0);

  std::string Line;
  if (spc>0)
    {
      if (size<spc)
        {
	  delete [] ss;
	  ss=new char[spc+1];
	  size=spc;
	}
      if (fh.good())
        {
	  fh.getline(ss,spc,'\n');
	  ss[spc]=0;           // incase line failed to read completely
	  Line=ss;
	}
    }
  return Line;
}


std::string 
getLine(std::istream& IX,const int spc)
  /*!
    Reads a line from the stream of max length spc.
    Trailing comments are removed. (with # or ! character)
    \param IX :: already open file handle
    \param spc :: max number of characters to read 
    \return string read.
  */

{
  static int size(0);
  static char* ss(0);

  std::string Line;
  if (spc>0)
    {
      if (size!=spc)
        {
	  delete [] ss;
	  ss=new char[spc+1];
	  size=spc;
	}
      if (IX.good())
        {
	  IX.getline(ss,spc,'\n');
	  ss[spc]=0;           // incase line failed to read completely
	  Line=ss;
	  // remove trailing comments
	  std::string::size_type pos = Line.find_first_of("#!");
	  if (pos!=std::string::npos)
	    Line.erase(pos); 
	}
    }
  return Line;
}


void
lowerString(std::string& LN) 
  /*!
    lowers the words in the string
    \param LN :: String to change
  */
{
  for(size_t i=0;i<LN.length();i++)
    LN[i]=static_cast<char>(tolower(LN[i]));
  
  return;
}

int
isEmpty(const std::string& A)
  /*!
    Determines if a string is only spaces
    \param A :: string to check
    \returns 1 on an empty string , 0 on failure
  */
{
  std::string::size_type pos=
    A.find_first_not_of(" \t");
  return (pos!=std::string::npos) ? 0 : 1;
}

int
stripComment(std::string& A)
  /*!
    removes the string after the comment type of 
    '$ ' or '!' or '#  '
    \param A :: String to process
    \return 1 if a comment was found / 0 otherwize
  */
{
  const long int index=hasComment(A,"!","$");
  if (index>=0)
    {
      A.erase(static_cast<size_t>(index),std::string::npos);
      return 1;
    }
  return 0;
}


long int
hasComment(const std::string& A)
  /*!
    Determine the start point of the comment
    '$ ' or '!' or '#  '
    Cannot be in a hard quote
    Cannot be in a weak quote without \ before 
    \param A :: String to process
    \retval Position of comment [-1 on no comment]
  */
{
  return hasComment(A,"#!","$");
}

long int
hasComment(const std::string& A,const std::string& singleKey,
	   const std::string& spcKey)
  /*!
    Determine the start point of the comment
    '$ ' or '!' or '#  '
    Cannot be in a hard quote
    Cannot be in a weak quote without \ before 
    \param A :: String to process
    \param singleKey :: single comment characters
    \param spcKey :: comments which require a space afterward
    \retval Position of comment [-1 on no comment]
  */
{
  const size_t LN=A.size();
  const size_t slen=singleKey.length();
  const size_t dlen=spcKey.length();

  int quote(0);
  int hquote(0);
  size_t index(0);
  while (index<LN)
    {
      if (A[index]=='\'' && (!index || A[index-1]!='\\'))
	hquote=1-hquote;
      if (!hquote && A[index]=='\'' && (!index || A[index-1]!='\\')) 
	quote=1-quote;
      // Comments:
      if (!(quote | hquote))
	{

	  for(size_t i=0;i<slen;i++)
	    if (A[index]==singleKey[i])
	      return static_cast<long int>(index);

	  if (index+1<LN && isspace(A[index+1]))
	    {
	      for(size_t i=0;i<dlen;i++)
		if (A[index]==spcKey[i])
		  return static_cast<long int>(index);
	    }
	}
      index++;
    }
  return -1;
}


int
quoteBlock(std::string& Line,std::string& Unit)
  /*!
    Splits a block out of a line based on quotes 
    \param Line :: Line to process 
    \param Unit :: Unit to dump
    \retval 0 :: failure
    \retval 1 :: success
  */
{
  Unit="";
  unsigned int index;
  int quote(0);
  for(index=0;index<Line.length() && isspace(Line[index]);index++) ;
  if (index==Line.length())
    return 0;
  for(;index<Line.length() && (quote || !isspace(Line[index]));index++)
    {
      if ((Line[index]=='\'' || Line[index]=='\"') &&
	  (index==0 || Line[index]!='\\'))
        {
	  if (quote)
	    {
	      Line.erase(0,index+1);
	      return 1;
	    }
	  quote=1;
	}
      else
	Unit+=Line[index];
    }
  Line.erase(0,index+1);
  return 1;
}

template<typename T>
int
itemize(std::string& A,std::string& Unit,T& Item) 
  /*!
    Splits a string into tokens and units depending
    on ability to parse type.
    \param A :: string to split
    \param Unit :: String item 
    \param Item :: Type item
    \retval 0 - Nothing to process
    \retval 1 - Item processes 					       
    \retval -1 - Units processed
   */
{
  if (!section(A,Unit))  return 0;
  return convert(Unit,Item) ? 1 : -1; 
}

std::string
singleLine(const std::string& A)
  /*!
    Removes all \n / double spaces and forward/back space
    \param A :: string to process
    \returns shortened string
  */
{
  std::string Out(A);
  std::replace(Out.begin(),Out.end(),'\n',' ');
  Out=StrFunc::stripMultSpc(Out);
  Out=StrFunc::fullBlock(Out);
  return Out;
}

std::string
frontBlock(const std::string& A)
  /*!
    Returns the string from the first non-space to the 
    last non-space 
    \param A :: string to process
    \returns shortened string
  */
{
  if (A.empty()) return "";

  std::string::size_type posA;
  
  for(posA=0;posA<A.size() && 
	isspace(A[posA]);posA++) ;

  if (posA==A.size()) return "";
  
  return A.substr(posA);
}

std::string
fullBlock(const std::string& A)
  /*!
    Returns the string from the first non-space to the 
    last non-space 
    \param A :: string to process
    \returns shortened string
  */
{
  if (A.empty()) return "";

  std::string::size_type posA,posB;
  
  for(posA=0;posA<A.size() && 
	isspace(A[posA]);posA++) ;

  for(posB=A.size();posB>0 &&
	isspace(A[posB-1]);posB--) ;
  if (posA==A.size()) return "";
  
  return A.substr(posA,posB-posA);
}

int
sectionBracket(std::string& A,std::string& out)
  /*!
    Given a string of type (.(.).)
    extract the full bracket section and remove from the string
    \param A :: string to intpu
    \param out :: string for output
    \return 1 if bracket closes correctly
   */
{
  std::string NStr(A);
  std::string::size_type pos(0);
  pos=NStr.find_first_of("(",pos);
  if(pos!=std::string::npos)
    {
      int bCnt(1);
      std::string::size_type posIndex(pos+1);
      for(;bCnt && posIndex<NStr.size();posIndex++)
        {
          if (NStr[posIndex]=='(') bCnt++;
          if (NStr[posIndex]==')') bCnt--;
        }
      if (!bCnt)
        {
          out=NStr.substr(pos+1,posIndex-(pos+2));
          A.erase(pos,posIndex-pos);
        }
      return 1;
    }
  return 0;
}
  
template<typename T>
int
sectPartNum(std::string& A,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. The string is then 
    erase up to the end of number.
    The diffierence between this and section is that
    it allows trailing characters after the number. 
    \param out :: place for output
    \param A :: string to process
    \returns 1 on success 0 on failure
   */ 
{
  if (A.empty())
    return 0;

  std::istringstream cx(A);
  T retval;
  if ((cx>>retval).fail())
    return 0;

  const std::ptrdiff_t xpt=cx.tellg();
  const int xc=cx.get();
  if (cx.fail())
    A.erase(0,static_cast<size_t>(xpt));
  else if (isspace(xc) || xc==',')
    A.erase(0,static_cast<size_t>(xpt+1));
  else  
    A.erase(0,static_cast<size_t>(xpt));
  out=retval;
  return 1; 
}


template<typename T>
int 
sectionRange(std::string& A,std::vector<T>& out)
  /*!
    Take a string of parts 
      -- A,B,C  .. list
      -- A:B:C  a stepped item list [C optional]
    the component between A:B
    \param A :: String to process
    \param out :: place for output
    \returns 1 on success 0 on failure
   */ 
{
  if (A.empty()) return 0;

  std::vector<std::string> Comp;
  std::string::size_type pos=A.find(',');
  while(pos!=std::string::npos)
    {
      Comp.push_back(A.substr(0,pos));
      A.erase(0,pos+1);
      pos=A.find(',');
    }

  if (!isEmpty(A))
    Comp.push_back(A);

  for(std::string& Unit : Comp)
    {
      T Val[3];
      pos=0;
      size_t i;
      for(i=0;pos!=std::string::npos && i<3;i++)
	{
	  pos=Unit.find(':');
	  if (!StrFunc::convert(Unit.substr(0,pos),Val[i]))
	    {
	      out.clear();
	      return 0;
	    }
	  Unit.erase(0,pos+1);
	}
      if (i==1)
	out.push_back(Val[0]);
      else
	{
	  do
	    {
	      out.push_back(Val[0]);
	      if (i==3)
		Val[0]+=Val[2];
	      else
		Val[0]++;
	    }
	  while(Val[0]<=Val[1]);
	}
    }
  return 1;
}

  
template<typename T>
int 
section(char* cA,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. The string is then filled with
    spaces upto the end of the [typename T] object
    \param cA :: char array for input and output. 
    \param out :: place for output
    \returns 1 on success 0 on failure
   */ 
{
  if (!cA) return 0;
  std::string sA(cA);
  const int item(section(sA,out));
  if (item)
    {
      strcpy(cA,sA.c_str());
      return 1;
    }
  return 0;
}

template<typename T>
int
section(std::string& A,T& out)
  /*! 
    Takes a character string and evaluates 
    the first \<T\> object. The string is then filled with
    spaces upto the end of the \<T\> object
    \param A :: string for input and output. 
    \param out :: place for output
    \return 1 on success 0 on failure
  */
{
  if (A.empty()) return 0;
  std::istringstream cx(A);
  T retval;
  if ((cx>>retval).fail())
    return 0;

  const std::ptrdiff_t xpt=cx.tellg();
  const int xc=cx.get();
  if (cx.fail())
    A.erase(0,static_cast<size_t>(xpt));
  else if (isspace(xc) || xc==',')
    A.erase(0,static_cast<size_t>(xpt+1));
  else
    return 0;

  out=retval;
  return 1;
}


template<typename T>
int
fortRead(std::string& A,const size_t N,T& out)
  /*!
    Component object in a fortran read
    The string is read upto N characters and
    cut.
    spaces upto the end of the [T] object.
    \param A :: string for input and output. 
    \param N :: number of characters in string
    \param out :: place for output
    \return 1 on success 0 on failure
  */
{
  std::string Part=A.substr(0,N);
  if (section(Part,out))
    {
      A.erase(0,static_cast<size_t>(N));
      return 1;
    }
  return 0;
}

template<typename T>
int
sectionMCNPX(std::string& A,T& out)
/*!
  Takes a character string and evaluates 
  the first [T] object. The string is then filled with
  spaces upto the end of the [T] object.
  This version deals with MCNPX numbers. Those
  are numbers that are crushed together like
  - 5.4938e+04-3.32923e-6
  \param out :: place for output
  \param A :: string for input and output. 
  \return 1 on success 0 on failure
*/
{
  if (A.empty()) return 0;
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (!cx.fail())
    {
      //      const int xpt=static_cast<int>(cx.tellg());
      const long int xpt=cx.tellg();
      const int xc=cx.get();
      if (!cx.fail() && !isspace(xc) 
	  && (xc!='-' || xpt<5))
	return 0;
      A.erase(0,static_cast<size_t>(xpt));
      out=retval;
      return 1;
    }
  return 0;
}

template<>
int
sectionCINDER(std::string& A,double& out)
/*!
  Takes a character string and evaluates 
  the first [T] object. The string is then filled with
  spaces upto the end of the [T] object.
  This version deals with MCNPX numbers. Those
  are numbers that are crushed together like
  - 5.4938-004
  \param out :: place for output
  \param A :: string for input and output. 
  \return 1 on success 0 on failure
*/
{
  if (A.empty()) return 0;
  std::istringstream cx;
  double retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (!cx.fail())
    {
      //      const int xpt=static_cast<int>(cx.tellg());
      const long int xpt=cx.tellg();
      const int xc=cx.get();
      if (cx.fail())
	{
	  A.erase(0,static_cast<size_t>(xpt));
	  out=retval;
	  return 1;
	}
      else if (isspace(xc) || xc==',')
	{
	  A.erase(0,static_cast<size_t>(xpt+1));
	  out=retval;
	  return 1;
	}
      else if (xc=='-' && xpt>3)
	{
	  const std::string held=A;
	  double revA,revB;
	  if (sectPartNum(A,revA) && sectPartNum(A,revB))
	    {
	      out=0.0;
	      return 1;
	    }
	  A=held;
	}
    }

  
  return 0;
}

void
writeFLUKA(const std::string& Line,std::ostream& OX)
  /*!
    Write out the line in the fixed FLUKA format WHAT(1-6).
    Replace " - " by space to write empty WHAT cards.
    \param Line :: full MCNPX line
    \param OX :: ostream to write to
  */
{
  // this is expensive
  std::istringstream iss(Line);
  std::vector<std::string> whats(std::istream_iterator<std::string>{iss},
				 std::istream_iterator<std::string>());

  size_t i(1);
  for (std::string& w : whats)
    {
      if (w=="-") w=" ";

      if (i % 8==0)
	OX<<std::setw(10)<<std::left<<w<<std::endl;
      else if (i % 8==1)
	OX<<std::setw(10)<<std::left<<w;	
      else 
	OX<<std::setw(10)<<std::right<<w;	
      i++;
    }
  
  if (i % 8 != 1) OX<<std::endl;
  return;
}


void
writeMCNPX(const std::string& Line,std::ostream& OX)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  (split on a space or comma)
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
*/
{
  writeControl(Line,OX,72,8);
  return;
}

void
writeMCNPXcont(const std::string& Line,std::ostream& OX)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  as if it is a split line
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
*/
{
  writeControl(Line,OX,72,-8);
  return;
}

void
writeControl(const std::string& Line,std::ostream& OX,
	     const size_t LNmax,int insertDepth)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0::72 after that 8 to 72
  (split on a space or comma)
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
  \param LNmax :: Maximium char count in a line
  \param insertDepth :: second line insert depth [-ve to include first line]
*/
{
  // Line
  size_t spcLen(0);
  if (insertDepth<0)
    {
      insertDepth *= -1;
      spcLen=static_cast<size_t>(insertDepth);
    }

  std::string::size_type pos(0);
  std::string X=Line.substr(0,LNmax-spcLen);    
  std::string::size_type posB=X.find_last_of(" ,");
  while(X.length() == LNmax-spcLen &&
	posB!=std::string::npos)
    {
      pos+=posB+1;
      if (!isspace(X[posB])) posB++;  // skip pass comma 
      X=fullBlock(X.substr(0,posB));
      if (!isEmpty(X))
	OX<<std::string(spcLen,' ')<<X<<std::endl;

      spcLen=static_cast<size_t>(insertDepth);
      X=Line.substr(pos,LNmax-spcLen);
      posB=X.find_last_of(" ,");
    }
    
  X=fullBlock(X);
  if (!isEmpty(X))
    OX<<std::string(spcLen,' ')<<X<<std::endl;
  return;
}

void
writeMCNPXcomment(const std::string& Line,std::ostream& OX)
/*!
  Write out the line in the limited form for MCNPX
  ie initial line from 0->72 after that 8 to 72
  (split on a space or comma). It puts int all in a comment
  so lines are cut by 2
  \param Line :: full MCNPX line
  \param OX :: ostream to write to
*/
{
  const size_t MaxLine(72);

  std::string::size_type pos(0);
  std::string X=Line.substr(0,MaxLine);
  std::string::size_type posB=X.find_last_of(" ,");
  size_t spc(0);
  while (posB!=std::string::npos && 
	 X.length()>=MaxLine-spc)
    {
      pos+=posB+1;
      if (!isspace(X[posB]))
	posB++;
      const std::string Out=X.substr(0,posB);
      if (!isEmpty(Out))
        {
	  OX<<"c ";
	  if (spc)
	    OX<<std::string(spc,' ');
	  OX<<X.substr(0,posB)<<std::endl;
	}
      spc=8;
      X=Line.substr(pos,MaxLine-spc);
      posB=X.find_last_of(" ,");
    }
  if (!isEmpty(X))
    {
      OX<<"c ";
      if (spc)
	OX<<std::string(spc,' ');
      OX<<X<<std::endl;
    }
  return;
}

std::vector<std::string>
StrParts(std::string Ln)
/*!
  Splits the sting into parts that are space delminated.
  \param Ln :: line component to strip
  \returns vector of components
*/
{
  std::vector<std::string> Out;
  std::string Part;
  while(section(Ln,Part))
    Out.push_back(Part);
  return Out;
}

template<typename T>
size_t
convPartNum(const std::string& A,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. 
    The diffierence between this and convert is that
    it allows trailing characters after the number. 
    \param A :: string to process
    \param out :: place for output
    \retval number of char read on success
    \retval 0 on failure
  */ 
{
  if (A.empty()) return 0;
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (cx.fail())
    return 0;
  // -ve on error
  const long int xpt=cx.tellg();
  out=retval;
  return (static_cast<size_t>(xpt)>A.size()) ? 
    A.size() : static_cast<size_t>(xpt); 
}


template<typename T>
int
convert(const std::string& A,T& out)
/*!
  Convert a string into a value 
  \param A :: string to pass
  \param out :: value if found
  \returns 0 on failure 1 on success
*/
{
  if (A.empty()) return 0;
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (cx.fail())  
    return 0;
  const int clast=cx.get();
  if (!cx.fail() && !isspace(clast))
    return 0;
  out=retval;
  return 1;
}

template<>
int
convert(const std::string& A,size_t& out)
/*!
  Convert a string into a value 
  \param A :: string to pass
  \param out :: value if found [size_t]
  \returns 0 on failure 1 on success
*/
{
  if (A.empty()) return 0;
  std::string Item;
  if (!convert(A,Item) || !isdigit(Item[0]))
    return 0;

  std::istringstream cx;
  size_t retval;
  cx.str(Item);
  cx.clear();
  cx>>retval;
  if (cx.fail())  
    return 0;
  const int clast=cx.get();
  if (!cx.fail() && !isspace(clast))
    return 0;
  // Note negative numbers are rejected:
  out=retval;
  return 1;
}

template<>
int
convert(const std::string& A,unsigned int& out)
/*!
  Convert a string into a value 
  \param A :: string to pass
  \param out :: value if found [size_t]
  \returns 0 on failure 1 on success
*/
{
  if (A.empty()) return 0;
  std::string Item;
  if (!convert(A,Item) || !isdigit(Item[0]))
    return 0;

  std::istringstream cx;
  unsigned int retval;
  cx.str(Item);
  cx.clear();
  cx>>retval;
  if (cx.fail())  
    return 0;
  const int clast=cx.get();
  if (!cx.fail() && !isspace(clast))
    return 0;
  // Note negative numbers are rejected:
  out=retval;
  return 1;
}

template<typename T>
int
convert(const char* A,T& out)
/*!
  Convert a string into a value 
  \param A :: string to pass
  \param out :: value if found
  \returns 0 on failure 1 on success
*/
{
  // No string, no conversion
  if (!A) return 0;
  std::string Cx=A;
  return convert(Cx,out);
}

template<typename T> 
int
setValues(const std::string& Line,const std::vector<int>& Index,
	  std::vector<T>& Out)
/*!  
  Call to read in various values in position x1,x2,x3 from the
  line.
  \param Line :: string to read
  \param Index :: Indexes to read
  \param Out :: OutValues [unchanged if not read]
  \retval 0 :: success
  \retval -ve on failure.
*/
{
  if (Index.empty())
    return 0;
  
  if(Out.size()!=Index.size())
    throw ColErr::MisMatch<size_t>(Index.size(),Out.size(),
				   "StrFunc::setValues");

  std::string modLine=Line;
  std::vector<int> sIndex(Index);     // Copy for sorting
  std::vector<size_t> OPt(Index.size());
  for(size_t i=0;i<Index.size();i++)
    OPt[i]=i;
  mathFunc::crossSort(sIndex,OPt);
  
  typedef std::vector<int>::const_iterator iVecIter;
  std::vector<int>::const_iterator sc=sIndex.begin();
  std::vector<size_t>::const_iterator oc=OPt.begin();
  int cnt(0);
  T value;
  std::string dump;
  while(sc!=sIndex.end() && *sc<0)
    {
      sc++;
      oc++;
    }
  
  while(sc!=sIndex.end())
    {
      if (*sc==cnt)
        {
	  if (!section(modLine,value))
	    return -1-static_cast<int>(
	      distance(static_cast<iVecIter>(sIndex.begin()),sc));  
	  // this loop handles repeat units
	  do
	    {
	      Out[static_cast<size_t>(*oc)]=value;
	      sc++;
	      oc++;
	    } while (sc!=sIndex.end() && *sc==cnt); 
	}
      else
        {
	  if (!section(modLine,dump))
	    return -1-static_cast<int>(
	      distance(static_cast<iVecIter>(sIndex.begin()),sc));  
	}
      cnt++;         // Add only to cnt [sc/oc in while loop]
    }
  // Success since loop only gets here if sc is exhaused.
  return 0;        
}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
int
sliceVector(V<T,Alloc>& A,const T& indexA,const T& indexB)
  /*!
    Given values indexA , indexB : 
    Find the values in the list and
    return just that bit. 
    \param A :: vect/list to slice
    \param indexA :: Component to find
    \param indexB :: Component to find
    \return -ve on error
    \retval +ve :: length of sliced section 
   */
{
  typename V<T,Alloc>::iterator ac=find(A.begin(),A.end(),indexA);
  typename V<T,Alloc>::iterator bc=find(A.begin(),A.end(),indexB);
  if (ac==A.end() || bc==A.end())
    return (ac==A.end()) ? -1 : -2;

  if (distance(ac,bc)>0)       // Normal direction
    {
      A.erase(bc+1,A.end());
      A.erase(A.begin(),ac);
    }
  else                           // reversed direction
    {
      A.erase(ac+1,A.end());
      A.erase(A.begin(),bc);
    }
  return 0;
}

std::vector<std::string>
splitParts(const std::string& Line,const char delim)
  /*!
    Split a string based on a a delimiter and avoiding quotes
    \param Line :: Line to split 
    \param delim :: deliminator
  */
{
  std::vector<std::string> Out;
  int hardQuote(0);
  int softQuote(0);
  std::string Unit;

  for(size_t index=0;index<Line.length();index++)
    {
      if (Line[index]=='\'' && index &&
	  Line[index]!='\\')
        {
	  hardQuote=1-hardQuote;
	}
      else if (!hardQuote && Line[index]=='\"' &&
	       index && Line[index]!='\\')
        {
	  softQuote=1-softQuote;
	}

      if ((!softQuote || !hardQuote) && Line[index]==delim)
	{
	  if (!Unit.empty())
	    {
	      Out.push_back(Unit);
	      Unit="";
	    }
	}
      else if (hardQuote || softQuote || !isspace(Line[index]))
	Unit+=Line[index];
    }

  if (!Unit.empty())
    Out.push_back(Unit);
	      
  return Out;

}

template<typename T>
void
writeLine(std::ostream& OX,const T& V,
	  size_t& itemCnt,const size_t lineCut)
  /*!
    Write the line in the WWG format of 13.6g
    \param OX :: Output stream
    \param V :: Value
    \param itemCnt :: Place in line
    \param lineCut :: Number of unit to put in line before CR
   */
{
  static boost::format DblFMT("%13.4f");
  static boost::format SciFMT("%13.4e");

  const double VUnit=static_cast<double>(V);

  const double AVal(std::fabs(VUnit));
  if (AVal>9.9e4 || (AVal<1e-2 && AVal>1e-38))
    OX<<(SciFMT % VUnit);
  else
    OX<<(DblFMT % VUnit);
  
  itemCnt++;
  if (itemCnt==lineCut)
    {
      OX<<std::endl;
      itemCnt=0;
    }
  return;
}

template<>
void
writeLine(std::ostream& OX,const Geometry::Vec3D& Vec,
	  size_t& itemCnt,const size_t lineCut)
  /*!
    Write the line in the WWG format of 13.6g
    \param OX :: Output stream
    \param V :: Value
    \param itemCnt :: Place in line
    \param lineCut :: Number of unit to put in line before CR
   */
{
  for(size_t i=0;i<3;i++)
    writeLine(OX,Vec[i],itemCnt,lineCut);
  return;
}
  


  
/// \cond TEMPLATE 

template void writeLine(std::ostream&,const double&,
			size_t&,const size_t);
template void writeLine(std::ostream&,const int&,
			size_t&,const size_t);
template void writeLine(std::ostream&,const long int&,
			size_t&,const size_t);
template void writeLine(std::ostream&,const size_t&,
			size_t&,const size_t);
  
template int itemize(std::string&,std::string&,double&);
template int itemize(std::string&,std::string&,int&);

  
template int sectionRange(std::string&,std::vector<double>&);
template int sectionRange(std::string&,std::vector<int>&);
template int sectionRange(std::string&,std::vector<size_t>&);
  
template int section(std::string&,double&);
template int section(std::string&,Geometry::Vec3D&);
template int section(std::string&,float&);
template int section(std::string&,int&);
template int section(std::string&,long int&);
template int section(std::string&,size_t&);
template int section(std::string&,unsigned int&);
template int section(std::string&,std::string&);
template int section(std::string&,DError::doubleErr&);

template int sectPartNum(std::string&,double&);
template int sectPartNum(std::string&,int&);
template int sectPartNum(std::string&,long int&);
template int sectPartNum(std::string&,size_t&);
template int sectionMCNPX(std::string&,double&);

template int fortRead(std::string&,const size_t,int&);

template int convert(const std::string&,double&);
template int convert(const std::string&,float&);
template int convert(const std::string&,std::string&);
template int convert(const std::string&,int&);
template int convert(const std::string&,long int&);
template int convert(const std::string&,Geometry::Vec3D&);
template int convert(const std::string&,Geometry::Quaternion&);
template int convert(const std::string&,DError::doubleErr&);

template size_t convPartNum(const std::string&,double&);
template size_t convPartNum(const std::string&,int&);
template size_t convPartNum(const std::string&,size_t&);
template size_t convPartNum(const std::string&,std::string&);

template int setValues(const std::string&,const std::vector<int>&,
		      std::vector<double>&);

template int sliceVector(std::vector<int>&,const int&,const int&);

/// \endcond TEMPLATE 

}  // NAMESPACE StrFunc
