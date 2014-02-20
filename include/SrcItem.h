/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/SrcItem.h
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
#ifndef SrcItem_h
#define SrcItem_h

namespace SDef
{

/*!
  \class SrcBase 
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief Top level for the different types 
  
  Describes a source distribution
*/

class SrcBase 
{
 public:
  
  /// Constructor
  SrcBase() {} 
  /// Copy constructor
  SrcBase(const SrcBase&) {}
  /// Assignment operator
  SrcBase& operator=(const SrcBase&) { return *this; }  
  virtual ~SrcBase() {}               ///< Destructor
  
  ///\cond ABSTRACT
  virtual SrcBase* clone() const =0; 
  virtual bool isActive() const=0;
  virtual int getDataType() const =0;
  virtual void setDataType(const int) =0;
  virtual void setDepType(const std::string&,const int) =0;
  virtual std::string getString() const =0;  
  ///\endcond ABSTRACT
};

/*!
  \class SrcItem
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief SrcItem Term
  
  Describes a source item, e.g. a dataType placeholder
  or a value / vector
*/

template<typename OutType>
class SrcItem : public SrcBase
{
 private:
  
  const std::string KeyType;           ///< Key type
  std::string depKey;                  ///< Dependent key
  int active;                          ///< Active
  int dtype;                           ///< Data type [0==static data]
  OutType Data;                        ///< Data if static

 public:
   
  SrcItem(const std::string&);
  SrcItem(const SrcItem&);
  SrcItem& operator=(const SrcItem&);
  virtual SrcItem* clone() const;
  virtual ~SrcItem();


  void setValue(const OutType&);
  /// access data
  const OutType& getData() const { return Data; }
  virtual void setDataType(const int);
  virtual void setDepType(const std::string&,const int);
  virtual int getDataType() const { return dtype; }  ///< get data
  virtual bool isActive() const { return active; }  ///< Is active
  int isData() const { return (!dtype) ? 1 : 0 ; }  ///< has a data card
  virtual std::string getString() const;
};

}   

#endif
