/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/CSGroup.h
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
#ifndef attachSystem_CSGroup_h
#define attachSystem_CSGroup_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
/*!
  \class CSGroup
  \version 1.0
  \author S. Ansell
  \date Febrary 2018
  \brief Allows an object to build a divided section
         within the Contained space
*/

class CSGroup 
{
 private:

  typedef std::shared_ptr<attachSystem::FixedComp> FCTYPE;
  typedef std::map<std::string,FCTYPE> FCMAP;
  
  ///< FC object that can be made into Countainedcop
  FCMAP FCobjects;

  size_t nDirection;                   ///< No of direction of cut
  int primaryCell;                     ///< Master cell
  int buildCell;                       ///< Space for new cell

  HeadRule inwardCut;                  ///< Outer surface
  HeadRule BBox;                       ///< Bounding box
  std::vector<LinkUnit> LCutters;      ///< Cutting dividers

  
 public:

  CSGroup();
  CSGroup(const FCTYPE&);
  CSGroup(const FCTYPE&,const FCTYPE&);
  CSGroup(const FCTYPE&,const FCTYPE&,const FCTYPE&);
  CSGroup(const CSGroup&);
  CSGroup& operator=(const CSGroup&);
  virtual ~CSGroup();

  
  void setConnect(const size_t,const Geometry::Vec3D&,
		  const Geometry::Vec3D&);
  void setLinkSurf(const size_t,const int);
  void setLinkSurf(const size_t,const HeadRule&);
  void setLinkCopy(const size_t,const FixedComp&,const long int);

  /// set primary cell
  void setPrimaryCell(const int C) { primaryCell=C; }
  /// set primary cell
  void setBuildCell(const int C) { buildCell=C; }
  /// access build cell
  int getBuildCell() const { return buildCell; }
  
  /// Get bounding box
  const HeadRule& getBBox() const { return BBox; }

  void registerFC(const std::shared_ptr<attachSystem::FixedComp>&);

  void buildWrapCell(Simulation&,const int,const int);
  void calcBoundary(Simulation&,const int,const size_t =6);

  void insertAllObjects(Simulation&);


  
};

}

#endif
 
