/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/ContainedSpace.h
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
#ifndef attachSystem_ContainedSpace_h
#define attachSystem_ContainedSpace_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
/*!
  \class ContainedSpace
  \version 1.0
  \author S. Ansell
  \date Febrary 2018
  \brief Allows an object to build a divided section
         within the Contained space
*/

class ContainedSpace  : public ContainedComp
{
 private:

  std::string FCName;                  ///< Fixed comp name [if available]
  size_t nDirection;                   ///< No of direction of cut
  int primaryCell;                     ///< Master cell
  int buildCell;                       ///< Space for new cell
  HeadRule BBox;                       ///< Bounding box
  std::vector<LinkUnit> LCutters;      ///< Cutting dividers

  std::pair<long int,long int> ABLink; ///< Link surfaces if set

  void initialize();
    
 public:

  ContainedSpace();
  ContainedSpace(const ContainedSpace&);
  ContainedSpace& operator=(const ContainedSpace&);
  virtual ~ContainedSpace();


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

  void registerSpaceCut(const long int,const long int);

  void buildWrapCell(Simulation&,const int,const int);
  void calcBoundary(Simulation&,const int,const size_t =6);
  void insertObjects(Simulation&);


  
};

}

#endif
 
