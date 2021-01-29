/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/ScatterPlate.h
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
    public attachSystem::FixedOffset
{
 private:
  
  int populated;                 ///< 1:var

  double width;             ///< Full Width
  double height;            ///< Full Height
  double depth;             ///< Full Depth 

  int defMat;               ///< Material
  
  std::string cutSurf;      ///< Extra cutting surface to the master object

  size_t nLayers;       ///< number of layers
  std::vector<double> cFrac;  ///< Plate Layer thicknesss (fractions)
  std::vector<int> cMat;      ///< Plate Layer materials

  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const FuncDataBase&);

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

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
 
