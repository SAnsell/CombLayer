/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLattribute.h
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
#ifndef XMLattribute_h
#define XMLattribute_h

namespace XML
{

/*!
  \class XMLattribute
  \brief atribute for XMLobjects
  \author S. Ansell
  \date December 2006
  \version 1.0
  
  Has a simple string base (currently)
*/

class XMLattribute
{
 public:

  typedef std::vector<std::string> AVEC;  ///< Storage for key/values

 private:
  
  int empty;                        ///< flag on string
  std::vector<std::string> AObj;    ///< Key lies
  std::vector<std::string> Val;     ///< Values 

 public:

  XMLattribute();
  XMLattribute(const std::string&,const std::string&);
  XMLattribute(const XMLattribute&);
  XMLattribute& operator=(const XMLattribute&);
  ~XMLattribute();

  void addAttribute(const std::string&,const std::string&);
  void addAttribute(const std::vector<std::string>&);
  int setAttribute(const std::string&,const std::string&);
  size_t hasAttribute(const std::string&) const;
  std::string getAttribute(const std::string&) const;

  /// Access size
  size_t getNum() const { return (!empty) ? (AObj.size()) : 0; }
  std::pair<std::string,std::string> getComp(const size_t) const;

  void writeXML(std::ostream&) const;   
};

std::ostream& operator<<(std::ostream&,const XMLattribute&);

}  // NAMESPACE XML

#endif
