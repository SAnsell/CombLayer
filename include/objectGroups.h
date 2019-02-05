/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/objectGroups.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef objectGroups_h
#define objectGroups_h

namespace attachSystem
{
  class FixedComp;
}

/*!
  \class objectGroups 
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Controls Object Registration in 10000 (or greater) blocks
*/

class objectGroups
{
 public:
 
  /// Storage type : name : activeUnits
  typedef std::map<std::string,groupRange> MTYPE;

  /// Storage type : cell/cellZone  : name
  typedef std::map<int,std::string> RTYPE;

  /// Storage of component pointers
  typedef std::shared_ptr<attachSystem::FixedComp> CTYPE;

  /// Index of them
  typedef std::map<std::string,CTYPE> cMapTYPE;

 private:

  const int cellZone;              ///< Range for each segment
  int cellNumber;                  ///< Current new cell number

  MTYPE regionMap;                 ///< Index of object numbers [name:grp]
  RTYPE rangeMap;                  ///< Range of objects [index : name]

  cMapTYPE Components;             ///< Pointer to real objects
  std::set<int> activeCells;       ///< All Active cells

  const attachSystem::FixedComp*
    getInternalObject(const std::string&) const;
  attachSystem::FixedComp*
    getInternalObject(const std::string&);

 protected:

  groupRange& getGroup(const std::string&);
  groupRange& inRangeGroup(const int);
  
 public:
  
  objectGroups();
  objectGroups(const objectGroups&);
  objectGroups& operator=(const objectGroups&);
  ~objectGroups();

  int getNextCell(const int) const;
  
  int cell(const std::string&,const size_t = 10000);
  
  std::string inRange(const int) const;
  bool hasCell(const std::string&,const int) const;

  int calcRenumber(const int) const;
    
  void addObject(const std::string&,const CTYPE&);
  void addObject(const CTYPE&);
  template<typename T> const T*
    getObject(const std::string&) const;
  template<typename T> T*
    getObject(const std::string&);
  template<typename T> const T*
    getObjectThrow(const std::string&,const std::string&) const;
  template<typename T> T*
    getObjectThrow(const std::string&,const std::string&);
  bool hasObject(const std::string&) const;

  bool isActive(const int) const;
  
  std::string addActiveCell(const int);
  void removeActiveCell(const int);
  void renumberCell(const int,const int);
  
  /// get active cells
  const std::set<int>& getActiveCells() const
     { return activeCells; }

  /// Get full components list
  const cMapTYPE& getComponents() const
    { return Components; }

  int getFirstCell(const std::string&) const;
  int getLastCell(const std::string&) const;
  std::vector<int> getObjectRange(const std::string&) const;
  const groupRange& getGroup(const std::string&) const;
  
  void reset();
  void rotateMaster();
  
  void write(const std::string&) const;

  std::ostream& writeRange(std::ostream&,const std::string&) const;
  
};


#endif
