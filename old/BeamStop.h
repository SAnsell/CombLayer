/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/BeamStop.h
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
#ifndef hutchSystem_BeamStop_h
#define hutchSystem_BeamStop_h

class Simulation;

namespace hutchSystem
{
  class ChipIRHutch;
  
/*!
  \class BeamStop
  \version 1.0
  \author S. Ansell
  \date May 2010
  \brief BeamStop [insert object]
*/

class BeamStop : public zoomSystem::LinearComp
{
 private:
  
  const int stopIndex;          ///< Index of surface offset
  const std::string keyName;    ///< KeyName
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:cell

  int insertCell;           ///< Cell to insert into
  Geometry::Vec3D Axis;     ///< movement axis
  Geometry::Vec3D XAxis;    ///< X-movement axis
  Geometry::Vec3D Centre;   ///< Centre point
  
  Geometry::Matrix<double> RBase;  ///< ReBase matrix

  double width;             ///< Full Width
  double depth;             ///< Full Depth
  double height;            ///< Full Height 
  int defMaterial;          ///< Default/backing material
  std::vector<beamBlock> BBlock; ///< Data for beamstop layers

  int nLayers;              ///< Number of layers
  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const Simulation&);
  void createUnitVector(const zoomSystem::LinearComp&);
  
  void createSurfaces(const Simulation&);
  void createDefSurfaces();
  void createObjects(Simulation&);
  void insertObjects(Simulation&);
  void layerProcess(Simulation&);

  int readFile(Simulation&,const std::string&);
  void setAxis(const Geometry::Vec3D&);
  void setCentre(const Geometry::Vec3D&);
  void reMapSurf(ReadFunc::OTYPE&) const;

 public:

  BeamStop(const int,const std::string&);
  BeamStop(const BeamStop&);
  BeamStop& operator=(const BeamStop&);
  ~BeamStop();

  /// Set insert cell
  void setInsertCell(const int);
  /// Set insert axis

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createAll(Simulation&,const hutchSystem::ChipIRHutch&,
		 const std::string&);

 };


}  

#endif
 
