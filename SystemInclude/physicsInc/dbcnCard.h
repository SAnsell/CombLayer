/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/dbcnCard.h
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
#ifndef physicsSystem_dbcnCard_h
#define physicsSystem_dbcnCard_h

namespace physicsSystem
{

/*!
  \class dbcnCard
  \version 1.0
  \date April 2015
  \author S.Ansell
  \brief Process the dbcn options
  
  Taken from document la-ur-13-23395
  
*/

class dbcnCard 
{
 private:

  /// index : type : data1 : data2 
  typedef std::tuple<size_t,size_t,long int,double> TTYPE;
  
  std::vector<int> activeFlag;         ///< [0 = j , 1 active]
  std::vector<std::string> nameOrder;   ///< Ordered list of names
  std::map<std::string,TTYPE> Comp;      ///< names for each unit
    
  template<typename T> T& getItem(const std::string&);
  template<typename T> const T& getItem(const std::string&) const;
  std::string itemString(const std::string&) const;
  
 public:
   
  dbcnCard();
  dbcnCard(const dbcnCard&);
  dbcnCard& operator=(const dbcnCard&);
  virtual ~dbcnCard();

  size_t keyType(const std::string&) const;
    
  void setActive(const std::string&,const int);

  template<typename T>
  void setComp(const std::string&,const T&);

  template<typename T>
  const T& getComp(const std::string&) const;

  void reset();
  void write(std::ostream&) const;   
};

}

#endif
