/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xmlInc/XMLgroup.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef XMLgroup_h
#define XMLgroup_h

namespace XML
{

/*!
  \class XMLgroup
  \brief XML container object
  \author S. Ansell
  \date February 2006
  \version 1.0
  
  Contains a set of XMLobjects

*/

class XMLgroup : public XMLobject 
{
 public:

  /// Name : Index of XMLobject in the Grp vector
  typedef std::multimap<std::string,size_t> holdType;   
    
 private:

  std::vector<XMLobject*> Grp;          ///< Orderd list of Objects
  holdType Index;                       ///< Map for searching for an object

  int countKey(const std::string&) const;
  XMLobject* findItem(const std::string&,const size_t =0) const;

 public:

  XMLgroup(XMLobject*);
  XMLgroup(XMLobject*,const std::string&);
  XMLgroup(XMLobject*,const std::string&,const int);
  XMLgroup(const XMLgroup&);
  virtual XMLgroup* clone() const;
  XMLgroup& operator=(const XMLgroup&);
  virtual ~XMLgroup();

  const holdType& getMap() const { return Index; }  ///< Get Map

  XMLobject* getObject(const size_t) const;
  XMLobject* getObj(const std::string&,const int =0) const;
  XMLgroup* getGrp(const std::string&,const int =0) const;

  // Find:: (deep-search) 
  XMLobject* findObj(const std::string&,const int =0) const;
  XMLgroup* findGroup(const std::string&,const int =0) const;
  template<typename T> T getItem(const std::string&,const size_t =0) const;
  XMLobject* getLastObj() const;


  template<typename T>
  T* getType(const int =0) const;
  template<typename T>
  T* getLastType(const int =0) const;

  template<typename T> int addComp(const std::string&,const T&);
  int addComp(const std::string&,const XMLobject*);
  int addManagedObj(XMLobject*);
  XMLgroup* addGrp(const std::string&);
  void deleteGrp();
  int deleteObj(XMLobject*);

  const XMLobject* findParent(const XMLobject*) const;
  
  int isEmpty() const { return Grp.empty(); }
  
  virtual int hasComponent(const std::string&) const;
  virtual std::string getComponent(const std::string&) const;
  
  

  std::vector<XMLobject*>::iterator begin() { return Grp.begin(); }   ///< Start of XMLObjects
  std::vector<XMLobject*>::iterator end() { return Grp.end(); }       ///< End of XMLObjects

  virtual void writeXML(std::ostream&) const;
  
};

}   /// NAMESPACE XML

#endif

