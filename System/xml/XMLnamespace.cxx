/********************************************************************* 
  CombLayer : MCNP(XP Input builder
 
 * File:   xml/XMLnamespace.cxx
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
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <sys/stat.h>
#include <time.h>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "regexBuild.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcomp.h"
#include "XMLload.h"
#include "XMLcollect.h"
#include "XMLnamespace.h"
#include "XMLiterator.h"

namespace XML
{

std::vector<std::string>
getUnits(std::string Line)
  /*!
    Deconvolve out the part of the < > line
    and return the keys
    \param Line :: Line to process
    \return Vector of keys
  */
{
  std::vector<std::string> Out;

  std::string::size_type posA=Line.find('<');
  std::string::size_type posB=Line.find('>');
  if (posA==std::string::npos || posB==std::string::npos)
    return Out;

  while(posA!=std::string::npos && posB!=std::string::npos)
    {
      // Remove > before <
      if (posB>posA)
	{
	  // get unit without space unit
	  std::string Unit;
	  for(posA++;posA!=posB && std::isspace(Line[posA]);posA++)
	    Unit+=Line[posA];
	  if (!Unit.empty())
	    Out.push_back(Unit);
	}	  

      Line.erase(0,posB);
      posA=Line.find('<');   // update
      posB=Line.find('>');      
    }
  return Out;
}

std::vector<std::string>
getParts(std::string KeyList)
  /*!
    Split the string into useful parts.
    The form is KeyA::KeyB::KeyC
    \param KeyList :: Line of the keys 
    \return string List
  */
{
  std::vector<std::string> Out;
  std::string CutKey(KeyList);
  std::string::size_type pos=KeyList.find("::");
  while(pos!=std::string::npos)
    {
      Out.push_back(KeyList.substr(0,pos));
      KeyList.erase(0,pos+2);
      pos=KeyList.find("::");
    }						
  Out.push_back(KeyList);
  return Out;
}

std::pair<int,std::string>
procKey(const std::string& Line)
  /*!
    Given a key : e.g. \<Monitor_1\>
    return the key with the folling flags 
    \param Line :: Line to pre-process
    \retval  0 :: Nothing found
    \retval  1 :: Key found (new)
    \retval -1 :: Key closed
    \retval 100 :: Key found and closed (data)
    \retval -100 :: Key found and closed (no data)
  */
{
  ELog::RegMethod RegA("XMLnamespace","procKey");

  typedef std::pair<int,std::string> retType;
  
  std::vector<std::string> Out=getUnits(Line);

  std::vector<std::string>::const_iterator vc;
  if (Out.empty())
    return retType(0,"");
  if (Out.size()==1)
    {
      const std::string& Tag=Out[0];
      if (Tag[0]=='/')
	return retType(-100,Tag.substr(1));
      if (Tag[Tag.size()-1]=='/')
	return retType(-100,Tag.substr(0,Tag.size()-1));
      return retType(1,Tag);
    }
  if (Out.size()==2)
    {
      const std::string TagA=Out[0];
      const std::string TagB=Out[1];
      if (TagA==TagB.substr(0,TagB.size()-1))
	return retType(100,TagA);
    }
  
  ELog::EM<<"Unable to decode XML:"<<Line<<ELog::endErr;
  return retType(0,"");
}

int
getGroupContent(XMLload& IFile,std::string& Key,
		std::vector<std::string>& Attrib,
		std::string& Data)
  /*!
    Given an open group \<key\> : Read until we encounter a new \<grp\> 
    or close a new group
    
    If we start from after \<alpha\> then continue.
    
    \param IFile :: Input file control
    \param Key :: Key name
    \param Attrib :: Attributes if found
    \param Data :: Lines of data read

    \retval 0 :: Failed
    \retval 1 :: opened a new object
    \retval 2 :: closed key 
    \retval -1 :: Null group
  */
{
  // Clear the user provided data
  Data.clear();

  std::string Line;         // Data strings  after e.g. <key> data data .... 
  std::string Group;        // Group is the </closekey> or next group e.g. <newKey>
  char c(0);                // Character to read from file

  // Required to finish regex:
  while(IFile.get(c) && c!='<')
    {
      Line+=c;
    }
  // Ok found either (a) a close group (b) a new open group
  // (c) a comment
  if (!Line.empty())
    Data=StrFunc::fullBlock(Line);
  if (c!='<') return 0;

  while(IFile.get(c) && c!='>')
    {
      Group += c;
    }
      
  IFile.pop();
  return procGroupString(Group,Key,Attrib);
}

int
findGroup(XMLload& IFile,const std::string& Key,const int noPop)
  /*!
    Search until we find a group with key
    and then stop. Reseting IFile to open '<'
    \param IFile :: Input file 
    \param Key :: Input key
    \param noPop :: No pop of the file on finish
    \return 1 :: success
    \return 0 :: fail
  */
{
  ELog::RegMethod RegA("XMLnamespace","findGroup");

  char c(IFile.current());   // Character to read from file
  int quote=0;
  const size_t LN(Key.length()+1);
  // Required to finish regex:

  do
    {
      if (c=='\'')                  // Quote
	quote=1-quote;
      if (c=='<' && !quote)         //start Point
        {
 	  const long int cPt=IFile.getPos();
	  std::string Group;
	  while(IFile.get(c) && isspace(c)) ;
	  Group+=c;
	  size_t cnt(0);
	  while(IFile.get(c) && !isspace(c) && c!='>' && cnt<LN)
	    {
	      Group+=c;
	      cnt++;
	    }

	  if (Group==Key)
	    {
	      IFile.setPos(cPt);
	      if (!noPop) IFile.pop();
	      return 1;
	    }
	}
    }  while(IFile.get(c));

  return 0;
}

int
getNextGroup(XMLload& IFile,std::string& Key,
	     std::vector<std::string>& Attrib)
/*!
    Given a file read until the next \<key\> or the 
    current system closes.
    If we start from \<alpha\>
    \param IFile :: Input file
    \param Key :: Key name
    \param Attrib :: Attribute if found
    \retval 0 :: Failed
    \retval 1 :: opened a new object
    \retval 2 :: closed key
    \retval -1 :: Null group
  */
{
  char c(IFile.current());   // Character to read from file
  int init(0);
  int quote=0;
  std::string Group;
  // Required to finish regex:
  do
    {
      if (c=='\'')                  // Quote
	quote=1-quote;
      Group+=c;
      if (c=='<' && !quote)         //start Point
        {
	  Group="";
	  init=1;
	}
    }
  while(IFile.get(c) && 
	(c!='>' || quote || !init));

  // Consume all:
  IFile.pop();
  return procGroupString(Group,Key,Attrib);
}

int
matchSubGrp(XMLload& IFile,const std::string& subGrp,const std::string& Value)
  /*!
    Match a subgroup of the current group:
    Does not change the IFile state just reads the current file to determine
    if the group has a match.
    - Expected use for \<name\>Something\</name\>
    - Expect to be at start of master group name:

    \param IFile :: XMLload state
    \param subGrp :: Sub group/Object value
    \param Value :: Value to check
    \return  1 on success / 0 on fail
   */
{
  const long int cPt=IFile.getPos();   // Current unmutable point:
  char c(IFile.current());           // Character to read from file
  int quote=0;
  std::string MasterGroup;       // Master group name
  int masterCnt(0);              // for repeat counts
  // Required to finish regex:
  do
    {
      if (c=='\'')                  // Quote
	quote=1-quote;
      if (c=='<' && !quote)         //start Point
        {
	  std::string Group;
	  while(IFile.get(c) && isspace(c)) ;
	  Group+=c;
	  int cnt(0);
	  while(IFile.get(c) && !isspace(c) && c!='>')
	    {
	      Group+=c;
	      cnt++;
	    }
	  // Explore group :
	  if (!Group.empty() && Group[0]!='/' && Group[Group.size()-1]!='/')
	    {
	      MasterGroup=Group;
	      masterCnt=1;
	    }
	}
    }  while(!masterCnt && IFile.get(c));

  // FIND SUB GROUP:
  do
    {
      if (c=='\'')                  // Quote
	quote=1-quote;
      if (c=='<' && !quote)         //start Point
        {
	  std::string Group;
	  while(IFile.get(c) && isspace(c)) ;
	  Group+=c;
	  int cnt(0);
	  while(IFile.get(c) && !isspace(c) && c!='>')
	    {
	      Group+=c;
	      cnt++;
	    }
	  if (!Group.empty())
	    {
	      if (Group[0]=='/')
	        {
		  if (Group.substr(1)==MasterGroup)
		    {
		      masterCnt--;
		      if (!masterCnt)  // failed:
		        {
			  IFile.setPos(cPt);
			  return 0;
			}
		    }
		}
	      else if (Group==subGrp)     // EXCELLENT Found key:
	        {
		  std::string Component;
		  // Read unit:
		  int flag(0);
		  while(IFile.get(c) && c!='<')
		    { 
		      if (flag)
			Component+=c;
		      if (c=='>')
			flag=1;
		    }
		  if (Component==Value)
		    {
		      IFile.setPos(cPt);
		      return 1;
		    }
		}
	    }
	}
    }  while(IFile.get(c));

  IFile.setPos(cPt);
  return 0;
}

int
splitGroup(XMLload& IFile,std::string& Key,
	   std::vector<std::string>& Attrib,std::string& Body)
  /*!
    From a file: read \<key attrib=.....\> Body \<key/\>
    \param IFile :: Input file
    \param Key :: Key of group
    \param Attrib :: Attributes (if any)
    \param Body :: Main component
    \retval 0 :: failure
    \retval 1 :: success
  */
{
  ELog::RegMethod RegA("XMLnamespace","splitGroup");
  // Note the extra .*? at the front :: 
  // This removes all the pre-junk.
  const int flag = getNextGroup(IFile,Key,Attrib);
  Body.erase(0,std::string::npos);
  if (!flag || flag==2)
    return 0;
  // group empty (success all done)
  if (flag==-1)
    return 1;
  // Now read body (until close)
  char c;
  std::string Line;
  std::string Part;
  std::string searchStr="/"+Key;
  int inKey(0);
  while(IFile.get(c))
    {
      if (!inKey)
        {
	  if (c!='<')
	    Line+=c;
	  else 
	    {
	      inKey=1;
	      Part="";
	    }
	}
      else
        {
	  if (c!='>')
	    Part+=c;
	  else
	    {
	      inKey=0;
	      Body+=Line;
	      if (Part==searchStr)
	        {
		  IFile.pop();
		  return 1;
		}
	    }
	}
    }
  Body+=Line;
  return 0;
}

int
splitObjGroup(XMLobject* AR,
	   std::vector<std::string>& Keys,
	   std::vector<std::string>& Body)
  /*!
    From a file: read \<key attrib=.....\> Body \<key/\>
    \param AR :: Input object/group
    \param Keys :: Keys for each sub-group
    \param Body :: Main component
    \retval 0 :: failure
    \retval 1 :: success
  */
{
  ELog::RegMethod RegA("XMLnamespace","splitObjGroup");
  XML::XMLgroup* AG;
  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
    {
      XML::XMLiterator SK(AG);
      do
	{
	  Keys.push_back(SK->getKey());
	  Body.push_back((*SK)->getComponent());
	} while(SK++);
    }
  else
    {
      Keys.push_back("");
      Body.push_back(AR->getItem<std::string>());
    }
  
  return 0;
}

int
getFilePlace(XMLload& IFile,const std::string& KeyList)
  /*!
    Determine a place in a file given a string
    \param IFile :: Input file
    \param KeyList :: Key list in the form A::B::C
    \retval -1 :: Empty Tag
    \retval 1 :: Opening tag
    \retval 0 :: nothing found
  */
{
  ELog::RegMethod RegA("XMLnamespace","getFilePlace");

  const size_t keyLen(KeyList.length());
  std::string keyVal;
  std::vector<std::string> Attrib;
  size_t len;
  std::string FullName,testStr;
  std::string::size_type pos;
  while (IFile.good())
    {
      const int flag=getNextGroup(IFile,keyVal,Attrib);
      switch (flag)
        {
	case 0:
	  return 0;
	case 1:            // Open new
	  FullName+="::"+keyVal;
	  len=FullName.length();
	  if (len>=keyLen)
	    {
	      testStr=FullName.substr(len-keyLen,std::string::npos);
	      if (testStr==KeyList)
		return 1;
	    }
	  break;
	case 2:
	  pos=FullName.rfind("::");
	  if (pos==std::string::npos)
	    return 0;
	  FullName.erase(pos);
	  break;
	case -1:
	  testStr=FullName+"::"+keyVal;
	  len=testStr.length();
	  testStr=testStr.substr(len-keyLen,std::string::npos);
	  if (testStr==KeyList)
	    return -1;
	  break;
	default:
	  ELog::EM<<"Error in switch flag: "<<flag<<ELog::endErr;
	}
    }
  return 0;
}



int
procGroupString(const std::string& Group,
	     std::string& Key,
	     std::vector<std::string>& AtVec)
  /*!
    Split a group into keys and attributes
    This will have a part \</key\>
    \param Group :: Group we are after
    \param Key :: Key value found
    \param AtVec :: Vector of attribute

    \retval 0 :: Failed
    \retval 1 :: opened a new object
    \retval 2 :: closed key
    \retval -1 :: Null group
  */
{
  AtVec.clear();
  // Found Attribute and normal key : <Key Att=this att2=this>
  std::string Part=Group;
  
  std::string Kval;
  if (!StrFunc::section(Part,Kval))
    return 0;

  if (Kval[0]=='/')           // closed group
    {
      Key=Kval.substr(1);
      return 2;
    }
  Key=Kval;
  // Note the reuse of the string
  while(StrFunc::section(Part,Kval))
    {
      AtVec.push_back(Kval);
    }
  if (Kval[Kval.length()-1]=='/')
    {
      if (AtVec.empty())
	Key=Kval.substr(0,Kval.length()-1);
      else
	AtVec.back()=Kval.substr(0,Kval.length()-1);
      return -1;
    }

  return 1;
}

int
collectBuffer(XMLload& IFile,std::string& Buffer)
  /*!
    Collect a buffer of lines from IFile between 
    the components.
    \param IFile :: Input file buffer
    \param Buffer :: vector of strings to find
    \return value
   */
{
  Buffer.erase(0,std::string::npos);
  std::string Line;
  char c;
  while (IFile.get(c) && c!='<')
    {
      Line+=c;
    }
  Buffer=Line;
  IFile.pop();
  return static_cast<int>(Buffer.size());
}

int
splitComp(XMLload& IFile,
	  std::string& closeKey,
	  std::string& Line)
  /*!
    After a \<key\> for a component is read
    read until \</key\> closes the system
    and pass the line object
    \param IFile :: Control File sytem
    \param closeKey :: \<key/\> that closed things
    \param Line :: component line
    \retval 0 :: Success
    \retval -1 : failed to find  
   */
{
  // Now read body (until close)
  char c;
  while(IFile.get(c) && c!='<')
    Line+=c;
  // Need a </key> to close the group
  if (!IFile.get(c) || c!='/')
    return -1;
  while(IFile.get(c) && c!='>')
    closeKey+=c;
  if (c!='>')
    return -2;
  IFile.pop();
  return 0;
}

int
splitLine(XMLload& IFile,std::string& closeKey,std::string& Line)
  /*!
    After a \<key\> for a component is read
    read until \<\\key\> closes the system
    and pass the line object.
    Only reads one Line . 
    \param IFile :: Control file system
    \param closeKey :: \<key/\> that closed things
    \param Line :: Line read
    \retval 0 :: Success
    \retval 1 :: finished
    \retval -1 :: failed
   */
{
  // Now read body (until close)
  char c;
  while(IFile.get(c) && c!='<')
    Line+=c;
  // Need a </key> to close the group
  if (!IFile.get(c) || c!='/')
    return -1;
  while(IFile.get(c) && c!='>')
    closeKey+=c;
  return 1;
}

template<typename K,typename D>
long int
getNumberIndex(const std::multimap<K,D>& MX,
	       const K& Key)
  /*!
    Carry out a binary search to determine the lowest
    value that can be indexed into MX . 
    - Tested in testXML::testBinSearch
    \param MX :: Map to search
    \param Key :: BaseKey to add numbers to. e.g. Alpha
    \return 0 :: success 
  */
{
  int index=1;
  int step=1;
  int lowBound(0);
  int highBound(-1);
  typename std::multimap<K,D>::const_iterator mc;

  // Boundary step:
  std::ostringstream cx;
  while(highBound<0)
    {
      cx.str("");
      cx<<Key<<index;
      mc=MX.find(cx.str());
      if (mc!=MX.end())
	lowBound=index;
      else 
	highBound=index;
      index+=step;
      step*=2;
    }
  // Search step
  while(highBound-lowBound>1)
    {
      const int mid=(highBound+lowBound)/2;
      cx.str("");
      cx<<Key<<mid;
      mc=MX.find(cx.str());
      if (mc!=MX.end())
	lowBound=mid;
      else 
	highBound=mid;
    }
  return highBound;
}

int
splitAttribute(std::string& AList,std::string& Key,std::string& Value)
  /*!
    Given an attribute list e.g.
    - File="Test" Out="junk" 
    Split into components
    \param AList :: Full component line
    \param Key :: Key found
    \param Value :: Value to the component found
    \retval 0 if nothing to do
    \retval -1 error
    \retval 1 :: success 
   */
{
  std::string::size_type pos=AList.find("=");     
  if (pos==std::string::npos)
    return 0;

  StrFunc::convert(AList.substr(0,pos),Key);
  AList.erase(0,pos+1);
  return (!XML::cutString(AList,Value)) ? -1 : 1;
}


long int
cutString(std::string& AList,std::string& Value)
  /*!
    Given a string with quotes e.g. File="cut"
    It takes out the "cut" part.

    \param AList :: String to find " " section and cut out
    \param Value :: Value to put
    \retval -ve :: Error with Length
    \retval Length of string extracted (possibly 0)
   */
{
  size_t start;
  size_t end;

  for(start=0;start<AList.length() && AList[start]!='"';start++) ;
  for(end=start+1;end<AList.length() && AList[end]!='"';end++) ;
  if (end>=AList.length())
    return -1;

  Value=AList.substr(start+1,end-(start+1));
  AList.erase(start,1+start-end);
  return static_cast<long int>(end-(start+1));
} 

std::string
procString(const std::string& Item)
  /*!
    Given a string Item 
    produce the string without the standard 
    symbol that XML requires out
    \param Item :: Line to be processed
    \todo Use a proper Entities class
    \return processed string
  */
{
  std::string::size_type pos=Item.find_first_of("&<>");
  if (pos==std::string::npos)
    return Item;
  std::ostringstream cx;
  std::string::size_type cutPos=0;
  do
    {
      cx<<Item.substr(cutPos,pos-cutPos);
      switch (Item[pos])
      {
	case '<':
	  cx<<"&lt;";
	  break;
	case '>':
	  cx<<"&gt;";
	  break;
	case '&':
	  cx<<"&amp;";
	  break;
      }
      cutPos=pos+1;
      pos=Item.find_first_of("&<>",cutPos);
    } while(pos!=std::string::npos);

  cx<<Item.substr(cutPos,std::string::npos);
  return cx.str();
}

int
matchPath(const std::string& A,const std::string& B)
  /*!
    Check that A and B match: B can contain a regular 
    expression.
    \param A :: Primary Key
    \param B :: Secondary Key / Regex
    \retval 1 :: Match (exact)
    \retval 2 :: Match [with regex]
   */
{
  if (A==B)
    return 1;

  if (StrFunc::StrLook(A,B))
    return 2;

  return 0;
}

/// \cond TEMPLATE

template long int 
getNumberIndex(const std::multimap<std::string,long int>&,
	       const std::string&);

template long int 
getNumberIndex(const std::multimap<std::string,size_t>&,
	       const std::string&);

template long int 
getNumberIndex(const std::multimap<std::string,unsigned int>&,
	       const std::string&);

template long int 
getNumberIndex(const std::multimap<std::string,int>&,
	       const std::string&);

/// \endcond TEMPLATE

}  // Namespace XML
