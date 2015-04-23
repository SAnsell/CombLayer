/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLdatablock.h
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
#ifndef XMLdatablock_h
#define XMLdatablock_h

namespace XML
{

  /*!
    \class XMLdatablock
    \version 1.0
    \author S. Ansell
    \date January 2008
    \brief The object is to hold a multi_array object
    
    T is the storage type and Size is the number of indexes 
   */

template<typename T,int Size>
class XMLdatablock : public XMLobject
{
 private:

  /// Storage for the datablock
  typedef boost::multi_array<T,Size> mapArray;
    
  int managed;                  ///< Is this data managed
  int contLine;                 ///< Number of components per line

  const mapArray* Data;         ///< Data 

  void setSizeAttributes();  

 public:

  XMLdatablock(XMLobject*,const std::string&);
  XMLdatablock(const XMLdatablock<T,Size>&);
  XMLdatablock<T,Size>* clone() const;
  XMLdatablock<T,Size>& operator=(const XMLdatablock<T,Size>&);
  ~XMLdatablock();

  void setContLine(int const IFlag) { contLine=IFlag; }  ///< Set continue size 
  void setManagedComp(const mapArray*);   
  void setUnManagedComp(const mapArray*);          // Assumes copy setting


  /// Const accessor to data
  const mapArray* getArray() const { return Data; } 

  virtual void writeXML(std::ostream&) const;    
  
};

}  // NAMESPACE XML

#endif
