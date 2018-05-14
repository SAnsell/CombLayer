/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/regexSupport.cxx
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
#ifndef NO_REGEX

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <regex>

#include "support.h"
#include "regexSupport.h"

namespace StrFunc
{

template<typename T>
int 
findComp(std::istream& IX,const std::regex& Re,T& Out)
  /*! 
    Finds the start of the tally 
    \param IX :: open file stream
    \param Re :: regular expression to match
    \param Out :: component in ( ) expression must be first.
    \returns count of line that matched (or zero on failure)
  */
{
  std::smatch ans;

  int cnt(1);
  std::string SStr=StrFunc::getLine(IX,512); 
  bool flag=std::regex_search(SStr,ans,Re);
  while(!IX.fail() && !flag)
    {
      cnt++;
      SStr=StrFunc::getLine(IX,512); 
      flag=std::regex_search(SStr,ans,Re);
    }
  if (flag)
    {
      std::string xout(ans[1].first,ans[1].second);
      if (StrFunc::convert(xout,Out))
	return cnt;
    }
  return 0;  
}

template<>
int 
findComp(std::istream& IX,const std::regex& Re,std::string& Out)
  /*! 
    Finds the start of the tally 
    \param IX :: open file stream
    \param Re :: regular expression to match
    \param Out :: component in ( ) expression must be first.
    \returns count of line that matched (or zero on failure)
  */
{
  std::smatch ans;

  int cnt(1);
  std::string SStr=StrFunc::getLine(IX,512); 
  bool flag=std::regex_search(SStr,ans,Re);
  while(!IX.fail() && !flag)
    {
      cnt++;
      SStr=StrFunc::getLine(IX,512); 
      flag=std::regex_search(SStr,ans,Re);
    }
  if (flag)
    {
      Out=std::string(ans[1].first,ans[1].second);
      return cnt;
    }
  return 0;
}
  
int
findPattern(std::istream& IX,const std::regex& Re,std::string& Out)
  /*! 
    Finds the start of the tally 
    \param IX :: open file stream
    \param Re :: regular expression to match
    \param Out :: string to place match 
    \returns count of line that matched (or zero on failure)
  */
{
  std::string fileStr;
  int cnt(1);

  std::smatch ans;
  
  fileStr=StrFunc::getLine(IX,512);
  bool flag=std::regex_search(fileStr,ans,Re);
  while(!IX.fail() && !flag)
    {
      cnt++;
      fileStr=StrFunc::getLine(IX,512); 
      flag=std::regex_search(fileStr,ans,Re);
    }
  
  if (!flag) return 0;
  Out = fileStr;
  return cnt;
}

  

int
StrLook(const char* Sx,const std::regex& Re)
  /*!
    Find the match in regular expression and return 1 if good match 
    \param Sx :: string to match
    \param Re :: regular expression to use
    \returns 0 on failure and 1 on success
  */
{
  std::cmatch ans;
  return (std::regex_search(Sx,ans,Re)) ? 1 : 0;
}

int
StrLook(const std::string& Text,const std::regex& Re)
  /*!
    Find the match in regular expression and return 1 if good match 
    \param Text :: string to match
    \param Re :: regular expression to use
    \returns 0 on failure and 1 on success
  */
{
  std::smatch ans;
  return (std::regex_search(Text,ans,Re)) ? 1 : 0;
}


// -----------------------------------------------------------------------------------------

  
template<typename T>
int
StrComp(const std::string& Text,const std::regex& Re,T& Aout,
	const size_t compNum) 
  /*!
    Find the match in regular expression and places number in Aout 
    \param Text :: string to search
    \param Re :: regular expression to use
    \param Aout :: Place to put Unit found
    \param compNum :: item to extract [0:N-1]
    \returns 0 on failure and 1 on success
  */
{
  std::sregex_iterator m1(Text.begin(),Text.end(),Re);
  std::sregex_iterator empty;
  // Failed search
  if (m1==empty || 
      ((*m1).size()+1)<= compNum )  
    return 0;

  return convert( (*m1)[compNum+1].str(),Aout);
}


template<>
int
StrComp(const std::string& Text,const std::regex& Re,
	std::string& Aout,const size_t compNum) 
  /*!
    Find the match in regular expression and places number in Aout 
    \param Text :: string to search
    \param Re :: regular expression to use
    \param Aout :: Place to put Unit found
    \param compNum :: item to extract [0:N-1]
    \returns 0 on failure and 1 on success
  */
{
  std::sregex_iterator m1(Text.begin(),Text.end(),Re);
  std::sregex_iterator empty;
  // Failed search
  if (m1==empty || 
      (*m1).size()+1<=compNum)  
    return 0;

  Aout=(*m1)[compNum+1].str();
  return 1; 
}

// -------------------------------------------------------------

std::vector<std::string> 
StrParts(std::string Text,const std::regex& Re)
  /*! 
    Find the match, return the disected items.
    Note it is complementary to support.h StrParts(Sdx)
    \param Text :: Input string (note implicit copy since altered)
    \param Re :: Regular expression for separator component
    \returns vector of string components
  */
{
  std::vector<std::string> Aout;

  std::sregex_iterator match=
    std::sregex_iterator(Text.begin(), Text.end(), Re);
  while(match != std::sregex_iterator())
    {
      for(size_t i=1;i<match->size();i++)
	Aout.push_back( (*match)[i].str() );	
      match++;
    }
  return Aout;
}   

// --------------------------------------------------------------


template<typename T>
int
StrFullCut(std::string& Text,const std::regex& Re,T& Aout,
       const long int compNum)
  /*! 
    Find the match, return the disected items:
    Then remove the whole of the match
    The regexpression must have  one  ( ) around the area to extract
    \param Text :: string to split, is returned with the string after
    the find (if successful).
    \param Re :: regular expression to use.
    \param Aout :: Value to extract
    \param compNum :: Index of matches [0->N-1] (-1 :: whole match)
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval 1 :: success
   */
{
  std::sregex_iterator m1(Text.begin(),Text.end(),Re);
  std::sregex_iterator empty;
  if (m1==empty)
    return 0;
  const size_t CI(static_cast<size_t>(compNum+1));
  if (CI >= m1->size())
    return 0;
  // StrFunc::Convert to required output form
  if (!StrFunc::convert((*m1)[CI].str(),Aout))
    return 0;
  // Found object 

  const size_t Zero(0);
  const size_t PStart=
    static_cast<size_t>((*m1).position(Zero));
  Text.erase(PStart,(*m1)[0].str().length());
  
  return 1;
}

template<typename T>
int
StrFullCut(std::string& Text,const std::regex& Re,
	   std::vector<T>& Aout)
  /*! 
    Find the match, return the disected items:
    Then remove the whole of the match
    The regexpression must have  one  ( ) around the area to extract
    \param Text :: string to split, is returned with the string after
    the find (if successful).
    \param Re :: regular expression to use.
    \param Aout :: Values to extract
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval 1 :: success
   */
{
  std::sregex_iterator m1(Text.begin(),Text.end(),Re);
  std::sregex_iterator empty;
  if (m1==empty)
    return 0;
  Aout.clear();
  for(unsigned int index=1;index<m1->size();index++)
    {
      T tmp;
      if (!StrFunc::convert((*m1)[index].str(),tmp))
	return 0;
      Aout.push_back(tmp);
    }
  // Found object 
  size_t Zero(0);
  const size_t PStart=
    static_cast<size_t>((*m1).position(Zero));
  Text.erase(PStart,(*m1)[0].str().length());
  return 1;
}


template<>
int
StrFullCut(std::string& Text,const std::regex& Re,
	   std::vector<std::string>& Aout)
  /*! 
    Find the match, return the disected items:
    Then remove the whole of the match
    The regexpression must have  one  ( ) around the area to extract
    This is specialised for string and thus does not need 
    a convert.
    \param Text :: string to split, is returned with the string after
    the find (if successful).
    \param Re :: regular expression to use.
    \param Aout :: Values to extract
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval 1 :: success
   */
{
  std::sregex_iterator m1(Text.begin(),Text.end(),Re);
  std::sregex_iterator empty;
  if (m1==empty)
    return 0;

  Aout.clear();
  for(size_t index=1;index<m1->size();index++)
    Aout.push_back((*m1)[index].str());

  // Found object 
  const size_t Zero(0);
  const size_t PStart=
    static_cast<size_t>((*m1).position(Zero));
  Text.erase(PStart,(*m1)[0].str().length());
  return 1;
}

// -----------------------------------------------------------------------

int
StrRemove(std::string& Sdx,std::string& Extract,const std::regex& Re)
/*! 
  Find the match, return the string - the bit 
  \param Sdx :: string to split, is returned with the string after
  the find (if successful).
  \param Extract :: Full piece extracted
  \param Re :: regular expression to use.
  \retval 0 :: failed to match the string or there were no parts to match.
  \retval 1 :: succes
*/
{
  std::sregex_token_iterator empty;

  std::cmatch ans;
  if (std::regex_search(Sdx.c_str(),ans,Re))
    {
      if (!ans[0].matched)       // no match
	return 0;
      std::string xout(ans[0].first,ans[0].second);
      Extract=std::string(ans[0].first,ans[0].second);
      Sdx= std::string(Sdx.c_str(),ans[0].first)+
	std::string(ans[0].second);
      return 1;
    }
  return 0;
}
// --------------------------------------------------------------------------
  

template<typename T>
int
StrFullSplit(const std::string& text,
	     const std::regex& Re,std::vector<T>& Aout)
  /*! 
    Find the match, return the disected items
    The regexpression must have ( ) around the area to extract.
    The function appends the results onto Aout.
    \param text :: string to split
    \param Re :: regular expression to use.
    \param Aout :: vector to add components to.
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval Number :: number of components added to Aout.
   */
{
  std::sregex_iterator m1(text.begin(),text.end(),Re);
  std::sregex_iterator empty;
  for(;m1!=empty;m1++)
    for(size_t index=1;index<m1->size();index++)
      {
	T tmp;
	if (!StrFunc::convert((*m1)[index].str(),tmp))
	  return static_cast<int>(Aout.size());
	Aout.push_back(tmp);
      }
  return static_cast<int>(Aout.size());
}

template<>
int
StrFullSplit(const std::string& text,
	     const std::regex& Re,std::vector<std::string>& Aout)
  /*! 
    Find the match, return the disected items
    The regexpression must have ( ) around the area to extract.
    The function appends the results onto Aout.
    \param text :: string to split
    \param Re :: regular expression to use.
    \param Aout :: vector to add components to.
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval Number :: number of components added to Aout.
   */
{
  std::sregex_iterator m1(text.begin(),text.end(),Re);
  std::sregex_iterator empty;
  for(;m1!=empty;m1++)
    for(size_t index=1;index<m1->size();index++)
      Aout.push_back((*m1)[index].str());

  return static_cast<int>(Aout.size());
}
// --------------------------------------------------------------------------

template<typename T>
int
StrSingleSplit(const std::string& text,
	     const std::regex& Re,std::vector<T>& Aout)
  /*! 
    Find the match, return the disected items
    The regexpression must have ( ) around the area to extract.
    The function appends the results onto Aout.
    
    \param text :: string to split
    \param Re :: regular expression to use.
    \param Aout :: vector to add components to.
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval Number :: number of components added to Aout.
   */
{
  std::sregex_iterator m1(text.begin(),text.end(),Re);
  std::sregex_iterator empty;
  if (m1!=empty)
    for(size_t index=1;index<m1->size();index++)
      {
	T tmp;
	if (!StrFunc::convert((*m1)[index].str(),tmp))
	  return static_cast<int>(Aout.size());
	Aout.push_back(tmp);
      }

  return static_cast<int>(Aout.size());
}

template<>
int
StrSingleSplit(const std::string& text,
	     const std::regex& Re,std::vector<std::string>& Aout)
  /*! 
    Find the match, return the disected items
    The regexpression must have ( ) around the area to extract.
    The function appends the results onto Aout.
    - Specialised to avoid convert for std::string
    \param text :: string to split
    \param Re :: regular expression to use.
    \param Aout :: vector to add components to.
    \retval 0 :: failed to match the string or there were no parts to match.
    \retval Number :: number of components added to Aout.
   */
{
  std::sregex_iterator m1(text.begin(),text.end(),Re);
  std::sregex_iterator empty;
  if (m1!=empty)
    {
      for(size_t index=1;index < m1->size();index++)
	Aout.push_back((*m1)[index].str());
      return 1;
    }
  return 0;
}

/// \cond TEMPLATE 

template int StrFullCut(std::string&,const std::regex&,
			std::string&,const long int);
template int StrFullCut(std::string&,const std::regex&,int&,const long int);
template int StrFullCut(std::string&,const std::regex&,double&,const long int);

// --------------------------------------------------------
template int StrFullSplit(const std::string&,const std::regex&,
			  std::vector<int>&);
template int StrFullSplit(const std::string&,const std::regex&,
			  std::vector<double>&);
// --------------------------------------------------------
template int StrSingleSplit(const std::string&,const std::regex&,
			  std::vector<int>&);
template int StrSingleSplit(const std::string&,const std::regex&,
			  std::vector<double>&);
// --------------------------------------------------------

template int StrComp(const std::string&,const std::regex&,double&,const size_t);
template int StrComp(const std::string&,const std::regex&,int&,const size_t);
// ------------------------------------------------------------------
template int findComp(std::istream&,const std::regex&,int&);

/// \endcond TEMPLATE 

}  // NAMESPACE StrFunc

#endif
