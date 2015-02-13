/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/ScatterPlate.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef hutchSystem_ScatterPlate_h
#define hutchSystem_ScatterPlate_h

class Simulation;

namespace hutchSystem
{
/*!
  \class ScatterPlate
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief Plate inserted in object 
*/

class ScatterPlate : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int spIndex;             ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int populated;                 ///< 1:var

  Geometry::Vec3D Axis;          ///< Beam  axis
  Geometry::Vec3D XAxis;         ///< Horrizontal axis
  Geometry::Vec3D ZAxis;         ///< Vertial axis
  Geometry::Vec3D Centre;        ///< Centre point

  double zAngle;            ///< Z angle rotation
  double xyAngle;           ///< XY angle rotation
  double fStep;             ///< Forward step
  double width;             ///< Full Width
  double height;            ///< Full Height
  double depth;             ///< Full Depth 

  int defMat;               ///< Material
  
  std::string cutSurf;      ///< Extra cutting surface to the master object

  size_t nLayers;       ///< number of layers
  std::vector<double> cFrac;  ///< Plate Layer thicknesss (fractions)
  std::vector<int> cMat;      ///< Plate Layer materials

  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  ScatterPlate(const std::string&);
  ScatterPlate(const ScatterPlate&);
  ScatterPlate& operator=(const ScatterPlate&);
  ~ScatterPlate();

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
