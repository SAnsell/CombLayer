/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/BlockZone.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef attachSystem_BlockZone_h
#define attachSystem_BlockZone_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
  class FixedComp;
  class CellMap;

  
/*!
  \class BlockZone
  \version 1.0
  \author S. Ansell
  \date October 2018
  \brief Builds an inner unit within an object

  This object is for objects that contain an inner space.
  The FCPtr and CellPtr need to point to the possessing object
  so that they don't become obsoleted as the shared_ptr is deleted.

*/

class BlockZone  :
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:

  HeadRule surroundHR;               ///< Rule of surround
  HeadRule frontHR;                  ///< Rule of front

  HeadRule backHR;                   ///< Rule of back
  HeadRule maxExtentHR;              ///< Max extent

  int voidMat;                       ///< Void material

  std::vector<int> insertCells;    ///< Cell to insert into
  std::map<int,const HeadRule> insertHR;  ///< Headrules of original cells
  
  void insertCell(Simulation&,const HeadRule&);
  
 public:

  BlockZone();
  BlockZone(const std::string&);
  BlockZone(const BlockZone&);
  BlockZone& operator=(const BlockZone&);
  ~BlockZone() {}         ///< Destructor

  void rebuildInsertCells(Simulation&);
  
  int merge(const BlockZone&);
  
  void setMaxExtent(const HeadRule&);
  void setSurround(const HeadRule&);
  void setFront(const HeadRule&);
  void setBack(const HeadRule&);

  void initFront(const HeadRule&);

  /// access surround
  const HeadRule& getSurround() const { return surroundHR; }
  /// access front
  const HeadRule& getFront() const { return frontHR; }
  /// access current back
  const HeadRule& getBack() const { return backHR; }
  /// access max outer surf
  const HeadRule& getMaxExtent() const { return maxExtentHR; }

  HeadRule getVolume() const;
  
  /// set the void material
  void setInnerMat(const int M) { voidMat=M; }

  void addInsertCell(const int);
  void addInsertCells(const std::vector<int>&);

  /// Accessor to insert cellls
  const std::vector<int>& getInsertCells() const
    { return insertCells; }
  void clearInsertCells() { insertCells.clear(); }
  
  
  int createUnit(Simulation&);

  int createUnit(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

  int createUnit(Simulation&,
		 const attachSystem::FixedComp&,
		 const std::string&);

  void createLinks(const attachSystem::FixedComp&,const long int);
  
  using FixedComp::createAll;  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

  void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const attachSystem::BlockZone&);

}

#endif
 
