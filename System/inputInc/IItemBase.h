/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   inputInc/IItemBase.h
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
  size_t maxSets;    ///< Max number of sets 
  size_t maxItems;   ///< Max items per set
  size_t reqItems;   ///< Required items per set

  /// DATA Items BEFORE conversion:
  std::vector<std::vector<std::string>> DItems;
  
 public:

  explicit IItem(const std::string&);
  IItem(const std::string&,const std::string&);

  IItem(const IItem&);
  IItem& operator=(const IItem&);
  virtual ~IItem() {}  ///< Destructor

    /// is active
  bool flag() const { return active; }
  

  size_t getNSets() const =0;
  size_t getNItems(const size_t =0) const =0;

  bool isValid(const size_t =0) const =0;

  ///\endcond ABSTRACT

  /// Set Description
  void setDesc(const std::string& D) {Desc=D;}
  /// Get Short key
  const std::string& getKey() const { return Key; }
  /// Get Long key
  const std::string& getLong() const { return Long; }
  /// Get Description
  const std::string& getDesc() const { return Desc; }  

  void setObj(const size_t,const size_t,const std::string&);
  void setObj(const size_t,const std::string&);
  void setObj(const std::string&);

  template<typename T>
  T getObj(const size_t,const size_t);
  template<typename T>
  T getObj(const size_t);
  template<typename T>
  T getObj();
  
  
  void writeSet(std::ostream&,const size_t) const =0;
  void write(std::ostream&) const =0;

};

std::ostream&
operator<<(std::ostream&,const IItem&);

}

#endif
 
