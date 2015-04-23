/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLcomment.h
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
#ifndef XMLcomment_h
#define XMLcomment_h


namespace XML
{

/*!
  \class XMLcomment
  \brief Hold an XML comment
  \author S. Ansell
  \date June 2007
  \version 1.0
  
  This holds the outgoing comment object

*/

class XMLcomment : public XMLobject
{
 public:

  typedef std::list<std::string> CStore;     ///< Storage type for lines of comment

 private:

  CStore Comp;         ///< list of read components

 public:

  
  XMLcomment(XMLobject*,const std::string&);
  XMLcomment(XMLobject*,const std::vector<std::string>&);
  XMLcomment(XMLobject*,const std::string&,const std::string&);
  XMLcomment(XMLobject*,const std::string&,const std::vector<std::string>&);
  XMLcomment(const XMLcomment&);
  virtual XMLcomment* clone() const; 
  XMLcomment& operator=(const XMLcomment&);     
  virtual ~XMLcomment();

  void addLine(const std::string&);          
  void setObject(const std::vector<std::string>&);

  const std::string& getFront() const;
  std::string& getFront();
  int pop();
  /// Accessor to Lines (begin)
  CStore::const_iterator begin() const { return Comp.begin(); }
  /// Accessor to Lines (end)
  CStore::const_iterator end() const { return Comp.end(); }

  void writeXML(std::ostream&) const;

};

}    // NAMESPACE XML

#endif
