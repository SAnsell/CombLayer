/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xml/XMLcollect.cxx
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
#include <map>
#include <list>
#include <sys/stat.h>
#include <time.h>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "fileSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcomment.h"
#include "XMLread.h"
#include "XMLcomp.h"
#include "XMLvector.h"
#include "XMLgrid.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLcollect.h"

namespace Geometry
{
  class Vec3D;
  class Quaternion;
}

namespace XML
{

XMLcollect::XMLcollect() : 
  Master(0,"metadata_entry"),WorkGrp(&Master)
  /*!
    Constructor : Creates a top object with
    Metadata_entry 
   */
{ }

XMLcollect::XMLcollect(const XMLcollect& A) : 
  depthKey(A.depthKey),Master(A.Master),
  WorkGrp(dynamic_cast<XMLgroup*>(Master.getObj(depthKey)))
  /*!
    Standard copy constructor
    but creates a possible WorkGrp object
    from they search against the master object
    \param A :: XMLcollect object to copy
  */
{}

XMLcollect&
XMLcollect::operator=(const XMLcollect& A) 
  /*!
    Standard assignment operator
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      depthKey=A.depthKey;
      Master=A.Master;
      WorkGrp=dynamic_cast<XMLgroup*>(Master.getObj(depthKey));
    }
  return *this;
}

XMLcollect::~XMLcollect()
  /*!
    Standard Destructor
  */
{}

void 
XMLcollect::clear()
  /*!
    Clears everything. Done by replacement
    WorkGrp is reset.
    The depthKey emptied.
  */
{
  Master=XMLgroup(0,"metadata_entry");
  WorkGrp=&Master;
  depthKey="";
  return;
}

void
XMLcollect::closeGrp()
  /*!
    Closes the group and moves up the stack one
  */
{
  std::string::size_type pos=depthKey.rfind("/");
  if (pos!=std::string::npos)
    {
      depthKey.erase(pos);
      WorkGrp=dynamic_cast<XMLgroup*>(WorkGrp->getParent());
      if (!WorkGrp)
        {
	  WorkGrp=&Master;
	  throw ColErr::ExBase("closeGrp : failed to convert key "+depthKey);
	}
      return;
    }
  depthKey.erase(0);
  WorkGrp=&Master;
  return;
}

int
XMLcollect::closeGrpWithCheck(const std::string& Key)
  /*!
    Closes the group and moves up the stack one
    \param Key :: Key to check
    \retval 0 :: success 
    \retval -1 :: failed to match key
  */
{
  std::string::size_type pos=depthKey.rfind("/");
  const int flag((WorkGrp && Key==WorkGrp->getKey()) ? 0 : -1);

  if (pos!=std::string::npos)
    {
      depthKey.erase(pos);
      if (WorkGrp)
	WorkGrp=dynamic_cast<XMLgroup*>(WorkGrp->getParent());
      if (!WorkGrp)
        {
	  WorkGrp=&Master;
	  throw ColErr::ExBase("closeGrp : failed to convert key "+depthKey);
	}
      return flag;
    }
  depthKey.erase(0);
  WorkGrp=&Master;
  return flag;
}

void
XMLcollect::addGrp(const std::string& GK)
  /*!
    Creates or finds a group with the key GK
    based on the current working group.
    Creates a name with Grp::GK
    \param GK :: Base/Non-base key
  */
{
  ELog::RegMethod RegA("XMLcollect","addGrp");
  
  XMLgroup* Gptr=WorkGrp->addGrp(GK);
  if (Gptr)
    {
      WorkGrp=Gptr;
      if (!depthKey.empty())
	depthKey+="/";
      depthKey += GK;
    }
  return;
}

int
XMLcollect::addNumGrp(const std::string& GK)
  /*!
    Creates a group with the key GK_Num
    such that Num is the lowest positive number
    Creates a name with Grp::GK_Num.
    \param GK Base/Non-base key
    \return Index Value of Grp number
  */
{
  const long int index=XML::getNumberIndex(WorkGrp->getMap(),GK);
  std::ostringstream cx;
  cx<<GK<<index;

  WorkGrp=WorkGrp->addGrp(cx.str());
  if (!depthKey.empty())
    depthKey+="/";
  depthKey += GK;
  return static_cast<int>(index);
}

template<template<typename T,typename A> class V,typename T,typename A>
long int
XMLcollect::addNumComp(const std::string& Key,const V<T,A>& ContX)
  /*!
    Adds a component with a vector of things to add 
    \param Key :: Key to used 
    \param ContX :: X Vector to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<Key;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  XMLgrid<V,T,A>* XG=new XMLgrid<V,T,A>(WorkGrp,cx.str());
  XG->setComp(0,ContX);
  WorkGrp->addManagedObj(XG);
  return out;
}


template<typename T>
long int
XMLcollect::addNumComp(const std::string& K,const T& V)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param K :: Key to used 
    \param V :: Value to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<K;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  WorkGrp->addComp<T>(cx.str(),V);
  return out;
}

template<>
long int
XMLcollect::addNumComp(const std::string& K,const std::string& V)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param K :: Key to used 
    \param V :: Value to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<K;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  WorkGrp->addComp<std::string>(cx.str(),V);
  return out;
}



template<template<typename T,typename A> class V,typename T,typename A>
long int
XMLcollect::addNumComp(const std::string& Key,const std::string& Fname,
		       const V<T,A>& ContX,const V<T,A>& ContY)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param Key :: Key to used 
    \param Fname :: auxillary filename
    \param ContX :: Vector component to add
    \param ContY :: Vector component to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<Key;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  XMLgroup* FG=WorkGrp->addGrp(cx.str());
  FG->addAttribute("file",Fname);
  StrFunc::writeFile(Fname,ContX,ContY);
  return out;
}

template<template<typename T,typename A> class V,typename T,typename A>
long int
XMLcollect::addNumComp(const std::string& Key,const V<T,A>& ContX,
		       const V<T,A>& ContY)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param Key :: Key to used 
    \param ContX :: Vector component to add
    \param ContY :: Vector component to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<Key;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  WorkGrp->addManagedObj(new XMLvector<V,T,A>(WorkGrp,cx.str(),ContX,ContY));
  return out;
}

template<template<typename T,typename A> class V,typename T,typename A>
long int
XMLcollect::addNumComp(const std::string& Key,const V<T,A>& ContX,
		       const V<T,A>& ContY,const V<T,A>& ContZ)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param Key :: Key to used 
    \param ContX :: Container object to add (first)
    \param ContY :: Container object to add (second)
    \param ContZ :: Container object to add (third)
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<Key;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  WorkGrp->addComp(cx.str(),XMLvector<V,T,A>(WorkGrp,Key,ContX,ContY,ContZ));
  return out;
}

template<template<typename T,typename A> class V,typename T,typename A>
long int
XMLcollect::addNumComp(const std::string& Key,const std::string& Fname,
		       const V<T,A>& ContX,const V<T,A>& ContY,
		       const V<T,A>& ContZ)
  /*!
    Adds a numbered component, ie K_Num,
    were K_Num is unique, thus allowing a group of things to be added.
    \param Key :: Key to used 
    \param Fname :: Filename for data steam (auxillary)
    \param ContX :: Vector component to add
    \param ContY :: Vector component to add
    \param ContZ :: Vector component to add
    \retval Cnt of new object 
  */
{
  std::ostringstream cx;  
  cx<<Key;
  const long int out = XML::getNumberIndex(WorkGrp->getMap(),cx.str());
  cx<<out;
  XMLgroup* FG=WorkGrp->addGrp(cx.str());
  FG->addAttribute("file",Fname);
  StrFunc::writeFile(Fname,ContX,ContY,ContZ);
  return out;
}
// ----------------------------
//         ADDCOMP  :FILE:
// ---------------------------


template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const std::string& Fname,
		    const V<T,A>& ContX, const V<T,A>& ContY)
/*!
    Sophisticated container adder: It takes two names and creates a file
    of the data, and then puts the filename into the object. 
    \param Key :: Key value to use
    \param Fname :: File to output 
    \param ContX :: Vector component to add
    \param ContY :: Vector component to add
    \return 0 on success
  */
{
  StrFunc::writeFile(Fname,ContX,ContY);
  const int retVal=WorkGrp->addComp<std::string>(Key,Fname);
  XMLgroup* FG=WorkGrp->addGrp(Key);
  FG->addAttribute("file",Fname);
  StrFunc::writeFile(Fname,ContX,ContY);
  return retVal;
}

template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const std::string& Fname,
		    const V<T,A>& ContX, const V<T,A>& ContY,
		    const V<T,A>& ContZ)
  /*!
    Sophisticated container adder: It takes two names and creates a file
    of the data, and then puts the filename into the object. 
    \param Key :: Key value to use
    \param Fname :: File to output 
    \param ContX :: Vector component to add
    \param ContY :: Vector component to add
    \param ContZ :: Vector component to add
    \return 0 on success
  */
{
  StrFunc::writeFile(Fname,ContX,ContY,ContZ);
  const int retVal=WorkGrp->addComp<std::string>(Key,Fname);
  XMLgroup* FG=WorkGrp->addGrp(Key);
  FG->addAttribute("file",Fname);
  StrFunc::writeFile(Fname,ContX,ContY);
  return retVal;
}

// ------------------------
// ADDCOMP
// ------------------------

template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const V<T,A>& ContX)
  /*!
    Adds a component with a vector of things to add 
    \param Key :: Key to used 
    \param ContX :: X Vector to add
    \retval Cnt of new object 
  */
{
  XMLgrid<V,T,A>* XG=new XMLgrid<V,T,A>(WorkGrp,Key);
  XG->setComp(0,ContX);
  return WorkGrp->addManagedObj(XG);
  return 0;
}

template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const V<T,A>& ContX,
		    const V<T,A>& ContY)
  /*!
    Adds a component with a vector of things to add 
    \param Key :: Key to used 
    \param ContX :: X Vector to add
    \param ContY :: Y Vector to add
    \retval Cnt of new object 
  */
{
  return WorkGrp->addManagedObj(new XMLvector<V,T,A>(WorkGrp,Key,ContX,ContY));
}

template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const V<T,A>& ContX,
		    const V<T,A>& ContY,const V<T,A>& ContZ)
  /*!
    Adds a component with a vector of things to add 
    \param Key :: Key to used 
    \param ContX :: X Vector to add
    \param ContY :: Y Vector to add
    \param ContZ :: Error Vector to add
    \retval Cnt of new object 
  */
{
  XMLgrid<V,T,A>* XG=new XMLgrid<V,T,A>(WorkGrp,Key);
  XG->setComp(0,ContX);
  XG->setComp(1,ContY);
  XG->setComp(2,ContZ);
  return WorkGrp->addManagedObj(XG);
}

template<template<typename T,typename A> class V,typename T,typename A>
int
XMLcollect::addComp(const std::string& Key,const V<T,A>& ContA,
		    const V<T,A>& ContB,const V<T,A>& ContC,
		    const V<T,A>& ContD)
  /*!
    Adds a component with a vector of things to add 
    \param Key :: Key to used 
    \param ContA :: X Vector to add
    \param ContB :: Y Vector to add
    \param ContC :: Y2 Vector to add
    \param ContD :: Y3 Vector to add
    \retval Cnt of new object 
  */
{
  XMLgrid<V,T,A>* XG=new XMLgrid<V,T,A>(WorkGrp,Key);
  XG->setComp(0,ContA);
  XG->setComp(1,ContB);
  XG->setComp(2,ContC);
  XG->setComp(3,ContD);
  return WorkGrp->addManagedObj(XG);
}

template<typename T>
int
XMLcollect::addComp(const std::string& K,const T& V)
  /*!
    Add a component to the system
    \param K :: Key to used
    \param V :: Value to add
    \retval 0 if key already exists 
    \retval 1 new key object added
  */
{
  return WorkGrp->addComp<T>(K,V);
}

template<>
int
XMLcollect::addComp(const std::string& K,const std::string& V)
  /*!
    Add a component to the system
    \param K :: Key to used
    \param V :: Value to add
    \retval 0 if key already exists 
    \retval 1 new key object added
  */
{
  return WorkGrp->addComp<std::string>(K,V);
}

template<>
int
XMLcollect::addComp(const std::string& K,tm* const& V)
  /*!
    Adds a component to the current group
    \param K :: Key to used
    \param V :: Value to add
    \retval 0 if key already exists 
    \retval 1 new key object added
  */
{
  return WorkGrp->addComp<std::string>(K,makeTimeString(V));
}

// ------------------------------------------------------------

void
XMLcollect::addComment(const std::string& Line)
  /*!
    Create a comment with line 
    \param Line :: Line object to add
  */
{
  XMLcomment* CPtr=new XMLcomment(WorkGrp,Line);
  WorkGrp->addManagedObj(CPtr);
  return;
}

template<typename T>
void
XMLcollect::addObjAttribute(const std::string& K,const T& V)
  /*!
    Adds an attribute of type T to the last object in the
    group
    \param K :: Key to used
    \param V :: Value to add (must be convertable by operator<<)
  */
{
  XMLobject* OPtr=WorkGrp->getLastObj();
  if (OPtr)
    OPtr->addAttribute(K,V);
  return;
}

template<typename T>
void
XMLcollect::addAttribute(const std::string& K,const T& V)
  /*!
    Adds an attribute of type T
    \param K :: Key to used
    \param V :: Value to add (must be convertable by operator<<)
  */
{
  std::ostringstream cx;
  cx<<V;
  WorkGrp->addAttribute(K,cx.str());
  return;
}

template<>
void
XMLcollect::addAttribute(const std::string& K,const std::string& V)
  /*!
    Adds an attribute (of type string)
    \param K :: Key to used
    \param V :: Value to add
  */
{
  WorkGrp->addAttribute(K,V);
  return;
}

template<typename T>
void
XMLcollect::addAttribute(const std::string& Comp,
			 const std::string& K,const T& V)
  /*!
    Add an attribute to a component. 
    - E.g. readType=file
    - K == readType , V == file
    \param Comp :: Compenent Key to use
    \param K :: Key to used (primary tag)
    \param V :: Value to add (secondary tag)
  */
{
  ELog::RegMethod RegA("XMLcollect","addAttribute<string,string>");
  XMLobject* Optr=WorkGrp->getObj(Comp);
  if (Optr)
    {
      std::ostringstream cx;
      cx<<V;
      Optr->addAttribute(K,cx.str());
    }
  else
    {
      ELog::EM<<"Error getting "<<Comp
	      <<" from Workgroup "<<*WorkGrp<<ELog::endErr;
    }
  return;
}


const XMLobject*
XMLcollect::findParent(const XMLobject* Optr) const
  /*!
    This is an ugly function to determine the parent
    of the Optr. Top down search. [Note that we have the pointer
    to Optr but this check that it is the case]
    \param Optr :: Pointer to find
    \return The pointer to Optr 
   */
{
  if (Optr==0 || Optr==&Master)  // No parent
    return 0;
  return Master.findParent(Optr);
}

int
XMLcollect::deleteObj(XMLobject* Optr)
  /*!
    This delete a particular object from the system
    \param  Optr :: object to delete
    \retval 1 :: success [object deleted]
    \retval 0 :: failure
  */
{
  XMLgroup* Parent=(Optr) ? 
    dynamic_cast<XMLgroup*>(Optr->getParent()) : 0;
    
  return (Parent) ? Parent->deleteObj(Optr) : 0;
}

std::string
XMLcollect::makeTimeString(const tm* TimePtr) const
  /*!
    Converts a unix clock (from gmtime (struc tm poiter)
    to a XML date format
    
    \param TimePtr :: time struct to process
    \return Time string
  */
{
  std::ostringstream cx;
  cx<<1900+TimePtr->tm_year<<"-";
  cx.fill('0');
  cx.width(2);
  cx<<TimePtr->tm_mon+1<<"-";
  cx.width(2);
  cx<<TimePtr->tm_mday;
  // time
  cx<<" ";
  cx<<std::setw(2)<<TimePtr->tm_hour<<":";
  cx<<std::setw(2)<<TimePtr->tm_min<<":";
  cx<<std::setw(2)<<TimePtr->tm_sec;
  return cx.str();
}

XMLobject*
XMLcollect::getObj(const std::string& KeyName,
		   const int IdNum) const
  /*!
    Given a key name find the closest match 
    \param KeyName :: Name to search
    \param IdNum :: Which match to return
    \return XMLobject
  */
{
  return Master.getObj(KeyName,IdNum);
}

XMLobject*
XMLcollect::findObj(const std::string& KeyName,
		   const int IdNum)
  /*!
    Given a key name find the closest match 
    unlike getObj it allows to descend to search.
    \param KeyName :: Name to search
    \param IdNum :: Which match to return
    \return XMLobject ptr (0 on failure)
  */
{
  if (KeyName.empty() || KeyName=="metadata_entry")
    return &Master;
  if (KeyName==Master.getKey())
    return (IdNum) ? Master.findObj(KeyName,IdNum-1) : &Master;

  return Master.findObj(KeyName,IdNum);
}

XMLgroup*
XMLcollect::findGroup(const std::string& KeyName,
		   const int IdNum)
  /*!
    Given a key name find the closest match 
    unlike getObj it allows to descend to search.
    \param KeyName :: Name to search
    \param IdNum :: Which match to return
    \return XMLgrp ptr (0 on failure)
  */
{

  XMLobject* OutPtr;
  if (KeyName.empty() || KeyName=="metadata_entry")
    OutPtr= &Master;
  else if (KeyName==Master.getKey())
    OutPtr=(IdNum) ? Master.findObj(KeyName,IdNum-1) : &Master;
  else 
    OutPtr=Master.findObj(KeyName,IdNum);
  
  return dynamic_cast<XMLgroup*>(OutPtr);
}

int
XMLcollect::setToKey(const std::string& KeyName,const int IdNum)
  /*!
    Given a Key and a number in indents to find
    determine if the key is to a group and set the 
    value to thate group/object.
    \param KeyName :: Name of the Key/Regex to find
    \param IdNum :: IndexNumber
    \retval -1 :: Object not found
    \retval -2 :: Object not a group
    \retval 0 :: success
  */
{
  XMLobject* XObj=Master.findObj(KeyName,IdNum);
  if (!XObj) // Not found
    return -1;
  XMLgroup* XGrp=dynamic_cast<XMLgroup*>(XObj);
  if (!XGrp)
    {
      XGrp=dynamic_cast<XMLgroup*>(XObj->getParent());
      if (!XGrp)
	throw ColErr::ExBase("setToKey::Error converting from XMLobject to XMLgroup");
      WorkGrp=XGrp;
      return -2;
    }
      
  WorkGrp=XGrp;
  return 0;
}

int
XMLcollect::loadXML(const std::string& Fname)
  /*!
    Takes a file and reads data from the files
    This is a two pass system:
    - First to get the groups
    - Second to get XMLread object
    \param Fname :: File to open
    \return -ve on failure and 0 on success
  */
{
  return loadXML(Fname,"metadata_entry");
}

int
XMLcollect::loadXML(const std::string& FName,const std::string& Key)
  /*!
    Given a key: load from the key.
    \param FName :: Filename
    \param Key :: Key to start on
    \retval -1 :: Failed to get open group
    \retval -2 :: Failed to get file
    \todo Convert Key to XPath
  */
{
  ELog::RegMethod RegA("XMLcollect","loadXML");

  if (InputFile.openFile(FName))
    return -1;
    // Required to finish regex:
  if (!XML::findGroup(InputFile,Key))
    return -2;
  std::string KeyV;
  std::vector<std::string> Attrib;
  std::string body;

  const int type=XML::getNextGroup(InputFile,KeyV,Attrib);
   if (type==-1)
    {
      // make null
      XMLcomp<nullObj>* NPtr=new XMLcomp<nullObj>(WorkGrp,Key);
      NPtr->addAttribute(Attrib);
      WorkGrp->addManagedObj(NPtr);
    }
  if (type!=1)
    return -3;
  return loadXML(KeyV,Attrib);
}

int
XMLcollect::loadXML(const std::string& FName,const std::string& Key,
		    const std::string& subKey,const std::string& Val)
  /*!
    Given a key: load from the key.
    \param FName :: Filename
    \param Key :: Key to start on
    \param subKey :: Key within object to find
    \param Val :: Value to that subKey
    \retval -1 :: Failed to get open group
    \retval -2 :: Failed to get file
    \todo Convert Key to XPath
  */
{
  
  if (InputFile.openFile(FName))
    return -1;
    // Required to finish regex:
  do
    {
      if (!XML::findGroup(InputFile,Key))
	return -2;
    } while( !XML::matchSubGrp(InputFile,subKey,Val) );
  
  // Search ahead simple stuff just process / 
  // complex stuff read the who block
  std::string KeyV;
  std::vector<std::string> Attrib;
  std::string body;

  const int type=XML::getNextGroup(InputFile,KeyV,Attrib);
  if (type==-1)
    {
      // make null
      XMLcomp<nullObj>* NPtr=new XMLcomp<nullObj>(WorkGrp,Key);
      NPtr->addAttribute(Attrib);
      WorkGrp->addManagedObj(NPtr);
    }
  if (type!=1)
    return -3;
  return loadXML(KeyV,Attrib);
}

int
XMLcollect::loadXML(const std::string& CKey,
		    const std::vector<std::string>& VAttrib)
  /*!
    Process from this group onwards
    This assumes we have just opened a group with
    XKey and Attribute. This code is a mess.
    \param CKey :: Current key
    \param VAttrib :: Attributes read from current key
    \return -ve on error , 0 on success
  */
{
  ELog::RegMethod RegA("XMLcollect","loadXML<string,vec<string> >");

  std::string Key;
  std::vector<std::string> Attrib;
  std::string Data;
  // sets cPointer for <key ...> <somthing>
  // to the start of something/
  int created((CKey=="metadata_entry") ? 1 : 0);
  do
    {
      const int type=XML::getGroupContent(InputFile,Key,Attrib,Data);
      if (type==1)       //group
        {
	  if (!created)
	    {
	      addGrp(CKey);
	      WorkGrp->addAttribute(VAttrib);
	      created=1;
	    }
	  if (loadXML(Key,Attrib))
	    return -1;
	}
      else if (type==-1)    // Null group
        {
	  if (!created)
	    {
	      addGrp(CKey);
	      WorkGrp->addAttribute(VAttrib);
	      created=1;
	    }	  
	  XMLcomp<nullObj>* NPtr=new XMLcomp<nullObj>(WorkGrp,Key);
	  NPtr->addAttribute(Attrib);
	  WorkGrp->addManagedObj(NPtr);
	}
      else if (type==2 && Key==CKey)  // XMLread items
        {
	  // If not created must be a data file:
	  if (!created)
	    {
	      XMLread* RPtr=new XMLread(WorkGrp,Key);
	      RPtr->addAttribute(VAttrib);
	      RPtr->setObject(Data);
	      WorkGrp->addManagedObj(RPtr);
	    }
	  else
	    closeGrp();

	  return 0;
	}
      else 
        {
	  ELog::EM<<"File Error"<<CKey<<" "<<Key<<ELog::endErr;
	  return -1;
	}
    } while(InputFile.good());
  // File failed
  return -1;
}

void
XMLcollect::writeXML(std::ostream& OX) const
  /*!
    Accessor to writeXML from Master
    \param OX :: output stream 
  */
{
  OX<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>"<<std::endl;
  Master.writeXML(OX);
  return;
}

};

/*!
\cond TEMPLATE
*/

template void XML::XMLcollect::addAttribute(const std::string&,const double&);
template void XML::XMLcollect::addAttribute(const std::string&,const int&);
template void XML::XMLcollect::addAttribute(const std::string&,const std::string&,
					    const double&);
template void XML::XMLcollect::addAttribute(const std::string&,const std::string&,
					    const std::string&);

template void XML::XMLcollect::addObjAttribute(const std::string&,
					       const std::string&);
template void XML::XMLcollect::addObjAttribute(const std::string&,
					       const double&);
template void XML::XMLcollect::addObjAttribute(const std::string&,const int&);
template void XML::XMLcollect::addObjAttribute(const std::string&,
					       const size_t&);


template int XML::XMLcollect::addComp(const std::string&,const XML::nullObj&);
template int XML::XMLcollect::addComp(const std::string&,const double&);
template int XML::XMLcollect::addComp(const std::string&,const long int&);
template int XML::XMLcollect::addComp(const std::string&,const int&);
template int XML::XMLcollect::addComp(const std::string&,const unsigned int&);
template int XML::XMLcollect::addComp(const std::string&,const size_t&);
template int XML::XMLcollect::addComp(const std::string&,const bool&);
template int XML::XMLcollect::addComp(const std::string&,const std::vector<double>&);
template int XML::XMLcollect::addComp(const std::string&,const std::vector<int>&);
template int XML::XMLcollect::addComp(const std::string&,const Geometry::Vec3D&);
template int XML::XMLcollect::addComp(const std::string&,const Geometry::Quaternion&);

template int XML::XMLcollect::addComp(const std::string&,const std::vector<std::string>&);
template int XML::XMLcollect::addComp(const std::string&,const std::vector<double>&,
				      const std::vector<double>&);
template int XML::XMLcollect::addComp(const std::string&,const std::vector<double>&,
				      const std::vector<double>&,const std::vector<double>&);

// File out:
template int XML::XMLcollect::addComp(const std::string&,const std::string&,
				      const std::vector<double>&,const std::vector<double>&);


template long int XML::XMLcollect::addNumComp(const std::string&,const XML::nullObj&);
template long int XML::XMLcollect::addNumComp(const std::string&,const double&);
template long int XML::XMLcollect::addNumComp(const std::string&,const int&);
template long int XML::XMLcollect::addNumComp(const std::string&,const bool&);
template long int XML::XMLcollect::addNumComp(const std::string&,const std::string&,
				      const std::vector<double>&,const std::vector<double>&);
template long int XML::XMLcollect::addNumComp(const std::string&,const Geometry::Vec3D&);
template long int XML::XMLcollect::addNumComp(const std::string&,const Geometry::Quaternion&);

/*!
\endcond TEMPLATE
*/

