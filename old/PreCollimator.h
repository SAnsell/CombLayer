/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/PreCollimator.h
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
#ifndef hutchSystem_PreCollimator_h
#define hutchSystem_PreCollimator_h

class Simulation;

namespace hutchSystem
{
/*!
  \class PreCollimator
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief PreCollimator [insert object]
*/

class PreCollimator : public zoomSystem::ContainedComp,
    public zoomSystem::LinearComp
{
 private:
  
  const int colIndex;           ///< Index of surface offset
  const std::string keyName;    ///< KeyName
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell

  std::vector<int> insertCells;    ///< Cell to insert into
  Geometry::Vec3D Axis;     ///< Beam  axis
  Geometry::Vec3D XAxis;    ///< Horrizontal axis
  Geometry::Vec3D ZAxis;    ///< Vertial axis
  
  int shapeType;            ///< Shape Type
  double shapeRadius;       ///< Shape size

  double xyAngle;           ///< Angle relative to LC 
  double zAngle;            ///< Angle relative LC 
  double fStep;             ///< origin forward step
  double xStep;             ///< Offset on XAxis of centre
  double zStep;             ///< Offset on ZAxis of centre
  Geometry::Vec3D Centre;   ///< Centre point 

  double width;             ///< Full Depth
  double height;            ///< Full Height
  double depth;             ///< Full Width

  int defMat;                ///< Material for roof
  
  double innerWall;          ///< inner wall thickness
  int innerWallMat;          ///< inner wall Material
  
  int nLayers;                ///< number of layers
  std::vector<double> cFrac;  ///< coll Layer thicknesss (fractions)
  std::vector<int> cMat;      ///< coll Layer materials

  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const Simulation&);
  void createUnitVector(const LinearComp&);

  void createCircleSurfaces();
  void createSquareSurfaces();  
  void createHexagonSurfaces();
  void createOctagonSurfaces();

  std::string createCircleObj(Simulation&);
  std::string createSquareObj(Simulation&);
  std::string createHexagonObj(Simulation&);
  std::string createOctagonObj(Simulation&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void insertObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  PreCollimator(const int,const std::string&);
  PreCollimator(const PreCollimator&);
  PreCollimator& operator=(const PreCollimator&);
  ~PreCollimator();

  void setShape(const int);

  void addInsertCell(const int);
  void setCentre(const Geometry::Vec3D&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createAll(Simulation&,const zoomSystem::LinearComp&);

};

}

#endif
 
