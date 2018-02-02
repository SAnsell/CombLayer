/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/objectRegister.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef ModelSupport_objectRegister_h
#define ModelSupport_objectRegister_h

namespace attachSystem
{
  class FixedComp;
}

namespace ModelSupport
{

/*!
  \class objectRegister 
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Controls Object Registration in 10000 (or greater) blocks
*/

class objectRegister
{
 private:
 
  /// Storage type : name : startPt : size 
  typedef std::map<std::string,std::pair<int,int> > MTYPE;
  /// Storage of component pointers
  typedef std::shared_ptr<attachSystem::FixedComp> CTYPE;
  /// Index of them
  typedef std::map<std::string,CTYPE> cMapTYPE;

  int cellNumber;                  ///< Current new cell number
  MTYPE regionMap;                 ///< Index of kept object number
  MTYPE renumMap;                  ///< Index of renumbered units
  std::set<int> activeCells;       ///< Active cells
  cMapTYPE Components;             ///< Pointer to real objects


  objectRegister();
  ///\cond SINGLETON
  objectRegister(const objectRegister&);
  objectRegister& operator=(const objectRegister&);
  ///\endcond SINGLETON

  const attachSystem::FixedComp*
    getInternalObject(const std::string&) const;
  attachSystem::FixedComp*
    getInternalObject(const std::string&);
  
 public:
  
  ~objectRegister();

  static objectRegister& Instance();

  int cell(const std::string&,const int = 10000);
  int getCell(const std::string&) const;
  int getLast(const std::string&) const;
  int getRange(const std::string&) const;
  
  std::string inRange(const int) const;
  bool hasCell(const std::string&,const int) const;


  int getRenumberCell(const std::string&) const;
  int getRenumberLast(const std::string&) const;
  int getRenumberRange(const std::string&) const;
  
  std::string inRenumberRange(const int) const;

  int calcRenumber(const int) const;
    
  void addObject(const std::string&,const CTYPE&);
  void addObject(const CTYPE&);
  template<typename T> const T*
    getObject(const std::string&) const;
  template<typename T>  T*
    getObject(const std::string&);
  template<typename T> const T*
    getObjectThrow(const std::string&,const std::string&) const;
  template<typename T>  T*
    getObjectThrow(const std::string&,const std::string&);
  bool hasObject(const std::string&) const;
  void setRenumber(const std::string&,const int,const int);
  
  void addActiveCell(const int);
  void removeActiveCell(const int);
  void renumberActiveCell(const int,const int);
  /// get active cells
  const std::set<int>& getActiveCells() const
     { return activeCells; }

  /// Get full components list
  const cMapTYPE& getComponents() const
    { return Components; }
    
  
  std::vector<int> getObjectRange(const std::string&) const;
  void reset();
  void rotateMaster();
  void write(const std::string&) const;
  
};

}

#endif
