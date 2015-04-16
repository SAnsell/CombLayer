/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/Table.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef hutchSystem_Table_h
#define hutchSystem_Table_h

class Simulation;

namespace hutchSystem
{

/*!
  \class Table
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Table [insert object]
*/

class Table : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int shapeType;          ///< Shape type [0 square : 1 triangle]
  const int tableIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  double xyAngle;           ///< Angle of top relative to LC 
  double zAngle;            ///< Angle of top relative LC 

  double fStep;             ///< origin forward step
  double xStep;             ///< Offset on X of centre
  Geometry::Vec3D Centre;   ///< Centre point 

  double height;           ///< height [from beam]
  double width;            ///< Full width [at back]
  double length;           ///< Full length

  double surThick;         ///< surface thickness
  double sideThick;        ///< Side thickness

  int topMat;              ///< Top material
  int defMat;              ///< Material for remainder
  
  int floorSurf;           ///< Floor surface

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  Table(const int,const std::string&);
  Table(const Table&);
  Table& operator=(const Table&);
  ~Table();

  /// Set floor surfacex
  void setFloor(const int SN) { floorSurf=SN; }

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  /// Value of the centre [beam position]
  const Geometry::Vec3D& getBeamPos() const { return Centre; }

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
