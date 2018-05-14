/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   inputInc/IItemBase.h
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
#ifndef mainSystem_IItemBase_h
#define mainSystem_IItemBase_h

namespace mainSystem
{
  /*!
    \class IItem
    \version 1.0
    \author S. Ansell
    \date March 2011
    \brief Holds a base Item
  */
class IItem
{
 private:

  std::string Key;   ///< Key
  std::string Long;  ///< Long Key
  std::string Desc;  ///< Description

  bool active;       ///< Has been set

  size_t activeSet;  ///< Current set:
  size_t activeItem;  ///< Current item:
  
  size_t maxSets;    ///< Max number of sets 
  size_t maxItems;   ///< Max items per set
  size_t reqItems;   ///< Required items per set
  //  bool joinFlag;     ///< Concaternate items
  
  /// DATA Items BEFORE conversion:
  std::vector<std::vector<std::string>> DItems;

  void checkIndex(const size_t,const size_t) const;
  
 public:

  explicit IItem(const std::string&);
  IItem(const std::string&,const std::string&);

  IItem(const IItem&);
  IItem& operator=(const IItem&);
  virtual ~IItem() {}  ///< Destructor


    /// is active
  bool flag() const { return active; }

  void setMaxN(const size_t,const size_t,const size_t);

  size_t getNSets() const;
  size_t getNItems(const size_t =0) const;

  /// Get required items
  size_t getReqItems() const { return reqItems; }
  /// Get Max items
  size_t getMaxItems() const { return maxItems; }

  bool isValid(const size_t =0) const;

  /// Set Description
  void setDesc(const std::string& D) {Desc=D;}
  /// Get Short key
  const std::string& getKey() const { return Key; }
  /// Get Long key
  const std::string& getLong() const { return Long; }
  /// Get Description
  const std::string& getDesc() const { return Desc; }  

  /// Set active
  void setActive() { active=1; }
  void setObj(const size_t,const size_t,const std::string&);
  void setObj(const size_t,const std::string&);
  void setObj(const std::string&);
  template<typename T>
  void setObjItem(const size_t,const size_t,const T&);

  const std::vector<std::string>& getObjectItems(const size_t) const;
  template<typename T> T getObj(const size_t,const size_t) const;
  template<typename T> T getObj(const size_t) const;
  template<typename T> T getObj() const;

  Geometry::Vec3D getCntVec3D(const size_t,size_t&) const;
  
  size_t addSet();
  bool addObject(const std::string&);
  
  void writeSet(std::ostream&,const size_t) const;
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const IItem&);

}

#endif
 
