/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/nameCard.h
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
#ifndef physicsSystem_nameCard_h
#define physicsSystem_nameCard_h

namespace physicsSystem
{

/*!
  \class nameCard
  \version 1.0
  \date May 2016
  \author S.Ansell
  \brief Named card of form name keyword = value ..
  
  
*/

class nameCard 
{
 private:

  /// enumeration for type
  enum class MData { DBL = 1, INT = 2, STR = 3, J = 4 };
  
  const std::string keyName;               ///< Main name
  const int writeType;                     ///< Write out type [name / flat]
  bool active;                             ///< Active
  
  std::vector<std::string> nameOrder;      ///< Ordered list of units
  std::map<std::string,MData> regNames;    ///< Registered names / type

  std::set<std::string> JUnit;             ///< Default items [superseeds]
  
  std::map<std::string,double> DUnit;      ///< Double units
  std::map<std::string,long int> IUnit;    ///< int units
  std::map<std::string,std::string> SUnit; ///< string units

  static const std::string& getTypeName(const MData&);
  static MData convertName(const std::string&);
  static void writeJ(std::ostream&,size_t&);
  
  void writeWithName(std::ostream&) const;
  void writeFlat(std::ostream&) const;
  
  template<typename T>
  void setItem(const std::string&,const T&);

  bool isAllDefault() const;
  
 public:
   
  nameCard(const std::string&,const int);
  nameCard(const nameCard&);
  nameCard& operator=(const nameCard&);
  virtual ~nameCard();

  void registerItems(const std::vector<std::string>&);

  /// set active
  void setActive(const bool A) { active=A; }

  void setDefItem(const std::string&);
  

  template<typename T>
  const T& getItem(const std::string&) const;


  template<typename T>
  void setRegIndex(const size_t,const T&);
  
  template<typename T>
  void setRegItem(const std::string&,const T&);

  void reset();
  void writeHelp(std::ostream&) const;
  
  void write(std::ostream&) const;
  
};

}

#endif
