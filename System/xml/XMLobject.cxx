/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLobject.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iterator>
#include <typeinfo>
#include <time.h>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcomp.h"
#include "XMLread.h"

std::ostream& 
XML::operator<<(std::ostream& OX,const XMLobject& A)
  /*!
    Calls XMLobject method write to output class
    \param OX :: Output stream
    \param A :: XMLobject to write
    \return Current state of stream
  */
{
  (&A)->writeXML(OX);
  return OX;
}

namespace XML
{

XMLobject::XMLobject(XMLobject* B) : 
  depth(0),empty(1),loaded(0),repNumber(0),Base(B)
  /*!
    Default constructor
    \param B :: Parent pointer
  */
{}

XMLobject::XMLobject(XMLobject* B,const std::string& K) :
  depth(0),empty(1),loaded(0),repNumber(0),Key(K),Base(B)
  /*!
    Default constructor (with key)
    \param B :: Parent pointer
    \param K :: Key value
  */
{}

XMLobject::XMLobject(XMLobject* B,const std::string& K,const int GN) :
  depth(0),empty(1),loaded(0),repNumber(GN),Key(K),Base(B)
  /*!
    Default constructor (with key)
    \param B :: Parent pointer
    \param K :: Key value
    \param GN :: Repeat number
  */

{}

XMLobject::XMLobject(const XMLobject& A) :
  depth(A.depth),empty(A.empty),loaded(A.loaded),
  repNumber(A.repNumber),Key(A.Key),Attr(A.Attr),Base(A.Base)
  /*!
    Copy constructor for use by derived classes
    \param A :: object to copy
  */
{}


XMLobject&
XMLobject::operator=(const XMLobject& A) 
  /*!
    Standard assignment operator (for use
    by derived classes)
    \param A :: Base object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      depth=A.depth;
      empty=A.empty;
      loaded=A.loaded;
      repNumber=A.repNumber;
      Key=A.Key;
      Attr=A.Attr;
      Base=A.Base;
    }
  return *this;
}

XMLobject*
XMLobject::clone() const
  /*!
    Clone function
    \return new XMLobject copy
  */
{
  return new XMLobject(*this);
}

void
XMLobject::addAttribute(const std::string& Name,
			const char* Val)
  /*!
    Adds an attriubte to the main system.
    \param Name :: Key name
    \param Val :: Value
   */
    
{
  Attr.addAttribute(Name,std::string(Val));
  return;
}

template<>
void
XMLobject::addAttribute(const std::string& Name,
			const std::string& Val)
  /*!
    Adds an attriubte to the main system.
    \param Name :: Key name
    \param Val :: Value
   */
    
{
  Attr.addAttribute(Name,Val);
  return;
}

template<typename T>
void
XMLobject::addAttribute(const std::string& Name,
			const T& Val)
  /*!
    Adds an attriubte to the main system.
    \param Name :: Key name
    \param Val :: Object that has a conversion to an outputstream
   */
{
  std::ostringstream cx;
  cx<<Val;
  Attr.addAttribute(Name,cx.str());
  return;
}

void
XMLobject::addAttribute(const std::vector<std::string>& Vec)
  /*!
    Adds an attriubte to the main system.
    \param Vec :: list of string compents in the form key="value"
   */
    
{
  Attr.addAttribute(Vec);
  return;
}

int
XMLobject::setAttribute(const std::string& Key,const std::string& Value)
  /*!
    Set attribute 
    \param Key :: Key Name
    \param Value :: Value size
    \retval 0 :: success
    \retval -1 :: failure 
   */
{
  return Attr.setAttribute(Key,Value);
}

size_t
XMLobject::hasAttribute(const std::string& Name) const
  /*!
    Given an attribute, find out if we have it
    \param Name :: Name of attribute to check
    \retval Index+1 if Name is found
    \retval 0 :: failure
   */
    
{
  return Attr.hasAttribute(Name);
}

template<typename T>
T
XMLobject::getAttribute(const std::string& Name,const T& defValue) const
  /*!
    Given an attribute, find out if we have it
    returns attribute
    \param Name :: Name of attribute to get
    \param defValue :: Default value
    \return Attribute value (empty string on failure)
   */
    
{
  const std::string X=Attr.getAttribute(Name);
  T OutValue;
  if (!StrFunc::convert(X,OutValue)) 
    return defValue;
  return OutValue;
}

template<>
std::string
XMLobject::getAttribute(const std::string& Name,
			const std::string& defValue) const
  /*!
    Given an attribute, find out if we have it
    returns attribute
    \param Name :: Name of attribute to get
    \param defValue :: Value to be returned if no value found
    \return Attribute value / defValue
   */
{
  const std::string Out=Attr.getAttribute(Name);
  if (Out.empty())
    return defValue;
  return Out;
}


std::string
XMLobject::getFullKey() const
  /*!
    Recursive system to build the full key.
    Note that is AVOIDS the top key (meta_data)
    \return full deliminated key
  */
{
  return (Base && Base->getParent()) ? Base->getFullKey()+"/"+Key : Key;
}  


std::string
XMLobject::getKeyBase() const
  /*!
    This is not really correct since assume _ is unique
    \return base string
  */
{
  size_t pos=Key.size();  
  if (!pos)
    return Key;  // this will be null.
  for(;pos>0 && isdigit(Key[pos-1]);pos--) ;
  return Key.substr(0,pos);
}

int
XMLobject::getKeyNum() const
  /*!
    This extacts the last readable integer from the 
    key name.
    \retval Number on success
    \retval -1 :: failure    
   */
{
  size_t pos=Key.size();  
  if (pos) 
    {
      for(;pos>0 && isdigit(Key[pos-1]);pos--) ;
      int out;
      if (StrFunc::convert(Key.substr(pos),out))
	return out;
    }
  return -1; 
}

void
XMLobject::writeDepth(std::ostream& OX,const int extra) const
  /*!
    Horrible function to write out to the
    stream the correct amount of spacitee
    \param OX :: stream to use
    \param extra :: extra to add to object
  */
{
  if (depth+extra<1)
    return;
  OX<<std::string(static_cast<size_t>(depth+extra),' ');
  return;
}

std::string
XMLobject::getCurrentFile(const int depth) const
  /*!
    Get a string based on 
    -XMLparentKey_XMLkey_rRepeatNumber
    \param depth :: Repeat depth (number of repeats)
    \return string values
  */
{
  const XMLobject* Parent=this;
  std::string Name;
  for(int i=0;i<depth && Parent;i++)
    {
      Name=(i) ? Parent->getKey()+"_"+Name : Parent->getKey();
      Parent=Parent->getParent();
    }

  if (repNumber)
    {
      std::ostringstream cx;
      cx<<Name<<"_r"<<repNumber;
      return cx.str();
    }
  return Name;
}


template<typename T>
const T&
XMLobject::getValue(const T& DefValue) const
  /*!

    Given an object convert into a value based on a default
    value or return the default value
    \todo check that XMLread is correctly converted
    \param DefValue :: Default value to return on failure
    \return Object reference
  */
{
  const  XML::XMLcomp<T>* CPobj=dynamic_cast<const XML::XMLcomp<T>* >(this);
  if (CPobj && !CPobj->isEmpty())
    return CPobj->getValue();
  // Ok but maybe convert an XMLread to a value object

  return DefValue;
}

template<typename T>
T
XMLobject::getItem() const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \return Object 
  */
{
  ELog::RegMethod RegA("XMLobject","getItem");

  const  XML::XMLcomp<T>* CPobj=dynamic_cast<const XML::XMLcomp<T>* >(this);
  if (!CPobj || CPobj->isEmpty())
    {
      if (!CPobj)
        {
	  // Try string:
	  T Value;
	  const XMLcomp<std::string>* CSobj=
	    dynamic_cast<const XML::XMLcomp<std::string>* >(this);
	  if (CSobj && !CSobj->isEmpty() && 
	      StrFunc::convert(CSobj->getValue(),Value))
	    return Value;
	  // Try XMLread
	  const XMLread* CRobj=
	    dynamic_cast<const XMLread*>(this);
	  if (CRobj && !CRobj->isEmpty() && 
	      StrFunc::convert(CRobj->getString(),Value))
	    return Value;
	  // Try XMLread
	  
	}
      throw ColErr::InContainerError<std::string>("baseValue",
						  "Failed to find baseItem");
    }
  return CPobj->getValue();
}

template<>
std::string
XMLobject::getItem() const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \return Object 
  */
{
  const  XML::XMLcomp<std::string>* CPobj=
    dynamic_cast<const XML::XMLcomp<std::string>* >(this);
  if (!CPobj || CPobj->isEmpty())
    {
      if (!CPobj)
        {
	  // Try XMLread
	  const XMLread* CRobj=
	    dynamic_cast<const XMLread*>(this);
	  if (CRobj && !CRobj->isEmpty())
	    return CRobj->getString();
	  
	}
      throw ColErr::InContainerError<std::string>("baseValue",
						  "XMLobject::getItem");
    }
  return CPobj->getValue();
}


template<typename T>
T
XMLobject::getItem(const std::string& KeyName) const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \todo check that XMLread is correctly converted
    \param KeyName :: To access attributes
    \return Object reference
  */
{
  if (Key.empty() || KeyName==Key)
    return getItem<T>();

  if (this->hasComponent(KeyName))
    {
      std::string Out=this->getComponent(KeyName);
      T ObjValue;
      if (StrFunc::convert(Out,ObjValue))
	return ObjValue;
    }
  throw ColErr::InContainerError<std::string>(KeyName,"XMLobject::getItem");
}

template<typename T>
T
XMLobject::getNamedItem(const std::string& KeyName) const
  /*!
    Given an object convert into a value based on a default
    value or the attribut name
    \param KeyName :: name of attribuite
    \return value
  */
{
  if (Key.empty() || KeyName==Key)
    return getItem<T>();
  const XMLgroup* AG=dynamic_cast<const XML::XMLgroup*>(this);
  return (AG) ? 
    AG->getItem<T>(KeyName) : getItem<T>();
}


int
XMLobject::hasComponent(const std::string& KeyName) const
  /*!
    Determine if has a component
    \param KeyName :: To access attributes
    \return true if componenet exist
  */
{
  if (Key.empty())
    return hasComponent();

  return (Attr.hasAttribute(KeyName)) ? 1 : 0;
}

template<typename T>
T
XMLobject::getDefItem(const std::string& KeyName,const T& defItem) const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \todo check that XMLread is correctly converted
    \param KeyName :: To access attributes
    \param defItem :: Default item
    \return Object reference
  */
{
  if (Key.empty() || KeyName==Key)
    return getItem<T>();

  if (this->hasComponent(KeyName))
    {
      std::string Out=this->getComponent(KeyName);
      T ObjValue;
      if (StrFunc::convert(Out,ObjValue))
	return ObjValue;
    }
  return defItem;
}


std::string
XMLobject::getComponent(const std::string& KeyName) const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \param KeyName :: To access attributes
    \return String of  value
  */
{
  if (Key.empty())
    return getComponent();

  if (Attr.hasAttribute(KeyName))
    return Attr.getAttribute(KeyName);
  
  return "";
}

int
XMLobject::hasComponent() const
  /*!
    Determine if we have a component
    \return true/false
  */
{
  return (!empty) ? 1 : 0;
}

std::string
XMLobject::getComponent() const
  /*!
    Given an object convert into a value based on a default
    value or return the default value
    \return String of  value
  */
{
  return (!empty) ? getItem<std::string>() : "";
}


int
XMLobject::writeInitXML(std::ostream& OX) const
  /*!
    Write out the header for an XML output 
    \param OX :: output stream
    \retval 0 :: open body
    \retval 1 :: closed object (empty)
   */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (this->isEmpty())
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return 1;
    }
  OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;
  return 0;
}

int
XMLobject::writeCloseXML(std::ostream& OX) const
  /*!
    Write out the close for an XML output 
    \param OX :: output stream
    \retval 0 :: open body
    \retval 1 :: closed object (empty)
   */
{
  if (this->isEmpty())
    return 1;

   writeDepth(OX);
   std::string::size_type pos=Key.find('%');
   const std::string KeyOut=Key.substr(0,pos);
   OX<<"</"<<KeyOut<<Attr<<">"<<std::endl;
   return 0;
}

/// \cond TEMPLATE
template 
const double& XMLobject::getValue(const double&) const;

template void XMLobject::addAttribute(const std::string&,const double&);
template void XMLobject::addAttribute(const std::string&,const int&);
template void XMLobject::addAttribute(const std::string&,const size_t&);


template double XMLobject::getAttribute(const std::string&,const double&) const;
template int XMLobject::getAttribute(const std::string&,const int&) const;
template size_t XMLobject::getAttribute(const std::string&,const size_t&) const;

template double XMLobject::getItem(const std::string&) const;
template int XMLobject::getItem(const std::string&) const;
template size_t XMLobject::getItem(const std::string&) const;
template std::string XMLobject::getItem(const std::string&) const;
template Geometry::Vec3D XMLobject::getItem(const std::string&) const;

template double XMLobject::getDefItem(const std::string&,const double&) const;
template int XMLobject::getDefItem(const std::string&,const int&) const;
template size_t XMLobject::getDefItem(const std::string&,const size_t&) const;
template std::string XMLobject::getDefItem(const std::string&,
					   const std::string&) const;

template int XMLobject::getItem() const;
template size_t XMLobject::getItem() const;
template Geometry::Vec3D XMLobject::getItem() const;
template double XMLobject::getItem() const;

template int XMLobject::getNamedItem(const std::string&) const;
template size_t XMLobject::getNamedItem(const std::string&) const;
template Geometry::Vec3D XMLobject::getNamedItem(const std::string&) const;
template double XMLobject::getNamedItem(const std::string&) const;
template std::string XMLobject::getNamedItem(const std::string&) const;

/// \endcond TEMPLATE


}   // NAMESPACE XML


