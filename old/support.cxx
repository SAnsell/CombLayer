/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/support.cxx
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
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"

namespace  StrFunc
{

int
processMainInput(const int AC,char** AV,
		 std::vector<int>& Value,
		 std::vector<std::string>& OptionVec)
  /*!
    This function processes the main input and moves AC ::
    \param AC :: Number of parameters (argc)
    \param AV :: argv list 
    \param Value :: integets of the main argument
    \return Numnber of options
  */
{
  // Note need to skip name of the program
  int cnt(0);
  for(int i=1;i<AC;i++)
    {
      if (AV[i][0]=='-')
        {
	  OptionVec.push_back(std::string(AV[i]+1));
	  cnt++;
	}
      else
	Value.push_back(i);
    }
  return cnt;
}

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
  int lastReal(-1);
  for(unsigned int i=0;i<Line.length();i++)
    {
      if (!isblank(Line[i]))
        {
	  lastReal=i;
	  spc=0;
	  Out+=Line[i];
	}
      else if (!spc)
        {
	  spc=1;
	  Out+=' ';
	}
    }
  lastReal++;
  if (lastReal<static_cast<int>(Out.length()))
    Out.erase(lastReal);
  return Out;
}

void
lowerString(std::string& LN) 
  /*!
    lowers the words in the string
    \param LN :: String to change
  */
{
  for(unsigned int i=0;i<LN.length();i++)
    LN[i]=tolower(LN[i]);
  return;
}

int
extractWord(std::string& Line,const std::string& Word,const int cnt)
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

  unsigned int minSize(cnt>static_cast<int>(Word.size()) ?  Word.size() : cnt);
  std::string::size_type pos=Line.find(Word.substr(0,minSize));
  if (pos==std::string::npos)
    return 0;
  // Pos == Start of find
  unsigned int LinePt=minSize+pos;
  for(;minSize<Word.size() && LinePt<Line.size()
	&& Word[minSize]==Line[LinePt];LinePt++,minSize++);

  Line.erase(pos,LinePt-(pos-1));
  return 1;
}


int
confirmStr(const std::string& S,const char* fullPhrase)
  /*!
    Check to see if S is the same as the
    first part of a phrase. (case insensitive)
    \param S :: string to check
    \param fullPhrase :: complete phrase.
    \returns 1 on success 
  */
{
  const int nS(S.length());
  int nC(strlen(fullPhrase));
  if (nS>nC)
    return 0;           
  for(int i=0;i<nS;i++)
    if (toupper(S[i])!=toupper(fullPhrase[i]))
      return 0;
  return 1;
}

int
getPartLine(std::istream& fh,std::string& Out,std::string& Excess,const int spc)
  /*!
    Gets a line and determine if there is addition component to add
    in the case of a very long line.
    \param fh :: input stream to get line 
    \param Out :: string up to last 'tab' or ' '
    \param Excess :: string after 'tab or ' ' 
    \param spc :: number of char to try to read 
    \retval 1 :: more line to be found
    \retval -1 :: Error with file
    \retval 0  :: line finished.
  */
{
  std::string Line;
  if (fh.good())
    {
      char* ss=new char[spc+1];
      const int clen=spc-Out.length();
      fh.getline(ss,clen,'\n');
      ss[clen+1]=0;           // incase line failed to read completely
      Out+=static_cast<std::string>(ss);
      delete [] ss;                   
      // remove trailing comments
      std::string::size_type pos = Out.find_first_of("#!");        
      if (pos!=std::string::npos)
        {
	  Out.erase(pos); 
	  return 0;
	}
      if (fh.gcount()==clen-1)         // cont line
        {
	  pos=Out.find_last_of("\t ");
	  if (pos!=std::string::npos)
	    {
	      Excess=Out.substr(pos,std::string::npos);
	      Out.erase(pos);
	    }
	  else
	    Excess.erase(0,std::string::npos);
	  fh.clear();
	  return 1;
	}
      return 0;
    }
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
getLine(std::istream& fh,const int spc)
  /*!
    Reads a line from the stream of max length spc.
    Trailing comments are removed. (with # or ! character)
    \param fh :: already open file handle
    \param spc :: max number of characters to read 
    \return String read.
  */
{
  char* ss=new char[spc+1];
  std::string Line;
  if (fh.good())
    {
      fh.getline(ss,spc,'\n');
      ss[spc]=0;           // incase line failed to read completely
      Line=ss;
      // remove trailing comments
    }
  delete [] ss;
  return Line;
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

void
stripComment(std::string& A)
  /*!
    Removes the string after the comment type of 
    '$ ' or '!' or '#  '
    \param A :: String to process
  */
{
  std::string::size_type posA=A.find("$ ");
  std::string::size_type posB=A.find("# ");
  std::string::size_type posC=A.find("!");
  if (posA>posB)
    posA=posB;
  if (posA>posC)
    posA=posC;
  if (posA!=std::string::npos)
    A.erase(posA,std::string::npos);
  return;
}

std::string
fullBlock(const std::string& A)
  /*!
    Returns the string from the first non-space to the 
    last non-space 
    \param A :: string to process
    \returns Shortened string
  */
{
  std::string::size_type posA=A.find_first_not_of(" ");
  std::string::size_type posB=A.find_last_not_of(" ");
  if (posA==std::string::npos)
    return "";
  return A.substr(posA,1+posB-posA);
}

template<typename T>
int
sectPartNum(std::string& A,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. The string is then 
    erase upt to the end of number.
    The diffierence between this and section is that
    it allows trailing characters after the number. 
    \param out :: place for output
    \param A :: string to process
    \returns 1 on success 0 on failure
   */ 
{
  if (A.empty())
    return 0;

  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  const int xpt=cx.tellg();
  if (xpt<0)
    return 0;
  A.erase(0,xpt);
  out=retval;
  return 1; 
}

template<typename T>
int 
section(char* cA,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. The string is then filled with
    spaces upto the end of the [typename T] object
    \param out :: place for output
    \param cA :: char array for input and output. 
    \returns 1 on success 0 on failure
   */ 
{
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
  /* 
    takes a character string and evaluates 
    the first [T] object. The string is then filled with
    spaces upto the end of the [T] object
    \param out :: place for output
    \param A :: string for input and output. 
    \return 1 on success 0 on failure
  */
{
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (cx.fail())
    return 0;
  const int xpt=cx.tellg();
  const char xc=cx.get();
  if (!cx.fail() && !isspace(xc) && xc!=',')
    return 0;
  A.erase(0,xpt);
  out=retval;
  return 1;
}

template<typename T>
int
sectionMCNPX(std::string& A,T& out)
  /* 
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
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (!cx.fail())
    {
      int xpt=cx.tellg();
      const char xc=cx.get();
      if (!cx.fail() && !isspace(xc)
	  && (xc!='-' || xpt<5))
	return 0;
      A.erase(0,xpt);
      out=retval;
      return 1;
    }
  return 0;
}

void
writeMCNPX(const std::string& Line,std::ostream& OX)
  /*!
    Write out the line in the limited form for MCNPX
    ie initial line from 0->72 after that 8 to 72
    (split on a space or comma)
    \param Line :: full MCNPX line
    \param OX :: ostream to write to
  */
{
  const int MaxLine(72);
  std::string::size_type pos(0);
  std::string X=Line.substr(0,MaxLine);
  std::string::size_type posB=X.find_last_of(" ,");
  int spc(0);
  while (posB!=std::string::npos && 
	 static_cast<int>(X.length())>=MaxLine-spc)
    {
      pos+=posB+1;
      if (!isspace(X[posB]))
	posB++;
      const std::string Out=X.substr(0,posB);
      if (!isEmpty(Out))
        {
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
      if (spc)
	OX<<std::string(spc,' ');
      OX<<X<<std::endl;
    }
  return;
}

void
writeMCNPXComment(const std::string& Line,std::ostream& OX)
  /*!
    Write out the line in the limited form for MCNPX
    ie initial line from 0->72 after that 8 to 72
    (split on a space or comma). It puts int all in a comment
    so lines are cut by 2
    \param Line :: full MCNPX line
    \param OX :: ostream to write to
  */
{
  const int MaxLine(72);
  std::string::size_type pos(0);
  std::string X=Line.substr(0,MaxLine);
  std::string::size_type posB=X.find_last_of(" ,");
  int spc(0);
  while (posB!=std::string::npos && 
	 static_cast<int>(X.length())>=MaxLine-spc)
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
int
convPartNum(const std::string& A,T& out)
  /*!
    Takes a character string and evaluates 
    the first [typename T] object. The string is then 
    erase upt to the end of number.
    The diffierence between this and section is that
    it allows trailing characters after the number. 
    \param out :: place for output
    \param A :: string to process
    \retval number of char read on success
    \retval 0 on failure
   */ 
{
  if (A.empty())
    return 0;

  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  const int xpt=cx.tellg();
  if (xpt<0)
    return 0;
  out=retval;
  return xpt; 
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
  std::istringstream cx;
  T retval;
  cx.str(A);
  cx.clear();
  cx>>retval;
  if (cx.fail())  
    return 0;
  const char clast=cx.get();
  if (!cx.fail() && !isspace(clast))
    return 0;
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
  if (!A)         // No string, no conversion
    return 0;
  std::string Cx=A;
  return convert(Cx,out);
}


template<typename T>
int
writeFile(const std::string& Fname,const T step,
	  const std::vector<T>& Y)
  /*!
    Write out the three vectors into a file of type dc 9
    \param step :: parameter to control x-step (starts from zero)
    \param Y :: Y column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  std::vector<T> Ex;   // Empty vector
  std::vector<T> X(Y.size());    // Empty vector
  for(unsigned int i=0;i<Y.size();i++)
    X[i]=step*i;
  return writeFile<T>(Fname,X,Y,Ex);
}

template<typename T>
int
writeStream(const std::string& Fname,const std::string& Title,
	    const std::vector<T>& X,const std::vector<T>& Y,
	    const std::vector<T>& Err,const int append)
  /*!
    Write out the three vectors into a file of type dc 2
    \param Title :: second line title
    \param X :: X column
    \param Y :: Y column
    \param Err :: Err column (Optionally Empty)
    \param Fname :: Name of the file
    \param append :: True if appending to a file is needed
    \returns 0 on success and -ve on failure
  */
{
  const int Npts(X.size()>Y.size() ? Y.size() : X.size());
  const int Epts(Npts>static_cast<int>(Err.size()) ? Err.size() : Npts);

  std::ofstream FX;
  if (append)
    FX.open(Fname.c_str(),std::ios::app);
  else
    FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"3 "<<Npts<<" "<<Epts<<std::endl;
  FX<<"# "<<Title<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  
  int i;
  for(i=0;i<Epts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<Err[i]<<std::endl;
  for(;i<Npts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<0.0<<std::endl;
  return 0;
}

template<typename T>
int
writeStream(const std::string& Fname,const std::string& Title,
	    const std::vector<T>& X,const std::vector<T>& Y,const int append)
  /*!
    Write out the three vectors into a file of type dc 2
    \param Fname :: Name of the file
    \param Title :: second line in the dc 2 type file
    \param X :: X column
    \param Y :: Y column
    \param append :: True if appending to a file is needed
    \returns 0 on success and -ve on failure
  */
{
  return writeStream(Fname,Title,X,Y,std::vector<T>(),append);
}


template<typename T>
int
writeFile(const std::string& Fname,const std::vector<T>& X,
	  const std::vector<T>& Y)
  /*!
    Write out the three vectors into a file of type dc 9
    \param Fname :: Name of the file
    \param X :: X column
    \param Y :: Y column
    \returns 0 on success and -ve on failure
  */
{
  std::vector<T> Ex;   // Empty vector
  return writeFile<T>(Fname,X,Y,Ex);
}

template<typename T>
int
writeFile(const std::string& Fname,const std::vector<T>& X,
	  const std::vector<T>& Y,const std::vector<T>& Err)
  /*!
    Write out the three vectors into a file of type dc 9
    \param X :: X column
    \param Y :: Y column
    \param Err :: Err column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  const int Npts(X.size()>Y.size() ? Y.size() : X.size());
  const int Epts(Npts>static_cast<int>(Err.size()) ? Err.size() : Npts);

  std::ofstream FX;

  FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"# "<<Npts<<" "<<Epts<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);

  int i;
  for(i=0;i<Epts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<Err[i]<<std::endl;
  for(;i<Npts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<0.0<<std::endl;
  return 0;
}

float
getVAXnum(const float A) 
  /*!
    Converts a vax number into a standard unix number
    \param A :: float number as read from a VAX file
    \returns float A in IEEE little eindian format
  */
{
  union 
   {
     char a[4];
     float f;
     int ival;
   } Bd;

  int sign,expt,fmask;
  float frac;
  double onum;

  Bd.f=A;
  sign  = (Bd.ival & 0x8000) ? -1 : 1;
  expt = ((Bd.ival & 0x7f80) >> 7);   //reveresed ? 
  if (!expt) 
    return 0.0;

  fmask = ((Bd.ival & 0x7f) << 16) | ((Bd.ival & 0xffff0000) >> 16);
  expt-=128;
  fmask |=  0x800000;
  frac = (float) fmask  / 0x1000000;
  onum= frac * sign * 
         pow(2.0,expt);
  return (float) onum;
}


template<class T> int 
setValues(const std::string& Line,
	  const int x1,T& A,const int x2,
	  T& B,const int x3,T& C)
  /*!  
    Call to read in various values in position x1,x2,x3 from the
    line.
    \param Line :: string to read
    \param x1 :: index [1-N] to section from the line
    \param x2 :: index [1-N] to section from the line
    \param x3 :: index [1-N] to section from the line

    \param A :: Value to get (if x1>0)
    \param B :: Value to get (if x2>0)
    \param C :: Value to get (if x3>0)
    
    \retval Number of items successfully read
    \retval 0 on failure.
  */
{
  // first check we have a character system
  if (Line.empty()) 
    return 0; 
  const int len=Line.size();
  // Strip off initial spaces
  int i;
  for(i=0;i<len && 
	(Line[i]==' '  || Line[i]==',' || Line[i]=='\t');i++);
  if (i==len) 
    return 0;
  // Count number of items to read:
  int maxT(0);
  if (x1>0)
    maxT++;
  if (x2>0)
    maxT++;
  if (x3>0)
    maxT++;
  // Nothing to read so go home
  if (!maxT) 
    return 0;
  
  char* ss=new char[len]; 
  int count=1;
  int total=0;
  T nmb;
  do {
    int j=0;
    for(;i<len && Line[i]!='\n'
	  && Line[i]!='\t' && Line[i]!=' ' && Line[i]!=',';i++)
      {
	ss[j]=Line[i];
	j++;
      }
    ss[j]=0;
    if (x1==count || x2==count || x3==count)
      {
	if (!convert(ss,nmb))
	  {
	    delete [] ss;
	    return total;
	  }
	if (x1==count)
	  A=nmb;
	else if (x2==count)
	  B=nmb;
	else 
	  C=nmb;
	total++;
      }
    count++;
    for(;i<len &&  
	  (Line[i]==' ' || Line[i]==',' || Line[i]=='\t');i++);
    } while (total!=maxT && i<len);
  delete [] ss;
  return total;
}


/// \cond TEMPLATE 

template int section(std::string&,double&);
template int section(std::string&,int&);
template int section(std::string&,std::string&);

template int section(char*,double&);
template int section(char*,int&);
template int section(char*,std::string&);

template int sectPartNum(std::string&,double&);
template int sectPartNum(std::string&,int&);
template int sectionMCNPX(std::string&,double&);

template int convert(const std::string&,double&);
template int convert(const std::string&,std::string&);
template int convert(const std::string&,int&);
template int convert(const std::string&,long int&);
template int convert(const char*,std::string&);
template int convert(const char*,double&);
template int convert(const char*,int&);
template int convPartNum(const std::string&,double&);
template int convPartNum(const std::string&,int&);

template int 
writeStream(const std::string&,const std::string&,
	    const std::vector<double>&,const std::vector<double>&, 
	    const std::vector<double>&,const int);

template int 
writeStream(const std::string&,const std::string&,
	    const std::vector<double>&,
	    const std::vector<double>&,const int);

template int writeFile(const std::string&,const std::vector<double>&,const std::vector<double>&,
	               const std::vector<double>&);
template int writeFile(const std::string&,const std::vector<double>&,const std::vector<double>&);

/// \endcond TEMPLATE 
};
