/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLobject.h
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
#ifndef XMLobject_h
#define XMLobject_h

namespace XML
{

/*!
  \class nullObj
  \brief Null object act as a template place holder
  \author S. Ansell
  \date February 2006
  \version 1.0
*/
class nullObj
{};


/*!
  \class XMLobject
  \brief Abstract XMLitem class
  \author S. Ansell
  \date February 2006
  \version 1.0
  
  Top level object which holds its depth and
  its keyname.

*/

class XMLobject
{
 protected:

  int depth;                ///< Indent level of the XML 
  int empty;                ///< Empty flag 
  int loaded;               ///< Flag to indicate read in
  int repNumber;            ///< Repeat number [0 none : 1 -N ]
  std::string Key;          ///< XML key to this object
  XMLattribute Attr;        ///< Attribute list
  XMLobject* Base;          ///< Base group

  int writeInitXML(std::ostream&) const;  
  int writeCloseXML(std::ostream&) const;  

 public:
  
  XMLobject(XMLobject*);
  XMLobject(XMLobject*,const std::string&);
  XMLobject(XMLobject*,const std::string&,int const);
  XMLobject(const XMLobject&);
  virtual XMLobject* clone() const;      
  XMLobject& operator=(const XMLobject&);
  virtual ~XMLobject() { }                ///< Destructor

  //  void addAttribute(const std::string&,const std::string&);
  void addAttribute(const std::vector<std::string>&);
  void addAttribute(const std::string&,const char*);
  template<typename T> void addAttribute(const std::string&,const T&);

  int setAttribute(const std::string&,const std::string&);
  size_t hasAttribute(const std::string&) const;
  template<typename T>
  T getAttribute(const std::string&,const T&) const;
  /// Accessor to attributes
  const XMLattribute& getAttr() const { return Attr; }

  /// sets the depth of indentation
  void setDepth(const int A) { depth=A; }
  void writeDepth(std::ostream&,int const= 0) const;

  const std::string& getKey() const { return Key; }  ///< get XML key
  std::string getFullKey() const;
  std::string getKeyBase() const;
  int getKeyNum() const;
  int getRepNum() const { return repNumber; }          ///< Get repeat number
  void setRepNum(const int RN) { repNumber=RN; }       ///< Set repeat number

  void setEmpty(const int flag= 1) { empty=flag; } ///< Force empty flag
  virtual int isEmpty() const { return empty; }    ///< Enquire if empty

  XMLobject* getParent() const { return Base; }        ///< Get Parent
  void setParent(XMLobject* B) { Base=B; }             ///< Set Parent

  std::string getCurrentFile(int const =1) const;

  // Single depth search:
  virtual int hasComponent(const std::string&) const;
  virtual int hasComponent() const;
  virtual std::string getComponent() const;
  virtual std::string getComponent(const std::string&) const;

  template<typename T> 
  const T& getValue(const T&) const;
  template<typename T> T getItem() const;
  template<typename T> T getItem(const std::string&) const;
  template<typename T> T getDefItem(const std::string&,const T&) const;
  template<typename T> T getNamedItem(const std::string&) const;

  /// WriteXML (Null)      
  virtual void writeXML(std::ostream&) const {}      

};

std::ostream& operator<<(std::ostream&,const XMLobject&);

}  // NAMESPACE XML

#endif
