/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLcomp.h
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
#ifndef XML_XMLcomp_h
#define XML_XMLcomp_h

namespace XML
{


/*!
  \class XMLcomp
  \brief XML base object
  \author S. Ansell
  \date February 2006
  \version 1.0
  
  Contains any type of XML value.
  Based on the template type T
*/

template<typename T>
class XMLcomp : public XMLobject
{
 private:
  
  T Value;         ///< An actual value!!  

 public:

  XMLcomp(XMLobject*,const std::string&);
  XMLcomp(XMLobject*,const std::string&,const T&);
  XMLcomp(const XMLcomp<T>&);
  XMLcomp<T>* clone() const;
  XMLcomp<T>& operator=(const XMLcomp<T>&);
  ~XMLcomp();

  T& getValue() { return Value; }              ///< Access value
  const T& getValue() const { return Value; }  ///< Access value const

  virtual void writeXML(std::ostream&) const;    

};

std::ostream& operator<<(std::ostream&,const XMLattribute&);

} // NAMESPACE MonteCarlo   

#endif
