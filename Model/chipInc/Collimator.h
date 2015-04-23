/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/Collimator.h
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
#ifndef hutchSystem_Collimator_h
#define hutchSystem_Collimator_h

class Simulation;

namespace hutchSystem
{
/*!
  \class Collimator
  \version 1.0
  \author S. Ansell
  \date May 2010
  \brief Collimator [insert object]
*/

class Collimator : public attachSystem::ContainedComp,
    public attachSystem::TwinComp
{
 private:
  
  const int colIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
 
  int insertCell;               ///< Cell to insert into
  Geometry::Vec3D Axis;         ///< movement axis
  Geometry::Vec3D XAxis;        ///< X-movement axis [Not down beam]
  
  double gap;                   ///< Gap [between blades]
  double offset;                ///< Offset [across blades] 

  double xStep ;                ///< X step accross beam
  double fStep ;                ///< forward step 
  double zStep ;                ///< Zstep of all
  double xyAngle;               ///< XY-Angle
  double zAngle;                ///< Zangle
  Geometry::Vec3D Centre;       ///< Centre point 

  double width;             ///< Full Width
  double depth;             ///< Full Depth
  double height;            ///< Full Height 

  int defMat;                 ///< Material for roof
  
  double innerWall;           ///< inner wall thickness
  int innerWallMat;           ///< inner wall Material
  
  size_t nLayers;       ///< number of layers
  std::vector<double> cFrac;  ///< coll Layer thicknesss (fractions)
  std::vector<int> cMat;      ///< coll Layer materials
  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const Simulation&);
  void createUnitVector(const TwinComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  Collimator(const std::string&);
  Collimator(const Collimator&);
  Collimator& operator=(const Collimator&);
  ~Collimator();

  void setAxis(const Geometry::Vec3D&);

  void setCentre(const Geometry::Vec3D&);
  void setMidFace(const Geometry::Vec3D&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createPartial(Simulation&,const attachSystem::TwinComp&);
  void createAll(Simulation&,const attachSystem::TwinComp&);

};

}

#endif
 
