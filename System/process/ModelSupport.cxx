/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ModelSupport.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <string>
#include <algorithm>
#include <utility>

#include "support.h"
#include "surfRegister.h"
#include "ModelSupport.h"

namespace ModelSupport
{

std::string
spcDelimString(const std::string& baseString)
  /*
    Space delimit the input string to remove : and ( without spaces
    \param baseString :: input string
    \return Striped string
  */
{
  std::ostringstream cx;
  
  int flag(0);
  char prevC('A');
  for(char c : baseString)
    {
      if (flag && c!=' ')
	cx<<' ';
      flag=0;
      if (c=='(' || c==')' || c==':')
	{
	  if (prevC!=' ')
	    cx<<' ';
	  flag=1;
	}
      // now write character
      cx<<c;
      prevC=c;

    }
  return cx.str();
}


std::string
getExclude(const int Offset)
  /*!
    Given a cell number construct the composite form
    for addition to getComposite
    \param Offset :: Cell value
    \return Cell exclude string
  */
{
  std::ostringstream cx;
  cx<<" #"<<Offset<<"T ";
  return cx.str();
}

std::string
removeOpenPair(const std::string& procString)
  /*!
    Remove double : : / : ) etc
    \param procString :: Input string
    \return stripped string
  */
{
  // Remove all == (:
  //            == :)
  //            ==

  // pre-pass to add spaces after each
  std::stringstream cx(procString);
  std::string CStr;
  do 
    {
      CStr=cx.str();
      cx.str("");
      for(size_t i=0;i<CStr.length();i++)
	{
	  if (CStr[i]=='(' || CStr[i]==')')
	    cx<<' ';
	  cx<<CStr[i];
	  if (CStr[i]=='(' || CStr[i]==')')
	    cx<<' ';
	}

      std::string Out(cx.str());
      cx.str("");
      
      std::string prevItem;
      std::string item;
      while(StrFunc::section(Out,item))
	{
	  if (prevItem=="(" && item==":")
	    {}
	  else if (prevItem==":" && item==")")
	    prevItem=item;
	  else if (prevItem=="(" && item==")")
	    prevItem="";
	  else if (prevItem==":" && item==":")
	    {}
	  else
	    {
	      cx<<prevItem<<" ";
	      prevItem=item;
	    }
	}
      cx<<" "<<prevItem;
    }  while(CStr!=cx.str());
    
  return CStr;
}

  

std::string
getComposite(const int SOffset,const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    \param SOffset :: signed Offset number to add [-ve reverse sense]
    \param BaseString :: BaseString number
    \return String with offset components
   */ 
{
  std::ostringstream cx;
  std::string OutUnit;
  int cellN;
  std::string segment=spcDelimString(BaseString);

  const int Offset((SOffset<0) ? -SOffset : SOffset);
  const int signV((SOffset<0) ? -1 : 1); 
  cx<<" ";
  while(StrFunc::section(segment,OutUnit))
    {
      if (!OutUnit.empty())
        {
	  if (OutUnit[0]=='T')
	    {
	      OutUnit[0]=' ';
	      if (StrFunc::convert(OutUnit,cellN))
		cx<<cellN;
	      else
		cx<<OutUnit;
	    }
	  else if (StrFunc::convert(OutUnit,cellN))
	    cx<<((cellN>0) ? signV*(cellN+Offset) :
                 signV*(cellN-Offset))<<" ";
	  else
	    cx<<OutUnit<<" ";
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,const int SOffset,
	     const int SminorOffset,const int SsecondOffset,
	     const std::string& baseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param SOffset :: Offset nubmer to add
    \param SminorOffset :: minor Offset nubmer to add [M]
    \param SsecondOffset :: second Offset nubmer to add [N]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  std::ostringstream cx;
  
  int cellN;
  int TrueNum,MinorNum,SecondNum;
  std::string segment=spcDelimString(baseString);
  std::string OutUnit;

  const int Offset((SOffset<0) ? -SOffset : SOffset);
  const int signV((SOffset<0) ? -1 : 1); 
  const int minorOffset((SminorOffset<0) ? -SminorOffset : SminorOffset);
  const int minorSignV((SminorOffset<0) ? -1 : 1); 
  const int secondOffset((SsecondOffset<0) ? -SsecondOffset : SsecondOffset);
  const int secondSignV((SsecondOffset<0) ? -1 : 1); 

  
  cx<<" ";
  while(StrFunc::section(segment,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (oL)
	{
	  TrueNum=MinorNum=SecondNum=0;
	  if (OutUnit[oL-1]=='T')
	    {
	      OutUnit[oL-1]=' ';
	      TrueNum=1;
	    }
	  else if (OutUnit[oL-1]=='M')
	    {
	      OutUnit[oL-1]=' ';
	      MinorNum=1;
	    }
	  else if (OutUnit[oL-1]=='N')
	    {
	      OutUnit[oL-1]=' ';
	      SecondNum=1;
	    }
	  if (StrFunc::convert(OutUnit,cellN))
	    {
	      if (TrueNum)
		cx<<SMap.realSurf(cellN);
	      else if (MinorNum)
		cx<<((cellN>0) ?
                     minorSignV*SMap.realSurf(cellN+minorOffset) 
		     : minorSignV*SMap.realSurf(cellN-minorOffset))<<" ";
	      else if (SecondNum)
		cx<<((cellN>0) ?
                     secondSignV*SMap.realSurf(cellN+secondOffset) 
		     : secondSignV*SMap.realSurf(cellN-secondOffset))<<" ";
	      else
		cx<<((cellN>0) ?
                     signV*SMap.realSurf(cellN+Offset) 
		     : signV*SMap.realSurf(cellN-Offset))<<" ";
	    }
	  else
	    cx<<OutUnit<<" ";
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,const int SOffset,
	     const int SminorOffset,
	     const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param SOffset :: Offset nubmer to add
    \param SminorOffset :: minor Offset nubmer to add [M]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  std::ostringstream cx;
  
  int cellN;
  int TrueNum,MinorNum;
  std::string segment=spcDelimString(BaseString);
  std::string OutUnit;

  const int Offset((SOffset<0) ? -SOffset : SOffset);
  const int signV((SOffset<0) ? -1 : 1); 
  const int minorOffset((SminorOffset<0) ? -SminorOffset : SminorOffset);
  const int minorSignV((SminorOffset<0) ? -1 : 1); 

  
  cx<<" ";
  while(StrFunc::section(segment,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (oL)
	{
	  TrueNum=MinorNum=0;
	  if (OutUnit[oL-1]=='T')
	    {
	      OutUnit[oL-1]=' ';
	      TrueNum=1;
	    }
	  else if (OutUnit[oL-1]=='M')
	    {
	      OutUnit[oL-1]=' ';
	      MinorNum=1;
	    }
	  if (StrFunc::convert(OutUnit,cellN))
	    {
	      if (TrueNum)
		cx<<SMap.realSurf(cellN);
	      else if (MinorNum)
		cx<<((cellN>0) ? minorSignV*SMap.realSurf(cellN+minorOffset) 
		     : minorSignV*SMap.realSurf(cellN-minorOffset))<<" ";
	      else
		cx<<((cellN>0) ? signV*SMap.realSurf(cellN+Offset) 
		     : signV*SMap.realSurf(cellN-Offset))<<" ";
	    }
	  else
	    cx<<OutUnit<<" ";
	}
    }
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,
	     const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers.
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,Offset,BaseString);
}





  

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfN)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfN :: Index item to add
    \return String with offset components
   */
{
  std::ostringstream cx;
  cx<<" "<<((surfN>0) ? SMap.realSurf(surfN+Offset) 
       : SMap.realSurf(surfN-Offset));
  return cx.str();
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfNA,
	     const int surfNB)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfNA :: Index item to add
    \param surfNB :: Index item to add
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,surfNA)+
    getComposite(SMap,Offset,surfNB);
}

std::string
getComposite(const surfRegister& SMap,
	     const int Offset,const int surfNA,
	     const int surfNB,const int surfNC)
  /*!
    Given a base string add an offset to the numbers
    If a number is trailed byT then it is a true number.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer  to add
    \param surfNA :: Index item to add
    \param surfNB :: Index item to add
    \param surfNC :: Index item to add
    \return String with offset components
   */
{
  return getComposite(SMap,Offset,surfNA)+
    getComposite(SMap,Offset,surfNB)+
    getComposite(SMap,Offset,surfNC);
}


std::string
getSetComposite(const surfRegister& SMap,
	     const int Offset,const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc. However, if a number is missing then 
    leave its component blank.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getSetComposite(SMap,Offset,Offset,Offset,BaseString);
}

std::string
getSetComposite(const surfRegister& SMap,
		const int Offset,const int minorOffset,
		const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc. However, if a number is missing then 
    leave its component blank.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getSetComposite(SMap,Offset,minorOffset,Offset,BaseString);
}

std::string
getSetComposite(const surfRegister& SMap,const int Offset,
		const int MinorOffset,const int SecondOffset,
		const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  std::ostringstream cx;
  
  int cellN;
  int TrueNum,MinorNum,SecondNum;
  std::string segment=spcDelimString(BaseString);
  std::string OutUnit;
  cx<<" ";
  while(StrFunc::section(segment,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (oL)
	{
	  TrueNum=MinorNum=SecondNum=0;
	  if (OutUnit[oL-1]=='T')
	    {
	      OutUnit[oL-1]=' ';
	      TrueNum=1;
	    }
	  else if (OutUnit[oL-1]=='M')
	    {
	      OutUnit[oL-1]=' ';
	      MinorNum=1;
	    }
	  else if (OutUnit[oL-1]=='N')
	    {
	      OutUnit[oL-1]=' ';
	      SecondNum=1;
	    }
	  if (StrFunc::convert(OutUnit,cellN))
	    {
	      int CN(cellN);
	      if (!TrueNum)
		{ 
		  if (MinorNum)
		    CN+=(cellN>0) ? MinorOffset : -MinorOffset;
		  else if (SecondNum)
		    CN+=(cellN>0) ? SecondOffset : -SecondOffset;
		  else
		    CN+=(cellN>0) ? Offset : -Offset;
		  if (SMap.hasSurf(CN))
		    cx<<SMap.realSurf(CN)<<" ";
		}
	    }
	  else
	    cx<<OutUnit<<" ";
	}
    }
  return removeOpenPair(cx.str());
}


std::string
getSeqIntersection(int A,int B,int step)
  /*!
    Generate a sequence of numbers in the string between the 
    step size as an MCNP intersect unit
    \param A :: First number
    \param B :: Second number
    \param step :: step size
    \return string in form 1 2 3 4  etc.
   */
{
  if (A>B) std::swap(A,B);
  if (step<0) step*=-1;
  if (!step) step=1;
  
  std::ostringstream cx;
  cx<<" ";
  for(int i=A;i<=B;i+=step)
    cx<<i<<" ";
  
  return cx.str();
}

std::string
getSeqUnion(int A,int B,int step)
  /*!
    Generate a sequence of numbers in the string between the 
    step size as an MCNP union unit
    \param A :: First number
    \param B :: Second number
    \param step :: step size
    \return string in form (1:2:3:4...)
   */
{
  if (A>B) std::swap(A,B);
  if (step<0) step*=-1;
  if (!step) step=1;
  
  std::ostringstream cx;
  cx<<"(";
  for(int i=A;i<B;i+=step)
    cx<<i<<":";
  cx<<B<<")";
  return cx.str();
}

std::string
getAltComposite(const surfRegister& SMap,const int Offset,
		const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If surfaces A,B,C exist. then choose surface A
    is A exists , if B then choose B  ...
    
    If there are multiple DIFFERENT surf with appendix A, B etc
    then ALL A,B,C need to be correct for it to take presidence
    
    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getAltComposite(SMap,Offset,Offset,Offset,BaseString);
}

std::string
getAltComposite(const surfRegister& SMap,const int Offset,
		const int MinorOffset,const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If surfaces A,B,C exist. then choose surface A
    is A exists , if B then choose B  ...
    
    If there are multiple DIFFERENT surf with appendix A, B etc
    then ALL A,B,C need to be correct for it to take presidence
    
    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getAltComposite(SMap,Offset,MinorOffset,Offset,BaseString);
}

std::string
getAltComposite(const surfRegister& SMap,const int Offset,
		const int MinorOffset,const int SecondOffset,
		const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    If surfaces A,B,C exist. then choose surface A
    is A exists , if B then choose B  ...
    
    If there are multiple DIFFERENT surf with appendix A, B etc
    then ALL A,B,C need to be correct for it to take presidence
    
    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param secondOffset :: second minor Offset nubmer to add [N]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  std::ostringstream cx;
  std::string segment=spcDelimString(BaseString);
  std::string segmentB=segment;

  std::string OutUnit;
  int cellN;
  size_t index(7);         // A,B,C
  while(StrFunc::section(segment,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (oL>=2)
	{
	  const char Unit=OutUnit[oL-1];
	  if (Unit=='A' || Unit=='B' || Unit=='C')
	    {
	      OutUnit[oL-1]=' ';
	      int Plus=Offset;
	      if (OutUnit[oL-2]=='T')
		{
		  Plus=0;
		  OutUnit[oL-2]=' ';
		}
	      else if (OutUnit[oL-2]=='N')
		{
		  Plus=SecondOffset;
		  OutUnit[oL-2]=' ';
		}
	      else if (OutUnit[oL-2]=='M')
		{
		  Plus=MinorOffset;
		  OutUnit[oL-2]=' ';
		}
	      if (StrFunc::convert(OutUnit,cellN))
		{
		  const int CN((cellN>0) ? cellN+Plus : cellN-Plus);
		  if (!SMap.hasSurf(CN))
		    {
		      if (Unit=='A') index &= 3;  // 011
		      if (Unit=='B') index &= 5;  // 101
		      if (Unit=='C') index &= 6;  // 110
		    }
		}
	    }
	}
    }  
  // preserve either A,B,C,' '.
  const char preserve=" CBBAAAA"[index];
  while(StrFunc::section(segmentB,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (oL)
	{
	  const char OU=OutUnit[oL-1];
	  if (preserve==OU)
	    OutUnit[oL-1]=' ';
	  else if (OU=='A' || OU=='B' || OU=='C' )
	    OutUnit="";
	    
	  cx<<OutUnit<<" ";
	}
      else
	cx<<OutUnit<<" ";
    }
  return getSetComposite(SMap,Offset,MinorOffset,SecondOffset,cx.str());
}

std::string
getRangeComposite(const surfRegister& SMap,
 		  const int IBase,
		  const int ILength,
		  const int IOffset,
		  const int Offset,
		  const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    The index select the equivilent starting point.
    The post designator R is the index form.
    

    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getRangeComposite(SMap,IBase,ILength,IOffset,
			   Offset,0,0,BaseString);
}

std::string
getRangeComposite(const surfRegister& SMap,
 		  const int IBase,
		  const int ILength,
		  const int IOffset,
		  const int Offset,
		  const int minorOffset,
		  const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    The index select the equivilent starting point.
    The post designator R is the index form.
    

    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  return getRangeComposite(SMap,IBase,ILength,IOffset,
			   Offset,minorOffset,0,BaseString);
}

std::string
getRangeComposite(const surfRegister& SMap,
 		  const int IBase,
		  const int IEnd,
		  const int IOffset,
		  const int Offset,
		  const int minorOffset,
		  const int secondOffset,
		  const std::string& BaseString)
  /*!
    Given a base string add an offset to the numbers
    The index select the equivilent starting point.
    The post designator R is the index form.
    
    If a cell does not exist ignore [no error]
    If a number is preceeded by T then it is a true number.
    Use T-4000 etc.
    \param SMap :: Surf register 
    \param Offset :: Offset nubmer to add
    \param minorOffset :: minor Offset nubmer to add [M]
    \param secondOffset :: second minor Offset nubmer to add [N]
    \param BaseString :: BaseString number
    \return String with offset components
   */
{
  std::ostringstream cx;
  std::string segment=spcDelimString(BaseString);
  const std::string segmentHold=segment;
  std::string OutUnit;
  
  // First count number of R units:
  const int ILength=1+IEnd-IBase;
  while(StrFunc::section(segment,OutUnit))
    {
      const size_t oL=OutUnit.length();
      if (OutUnit[oL-1]=='R')
	{
	  OutUnit[oL-1]=' ';
	  char keyUnit(' ');
	  if (oL>2 && std::isalpha(OutUnit[oL-2]))
	    {
	      keyUnit=OutUnit[oL-2];
	      OutUnit[oL-2]=' ';
	    }
	  int surfN;
	  if (StrFunc::section(OutUnit,surfN))
	    {
	      const int signV(surfN>0 ? 1 : -1);
	      surfN*=signV;
	      int oNum((surfN-IBase+IOffset) % ILength);
	      if (oNum<0) oNum+=ILength;
	      cx<<signV*(oNum+IBase)<<keyUnit;
	    }
	}
      else
	cx<<OutUnit<<" ";
    }
  return getSetComposite(SMap,Offset,minorOffset,secondOffset,cx.str());
}

  
}  // NAMESPACE ModelSupport
